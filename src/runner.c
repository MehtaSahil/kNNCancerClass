#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dataframe.h"

double distance(struct ntuple p1, struct ntuple p2, int nfeatures);
struct ntuple *sorted_distances(struct ntuple p1, int nfeatures,
				struct dataframe *df);
double get_result(struct ntuple p1, struct dataframe *train, int k);

int main(int argc, char *argv[])
{
	int i, k;

	if (argc != 2)
	{
		printf("setting k to default value of : %d\n", 500);
		k = 500;
	}
	else
	{
		k = atoi(argv[1]);
	}

	int num_true = 0;
	int num_false = 0;

	struct dataframe train;
	struct dataframe test;

	init_dataframe(&train, "../data/breast-cancer-wisconsin-train.data");
	init_dataframe(&test, "../data/breast-cancer-wisconsin-test.data");

	for (i = 0; i < test.numrows; i++)
	{
		struct ntuple p1 = test.entries[i];
		double res = get_result(p1, &train, k);
		if (&res == NULL)
		{
			printf("invalid k\n");
			return 1;
		}

		int correct = p1.result == res;
		// printf("%.1f : %.1f : %d\n", p1.result, res, correct);

		if (correct)
			num_true++;
		else
			num_false++;
	}

	int total = num_true + num_false;
	double p_right = ((double)num_true) / ((double)total) * 100;
	double p_wrong = ((double)num_false) / ((double)total) * 100;
	printf("%d\n", k);
	printf("right : %f \%\n", p_right);
	printf("wrong : %f \%\n", p_wrong);
	printf("\n");

	free_dataframe(&train);
	free_dataframe(&test);
	return 0;
}

double distance(struct ntuple p1, struct ntuple p2, int nfeatures)
{
	int i;
	double sum_of_squares = 0;
	for (i = 0; i < nfeatures; i++)
	{
		double tempdiff = p1.features[i] - p2.features[i];
		sum_of_squares += pow(tempdiff, 2);
	}

	return sqrt(sum_of_squares);
}

struct ntuple *sorted_distances(struct ntuple p1, int nfeatures,
				struct dataframe *df)
{
	int i, j;
	double *distances;
	struct ntuple *sorted_ntuples;

	distances = malloc(df->numrows * sizeof(double));
	if (!distances)
	{
		printf("malloc of distances failed\n");
		return;
	}

	sorted_ntuples = malloc(df->numrows * sizeof(struct ntuple));
	if (!sorted_ntuples)
	{
		printf("malloc of sorted_ntuples failed\n");
		return;
	}

	/* fill distances */
	for (i = 0; i < df->numrows; i++)
	{
		distances[i] = distance(p1, df->entries[i], df->numcols - 1);
		sorted_ntuples[i] = df->entries[i];
	}

	/* selection sort O(n^2)*/
	for (i = 0; i < df->numrows; i++)
	{
		for (j = 0; j < df->numrows; j++)
		{
			if (distances[j] > distances[i])
			{
				double tempdouble = distances[i];
				distances[i] = distances[j];
				distances[j] = tempdouble;

				/* maintain ordering in ntuples array */
				struct ntuple tempstruct = sorted_ntuples[i];
				sorted_ntuples[i] = sorted_ntuples[j];
				sorted_ntuples[j] = tempstruct;
			}
		}
	}

	free(distances);
	return sorted_ntuples;
}

double get_result(struct ntuple p1, struct dataframe *train, int k)
{
	if (k > train->numrows)
	{
		printf("cannot retrieve more samples than rows\n");
		return;
	}

	int i;

	/* variables to count occurrences of 2.0 and 4.0 as results */
	int c2 = 0;
	int c4 = 0;

	struct ntuple *distances = sorted_distances(p1, train->numcols - 1,
						    train);

	for (i = 0; i < k; i++)
	{
		double res = distances[i].result;
		if (res == 2)
			c2++;
		else if (res == 4)
			c4++;
		else
			printf("error : unrecognized result\n");
	}

	free(distances);

	if (c4 > c2)
		return 4.0;
	if (c2 > c4)
		return 2.0;

	return 0.0;
}
