#include <stdio.h>
#include <stdlib.h>
#include "dataframe.h"

int main(int argc, char *argv[])
{
	struct dataframe df;
	init_dataframe(&df, "breast-cancer-wisconsin-nomissing.data");

	return 0;
}

int init_dataframe(struct dataframe *df, char *filename)
{
	int i;

	df->numrows = 10;
	df->numcols = num_lines_in_file(filename);

	if (!df->numcols)
	{
		printf("failed to init df->numcols\n");
		free_dataframe(df);
		return 1;
	}

	df->entries = malloc(df->numcols * sizeof(struct ntuple));
	for (i = 0; i < df->numcols; i++)
	{
		df->entries[i].features =
				malloc((df->numrows-1) * sizeof(double));

		if (!df->entries[i].features)
		{
			printf("failed to init df->entries[%d].features\n", i);
			free_dataframe(df);
			return 1;
		}

		df->entries[i].result = 0;
	}

	df->colmeans	= malloc(df->numrows * sizeof(double));
	df->colmedians	= malloc(df->numrows * sizeof(double));
	df->colmaxes	= malloc(df->numrows * sizeof(double));
	df->colmins	= malloc(df->numrows * sizeof(double));

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
	for (i = 0; i < df->numcols; i++)
		free(df->entries[i].features);
	free(df->entries);

	free(df->colmeans);
	free(df->colmedians);
	free(df->colmaxes);
	free(df->colmins);
}

void normalize_all_cols(struct dataframe *df);

void normalize_col(struct dataframe *df, int col);

void fill_dataframe(struct dataframe *df, char *filename, char *delimiter);

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
