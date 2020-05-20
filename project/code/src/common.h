#ifndef _COMMON_H_
#define _COMMON_H_

#define CORES 16
#define ROW 4
#define COL 4

/* struct for the shared memory  */
typedef struct shm_st {
	unsigned int active[16];
	unsigned int corenum[16];
    unsigned int coreid[16];
    unsigned int row[16];
    unsigned int col[16];
	unsigned long long total_us;
} shm;

/* srtuct to for a complex number not required since we
	are only dealing with rea matrix  */
/* typedef struct complex_st {
	double r;
	double i;
} complex; */

#endif	/* _COMMON_H_ */
