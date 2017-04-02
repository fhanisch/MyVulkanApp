#ifndef MATRIX_H
#define MATRIX_H

typedef float vec4[4];
typedef vec4 mat4[4];

void printMatrix4(mat4 M, char *name);
void zero4(mat4 M);
void identity4(mat4 M);
void transpose4(mat4 M, mat4 N);

#endif // MATRIX_H
