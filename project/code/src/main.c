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
    unsigned long long par_epiphany_two_core;
    unsigned long long par_epiphany_16_core;
    /* Sequential execution on the parallella core */
    seq_parallella = sequential_qr_parallella();
    printf("Sequential execution on parallella core \n");
    printf("%lf us \n", seq_parallella);

    /* Sequential execution on a single epiphany core */
    seq_epiphany = sequential_qr_epiphany();
    printf("Sequential execution on epiphany core \n");
    printf("%llu us \n", seq_epiphany);

    /* Parallel execution on two epiphany cores */
    par_epiphany_two_core = parallel_qr_epiphany();
    printf("Parallel execution on two cores \n");
    printf("%llu us \n", par_epiphany_two_core);

    /* Parallel execution on 16 cores  */
    par_epiphany_16_core = parallel_16_qr_epiphany();
    printf("Parallel execution in 16 cores \n");
    printf("%llu us \n", par_epiphany_16_core);
    return 0;
}