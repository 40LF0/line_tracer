/*
 * main.cc
 *
 *  Created on: 2022. 11. 14.
 *      Author: Chung_Seung_Yeon
 */

/*#include "led.h"
#include "switch.h"
#include "Clock.h"
#include "IR_sensor.h"
#include "motor.h"
#include "interrupt.h"
#include "tachometer.h"
#include <iostream>*/

#include "Robot.h"


void task(){
    printf("interrupt occurs!\n");
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

    // create interrupt_handler
    Interrupt Interrupt_handler;
    Interrupt_handler.TimerA2_Init(&task,50000);


    while(1){};
}




