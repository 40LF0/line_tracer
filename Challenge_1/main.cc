/*
 * main.cc
 *
 *  Created on: 2022. 11. 14.
 *      Author: Chung_Seung_Yeon
 */

#include "led.h"
#include "switch.h"
#include "Clock.h"
#include "IR_sensor.h"
#include "motor.h"
#include <iostream>


void task(){
    printf("interrupt occurs!\n");
}

void TA2_0_IRQHandler(void){
    TIMER_A2->CCTL[0] &= ~0x0001;

    (*TimerA2Task)();
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

    //TimerA2_Init(&task,50000);

    while(1){};
}




