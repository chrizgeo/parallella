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
                shm->go[2] = 1;
                if(E_ERR == e_write(e_mem, 0, 0, (off_t)0, shm, sizeof(shm_t)))
		            printf("Can't write to memory!\n");

                while(!ready) {
                    e_read(dev, 0, 0, READY, &ready, sizeof(ready));
                }

                ready = 0;
                e_write(dev, 0, 0, READY, &ready, sizeof(ready));
                e_write(dev, 0, 1, READY, &ready, sizeof(ready));
                //print_matrix(R);
                matrix_copy_from_epiphany(dev, 0, 0, Cmat, R);
                // printf("Step %d \n", col);

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

/*  Find the QR with givens rotation method on the epiphany core
    Parameters are the input matrix and the Q and R matrices
    This method only computes the R matrix */
void e_givens_qr_two_core(void *dev, shm_t *shm, void *e_mem, matrix *input, matrix *R)
{
    double c = 0.0, s = 0.0;
    unsigned clr = 0;
    unsigned set = 0;
    unsigned ready1 = 0, ready2 = 0;
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
                matrix_copy_to_epiphany(dev, 0, 1, &G, Amat);
                matrix_copy_to_epiphany(dev, 0, 1, &r_temp, Bmat);
                matrix_copy_to_epiphany(dev, 0, 0, R, Cmat);
                e_write(dev, 0, 0, Rcomp, &row, sizeof(row));
                e_write(dev, 0, 0, Ccomp, &col, sizeof(col));
                e_write(dev, 0, 1, Rcomp, &row, sizeof(row));
                e_write(dev, 0, 1, Ccomp, &col, sizeof(col));
                shm->go[1] = 1;
                shm->go[2] = 1;
                if(E_ERR == e_write(e_mem, 0, 0, (off_t)0, shm, sizeof(shm_t)))
		            printf("Can't write to memory!\n");

                while(!(ready1&&ready2)) {
                    e_read(dev, 0, 0, READY, &ready1, sizeof(ready1));
                    e_read(dev, 0, 1, READY, &ready2, sizeof(ready2));
                    //printf("Epiphany core data not ready \n ");
                }

                ready1 = 0;
                e_write(dev, 0, 0, READY, &ready1, sizeof(ready1));
                e_write(dev, 0, 1, READY, &ready1, sizeof(ready1));

                //printf("%x \n", (off_t) Cmat + col*SZ1);
            }
            // printf("Column %d \n", col);
            matrix_copy_from_epiphany(dev, 0, 0, Cmat, R);
        }
    }
    
    /* Set the core idle */
    shm->active[1] = 0;
    shm->active[2] = 0;
    if(E_ERR == e_write(e_mem, 0, 0, (off_t)0, shm, sizeof(shm_t)))
		printf("Can't write to memory!\n");
    print_matrix(R);
    matrix_free(&r_temp);
    matrix_free(&G);
}

/*  Find the QR with givens rotation method on the epiphany core
    Parameters are the input matrix and the Q and R matrices
    This method only computes the R matrix */
void e_givens_qr_16_core(void *dev, shm_t *shm, void *e_mem, matrix *input, matrix *R)
{
    double c = 0.0, s = 0.0;
    unsigned clr = 0;
    unsigned set = 0;
    unsigned ready[16], all_ready;
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
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                    matrix_copy_to_epiphany(dev, i, j, &G, Amat);
                    matrix_copy_to_epiphany(dev, i, j, &r_temp, Bmat);
                    e_write(dev, i, j, Rcomp, &row, sizeof(row));
                    e_write(dev, i, j, Ccomp, &col, sizeof(col));
                    }
                }
                for(int i = 0; i < 16; i++) {
                    shm->go[i] = 1;
                }
                if(E_ERR == e_write(e_mem, 0, 0, (off_t)0, shm, sizeof(shm_t)))
		            printf("Can't write to memory!\n");

                while(all_ready != 16) {
                    for(int i = 0; i < 4; i++) {
                        for(int j = 0; j < 4; j++) {
                            e_read(dev, i, j, READY, &ready[i*4 + j], sizeof(unsigned));
                            all_ready += ready[i*4 + j];
                        }
                    }
                }
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                        e_write(dev, i, j, READY, &clr, sizeof(clr));
                    }
                }
            }
            // printf("Column %d \n", col);
            matrix_copy_from_epiphany(dev, 0, 0, Cmat, R);
        }
    }
    
    /* Set the core idle */
    for(int i = 0; i < 16; i++) {
        shm->active[i] = 0;
    }

    if(E_ERR == e_write(e_mem, 0, 0, (off_t)0, shm, sizeof(shm_t)))
		printf("Can't write to memory!\n");
    print_matrix(R);
    matrix_free(&r_temp);
    matrix_free(&G);
}