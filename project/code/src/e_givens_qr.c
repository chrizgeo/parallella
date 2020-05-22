#include <stdio.h>
#include <stdlib.h>
#include <e_lib.h>
#include <math.h>
#include "e_matrix.h"

/* Find the sine and cosine elements to do the transformation */
int find_parameters(double* x, double* y, double* c, double* s)
{
    if((*y) != 0.0) {
        double distance;
        distance = sqrt((*x)*(*x) + (*y)*(*y));
        *s = (*y) / distance;
        *c = (*x) / distance;
        return 0;
        }
    else {
        return 1;
    }
}

/*  Find the QR with givens rotation method
    Parameters are the input matrix and the Q and R matrices */
void givens_qr(volatile matrix *input, volatile matrix *R, volatile matrix *Q)
{
    double c = 0.0, s = 0.0;
    matrix r_temp, q_temp, G, G_trans;
    matrix_init(&q_temp, Q->rows, Q->cols);
    matrix_init(&r_temp, R->rows, R->cols);
    matrix_init(&G, input->rows, input->cols);
    matrix_init(&G_trans, input->cols, input->rows);
    matrix_make_identity(Q); 
    matrix_copy(input , R);
    for(unsigned col = 0; col < input->cols - 1; col++) {
        for(unsigned row = col + 1; row < input->rows; row++) {
            if(!find_parameters(get_matrix_element(R, col, col), get_matrix_element(R, row, col), &c, &s)) {
                matrix_make_identity(&G);
                set_matrix_element(&G, col, col, c);
                set_matrix_element(&G, col, row, s);
                set_matrix_element(&G, row, col, -s);
                set_matrix_element(&G, row, row, c);
                matrix_make_transpose(&G, &G_trans);

                matrix_copy(Q, &q_temp);
                matrix_copy(R, &r_temp);
                for(unsigned i= 0; i < Q->rows; i++) {
                    matrix_multiply_row(&q_temp, &G_trans, Q, row, i);
                    matrix_multiply_row(&q_temp, &G_trans, Q, col, i);
                    matrix_multiply_row(&G, &r_temp, R, i, row);
                    matrix_multiply_row(&G, &r_temp, R, i, col);

                }
            }
        }
    }
    matrix_free(&q_temp);
    matrix_free(&r_temp);
    matrix_free(&G);
    matrix_free(&G_trans);
}