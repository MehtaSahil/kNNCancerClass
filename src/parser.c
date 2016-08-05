#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int print_delimited_words(char *string, char *delimiter);

int main(int argc, char *argv[])
{
	FILE *f = fopen("breast-cancer-wisconsin-nomissing.data", "r");
	if (f == NULL)
	{
		printf("failed to open file\n");
		return 1;
	}

	char *buf = malloc(50 * sizeof(char));

	/* while (fgets(buf, 50, f))
		printf("%s", buf); */

	int i;
	for (i = 0; i < 5; i++)
	{
		fgets(buf, 50, f);
		print_delimited_words(buf, ",");
	}

	if(fclose(f))
	{
		printf("failed to close file\n");
		free(buf);
		return 1;
	}

	free(buf);
	return 0;
}

int print_delimited_words(char *instr, char *delimiter)
{

	char *token, *string, *tofree;

	int tracker = 0;

	tofree = string = strdup(instr);
	while ((token = strsep(&string, delimiter)) != NULL)
	{
		/* TODO : handle questionmark as mean/median of column */

		/* final char is bundled with newline */
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

	return 0;
}
