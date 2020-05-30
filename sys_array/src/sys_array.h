/* 
    sys_array.h */
#ifndef __SYS_ARR_H__
#define __SYS_ARR_H__

#include <stdint.h>

#define NCORES 16
#define MATROW 4
#define MATCOL 4

typedef struct {
    uint32_t coreid;
    uint32_t corenum;
    uint32_t col;
    uint32_t row;
} __attribute__ ((packed)) core_t;

typedef struct {
    uint32_t ready;
    uint32_t go;
    uint32_t done;
    uint32_t timer0_ticks;
} __attribute__ ((packed)) mbox_t;

typedef struct {
    double IN[MATROW*MATCOL];
    double OUT[MATROW*MATCOL];
    mbox_t core;
} __attrbute__ ((packed)) shared_buf_t;

typedef struct {

    void* pBase;
    double* pIN;
    double* pOUT;
    mbox_t *pCore;

}  mboxPtr_t;

#endif  /* __SYS_ARR_H */ 