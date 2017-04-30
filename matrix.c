#include <stdio.h>
#include <stdlib.h>
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
			printf("\t%0.1f", M[j][i]);
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

void dup4(mat4 dst, mat4 src)
{
	int i, j;
	for (i = 0; i < 4; ++i)
		for (j = 0; j < 4; ++j) dst[i][j] = src[i][j];
}

void scale4(mat4 M, float sx, float sy, float sz)
{
	M[0][0] = M[0][0] * sx;
	M[1][1] = M[1][1] * sy;
	M[2][2] = M[2][2] * sz;
}

void getTrans4(mat4 T, float x, float y, float z)
{
	identity4(T);
	T[3][0] = x;
	T[3][1] = y;
	T[3][2] = z;
}

void getRotX4(mat4 M, float phi)
{
	float s = sinf(phi);
	float c = cosf(phi);

	identity4(M);
	M[1][1] = c; M[2][1] = -s;
	M[1][2] = s; M[2][2] = c;
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

void invert4(mat4 T, mat4 M)
{
	float s[6];
	float c[6];
	s[0] = M[0][0] * M[1][1] - M[1][0] * M[0][1];
	s[1] = M[0][0] * M[1][2] - M[1][0] * M[0][2];
	s[2] = M[0][0] * M[1][3] - M[1][0] * M[0][3];
	s[3] = M[0][1] * M[1][2] - M[1][1] * M[0][2];
	s[4] = M[0][1] * M[1][3] - M[1][1] * M[0][3];
	s[5] = M[0][2] * M[1][3] - M[1][2] * M[0][3];

	c[0] = M[2][0] * M[3][1] - M[3][0] * M[2][1];
	c[1] = M[2][0] * M[3][2] - M[3][0] * M[2][2];
	c[2] = M[2][0] * M[3][3] - M[3][0] * M[2][3];
	c[3] = M[2][1] * M[3][2] - M[3][1] * M[2][2];
	c[4] = M[2][1] * M[3][3] - M[3][1] * M[2][3];
	c[5] = M[2][2] * M[3][3] - M[3][2] * M[2][3];

	/* Assumes it is invertible */
	float idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

	T[0][0] = (M[1][1] * c[5] - M[1][2] * c[4] + M[1][3] * c[3]) * idet;
	T[0][1] = (-M[0][1] * c[5] + M[0][2] * c[4] - M[0][3] * c[3]) * idet;
	T[0][2] = (M[3][1] * s[5] - M[3][2] * s[4] + M[3][3] * s[3]) * idet;
	T[0][3] = (-M[2][1] * s[5] + M[2][2] * s[4] - M[2][3] * s[3]) * idet;

	T[1][0] = (-M[1][0] * c[5] + M[1][2] * c[2] - M[1][3] * c[1]) * idet;
	T[1][1] = (M[0][0] * c[5] - M[0][2] * c[2] + M[0][3] * c[1]) * idet;
	T[1][2] = (-M[3][0] * s[5] + M[3][2] * s[2] - M[3][3] * s[1]) * idet;
	T[1][3] = (M[2][0] * s[5] - M[2][2] * s[2] + M[2][3] * s[1]) * idet;

	T[2][0] = (M[1][0] * c[4] - M[1][1] * c[2] + M[1][3] * c[0]) * idet;
	T[2][1] = (-M[0][0] * c[4] + M[0][1] * c[2] - M[0][3] * c[0]) * idet;
	T[2][2] = (M[3][0] * s[4] - M[3][1] * s[2] + M[3][3] * s[0]) * idet;
	T[2][3] = (-M[2][0] * s[4] + M[2][1] * s[2] - M[2][3] * s[0]) * idet;

	T[3][0] = (-M[1][0] * c[3] + M[1][1] * c[1] - M[1][2] * c[0]) * idet;
	T[3][1] = (M[0][0] * c[3] - M[0][1] * c[1] + M[0][2] * c[0]) * idet;
	T[3][2] = (-M[3][0] * s[3] + M[3][1] * s[1] - M[3][2] * s[0]) * idet;
	T[3][3] = (M[2][0] * s[3] - M[2][1] * s[1] + M[2][2] * s[0]) * idet;
}

// Vulkan Projection Matrix for NDC-Tranformation
void getFrustum(mat4 M, float r, float t, float n, float f)
{
	M[0][0] = n / r; M[1][0] = 0.0f;   M[2][0] = 0.0f;         M[3][0] = 0.0f;
	M[0][1] = 0.0f;  M[1][1] = -n / t; M[2][1] = 0.0f;         M[3][1] = 0.0f;
	M[0][2] = 0.0f;  M[1][2] = 0.0f;   M[2][2] = -f / (f - n); M[3][2] = -f*n / (f - n);
	M[0][3] = 0.0f;  M[1][3] = 0.0f;   M[2][3] = -1.0f;        M[3][3] = 0.0f;
}

void createMeshGrid(float **mesh, unsigned int *size, unsigned int m, unsigned int n)
{
	unsigned int i, j, k;

	*size = 2 * m * n * sizeof(float);
	*mesh = malloc(*size);
	k = 0;

	for (i = 0; i<m; i++)
		for (j = 0; j<n; j++)
		{
			(*mesh)[k] = (float)j / ((float)n - 1);
			k++;
			(*mesh)[k] = (float)i / ((float)m - 1);
			k++;
		}
}

void createMeshGridIndices(unsigned short **indices, unsigned int *indicesLen, unsigned int *indicesSize, unsigned int m, unsigned int n)
{
	unsigned int i, j;

	*indicesLen = 3 * 2 * (m - 1)*(n - 1);
	*indicesSize = *indicesLen * sizeof(unsigned short);
	*indices = malloc(*indicesSize);

	for (i = 0; i<m - 1; i++)
		for (j = 0; j<n - 1; j++)
		{
			(*indices)[6 * (i*(n - 1) + j) + 0] = i*n + j;
			(*indices)[6 * (i*(n - 1) + j) + 1] = i*n + j + 1;
			(*indices)[6 * (i*(n - 1) + j) + 2] = (i + 1)*n + j;

			(*indices)[6 * (i*(n - 1) + j) + 3] = i*n + j + 1;
			(*indices)[6 * (i*(n - 1) + j) + 4] = (i + 1)*n + j;
			(*indices)[6 * (i*(n - 1) + j) + 5] = (i + 1)*n + j + 1;
		}
}