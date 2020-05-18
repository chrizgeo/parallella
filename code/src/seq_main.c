/* Program to find the QR decomposition in a 
    sequential method using the Givens rotation method */

/*  TODO:   Correct implementation of givens method
            Add computation of times
            Port to ARM on parallella
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "matrix.h"
/* #define INPUT_ROWS 2
#define INPUT_COLS 2
double input_data[4] = {1.0, 2.0, 3.0, 4.0}; */
#define INPUT_ROWS 16
#define INPUT_COLS 16
double input_data[256] = {
 5.124575, 7.898948, 7.946213, 3.592056, 6.081988, 7.166724, 7.788865, 9.798719, 4.258254, 1.054377, 7.652938, 3.552302, 2.828004, 1.762015, 2.638748, 8.069129,
 2.522771, 1.012173, 8.776333, 6.716550, 4.709401, 1.210326, 1.280568, 3.150382, 9.152067, 2.220713, 4.314055, 2.801475, 0.355802, 4.751442, 2.452192, 8.104572,
 8.535061, 1.283802, 0.308171, 1.881512, 6.372757, 4.749593, 5.185006, 8.183881, 0.145697, 9.572844, 4.968377, 2.484793, 3.972274, 0.722540, 4.236591, 8.985795,
 3.747190, 1.811495, 6.834885, 9.154308, 5.894849, 0.924535, 4.470197, 0.745557, 2.168096, 6.291060, 6.742811, 0.373962, 4.722142, 6.529717, 9.824009, 7.356618,
 6.461459, 9.131959, 7.072122, 0.483971, 1.670396, 8.949241, 4.018287, 3.546607, 8.672633, 0.835284, 1.438314, 5.034810, 6.390177, 3.856355, 2.282590, 2.083286,
 9.260374, 6.927009, 4.018142, 0.385422, 1.099953, 3.724873, 9.097155, 9.176734, 6.069993, 8.241838, 9.264992, 1.061532, 3.459950, 2.737167, 2.078934, 3.678259,
 6.428624, 0.599322, 6.768367, 8.416201, 2.400816, 6.837505, 0.179495, 4.902166, 7.376048, 9.049492, 7.510052, 1.206717, 5.326238, 2.406306, 4.195184, 9.811339,
 8.330168, 8.243491, 5.997973, 8.827311, 5.079985, 6.432596, 3.651846, 9.114349, 1.086232, 7.299483, 4.097865, 8.284714, 0.099244, 9.042177, 1.308067, 3.394185,
 6.530458, 2.984305, 6.574254, 5.616988, 9.094666, 5.588702, 5.533086, 7.406141, 4.161861, 0.238815, 9.648564, 6.822000, 0.370360, 4.103983, 8.835959, 5.350417,
 6.909482, 7.122869, 4.538856, 0.413352, 0.563682, 8.213211, 3.641620, 3.405092, 1.848284, 4.093572, 8.746218, 8.198015, 4.431138, 8.115096, 3.752868, 9.309067,
 4.522346, 4.953636, 6.954018, 4.096635, 1.076255, 2.290696, 4.178736, 1.015898, 5.956050, 5.301290, 3.462898, 9.802859, 9.976361, 8.454410, 2.433835, 3.138024,
 6.622229, 8.626863, 4.287552, 5.543465, 5.243287, 7.461806, 2.883427, 3.698015, 4.793037, 1.159368, 1.629081, 2.267589, 0.671444, 9.046292, 4.284649, 2.955473,
 4.077998, 8.902470, 5.945147, 3.556053, 8.246637, 4.732425, 0.047543, 5.104174, 6.272343, 6.639598, 9.528616, 1.437121, 6.459073, 0.140771, 9.660500, 8.103308,
 7.141063, 7.821795, 4.805562, 5.816616, 8.541925, 0.016668, 2.015325, 4.703988, 5.374593, 0.995246, 5.829943, 5.670019, 8.307987, 6.370590, 9.468715, 1.161778,
 7.457226, 0.099300, 7.410219, 0.187684, 1.784756, 1.066501, 8.859654, 6.758221, 9.879946, 3.690862, 4.245235, 3.524249, 2.964446, 4.373680, 2.311288, 2.500651,
 4.752189, 0.592439, 2.946092, 0.001511, 6.350062, 6.068312, 2.986027, 0.368879, 3.349494, 5.183024, 7.386629, 0.150104, 9.933439, 1.343407, 0.192133, 6.214067
};

double *output_data;

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
void givens_qr(matrix *input, matrix *R, matrix *Q)
{
    double c = 0.0, s = 0.0;
    matrix q_temp, r_temp, G, G_trans;
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
                    //rotate(&c, &s, get_matrix_element(R, row, i), get_matrix_element(R, col, i));
                    matrix_multiply_row(&q_temp, &G_trans, Q, row, i);
                    matrix_multiply_row(&q_temp, &G_trans, Q, col, i);
                    matrix_multiply_row(&G, &r_temp, R, i, row);
                    matrix_multiply_row(&G, &r_temp, R, i, col);

                }
            }
        }
    }
}

int main(int argc, char** argv)
{

    matrix input_matrix, R, Q;
    /* Init the matrices */
    matrix_init(&input_matrix, INPUT_ROWS, INPUT_COLS);
    matrix_init(&R, INPUT_ROWS, INPUT_COLS);
    matrix_init(&Q, INPUT_ROWS, INPUT_COLS);
    /* Copy data to the input matrix */
    matrix_copy_from_array(&input_matrix, input_data);
    printf("The input matrix is \n");
    print_matrix(&input_matrix);
    /* Find the QR decomposition */
    givens_qr(&input_matrix, &R, &Q);
    /* allocate space for ouput matrix */   
    output_data = malloc(INPUT_ROWS*INPUT_COLS*sizeof(double));
    /* copy and ouput data to array */
    matrix_copy_to_array(&R, output_data);
    printf("The  R matrix is \n");
    print_matrix(&R);
    /* free the memory */
    
    /* copy and ouput data to array */
    matrix_copy_to_array(&Q, output_data);
    printf("The  Q matrix is \n");
    print_matrix(&Q);
    /* free the memory */
    matrix_free(&input_matrix);
    matrix_free(&R);
    matrix_free(&Q);
    return 0;
}