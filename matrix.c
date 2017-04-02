#include <stdio.h>
#include <string.h>
#include "matrix.h"

void printMatrix4(mat4 M, char *name)
{
	int i, j;

	printf("%s = \n",name);

	for (i = 0; i < 4; i++)
	{		
		for (j = 0; j < 4; j++)
			printf("\t%0.1f", M[i][j]);
		printf("\n");
	}
}

void zero4(mat4 M)
{
	memset(M, 0, sizeof(mat4));
}

void identity4(mat4 M)
{
	int i, j;

	for(i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
		{
			if (i == j)
				M[i][j] = 1.0f;
			else
				M[i][j] = 0.0f;
		}
}

void transpose4(mat4 M, mat4 N)
{
	int i, j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			M[i][j] = N[j][i];
}