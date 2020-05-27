#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "e-hal.h"
#include "common.h"

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
    Parameters are the input matrix and the Q and R matrices
    This method only computes the R matrix */
void givens_qr(matrix *input, matrix *R)
{
    double c = 0.0, s = 0.0;
    matrix r_temp, G;
    matrix_init(&r_temp, R->rows, R->cols);
    matrix_init(&G, input->rows, input->cols);
    matrix_copy(input , R);
    for(unsigned col = 0; col < input->cols - 1; col++) {
        for(unsigned row = col + 1; row < input->rows; row++) {
            if(!find_parameters(get_matrix_element(R, col, col), get_matrix_element(R, row, col), &c, &s)) {
                matrix_make_identity(&G);
                set_matrix_element(&G, col, col, c);
                set_matrix_element(&G, col, row, s);
                set_matrix_element(&G, row, col, -s);
                set_matrix_element(&G, row, row, c);

                matrix_copy(R, &r_temp);
                for(unsigned i= 0; i < R->rows; i++) {
                    //matrix_multiply_row(&G, &r_temp, R, i, row);
                    matrix_multiply_row(&G, &r_temp, R, i, col);

                }
            }
        }
    }
    matrix_free(&r_temp);
    matrix_free(&G);
}

/*  Find the QR with givens rotation method on the epiphany core
    Parameters are the input matrix and the Q and R matrices
    This method only computes the R matrix */
void e_givens_qr_single_core(void *dev, shm_t *shm, void *e_mem, matrix *input, matrix *R)
{
    double c = 0.0, s = 0.0;
    unsigned clr = 0;
    unsigned set = 0;
    unsigned ready = 0;
    unsigned go = 0;
    matrix r_temp, G;
    matrix_init(&r_temp, R->rows, R->cols);
    matrix_init(&G, input->rows, input->cols);
    matrix_copy(input , R);
    for(unsigned col = 0; col < input->cols - 1; col++) {
        for(unsigned row = col + 1; row < input->rows; row++) {
            if(!find_parameters(get_matrix_element(R, col, col), get_matrix_element(R, row, col), &c, &s)) {
                matrix_make_identity(&G);
                set_matrix_element(&G, col, col, c);
                set_matrix_element(&G, col, row, s);
                set_matrix_element(&G, row, col, -s);
                set_matrix_element(&G, row, row, c);

                matrix_copy(R, &r_temp);
                matrix_copy_to_epiphany(dev, 0, 0, &G, Amat);
                matrix_copy_to_epiphany(dev, 0, 0, &r_temp, Bmat);
                matrix_copy_to_epiphany(dev, 0, 0, R, Cmat);
                e_write(dev, 0, 0, Rcomp, &row, sizeof(row));
                e_write(dev, 0, 0, Ccomp, &col, sizeof(col));
                shm->go[1] = 1;
                if(E_ERR == e_write(e_mem, 0, 0, (off_t)0, shm, sizeof(shm_t)))
		            printf("Can't write to memory!\n");

                while(!ready) {
                    e_read(dev, 0, 0, READY, &ready, sizeof(ready));
                    //printf("Epiphany core data not ready \n ");
                }

                ready = 0;
                e_write(dev, 0, 0, READY, &ready, sizeof(ready));
                matrix_copy_from_epiphany(dev, 0, 0, Cmat, R);
                //printf("Step %d \n", col);
                //print_matrix(R);

            }
        }
    }
    /* Set the core idle */
    shm->active[1] = 0;
    if(E_ERR == e_write(e_mem, 0, 0, (off_t)0, shm, sizeof(shm_t)))
		printf("Can't write to memory!\n");
    print_matrix(R);
    matrix_free(&r_temp);
    matrix_free(&G);
}