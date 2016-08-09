#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataframe.h"

void print_delimited_words(char *instr, char *delimeter);

int main(int argc, char *argv[])
{
	struct dataframe df;
	char *filename = strdup("breast-cancer-wisconsin-nomissing.data");
	init_dataframe(&df, filename);

	fill_dataframe(&df, filename, " ");

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

void fill_dataframe(struct dataframe *df, char *filename, char *delimiter)
{
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		printf("failed to open file : %s\n", filename);
		return;
	}

	char *buf = malloc(50 * sizeof(char));

	int i;
	for (i = 0; i < 5; i++)
	{
		fgets(buf, 50, f);
		print_delimited_words(buf, ",");
	}

	if (fclose(f))
	{
		printf("failed to close file : %s\n", filename);
		free(buf);
		return;
	}

	free(buf);
}

void print_delimited_words(char *instr, char *delimiter)
{
	char *token, *string, *tofree;

	/* avoid the ID entry in data */
	int tracker = 0;

	tofree = string = strdup(instr);
	while ((token = strsep(&string, delimiter)) != NULL)
	{
		if (token[1] == '\n')
			printf("| %-5.1f", (double)(token[0] - '0'));
		else if (tracker != 0)
			printf("%-5.1f ", (double)atof(token));

		tracker++;
	}

	printf("\n");

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
