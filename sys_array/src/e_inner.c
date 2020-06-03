#include "e-lib.h"
#include "static_buffers.h"
#include "common_buffers.h"

void compute();
void init();
void write_csr();
void read_csr();
void write_x(double* x_out);
void read_x(double* x_in);

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

    /* Init barriers */
    e_barrier_init(barriers, tgt_bars);


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
    Mailbox.pOutcore = Mailbox.pBase + offsetof(shared_buf_t, outcore[0]);
    Mailbox.pIncore = Mailbox.pBase + offsetof(shared_buf_t, incore[0]);
    
    me.incore = Mailbox.pIncore[me.corenum];
    me.outcore = Mailbox.pOutcore[me.corenum];
    me.CSR = (void *) &CSR[0];
    me.X = (void *) &X;
    me.inCSR =  e_get_global_address(me.rowprev, me.colprev, me.CSR);
    me.outCSR  = e_get_global_address(me.rownext, me.colnext, me.CSR);
    me.inX = e_get_global_address(me.rowup, me.colup, me.X);
    me.outX = e_get_global_address(me.rowdown, me.coldown, me.X);
}


void compute()
{
    Mailbox.pOUT[me.corenum] = 0.0;
    double c = 0.0, s = 0.0, r = 0.0;
    double x_in = 0.0, x_out = 0.0;
    for(int i = 0; i < MATROW; i++) {
        
        /* Sync cores */
        e_barrier(barriers, tgt_bars);
        /* If this is a core which takes input from data, get the value from input matrix */
        if(!me.incore) {
            read_x(&x_in);
        }
        else {
            x_in = Mailbox.pIN[i*MATCOL + me.col];
        }
        read_csr();
        c = ((double *)me.CSR)[_C];
        s = ((double *)me.CSR)[_S];
        r = ((double *)me.CSR)[_R];

        x_out = c*x_in - s*r;
        r = s*x_in + c*r;
        
        if(!me.outcore) {
            write_csr();
        }
        write_x(&x_out);
        /* Sync cores */
        e_barrier(barriers, tgt_bars);
    }
    Mailbox.pOUT[me.corenum] = x_out;
}

void read_csr()
{
    e_read(&e_group_config, me.CSR, me.rowprev, me.colprev, me.inCSR, sizeof(CSR));
}

void write_csr()
{
    e_write(&e_group_config, me.CSR, me.rownext, me.colnext, me.outCSR, sizeof(CSR));
}

void read_x(double* x_in)
{
    e_read(&e_group_config, x_in, me.rowup, me.colup, me.inX, sizeof(X));
}

void write_x(double* x_out)
{
    e_write(&e_group_config, x_out, me.rowdown, me.coldown, me.outX, sizeof(X));
}