#ifndef MATRIX_H
#define MATRIX_H

#define PI 3.14159f

typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec4 mat4[4];

void printMatrix4(mat4 M, char *name);
void zero4(mat4 M);
void identity4(mat4 M);
void transpose4(mat4 M, mat4 N);
void dup4(mat4 dst, mat4 src);
void scale4(mat4 M, float sx, float sy, float sz);
void getTrans4(mat4 T, float x, float y, float z);
void getRotX4(mat4 M, float phi);
void getRotY4(mat4 M, float phi);
void getRotZ4(mat4 M, float phi);
void mult4(mat4 M, mat4 a, mat4 b);
void invert4(mat4 T, mat4 M);
void getFrustum(mat4 M, float r, float t, float n, float f);
void createMeshGrid(float **mesh, unsigned int *size, unsigned int m, unsigned int n);
void createMeshGridIndices(unsigned short **indices, unsigned int *indicesLen, unsigned int *indicesSize, unsigned int m, unsigned int n);

#endif // MATRIX_H
