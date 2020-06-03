#include "lookup_4x4.h"

void find_next_core(core_t* me)
{
    *(me->rownext) = rownext[*(me->corenum)];
    *(me->colnext) = colnext[*(me->corenum)];
}

void find_prev_core(core_t* me)
{
    *(me->rowprev) = rowprev[*(me->corenum)];
    *(me->colprev) = colprev[*(me->corenum)];

}

void find_down_core(core_t* me)
{
    *(me->rowdown) = rowdown[*(me->corenum)];
    *(me->coldown) = coldown[*(me->corenum)];
}

void find_up_core(core_t* me)
{
    *(me->rowup) = rowup[*(me->corenum)];
    *(me->colup) = colup[*(me->corenum)];
}