#ifndef IO_H
#define IO_H
#include "definitions.h"

void keyboard(unsigned char key, int x, int y);
void print_help();
void special(int k,int x,int y);
void print_Matrix(double *m);
void matrix_inv(elm_matriz *a,elm_matriz *b);

#endif // IO_H
