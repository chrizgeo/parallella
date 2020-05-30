/* Host application for the systolic array implimentation  */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "e-hal.h"
#include "e-loader.h"

#include "sys_array.h"

shared_buffer_t Mailbox;

int main (int argc, char *argv[])
{
    e_epiphany_t Epiphany, *pEpiphany;
    e_mem_t DRAM, *pDRAM;
    unsigned int msize;
    unsigned int addr;
    size_t sz;
    
    pEpiphany = &Epiphany;
    pDRAM = &DRAM;
    msize = 0x00400000;

    e_set_host_verbosity(H_D0);
    e_init(NULL);
    e_reset_system();
    
    if(e_alloc(pDRAM, 0x01000000, msize)) {
        printf("\n ERROR Can't allocate Shared RAM");
        exit(1);
    }

    if(e_open(pEpiphany, 0, 0, 4, 4)) {
        printf("ERROR Can't connect to workgroup");
        exit(1);
    }

    /* Clear contents of the mailbox */
    memset(&Mailbox, 0, sizeof(Mailbox));
    e_write(pDRAM, 0, 0, 0, &Mailbox, sizeof(Mailbox));

    /*  TODO 
        Load program to epiphany */

    /*  TODO 
        Dynamic input array  */
    Mailbox.IN[] =  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    Mailbox.OUT[] = {1, ,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    
    /* Copy the input matrix to the epiphany system */
    addr = offsetof(shared_buf_t, IN[0]);
    sz = sizeof(Mailbox.IN);
    e_write(pDRAM, 0, 0, addr, (void *) Mailbox.IN, sz);

    /* Fill the ouput matrix with 1*/
    addr = offsetof(shared_buf_t, OUT[0]);
    sz = sizeof(Mailbox.OUT);
    e_write(pDRAM, 0, 0, addr, (void *) Mailbox.OUT, sz);

    /* On your march */
    while(!Mailbox.core.ready) {
        e_read(pDRAM, 0, 0, offsetof(shared_buf_t,  core_ready), &Mailbox.core.ready, sizeof(Mailbox.core.ready));
    }

    /* Get set */
    Mailbox.core.go = 1;
    
    /* GO!!!! */
    e_write(pDRAM, 0, 0, offsetof(shared_buf_t, core_go), &Mailbox.core.go, sizeof(Mailbox.core.go));

    /* Wait fingers crossed that the ouput is correct */
    while(!Mailbox.core.done) {
        e_read(pDRAM, 0, 0, offsetof(shared_buf_t, core.done), &Mailbox.core.done, sizeof(Mailbox.core.done));
    }

    /* Read the  output matrix */
    addr = offsetof(shared_buf_t, OUT[0]);
    sz = sizeof(Mailbox.OUT);
    e_read(pDRAM, 0, 0, addr, (void *) Mailbox.C, sz);

    /* Print the ouput matrix */ 
    for(int i = 0; i < MATROW; i++) {
        printf("\n");
        for(int j = 0; j < MATCOL; j++) {
            printf("%f, ", Mailbox.OUT[j*MATROW + i]);
        }
    }
    return 0;    
}