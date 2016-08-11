#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <errno.h>
#include "dataframe.h"

int init_dataframe(struct dataframe *df, char *filename)
{
	int i;

	/* cols = number of features per entry (including result)
 	 * rows = number of entries in data set */
	df->numcols = 10;
	df->numrows = num_lines_in_file(filename);

	if (!df->numrows)
		return 1;

	df->entries = malloc(df->numrows * sizeof(struct ntuple));
	for (i = 0; i < df->numrows; i++)
	{
		df->entries[i].features =
				malloc((df->numcols - 1) * sizeof(double));

		if (!df->entries[i].features)
		{
			printf("failed to init df->entries[%d].features\n", i);

			/* stopping condition for free_dataframe */
			df->entries[i].features = NULL;
			free_dataframe(df);
			return 1;
		}

		df->entries[i].result = 0;
	}

	if (fill_dataframe(df, filename, ","))
	{
		printf("failed to fill dataframe\n");
		return 1;
	}

	normalize_all_features(df);

	return 0;
}

int free_dataframe(struct dataframe *df)
{
	printf("freeing resources\n");

	if (df->entries == NULL)
		return 0;

	int i;
	for (i = 0; i < df->numrows; i++)
	{
		/* free all features allocated before malloc failure */
		if (df->entries[i].features == NULL)
			break;
		free(df->entries[i].features);
	}
	free(df->entries);

	return 0;
}

void normalize_all_features(struct dataframe *df)
{
	int r, c;

	for (c = 0; c < df->numcols - 1; c++)
	{
		double *feature_col = df_col_to_arr(df, c);
		normalize_double_arr(feature_col, df->numrows);

		/* overwrite existing df values to reflect normalization */
		for (r = 0; r < df->numrows; r++)
		{
			/* multiply by 1000 to avoid roundoff error */
			df->entries[r].features[c] = feature_col[r] * 1000;
		}

		free(feature_col);
	}
}

void normalize_double_arr(double *arr, int length)
{
	if (length == 1)
	{
		arr[0] = 1.0;
		return;
	}

	if (length <= 0)
		return;

	int i;
	double min = double_arr_min(arr, length);
	double max = double_arr_max(arr, length);

	/* normalize IN PLACE */
	for (i = 0; i < length; i++)
		arr[i] = (arr[i] - min) / (max - min);
}

int fill_dataframe(struct dataframe *df, char *filename, char *delimiter)
{
	FILE *f = fopen(filename, "r");
	char *buf;
	int spot, chars_to_read = 50;

	if (f == NULL)
	{
		printf("failed to open file : %s\n", filename);
		return 1;
	}

	buf = malloc(chars_to_read * sizeof(char));
	spot = 0;
	while (fgets(buf, chars_to_read, f))
	{
		fill_delimited(buf, df->entries[spot].features,
				    &df->entries[spot].result, delimiter);
		spot++;
	}

	if (fclose(f))
	{
		printf("failed to close file : %s\n", filename);
		free(buf);
		return 1;
	}

	free(buf);
	return 0;
}

void fill_delimited(char *instr, double *tofill, double *res, char *delimiter)
{
	char *token, *string;

	/* avoid the ID entry in data (first) */
	int tracker = 0;
	int spot = 0;

	string = strdup(instr);
	while ((token = strsep(&string, delimiter)) != NULL)
	{
		/* last character and \n are grouped in a token */
		if (token[1] == '\n')
			*res = (double)(token[0] - '0');
		else if (tracker != 0)
			tofill[spot++] = (double)atof(token);

		tracker++;
	}

	free(token);
	free(string);
}

int num_lines_in_file(char *filename)
{
	int chars_to_read = 50;
	int num_lines = 0;
	char *buf;
	buf = malloc(chars_to_read * sizeof(char));

	FILE *f = fopen(filename, "r");
	if (!f)
	{
		printf("could not open file : %s\n", filename);
		free(buf);
		return;
	}

	while (fgets(buf, chars_to_read, f))
		num_lines++;

	free(buf);
	return num_lines;
}


double *df_col_to_arr(struct dataframe *df, int col)
{
	/* if col is out of feature bounds */
	if (col >= df->numcols - 1)
	{
		printf("there is no col <%d> in df\n", col);
		return NULL;
	}

	double *to_return;
	to_return = malloc(df->numrows * sizeof(double));
	if (!to_return)
	{
		printf("failed to malloc to_return in df_col_to_arr\n");
		return NULL;
	}

	/* fill to_return */
	int r;
	for (r = 0; r < df->numrows; r++)
		to_return[r] = df->entries[r].features[col];

	return to_return;
}

double double_arr_mean(double *arr, int length)
{
	if (length <= 0)
	{
		printf("cannot find mean of array with zero elements\n");
		return (double)-EINVAL;
	}

	int i;
	double sum = 0;
	for (i = 0; i < length; i++)
		sum += arr[i];

	return sum / ((double)length);
}

double double_arr_median(double *arr, int length)
{
	if (length <= 0)
	{
		printf("cannot find median of array with zero elements\n");
		return (double)-EINVAL;
	}

	double median;
	double *duparr;
	int i, j;

	duparr = malloc(length * sizeof(double));
	if (!duparr)
	{
		printf("failed to malloc duparr\n");
		return;
	}

	/* copy data to avoid manipulating original */
	for (i = 0; i < length; i++)
		duparr[i] = arr[i];

	/* selection sort O(n^2) */
	for (i = 0; i < length; i++)
	{
		for (j = i; j < length; j++)
		{
			if (duparr[j] < duparr[i])
			{
				double temp = duparr[i];
				duparr[i] = duparr[j];
				duparr[j] = temp;
			}
		}
	}

	int median_index = (int)(length / 2);

	if (length % 2 == 0)
	{
		double num1 = duparr[median_index];
		double num2 = duparr[median_index + 1];
		double sum = num1 + num2;
		median = sum / 2;
	}
	else
	{
		median = duparr[median_index];
	}

	free(duparr);
	return median;
}

double double_arr_min(double *arr, int length)
{
	if (length <= 0)
	{
		printf("cannot find min of array with zero elements\n");
		return (double)-EINVAL;
	}

	int i;
	double min = DBL_MAX;
	for (i = 0; i < length; i++)
	{
		if (arr[i] < min)
			min = arr[i];
	}

	return min;
}

double double_arr_max(double *arr, int length)
{
	if (length <= 0)
	{
		printf("cannot find max of array with zero elements\n");
		return (double)-EINVAL;
	}

	int i;
	double max = -DBL_MAX;
	for (i = 0; i < length; i++)
	{
		if (arr[i] > max)
			max = arr[i];
	}

	return max;
}

int print_n_df_rows(struct dataframe *df, int nrows)
{
	if (!df)
	{
		printf("cannot print rows from null dataframe\n");
		return -EINVAL;
	}

	if (nrows > df->numrows)
	{
		printf("cannot print %d rows from dataframe with %d rows\n",
							nrows, df->numrows);
		return -EINVAL;
	}

	int i, j;
	for (i = 0; i < nrows; i++)
	{
		printf("%d : ", i);
		for (j = 0; j < df->numcols - 1; j++)
		{
			printf("%-7.2f ", df->entries[i].features[j]);
		}
		printf(" | %-7.2f", df->entries[i].result);
		printf("\n");
	}

	return 0;
}
