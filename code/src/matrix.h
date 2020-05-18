#ifndef _MATRIX_H_
#define _MATRIX_H_

/* 	struct for matrix data.
	we store matrix as a big array with rows*column elements. */
typedef struct matrix_st {
	unsigned rows;
	unsigned cols;
	double* data;
} matrix;

/* Matrix methods  */
void matrix_init(matrix* mat, unsigned rows, unsigned cols);

/* Access a matrix element */
double* get_matrix_element(matrix *mat, unsigned row, unsigned col);
void set_matrix_element(matrix *mat, unsigned row, unsigned col, double val);
void matrix_free(matrix* mat);
void matrix_make_identity(matrix* mat);
void matrix_make_transpose(matrix* src_mat, matrix* dest_mat);
void matrix_multiply_row(matrix* l, matrix* r, matrix* out, unsigned row, unsigned col);
void matrix_copy(matrix* src_mat, matrix* dest_mat);
void matrix_copy_from_array(matrix* dest_mat, double* input_data);
void matrix_copy_to_array(matrix* src_mat, double* output_data);
void print_matrix(matrix* mat);

#endif  /* _MATRIX_H_ */