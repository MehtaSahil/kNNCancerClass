#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataframe.h"

int main(int argc, char *argv[])
{
	int rc;

	struct dataframe df;
	char *filename = strdup("breast-cancer-wisconsin-nomissing.data");
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
	

	int i, j;
	for (i = 0; i < df.numrows; i++)
	{
		printf("%d : ", i);
		for (j = 0; j < df.numcols - 1; j++)
		{
			printf("%-5.1f ", df.entries[i].features[j]);
		}
		printf(" | %-5.1f", df.entries[i].result);
		printf("\n");
	}

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
	int spot;

	if (f == NULL)
	{
		printf("failed to open file : %s\n", filename);
		return 1;
	}

	buf = malloc(50 * sizeof(char));
	spot = 0;
	while (fgets(buf, 50, f))
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
	char *token, *string, *tofree;

	/* avoid the ID entry in data (first) */
	int tracker = 0;
	int spot = 0;

	tofree = string = strdup(instr);
	while ((token = strsep(&string, delimiter)) != NULL)
	{
		if (token[1] == '\n')
			*res = (double)(token[0] - '0');
		else if (tracker != 0)
			tofill[spot++] = (double)atof(token);

		tracker++;
	}

	free(token);
	free(string);
	free(tofree);
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
