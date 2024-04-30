/*
 * timer.h
 *
 * Created: 2/20/2024 10:45:28
 *  Author: Student
 */ 


#ifndef TIMER_H_
#define TIMER_H_
#include <stdio.h>
//void Timer1_cmp_start (uint16_t porovnani);
void Timer1_cmp_start();
void Timer1Stop();
void Timer2_fastpwm_start(uint8_t strida);



#endif /* TIMER_H_ */