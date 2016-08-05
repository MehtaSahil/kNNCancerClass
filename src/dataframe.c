#include <stdio.h>
#include <stdlib.h>
#include "dataframe.h"

int main(int argc, char *argv[])
{

	return 0;
}

int init_dataframe(struct dataframe *df);

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
