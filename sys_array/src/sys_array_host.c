/* Host application for the systolic array implimentation  */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>

#include "e-hal.h"
#include "e-loader.h"

#include "sys_array.h"
#include "common_buffers.h"

int main(int argc, char *argv[]);
int load_programs(e_epiphany_t *pEpiphany);
void mailbox_init(shared_buf_t* Mailbox);

shared_buf_t Mailbox;

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
    
    if(e_alloc(pDRAM, 0x00000000, msize)) {
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

    /* Set input and ouput core details */
    mailbox_init(&Mailbox);
    /* Write info to memory */
    e_write(pDRAM, 0, 0, 0, &Mailbox, sizeof(Mailbox));

    /*  Load program to epiphany */
    load_programs(pEpiphany);

    /*  TODO 
        Dynamic input array  */
        /* Print the ouput matrix */ 
    for(int i = 0; i < MATROW; i++) {
        for(int j = 0; j < MATCOL; j++) {
            Mailbox.IN[i*MATCOL + j] = i*MATCOL + j + 1;
            Mailbox.OUT[i*MATCOL + j] = -1;
        }
    }

#if DEBUG
    printf("Setting up the IN and OUT arrays START \n");
    /* Print input matrix */
    for(int i = 0; i < MATROW; i++) {
        printf("\n");
        for(int j = 0; j < MATCOL; j++) {
            printf("%f, ", Mailbox.IN[i*MATCOL + j]);
        }
    }
    printf("\n");
#endif /* DEBUG */

    /* Copy the input matrix to the epiphany system */
    addr = offsetof(shared_buf_t, IN[0]);
    sz = sizeof(Mailbox.IN);
    e_write(pDRAM, 0, 0, addr, (void *) Mailbox.IN, sz);

    /* Fill the ouput matrix with 1*/
    addr = offsetof(shared_buf_t, OUT[0]);
    sz = sizeof(Mailbox.OUT);
    e_write(pDRAM, 0, 0, addr, (void *) Mailbox.OUT, sz);

#if DEBUG
    printf("Setting up the IN and OUT arrays END \n");
    printf("Waiting for core ready\n");
    e_read(pDRAM, 0, 0, offsetof(shared_buf_t,  core.ready), &Mailbox.core.ready, sizeof(Mailbox.core.ready));
    //print("Memory base 0x%x \n", e_emem_config.base);
    printf("Current value in ready : %d  \n", Mailbox.core.ready);
#endif /* DEBUG */
    
    /* On your march */
    while(!Mailbox.core.ready) {
        e_read(pDRAM, 0, 0, offsetof(shared_buf_t,  core.ready), &Mailbox.core.ready, sizeof(Mailbox.core.ready));
    }

#if DEBUG
    printf("Signalling the epiphany cores \n");
#endif /* DEBUG */
    /* Get set */
    Mailbox.core.go = 1;

    /* GO!!!! */
    e_write(pDRAM, 0, 0, offsetof(shared_buf_t, core.go), &Mailbox.core.go, sizeof(Mailbox.core.go));

#if DEBUG
    e_read(pDRAM, 0, 0, offsetof(shared_buf_t,  core.go), &Mailbox.core.go, sizeof(Mailbox.core.go));
    printf("Current value in go : %d  \n", Mailbox.core.go);
#endif /* DEBUG */

    printf("Wait for output ready from the epiphany cores \n");
    /* Wait fingers crossed that the ouput is correct */
    while(!Mailbox.core.done) {
        e_read(pDRAM, 0, 0, offsetof(shared_buf_t, core.done), &Mailbox.core.done, sizeof(Mailbox.core.done));
    }

#if DEBUG
    printf("Reading the output from shared memory \n");
#endif /* DEBUG */
    /* Read the  output matrix */
    addr = offsetof(shared_buf_t, OUT[0]);
    sz = sizeof(Mailbox.OUT);
    e_read(pDRAM, 0, 0, addr, (void *) Mailbox.OUT, sz);

    /* Print the ouput matrix */ 
    for(int i = 0; i < MATROW; i++) {
        printf("\n");
        for(int j = 0; j < MATCOL; j++) {
            printf("%f, ", Mailbox.OUT[i*MATCOL + j]);
        }
    }
    printf(" \n");
    return 0;    
}

int load_programs(e_epiphany_t *pEpiphany)
{
    int res = E_OK;

#if DEBUG
    int cores = 0;
    printf("Loading code to epiphany cores  START\n");
#endif /* DEBUG */

    /* Load boundary code */
    res = e_load("e_boundary.elf", pEpiphany, 0, 0, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_boundary.elf", pEpiphany, 1, 1, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_boundary.elf", pEpiphany, 2, 2, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_boundary.elf", pEpiphany, 3, 3, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;

    /* Load inner cell codes  */
    res = e_load("e_inner.elf", pEpiphany, 0, 1, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_inner.elf", pEpiphany, 0, 2, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_inner.elf", pEpiphany, 0, 3, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_inner.elf", pEpiphany, 1, 2, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_inner.elf", pEpiphany, 1, 3, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_inner.elf", pEpiphany, 2, 3, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;

    /* Load the dummy code */
    res = e_load("e_dummy.elf", pEpiphany, 1, 0, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_dummy.elf", pEpiphany, 2, 0, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_dummy.elf", pEpiphany, 3, 0, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_dummy.elf", pEpiphany, 2, 1, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_dummy.elf", pEpiphany, 3, 1, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;
    res = e_load("e_dummy.elf", pEpiphany, 3, 2, E_FALSE);
    if (res == E_ERR) {
        fprintf(stderr, "Error loading Epiphany program.\n");
        exit(1);
    }
    cores++;

    e_start_group(pEpiphany);

#if DEBUG
    printf("%d cores loaded \n", cores);
    printf("Loading code to epiphany cores END \n");
#endif /* DEBUG */

}

void mailbox_init(shared_buf_t *Mailbox)
{
    unsigned row, col, corenum;
    for(row = 0; row < ROW; row++) {
        for(col = 0; col < COL; col++) {
            corenum = row*COL + col;
            if(row == 0)
                Mailbox->incore[corenum] = 1;
            else
                Mailbox->incore[corenum] = 0;
            if(col == COL-1)
                Mailbox->outcore[corenum] = 1;
            else
                Mailbox->outcore[corenum] = 0;
        }
    }
}