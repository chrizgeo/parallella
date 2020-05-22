#ifndef _E_MATRIX_H_
#define _E_MATRIX_H_

/* 	struct for matrix data.
	we store matrix as a big array with rows*column elements. */
typedef struct matrix_st {
	unsigned rows;
	unsigned cols;
	double* data;
} matrix;

/* Matrix methods  */
void matrix_init(volatile matrix* mat, unsigned rows, unsigned cols);

/* Access a matrix element */
double* get_matrix_element(volatile matrix *mat, unsigned row, unsigned col);
void set_matrix_element(volatile matrix *mat, unsigned row, unsigned col, double val);
void matrix_free(volatile matrix* mat);
void matrix_make_identity(volatile matrix* mat);
void matrix_make_transpose(volatile matrix* src_mat, volatile matrix* dest_mat);
void matrix_multiply_row(volatile matrix* l, volatile matrix* r, volatile matrix* out, unsigned row, unsigned col);
void matrix_copy(volatile matrix* src_mat, volatile matrix* dest_mat);
//void matrix_copy_from_array(volatile matrix* dest_mat, double* input_data);
//void matrix_copy_to_array(volatile matrix* src_mat, double* output_data);
//void print_matrix(volatile matrix* mat);

#endif  /* _MATRIX_H_ */