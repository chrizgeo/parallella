/* Helper file for matrix operations  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include <math.h>
#include "common.h"
#include "e-hal.h"

size_t SZ = INPUT_ROWS*INPUT_COLS*sizeof(double);
/* Allocate memory for matrix data  */
void matrix_init(matrix *mat, unsigned rows, unsigned cols)
{
    /* Allocate memory for data */
    if(NULL == (mat->data = malloc(rows * cols * sizeof(double)))){
        printf("Couldn't allocate memory for matrix \n");
        exit (1);
    }

    /* Assign rows and cols values */
    mat->rows = rows;
    mat->cols = cols;    
}

/* Get a matrix element from indexes */
double* get_matrix_element(matrix *mat, unsigned row, unsigned col)
{
    return (mat->data + (col * mat->rows + row));
}

/* Set a matrix value with indexes */
void set_matrix_element(matrix *mat, unsigned row, unsigned col, double val)
{
    mat->data[(col * mat->rows) + row] = (double) val;
}

/* Free the allocated matrix */
void matrix_free(matrix *mat)
{
    /* Free memory */
    free(mat->data);

    /* reset dimensions */
    mat->rows = 0;
    mat->cols = 0;

}

/*  make the given matrix an identity matrix
    no dimension check is provided.
    make sure the input is square matrix */
void matrix_make_identity(matrix *mat)
{
    for(unsigned r = 0; r < mat->rows; r++) {
        for(unsigned c = 0; c < mat->cols; c++) {
            if(c == r)
                set_matrix_element(mat, r, c,  (double) 1);
            else
                set_matrix_element(mat, r, c, (double) 0);
        }
    }
}

/*  Create transpose of a given matrix */
void matrix_make_transpose(matrix *src_mat, matrix *dest_mat)
{
    for(unsigned r =0; r < src_mat->rows; r++ )  {
        for(unsigned c = 0; c < src_mat->cols; c++)  {
            set_matrix_element(dest_mat, r, c, *(get_matrix_element(src_mat, c, r)));
        }
    }
}

/* Perform a givens multiplication with only the given rows and columns */
void matrix_multiply_row(matrix *l, matrix *r, matrix *out, unsigned row, unsigned col)
{
    double val;
    val = 0.0;
    for(unsigned i =0; i < l->rows; i++) {
        val += (*get_matrix_element(l, row, i))*(*get_matrix_element(r, i, col));
    }
    set_matrix_element(out, row, col, val);
}

/*  Copy one matrix to another
    The destination matrix should be allocated 
    prior to calling this function */
void matrix_copy(matrix *src_mat, matrix *dest_mat)
{
    memcpy(dest_mat->data, src_mat->data, src_mat->rows*src_mat->cols*sizeof(double));
}

/*  Copy matrix data from an array   */
void matrix_copy_from_array(matrix *dest_mat, double *input_data)
{
    memcpy(dest_mat->data, input_data, dest_mat->rows*dest_mat->cols*sizeof(double));
}

/* Copy matrix data to array */
void matrix_copy_to_array(matrix *src_mat, double *output_data)
{
    memcpy(output_data, src_mat->data, src_mat->rows*src_mat->cols*sizeof(double));
}

/* Print a matrix data to screen */
void print_matrix(matrix *mat)
{
    printf("[ \n");
    for(int i=0; i < mat->rows; i++) {
        for(int j = 0; j < mat->cols; j++) {
            printf("%lf, ", *(get_matrix_element(mat, i, j)));
        }
        printf("\n");
    }
    printf("] \n");
}


/* Methods to copy matrix data to epiphany core */
void matrix_copy_from_epiphany(void *dev, unsigned row, unsigned col,unsigned eMatrix, matrix *dest_mat)
{
    e_read(dev, row, col, (off_t) eMatrix, dest_mat->data, SZ);
}

void matrix_copy_to_epiphany(void *dev, unsigned row, unsigned col, matrix *src_mat, unsigned eMatrix)
{
    e_write(dev, row, col, (off_t) eMatrix, src_mat->data, SZ);
}