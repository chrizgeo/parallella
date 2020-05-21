/*  Epiphany code for sequential execution
    This code runs the givens rotation method on 
    a single epiphany core. */

#include <stdint.h>
#include <math.h>
#include <e_lib.h>
#include <e_time.h>
#include "common.h"

volatile shm_t shm SECTION(".shared_dram");

unsigned int row, col, corenum, coreid;
unsigned long long timer_count;
unsigned long long total_us;

int main(void)
{
    unsigned corenum = *(unsigned *) 0x5000;
    unsigned active = *(unsigned *) 0x6000;
    unsigned done = *(unsigned *) 0x7000;
    if(!active) {
        /* We are not using this core, leave idle */
        __asm__ __volatile__("idle");
    }
    else {
        /* Do your thing here */

        /* leave core idle after our operations are complete */
        __asm__ __volatile__("idle");
    }
}