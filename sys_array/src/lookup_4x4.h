#include "sys_array.h"

unsigned rownext[NCORES] = [0, 0, 0, 99,
                            99, 1, 1, 99,
                            99, 99, 2, 99,
                            99, 99, 99, 99];

unsigned colnext[NCORES] = [1, 2, 3, 99,
                            99, 2, 3, 99,
                            99, 99, 3, 99,
                            99, 99, 99, 99];

unsigned rowprev[NCORES] = [99, 0, 0, 0,
                            99, 99, 1, 1,
                            99, 99, 99, 2,
                            99, 99, 99, 99];

unsigned colprev[NCORES] = [99, 0, 1, 2,
                            99, 99, 1, 1,
                            99, 99, 99, 2,
                            99, 99, 99, 99];

unsigned rowup[NCORES] =   [99, 99, 99, 99,
                            99, 0, 0, 0,
                            99, 99, 1, 1,
                            99, 99, 99, 2];

unsigned colup[NCORES] =   [99, 99, 99, 99,
                            99, 1, 2, 3,
                            99, 99, 2, 3,
                            99, 99, 99, 3];

unsigned rowdown[NCORES] = [99, 1, 1, 1,
                            99, 99, 2, 2,
                            99, 99, 99, 3,
                            99, 99, 99, 99];

unsigned coldown[NCORES] = [99, 1, 2, 3,
                            99, 99, 2, 3,
                            99, 99, 99, 3,
                            99, 99, 99, 99];
                        
void find_next_core(core_t* me);
void find_prev_core(core_t* me);
void find_up_core(core_t* me);
void find_down_core(core_t* me);