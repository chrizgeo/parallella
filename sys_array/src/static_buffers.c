#include "sys_array.h"
#include "static_buffers.h"

            core_t me  SECTION("section_core");
volatile    shared_buf_ptr_t Mailbox SECTION("section_core");
volatile    e_barrier_t barriers[NCORES]  SECTION("section_core");
volatile    e_barrier_t *tgt_bars[NCORES] SECTION("section_core");