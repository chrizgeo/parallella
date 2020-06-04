#include "e-lib.h"
#include <math.h>
#include "static_buffers.h"
#include "common_buffers.h"

void compute();
void init();
void read_x(double* x_in);
void write_cs();

void delay()
{
	for(volatile int i = 0; i < 10000000; i++)
		for(volatile int j = 0; j < 100; j++)
			;
}

int main(int argc, char *argv[])
{

    /* Init core data and pointers */
    init();

    /* Init host sync signals */

    if(me.corenum == 0)
        Mailbox.pCore->ready = 1;
    
    /* Init barriers */
    e_barrier_init(barriers, tgt_bars);

    if(me.corenum == 0) {
        /*  Wait for go */
        while(!Mailbox.pCore->go) {};
    }

    /* Sync cores */
    e_barrier(barriers, tgt_bars);

    /* Perform the computation */
    compute();
    
    /* Sync cores We need all cores to finish the computations */
    e_barrier(barriers, tgt_bars);

    /* Computation done, notify host */

    if(me.corenum == 0) {
        Mailbox.pCore->done = 1;
    }

    return 0;
}

void init()
{
    /* Init core information */
    me.coreid = e_get_coreid();
    me.row = e_group_config.core_row;
    me.col = e_group_config.core_col;
    me.corenum = me.row * e_group_config.group_cols + me.col;
    e_neighbor_id(E_NEXT_CORE, E_ROW_WRAP, &me.rownext, &me.colnext);
    e_neighbor_id(E_PREV_CORE, E_ROW_WRAP, &me.rowprev, &me.colprev);
    e_neighbor_id(E_NEXT_CORE, E_COL_WRAP, &me.rowdown, &me.coldown);
    e_neighbor_id(E_PREV_CORE, E_COL_WRAP,  &me.rowup, &me.colup);
    
    /* Int pionters */
    Mailbox.pBase = (void *) e_emem_config.base;
    Mailbox.pIN = Mailbox.pBase + offsetof(shared_buf_t, IN[0]);
    Mailbox.pOUT = Mailbox.pBase + offsetof(shared_buf_t, OUT[0]);
    Mailbox.pCore = Mailbox.pBase + offsetof(shared_buf_t, core);

    me.incore = Mailbox.pCore->incore[me.corenum];
    me.outcore = Mailbox.pCore->outcore[me.corenum];
    me.CS = (void *) &CS[0];
    me.X = (void *) &X;
    me.outCS  = e_get_global_address(me.rownext, me.colnext, me.CS);
    me.outX = e_get_global_address(me.rowdown, me.coldown, me.X);
}

void compute()
{
    double c = 0.0, s = 0.0, r = 0.0, a, b, dist;
    double x_in = 0.0;
    int iterations = 0;
    iterations = Mailbox.pCore->iterations[me.corenum];
    for(int i = 0; i < MATROW + MATCOL; i++) {
        
        /* Sync cores */
        e_barrier(barriers, tgt_bars);
        if(Mailbox.pCore->active[me.corenum] == 1 && iterations < MATROW)
        {
            /* If this is a core which takes input from data, get the value from input matrix */
            if(!me.incore) {
                x_in = *(double *)me.X;
            }
            else {
                x_in = Mailbox.pIN[i*MATCOL + me.col];
            }
            
            if(x_in == 0) {
                c = 1.0;
                s = 0.0;
                ((double *)me.CS)[_C] = c;
                ((double *)me.CS)[_S] = s;
            }
            else {
                a = r*r;
                b = x_in*x_in;
                dist = sqrt(a+b);
                c = r/dist;
                s = x_in/dist;
                r = dist;
                ((double *)me.CS)[_C] = c;
                ((double *)me.CS)[_S] = s;
            }            
            iterations++;

        } 
        /* Sync cores */
        e_barrier(barriers, tgt_bars);
        if(Mailbox.pCore->active[me.corenum] == 1 && Mailbox.pCore->iterations[me.corenum] < MATROW)
        {
            if(!me.outcore) {
                write_cs();
            }
            if(iterations == 1)
                Mailbox.pCore->active[me.rowdown * e_group_config.group_cols + me.coldown] = 1;
        }       
    }
    Mailbox.pOUT[me.corenum] = r;
}

void write_cs()
{
    e_write(&e_group_config, me.CS, me.rownext, me.colnext, me.outCS, sizeof(CS));
}

