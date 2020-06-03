#ifndef __STATIC_BUFFERS_H__
#define __STATIC_BUFFERS_H__

#include <e-lib.h>
#include "sys_array.h"

extern volatile double CSR[3];
extern volatile double X; 
extern          core_t me;
extern volatile shared_buf_ptr_t Mailbox;
extern volatile e_barrier_t barriers[NCORES];
extern volatile e_barrier_t* tgt_bars[NCORES];

#endif /* __STATIC_BUFFERS_H__ */