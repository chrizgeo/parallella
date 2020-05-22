/*  Header file for helper function to find the execution time 
    the epiphany core */
#ifndef _E_TIME_H_
#define _E_TIME_H_

#define TIME_US(clock_ticks) (unsigned)(clock_ticks/600U)
/* function definitions */
void delay();
unsigned long long check_timer_count(unsigned long long);
void init_timer(unsigned long long*);
unsigned long long calc_time(unsigned long long*);
#endif