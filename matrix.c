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

void getTrans4(mat4 T, float x, float y, float z)
{
	identity4(T);
	T[3][0] = x;
	T[3][1] = y;
	T[3][2] = z;
}

void getRotY4(mat4 M, float phi)
{
	float s = sinf(phi);
	float c = cosf(phi);

	identity4(M);
	M[0][0] = c; M[2][0] = -s;
	M[0][2] = s; M[2][2] = c;
}

void getRotZ4(mat4 M, float phi)
{
	float s = sinf(phi);
	float c = cosf(phi);
	
	identity4(M);
	M[0][0] = c; M[1][0] = -s;
	M[0][1] = s; M[1][1] =  c;
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

// Vulkan Projection Matrix for NDC-Tranformation
void getFrustum(mat4 M, float r, float t, float n, float f)
{
	M[0][0] = n / r; M[1][0] = 0.0f;   M[2][0] = 0.0f;         M[3][0] = 0.0f;
	M[0][1] = 0.0f;  M[1][1] = -n / t; M[2][1] = 0.0f;         M[3][1] = 0.0f;
	M[0][2] = 0.0f;  M[1][2] = 0.0f;   M[2][2] = -f / (f - n); M[3][2] = -f*n / (f - n);
	M[0][3] = 0.0f;  M[1][3] = 0.0f;   M[2][3] = -1.0f;        M[3][3] = 0.0f;
}