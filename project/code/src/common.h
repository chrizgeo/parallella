#ifndef _COMMON_H_
#define _COMMON_H_

#include "matrix.h"

#define CORES 16
#define ROW 4
#define COL 4
#define CORENUM_ADDR 0x5000
#define ACTIVE_ADDR 0x6000
#define DONE_ADDR 0x7000

/* struct for the shared memory  */
typedef struct shm {
	unsigned int active[16];
	unsigned int corenum[16];
    unsigned int coreid[16];
    unsigned int row[16];
    unsigned int col[16];
	unsigned long long total_us[16];
	matrix A, Q, R;
} shm_t;


#endif	/* _COMMON_H_ */
