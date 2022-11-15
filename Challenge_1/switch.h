/*
 * switch.h
 *
 *  Created on: 2022. 11. 14.
 *      Author: Chung_Seung_Yeon
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#include "msp.h"

class Switch_control {
    public:
    void static switch_control_init(){
        // Setup switch as GPIO
        P1->SEL0 &= ~0x12;
        P1->SEL1 &= ~0x12;

        // Setup switch as Input
        P1->DIR &= ~0x12;

        // Enable pul-up resistors
        P1->REN |= 0x12;
        // Now pull-up
        P1->OUT != 0x12;
    }
    bool static is_pressed(){
        if(P1->IN & 0x02 == 0){
            return true;
        }
        else{
            return false;
        }
    }

    Switch_control(){
        switch_init();
    }

};


#endif /* SWITCH_H_ */
