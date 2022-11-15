/*
 * interrupt.h
 *
 *  Created on: 2022. 11. 15.
 *      Author: Chung_Seung_Yeon
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "msp.h"

class Interrupt{
    public:
    static void TA2_0_IRQHandler(void){
        TIMER_A2->CCTL[0] &= ~0x0001;

        (*TimerA2Task)();
    }
    static void (*TimerA2Task)(void);

    static void TimerA2_Init(void(*task)(void),uint16_t period){
        TimerA2Task = task;
        TIMER_A2->CTL = 0x0280;
        TIMER_A2->CCTL[0] = 0x0010;
        TIMER_A2->CCR[0] = (period - 1);
        TIMER_A2->EX0 = 0x0005;
        NVIC->IP[3] = (NVIC->IP[3]&0xFFFFFF00)|0x00000040;
        NVIC->ISER[0] = 0x00001000;
        TIMER_A2->CTL |=0x0014;

    }

    Interrupt(){}
};



#endif /* INTERRUPT_H_ */
