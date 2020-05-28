/*  Epiphany code for sequential execution
    This code runs the givens rotation method on 
    a single epiphany core. */

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <e_lib.h>
#include "common.h"
#include "givens_qr.h"
#include "e-lib.h"

#define TIME_US(clock_ticks) (clock_ticks/600U)

volatile shm_t shm SECTION(".shared_dram");

unsigned int row, col, corenum, coreid;
unsigned long long timer_count;
unsigned long long total_us;

void delay()
{
	for(volatile int i = 0; i < 1000000; i++)
		for(volatile int j = 0; j < 100; j++)
			;
}

void chk_timer_count()
{
    unsigned long long timer_clk;
    
    timer_clk = e_ctimer_get(E_CTIMER_0);
    
    if(timer_clk <= 0)
    {
        timer_count++;
        e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
        e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
    }
    
}

void init_timer()
{
    timer_count=0;
    e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
    e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
}

void calc_time()
{
    unsigned long long timer_clk;
    
    timer_clk = E_CTIMER_MAX - e_ctimer_get(E_CTIMER_0);
    
	total_us = ((timer_count*TIME_US(E_CTIMER_MAX)) + TIME_US(timer_clk));
}

void givens_multiply(double* A, double* B, double* C, unsigned row, unsigned col)
{
    double val = 0.0;
    for(unsigned j = 0; j < INPUT_ROWS; j++) {
        val += A[j*INPUT_ROWS + row] * B[col*INPUT_ROWS + j];
    }
    e_write(&e_group_config, &val, 0, 0, &(C[col*INPUT_ROWS + row]), sizeof(val));
}
int main(void)
{
    unsigned corenum = *(unsigned *) CORENUM_ADDR;
    unsigned row = *(unsigned *) Rcomp;
    unsigned col = *(unsigned *) Ccomp;
    unsigned *done, *ready;
    done = (unsigned *) DONE_ADDR;
    ready = (unsigned *) READY;
    double *A, *B, *C;
    A = (double *) Amat;
    B = (double *) Bmat;
    C = (double *) Cmat;
    coreid = e_get_coreid();
    e_coords_from_coreid(coreid, &col, &row);
    shm.coreid[corenum] = coreid;
	shm.corenum[corenum] = corenum;
    init_timer();
    while(shm.active[corenum]) {
        /* Do your thing here */
        while(!shm.go[corenum]);
        row = *(unsigned *) Rcomp;
        col = *(unsigned *) Ccomp;
        givens_multiply(A, B, C, corenum, col);
        shm.go[corenum] = 0;
        /* Raise ready flag */
        (*(ready)) = 0x00000001;                
        
    }
    chk_timer_count();
	calc_time();
    shm.total_us[corenum] = total_us;
    /* Raise the done flag */
    (*(done)) = 0x00000001;
    /* leave core idle after our operations are complete */
    __asm__ __volatile__("idle");
}