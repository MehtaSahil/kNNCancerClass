#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dataframe.h"

double distance(struct ntuple p1, struct ntuple p2, int nfeatures);
struct ntuple *sorted_distances(struct ntuple p1, int nfeatures,
				struct dataframe *df);

int main(int argc, char *argv[])
{
	int i;
	struct dataframe train;
	struct dataframe test;

	init_dataframe(&train, "data/breast-cancer-wisconsin-train.data");
	init_dataframe(&test, "data/breast-cancer-wisconsin-test.data");

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

				struct ntuple tempstruct = sorted_ntuples[i];
				sorted_ntuples[i] = sorted_ntuples[j];
				sorted_ntuples[j] = tempstruct;
			}
		}
	}

	free(distances);
	return sorted_ntuples;
}
