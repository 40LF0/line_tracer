/*
 * motor.h
 *
 *  Created on: 2022. 11. 15.
 *      Author: Chung_Seung_Yeon
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "msp.h"
#include <stdio.h>

class Motor{
    public:
    static void pwm_init34(uint16_t period,uint16_t duty3,uint16_t duty4){

        TIMER_A0->CCR[0] = period;

        TIMER_A0->EX0 = 0x0000;

        TIMER_A0->CCTL[3] = 0x0040;
        TIMER_A0->CCR[3] = duty3;
        TIMER_A0->CCTL[4] = 0x0040;
        TIMER_A0->CCR[4] = duty4;

        TIMER_A0->CTL = 0x02F0;

        P2->DIR |= 0xC0;
        P2->SEL0 |= 0xC0;
        P2->SEL1 &= ~0xC0;
    }

    static void motor_control_init(void){
        P3->SEL0 &= ~0xC0;
        P3->SEL1 &= ~0xC0;
        P3->DIR |= 0xC0;
        P3->OUT &= ~0xC0;

        P5->SEL0 &= ~0x30;
        P5->SEL1 &= ~0x30;
        P5->DIR |= 0x30;
        P5->OUT &= ~0x30;

        P2->SEL0 &= ~0xC0;
        P2->SEL1 &= ~0xC0;
        P2->DIR |= 0xC0;
        P2->OUT &= ~0xC0;

        pwm_init34(7500,0,0);

    }

    static void move(uint16_t leftDuty,uint16_t rightDuty){
        P3->OUT |= 0xC0;
        TIMER_A0->CCR[3] = leftDuty;
        TIMER_A0->CCR[4] = rightDuty;
    }

    static void left_forward(){
        P5->OUT &= ~0x10;
    }
    static void left_backward(){
        P5->OUT |= 0x10;
    }
    static void right_forward(){
        P5->OUT &= ~0x20;
    }
    static void right_backward(){
        P5->OUT |= 0x20;
    }


    Motor(){
        motor_control_init();
    }

};


#endif /* MOTOR_H_ */
