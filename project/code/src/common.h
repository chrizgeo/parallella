#ifndef _COMMON_H_
#define _COMMON_H_

#include "matrix.h"

#define CORES 16
#define ROW 4
#define COL 4
#define DONE_ADDR 0x7000
#define ACTIVE_ADDR 0x7010
#define CORENUM_ADDR 0x7020
#define GO 0x7030
#define READY 0x7040
#define Rcomp 0x7050
#define Ccomp 0x7060
#define Amat 0x5000
#define Bmat 0x3000
#define Cmat 0x1000
#define INPUT_ROWS 16
#define INPUT_COLS 16
/* struct for the shared memory  */
typedef struct shm {
	unsigned int active[16];
    unsigned int go[16];
    unsigned int ready[16];
	unsigned int corenum[16];
    unsigned int coreid[16];
    unsigned int row[16];
    unsigned int col[16];
	unsigned long long total_us[16];
} shm_t;


#endif	/* _COMMON_H_ */
