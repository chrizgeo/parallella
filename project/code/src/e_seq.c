/*  Epiphany code for sequential execution
    This code runs the givens rotation method on 
    a single epiphany core. */

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <e_lib.h>
#include "e_time.h"
#include "common.h"
#include "givens_qr.h"

#define TIME_US(clock_ticks) (clock_ticks/600U)

volatile shm_t shm SECTION(".shared_dram");

unsigned int row, col, corenum, coreid;
unsigned long long  timer_count = 0;
//unsigned long long total_us;

int main(void)
{
    unsigned corenum = *(unsigned *) CORENUM_ADDR;
    unsigned active = *(unsigned *) ACTIVE_ADDR;
    unsigned *done = (unsigned *) DONE_ADDR;

    coreid = e_get_coreid();
    e_coords_from_coreid(coreid, &col, &row);
    shm.coreid[corenum] = coreid;
    shm.row[corenum] = row;
    shm.col[corenum] = col;
	shm.corenum[corenum] = corenum;
	shm.active[corenum] = active;

    if(!active) {
        /* We are not using this core, leave idle */
        __asm__ __volatile__("idle");
    }
    else {
        /* Do your thing here */
        init_timer(&timer_count);
        //givens_qr(&shm.A, &shm.R, &shm.Q);
        check_timer_count(&timer_count);
        shm.total_us[corenum] = calc_time(&timer_count);
        /* Raise the done flag */
		(*(done)) = 0x00000001;
        /* leave core idle after our operations are complete */
        __asm__ __volatile__("idle");
    }
}