#include <stdio.h>
#include <string.h>
#include "defassert.h"

int IntegerAreEqual(long long x, long long y)
{
	printf("%d ? %d == %d\n", x == y, x, y);
	return x == y;
}

int NumericAreEqual(double x, double y)
{
	int rc = 0;
	double sigma = 0.001;
	if (x > y - sigma && x < y + sigma)
		rc = 1;

	printf("%d ? %.2f == %.2f\n", rc, x, y);
	return rc;
}

int StringAreEqual(char *str1, char *str2)
{
	int i, rc = 1;
	int len1 = strlen(str1);
	int len2 = strlen(str2);

	if (len1 != len2)
	{
		rc = 0;
		goto declare_strings_equal;
	}

	for (i = 0; i < len1; i++)
	{
		if (str1[i] != str2[i])
		{
			rc = 0;
			goto declare_strings_equal;
		}
	}

declare_strings_equal:
	printf("%d ? %s == %s\n", rc, str1, str2);
}
