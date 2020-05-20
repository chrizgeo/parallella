/* Helper to find the givens parameters and perform the rotation */
#ifndef _GIVENS_QR_H
#define _GIVENS_QR_H
/* function declarations */
int find_parameters(double* x, double* y, double* c, double* s);
void givens_qr(matrix* input, matrix* Q, matrix* R);
#endif