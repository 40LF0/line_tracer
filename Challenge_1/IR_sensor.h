/*
 * IR_sensor.h
 *
 *  Created on: 2022. 11. 14.
 *      Author: Chung_Seung_Yeon
 */

#ifndef IR_SENSOR_H_
#define IR_SENSOR_H_

#include "msp.h"

class IR_sensor {
    public:
    static void IR_senor_init(){
        P5->SEL0 &= ~0x08;
        P5->SEL1 &= ~0x08; // GPIO
        P5->DIR |= 0x08;   // OUTPUT
        P5->OUT &= ~0x08;  // turn off 4 even IR LEDS

        P9->SEL0 &= ~0x04;
        P9->SEL1 &= ~0x04; // GPIO
        P9->DIR |= 0x04;   // OUTPUT
        P9->OUT &= ~0x04;  // turn off 4 odd IR LEDS

        P7->SEL0 &= ~0xFF;
        P7->SEL1 &= ~0xFF; // GPIO
        P7->DIR &= ~0xFF;  // INPUT
    }
    static int is_IR_senor_discharge(int n){
        // return -1 if value n is not available
        // return 0 if sensor is discharged
        // return 1 if sensor is still charged

        // when surface is white, sensor will be discharged quickly
        // when surface is black, sensor will be discharged slowly

        if(n < 1 || n > 8){
            return -1;
        }
        return P7 -> IN & (1 << (n-1));


    }

    static int* IR_sensor_discharge_time_table(int time_us){

        int* flag = new int[8];
        for(int i = 0 ; i < 8 ; i++){
            flag[i] =-1;
        }

        // Turn on IR LEDs
        P5->OUT |= 0x08;
        P9->OUT |= 0x04;

        // Make P7.0-P7.7 as output
        P7->DIR =0xFF;
        // Charges a capacitor
        P7->OUT = 0xFF;
        // Wait for fully charged
        Clock_Delay1us(10);

        // Make P7.0-P7.7 as input
        P7->DIR =0x00;

        for(int i = 0 ; i < time_us ; i++){
            for(int j = 0 ; j < 8 ; j++){
                if(flag[j]== -1){
                    int sensor = is_IR_senor_discharge(j);
                    if(!sensor){
                        flag[j] = i;
                    }
                }
            }
            Clock_Delay1us(1);
        }
        // Turn off IR LEDs
        P5->OUT &= ~0x08;
        P9->OUT &= ~0x04;

        return flag;
        // at some point, we have to delete flag.
    }


    IR_sensor(){
        IR_senor_init();
    }

};




#endif /* IR_SENSOR_H_ */
