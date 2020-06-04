/* 
    sys_array.h */
#ifndef __SYS_ARR_H__
#define __SYS_ARR_H__

#include <stdint.h>

#define DEBUG 1

#define NCORES 16
#define ROW 4
#define COL 4 
#define MATROW 4
#define MATCOL 4
#define _C 0
#define _S 1


typedef struct {
    unsigned coreid;
    unsigned corenum;
    unsigned col;
    unsigned row;
    unsigned incore;
    unsigned outcore;
    unsigned colnext;
    unsigned rownext;
    unsigned colprev;
    unsigned rowprev;
    unsigned coldown;
    unsigned rowdown;
    unsigned colup;
    unsigned rowup;
    void* CS;
    void* X;
    void* outCS;
    void* outX;
} __attribute__ ((packed)) core_t;

typedef struct {
    unsigned ready;
    unsigned go;
    unsigned done;
    unsigned timer0_ticks;
    unsigned incore[NCORES];
    unsigned outcore[NCORES];
    unsigned iterations[NCORES];
    unsigned active[NCORES];
} __attribute__ ((packed)) mbox_t;

typedef struct {
    double IN[MATROW*MATCOL];
    double OUT[MATROW*MATCOL];
    mbox_t core;
} __attribute__ ((packed)) shared_buf_t;

typedef struct {

    void* pBase;
    double* pIN;
    double* pOUT;
    mbox_t *pCore;
}  shared_buf_ptr_t;

#endif  /* __SYS_ARR_H */ 