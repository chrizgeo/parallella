/*  The main program runs a set of programs 
    sequentially and output the time taken by each to 
    the console */
#include <stdio.h>
#include <stdlib.h>
#include "seq_main.h"

int main(int argc, char** argv)
{
    double seq_parallella;
    unsigned long long seq_epiphany;
    /* Sequential execution on the parallella core */
    seq_parallella = sequential_qr_parallella();
    printf("Sequential execution on parallella core \n");
    printf("%lf us \n", seq_parallella);

    /* Sequential execution on a single epiphany core */
    seq_epiphany = sequential_qr_epiphany();
    printf("Sequential execution on epiphany core \n");
    printf("%llu us \n", seq_epiphany);
    return 0;
}