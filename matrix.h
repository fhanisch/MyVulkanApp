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
void dup4(mat4 M, mat4 N);
void getTrans4(mat4 T, float x, float y, float z);
void getRotY4(mat4 M, float phi);
void getRotZ4(mat4 M, float phi);
void mult4(mat4 M, mat4 a, mat4 b);
void getFrustum(mat4 M, float r, float t, float n, float f);

#endif // MATRIX_H
