#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "dataframe.h"

int main(int argc, char *argv[])
{
	int rc;

	struct dataframe df;
	char *filename = strdup("data/breast-cancer-wisconsin-nomissing.data");
	rc = init_dataframe(&df, filename);
	if (rc)
	{
		printf("failed to init dataframe\n");
		free(filename);
		return 1;
	}

	rc = fill_dataframe(&df, filename, ",");
	if (rc)
	{
		printf("failed to fill dataframe\n");
		free(filename);
		return 1;
	}

	int i;
	double *testarr;
	testarr = malloc(5 * sizeof(double));
	testarr[0] = -1.2;
	testarr[1] = 45.6;
	testarr[2] = -1234;
	testarr[3] = 1233.7;
	testarr[4] = 671;

	for (i = 0; i < 5; i++)
		printf("%.1f\n", testarr[i]);
	printf("mean : %.1f\n", double_arr_mean(testarr, 5));
	printf("median : %.1f\n", double_arr_median(testarr, 5));
	printf("min : %.1f\n", double_arr_min(testarr, 5));
	printf("max : %.1f\n", double_arr_max(testarr, 5));

	free_dataframe(&df);
	free(filename);
	return 0;
}

int init_dataframe(struct dataframe *df, char *filename)
{
	int i;

	/* cols = number of features per entry (including result)
 	 * rows = number of entries in data set */
	df->numcols = 10;
	df->numrows = num_lines_in_file(filename);

	if (!df->numrows)
	{
		printf("failed to init df->numcols\n");
		free_dataframe(df);
		return 1;
	}

	df->entries = malloc(df->numrows * sizeof(struct ntuple));
	for (i = 0; i < df->numrows; i++)
	{
		df->entries[i].features =
				malloc((df->numcols - 1) * sizeof(double));

		if (!df->entries[i].features)
		{
			printf("failed to init df->entries[%d].features\n", i);
			free_dataframe(df);
			return 1;
		}

		df->entries[i].result = 0;
	}

	df->colmeans	= malloc(df->numcols * sizeof(double));
	df->colmedians	= malloc(df->numcols * sizeof(double));
	df->colmaxes	= malloc(df->numcols * sizeof(double));
	df->colmins	= malloc(df->numcols * sizeof(double));

	if (!df->colmeans || !df->colmedians
	    || !df->colmaxes || !df->colmins)
	{
		printf("failed to init df statistical fields\n");
		free_dataframe(df);
		return 1;
	}

	return 0;
}

int free_dataframe(struct dataframe *df)
{
	int i;
	for (i = 0; i < df->numrows; i++)
		free(df->entries[i].features);
	free(df->entries);

	free(df->colmeans);
	free(df->colmedians);
	free(df->colmaxes);
	free(df->colmins);
}

void normalize_all_cols(struct dataframe *df);

void normalize_col(struct dataframe *df, int col);

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
		return;
	}

	double *to_return;
	to_return = malloc(df->numrows * sizeof(double));
	if (!to_return)
	{
		printf("failed to malloc to_return in df_col_to_arr\n");
		return;
	}

	/* fill to_return */
	int r;
	for (r = 0; r < df->numrows; r++)
		to_return[r] = df->entries[r].features[col];

	return to_return;
}

double double_arr_mean(double *arr, int length)
{
	int i;
	double sum = 0;
	for (i = 0; i < length; i++)
		sum += arr[i];

	return sum / ((double)length);
}

double double_arr_median(double *arr, int length)
{
	double median;

	/* selection sort O(n^2) */
	int i, j;
	for (i = 0; i < length; i++)
	{
		for (j = i; j < length; j++)
		{
			if (arr[j]  < arr[i])
			{
				double temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
	}

	int median_index = (int)(length / 2);

	if (length % 2 == 0)
	{
		double num1 = arr[median_index];
		double num2 = arr[median_index + 1];
		double sum = num1 + num2;
		median = sum / 2;
	}
	else
	{
		median = arr[median_index];
	}

	return median;
}
	
double double_arr_min(double *arr, int length)
{
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
		return 1;
	}

	if (nrows > df->numrows)
	{
		printf("cannot print %d rows from dataframe with %d rows\n",
							nrows, df->numrows);
		return 1;
	}

	int i, j;
	for (i = 0; i < nrows; i++)
	{
		printf("%d : ", i);
		for (j = 0; j < df->numcols - 1; j++)
		{
			printf("%-5.1f ", df->entries[i].features[j]);
		}
		printf(" | %-5.1f", df->entries[i].result);
		printf("\n");
	}

	return 0;
}
