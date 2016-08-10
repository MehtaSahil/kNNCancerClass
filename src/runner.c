#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dataframe.h"

double distance(struct ntuple p1, struct ntuple p2, int nfeatures);
double *sorted_distances(struct ntuple p1, int nfeatures, struct dataframe *df);

int main(int argc, char *argv[])
{
	int i;
	struct dataframe train;
	struct dataframe test;

	init_dataframe(&train, "data/breast-cancer-wisconsin-train.data");
	init_dataframe(&test, "data/breast-cancer-wisconsin-test.data");

	double *distances = sorted_distances(test.entries[0],
					     test.numcols - 1,
					     &test);

	for (i = 0; i < test.numrows; i++)
		printf("%f\n", distances[i]);

	free(distances);
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

double *sorted_distances(struct ntuple p1, int nfeatures, struct dataframe *df)
{
	int i, j;
	double *distances;
	distances = malloc(df->numrows * sizeof(double));
	if (!distances)
	{
		printf("malloc of distances failed\n");
		return;
	}

	for (i = 0; i < df->numrows; i++)
	{
		distances[i] = distance(p1, df->entries[i], df->numcols - 1);
	}

	/* selection sort O(n^2)*/
	for (i = 0; i < df->numrows; i++)
	{
		for (j = 0; j < df->numrows; j++)
		{
			if (distances[j] > distances[i])
			{
				double temp = distances[i];
				distances[i] = distances[j];
				distances[j] = temp;
			}
		}
	}

	return distances;
}
