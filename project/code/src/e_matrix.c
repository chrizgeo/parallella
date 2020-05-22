/* Helper file for matrix operations  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "e_matrix.h"
#include <math.h>

/* Allocate memory for matrix data  */
void matrix_init(volatile matrix *mat, unsigned rows, unsigned cols)
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
double* get_matrix_element(volatile matrix *mat, unsigned row, unsigned col)
{
    return (mat->data + (col * mat->rows + row));
}

/* Set a matrix value with indexes */
void set_matrix_element(volatile matrix *mat, unsigned row, unsigned col, double val)
{
    mat->data[(col * mat->rows) + row] = (double) val;
}

/* Free the allocated matrix */
void matrix_free(volatile matrix *mat)
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
void matrix_make_identity(volatile matrix *mat)
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
void matrix_make_transpose(volatile matrix *src_mat, volatile matrix *dest_mat)
{
    for(unsigned r =0; r < src_mat->rows; r++ )  {
        for(unsigned c = 0; c < src_mat->cols; c++)  {
            set_matrix_element(dest_mat, r, c, *(get_matrix_element(src_mat, c, r)));
        }
    }
}

/* Perform a givens multiplication with only the given rows and columns */
void matrix_multiply_row(volatile matrix *l, volatile matrix *r, volatile matrix *out, unsigned row, unsigned col)
{
    double val;
    val = 0.0;
    for(unsigned i =0; i < l->rows; i++) {
        val += (*get_matrix_element(l, row, i))*(*get_matrix_element(r, i, col));
        set_matrix_element(out, row, col, val);
    }    
}

/*  Copy one matrix to another
    The destination matrix should be allocated 
    prior to calling this function */
void matrix_copy(volatile matrix *src_mat, volatile matrix *dest_mat)
{
    memcpy(dest_mat->data, src_mat->data, src_mat->rows*src_mat->cols*sizeof(double));
}

// /*  Copy matrix data from an array   */
// void matrix_copy_from_array(volatile matrix *dest_mat, double *input_data)
// {
//     memcpy(dest_mat->data, input_data, dest_mat->rows*dest_mat->cols*sizeof(double));
// }

// /* Copy matrix data to array */
// void matrix_copy_to_array(volatile matrix *src_mat, double *output_data)
// {
//     memcpy(output_data, src_mat->data, src_mat->rows*src_mat->cols*sizeof(double));
// }

// /* Print a matrix data to screen */
// void print_matrix(volatile matrix *mat)
// {
//     printf("[ \n");
//     for(int i=0; i < mat->rows; i++) {
//         for(int j = 0; j < mat->cols; j++) {
//             printf("%lf, ", *(get_matrix_element(mat, i, j)));
//         }
//         printf("\n");
//     }
//     printf("] \n");
// }