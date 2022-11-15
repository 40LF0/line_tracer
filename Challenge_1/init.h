/*
 * init.h
 *
 *  Created on: 2022. 11. 14.
 *      Author: Chung_Seung_Yeon
 */

#ifndef INIT_H_
#define INIT_H_

#include "msp.h"


void (*TimerA2Task)(void);

void TimerA2_Init(void(*task)(void),uint16_t period){
    TimerA2Task = task;
    TIMER_A2->CTL = 0x0280;
    TIMER_A2->CCTL[0] = 0x0010;
    TIMER_A2->CCR[0] = (period - 1);
    TIMER_A2->EX0 = 0x0005;
    NVIC->IP[3] = (NVIC->IP[3]&0xFFFFFF00)|0x00000040;
    NVIC->ISER[0] = 0x00001000;
    TIMER_A2->CTL |=0x0014;

}











#endif /* INIT_H_ */
