/*  The main program runs a set of programs 
    sequentially and output the time taken by each to 
    the console */
#include <stdio.h>
#include <stdlib.h>
#include "seq_main.h"

int main(int argc, char** argv)
{
    double sequential_parallella;
    sequential_parallella = sequential_qr_parallella();
    printf("Sequential execution on parallella core \t \n");
    printf("%lf us \n", sequential_parallella);
    return 0;
}