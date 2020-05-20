/*  Epiphany helper code for delay 
    and time operations */
#include <stdint.h>
#include <e_lib.h>

void delay()
{
    for(volatile int i = 0; i < 1000000; i++)
        for(volatile int j = 0; j < 100; j++)
            ;
}

void check_timer_count(unsigned long long* timer_count)
{
    unsigned long long timer_clk;
    timer_clk = e_ctimer_get(E_CTIMER_0);
    if(timer_clk <= 0)
    {
        (*)timer_count++;
        e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
        e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
    }
}

void init_timer(unsigned long long* timer_count)
{
    (*)timer_count = 0;
    e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
    e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
}

unsigned long long calc_time(unsigned long long* timer_count)
{
    unsigned long long timer_clk;
    timer_clk = E_CTIMER_MAX - e_ctimer_get(E_CTIMER_0);
    return TIME_US(((*)timer_count*E_CTIMER_MAX) + timer_clk);
}