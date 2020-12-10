#ifndef IO_H
#define IO_H
#include "definitions.h"

void keyboard(unsigned char key, int x, int y);
void print_help();
void special(int k,int x,int y);
void print_Matrix(double *m);
void matrix_inv(elm_matriz *a,elm_matriz *b);
void modo_analisis(double X[3],double Y[3],double Z[3],double At[3],int Tecla);

#endif // IO_H
