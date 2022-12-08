/*
 * led.h
 *
 *  Created on: 2022. 11. 14.
 *      Author: Chung_Seung_Yeon
 */


#ifndef LED_H_
#define LED_H_

#define LED_RED 1
#define LED_GREEN (LED_RED << 1)
#define LED_BLUE (LED_RED << 2)

#include "msp.h"

class LED {
    public:

    static void LED_init(){
         // Set P2 as GPIO
         P2->SEL0 &= ~0x07;
         P2->SEL1 &= ~0x07;

         // Input or Output
         // Current type is output
         P2->DIR |= 0x07;

         // Turn off LED
         P2->OUT &= ~0x07;
     }
    static void turn_on_led(){
         P2->OUT |= 0x07;
     }
    static void turn_off_led(){
         P2->OUT &= ~0x07;
     }
    static void turn_on_led_color(int color){
         P2->OUT &= ~0x07;
         P2->OUT |= color;
     }

    static void turn_on_RED(){
         turn_on_led_color(LED_RED);
     }
    static void turn_on_GREEN(){
         turn_on_led_color(LED_GREEN);
     }
    static void turn_on_BLUE(){
         turn_on_led_color(LED_BLUE);
     }

     LED(){
         LED_init();
     }
     ~LED(){
         LED_init();
     }
};


#endif /* LED_H_ */
