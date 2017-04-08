#include <stdio.h>
#include <string.h>
#include <math.h>
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

void dup4(mat4 M, mat4 N)
{
	int i, j;
	for (i = 0; i < 4; ++i)
		for (j = 0; j < 4; ++j) M[i][j] = N[i][j];
}

void getRotZ4(mat4 M, float phi)
{
	float s = sinf(phi);
	float c = cosf(phi);
	
	identity4(M);
	M[0][0] = c; M[0][1] = s;
	M[1][0] = -s; M[1][1] = c;
}

void mult4(mat4 M, mat4 a, mat4 b)
{
	int k, r, c;
	for (c = 0; c < 4; ++c)
		for (r = 0; r < 4; ++r) {
			M[c][r] = 0.f;
			for (k = 0; k < 4; ++k) M[c][r] += a[k][r] * b[c][k];
		}
}