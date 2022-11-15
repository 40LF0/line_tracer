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

    void static IR_senor_init(){
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
    int static is_IR_senor_discharge(int n){
        // return -1 if value n is not available
        // return 0 sensor is discharged
        // return 1 sensor is still charged

        if(n < 1 || n > 8){
            return -1;
        }
        return P7 -> IN & (1 << (n-1));


    }


    IR_sensor(){
        IR_senor_init();
    }

};




#endif /* IR_SENSOR_H_ */
