#include <stdio.h>
#include <errno.h>
#include "dataframe.h"
#include "assert/defassert.h"

void TEST_normalize_double_arr();
void TEST_double_arr_mean();
void TEST_double_arr_median();
void TEST_double_arr_min();
void TEST_double_arr_max();

double arr[5], singlearr[1], emptyarr[0];

int main(int argc, char *argv[])
{
	/* init global vars */
	arr[0] = 1.2;
	arr[1] = 5.6;
	arr[2] = 7.8;
	arr[3] = 2.3;
	arr[4] = 11.7;

	singlearr[0] = 5.234;

	/* run tests */
	TEST_normalize_double_arr();
	TEST_double_arr_mean();
	TEST_double_arr_median();
	TEST_double_arr_min();
	TEST_double_arr_max();
}

void TEST_normalize_double_arr()
{
	StartTestStructure("TEST_normalize_double_arr");

	/* copy values to prevent affecting other tests */
	int i;
	double duparr[5], dupsinglearr[1];
	for (i = 0; i < 5; i++)
		duparr[i] = arr[i];
	dupsinglearr[0] = singlearr[0];

	/* should not fail on empty array */
	normalize_double_arr(emptyarr, 0);

	/* should do nothing to data in empty arrays */
	normalize_double_arr(duparr, 0);
	for (i = 0; i < 5; i++)
		NumericAreEqual(duparr[i], arr[i]);

	/* normalization should work properly
 	 * arr[i] = (arr[i] - min) / (max - min) */
	normalize_double_arr(duparr, 5);
	NumericAreEqual(0.0, duparr[0]);
	NumericAreEqual(0.419047619, duparr[1]);
	NumericAreEqual(0.628571429, duparr[2]);
	NumericAreEqual(0.104761905, duparr[3]);
	NumericAreEqual(1.0, duparr[4]);

	/* single-element arrays should normalize to 1.0 */
	normalize_double_arr(dupsinglearr, 1);
	NumericAreEqual(dupsinglearr[0], 1.0);
}

void TEST_double_arr_mean()
{
	StartTestStructure("TEST_double_arr_mean");

	double mean = double_arr_mean(arr, 5);
	NumericAreEqual(mean, 5.72);
}

void TEST_double_arr_median()
{
	StartTestStructure("TEST_double_arr_median");

	double median = double_arr_median(arr, 5);
	NumericAreEqual(median, 5.6);
}

void TEST_double_arr_min()
{
	StartTestStructure("TEST_double_arr_min");

	double min = double_arr_min(arr, 5);
	NumericAreEqual(min, 1.2);
}

void TEST_double_arr_max()
{
	StartTestStructure("TEST_double_arr_max");

	double max = double_arr_max(arr, 5);
	NumericAreEqual(max, 11.7);

	max = double_arr_max(singlearr, 1);
	NumericAreEqual(max, 5.234);

	max = double_arr_max(emptyarr, 0);
	NumericAreEqual(max, (double)-EINVAL);
}
