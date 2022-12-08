/*
 * main.cc
 *
 *  Created on: 2022. 11. 14.
 *      Author: Chung_Seung_Yeon
 */

/*#include "led.h"
=======
#include <iostream>
#include "led.h"
>>>>>>> ea7ee10d954743caad2967801526b51f6bad428b
#include "switch.h"
#include "Clock.h"
#include "IR_sensor.h"
#include "motor.h"
<<<<<<< HEAD
#include "interrupt.h"
#include "tachometer.h"
#include <iostream>*/

#include "Robot.h"

std::function<void()> task = [](){
    printf("interrupt occurs!\n");
};

std::function<void()> TimerA2Task;

void TimerA2_Init(std::function<void()> task,uint16_t period){
        TimerA2Task = task;
        TIMER_A2->CTL = 0x0280;
        TIMER_A2->CCTL[0] = 0x0010;
        TIMER_A2->CCR[0] = (period - 1);
        TIMER_A2->EX0 = 0x0005;
        NVIC->IP[3] = (NVIC->IP[3]&0xFFFFFF00)|0x00000040;
        NVIC->ISER[0] = 0x00001000;
        TIMER_A2->CTL |=0x0014;

}

void TA2_0_IRQHandler(void){
        TIMER_A2->CCTL[0] &= ~0x0001;

        TimerA2Task();
}

void main(void)
{
    // Clock Initialization
    Clock_Init48MHz();
    // we can use `Clock_Delay1ms(uint32_t n)`
    // or `Clock_Delay1us(uint32_t n)` to measure time


    // create LED controller
    LED LED_control;

    // create switch controller
    Switch_control switch_control;

    // create IR_sensor controller
    IR_sensor IR_sensor_control;

    // create Motor_controller
    Motor Moter_control;

    // interrupt_handler Initialization
    TimerA2_Init(task,50000);

    printf("Enter while loop\n");

    while(1){};
}




