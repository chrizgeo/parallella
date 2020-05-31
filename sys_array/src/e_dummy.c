#include "e-lib.h"
#include "static_buffers.h"
#include "common_buffers.h"

void compute();
void init();

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

    /* Int pionters */
    Mailbox.pBase = (void *) e_emem_config.base;
    Mailbox.pIN = Mailbox.pBase + offsetof(shared_buf_t, IN[0]);
    Mailbox.pOUT = Mailbox.pBase + offsetof(shared_buf_t, OUT[0]);
    Mailbox.pCore = Mailbox.pBase + offsetof(shared_buf_t, core);

}

void compute()
{
    /*  I am not doing anything here since i am dummy
        tah tah tah tatataah tah tah tah tatataah
        'Chacko, find the dummy to dummy distance' */
    for(int i = 0; i < 10; i++);
}