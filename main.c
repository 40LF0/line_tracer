#include "msp.h"
#include "Clock.h" //48MHz
#include <stdint.h>
#include <stdio.h>

/**
 * main.c
 */

int IRinfo[8];
int prevSensor[8];

int milisec;
int sec;
int InitFlag;
int sensorVal;
int count = 0; //global counter to indicate motor stoppage
void (*TimerA2Task)(void);
void task();
void timer_A3_capture_init();

uint16_t first_left;
uint16_t first_right;

uint16_t period_left;
uint16_t period_right;

uint32_t left_count;

void led_init(){
  // set P2 as GPIO
  P2->SEL0 &= ~0x07; //P2 -> pointer that points to a memory
  P2->SEL1 &= ~0x07;

  //input or output
  //curr type = output
  P2->DIR |= 0x07;

  //turn off
  P2->OUT &= ~0x07;
  //printf("led_init()\n");
}

void turn_on_led(int color){
  P2->OUT &= ~0x07;
	P2->OUT |= color;
  //printf("turn_on_led()\n");
}

void turn_off_led(){
  P2->OUT &= ~0x07;
  //printf("turn_off_led()\n");
}

void switch_init(){
  //set up switch as the io
  P1->SEL0 &= ~0x12;
  P1->SEL1 &= ~0x12;

  //set up switch as input
  P1->DIR &= ~0x12;

  //enable pull-up registers
  P1-> REN |= 0x12;

  //now pull-up
  P1->OUT |= 0x12;

  //printf("switch_init()\n");
}

void pwm_init34(uint16_t period, uint16_t duty3, uint16_t duty4){
	TIMER_A0->CCR[0] = period;

	TIMER_A0->EX0 = 0x0000;

	TIMER_A0->CCTL[3] = 0x0040;
	TIMER_A0->CCR[3] = duty3;
	TIMER_A0->CCTL[4] = 0x0040;
	TIMER_A0->CCR[4] = duty4;

	//0x200 -> SMCLK
	//0b1100 0000 -> input divider / 8
	//0b0011 0000 -> up/down
	TIMER_A0->CTL = 0x02F0;

	P2->DIR |= 0xC0;
	P2->SEL0 |= 0xC0;
	P2->SEL1 &= ~0xC0;
}

void motor_init(void){
	P3->SEL0 &= ~0xC0;
	P3->SEL1 &= ~0xC0;
	P3->DIR |- 0xC0;
	P3->OUT &= ~0xC0;

	P5->SEL0 &= ~0x30;
	P5->SEL1 &= ~0x30;
	P5->DIR |= 0x30;
	P5->OUT &= ~0x30;

	P2->SEL0 &= ~0xC0;
	P2->SEL1 &= ~0xC0;
	P2->DIR |= 0xC0;
	P2->OUT &= ~0xC0;

	pwm_init34(7500, 0, 0);
}

void IRsensor_init(){
	P5->SEL0 &= ~0x08;
	P5->SEL1 &= ~0x08;
	P5->DIR |= 0x08;
	P5->OUT &= ~0x08;

	P9->SEL0 &= ~0x04;
	P9->SEL1 &= ~0x04;
	P9->DIR |= 0x04;
	P9->OUT &= ~0x04;

	P7->SEL0 &= ~0xFF;
	P7->SEL1 &= ~0xFF;
	P7->DIR &= ~0xFF;
}

void InitF(){
	Clock_Init48MHz();
	led_init();
	switch_init();
	motor_init();
	IRsensor_init();
    //TimerA2_Init(task, 10000);

	timer_A3_capture_init();
	InitFlag=1;
}


void motor_example(){
	//move forward
	P5->OUT &= ~0x30;
	P2->OUT |= 0xC0;
	P3->OUT |= 0xC0;
	Clock_Delay1ms(100);

	//stop
	P2->OUT &= ~0xC0;
	Clock_Delay1ms(1000);

	InitFlag=0;
}

void motor_speedcontrol(){
	int speed = 1000;
	while(1){
		//PWM High
		P5->OUT &= ~0x30;
		P2->OUT |= 0xC0;
		P3->OUT |= 0xC0;
		Clock_Delay1us(speed);

		// PWM Low
		P2->OUT &= ~0xC0;
		Clock_Delay1us(10000-speed);
	}
}



void DC_Motor_Interface(int flag, uint16_t leftDuty, uint16_t rightDuty){
	if(flag == 1){
		//move
		P3->OUT |= 0xC0;
		TIMER_A0->CCR[3] = leftDuty;
		TIMER_A0->CCR[4] = rightDuty;
	}

	if(flag == 2){
		//left_forward
		P5->OUT &= ~0x10;
	}

	if(flag == 3){
		//left_backward
		P5->OUT |= 0x10;
	}

	if(flag == 4){
		//right_forward
		P5->OUT &= ~0x20;
	}

	if(flag == 5){
		//right_backward
		P5->OUT |= 0x20;
	}
}

void TA2_0_IRQHandler(){
	TIMER_A2->CCTL[0] &= ~0x0001;
	(*TimerA2Task)();
}

int get_length(){
	int k = 0;
	for(k = 0; k < 8; k++){
		if(IRinfo[k] == 1){
			break;
		}
	}

	//Get last black index + 1 -> j
	int j = 0; 
	for(j = k + 1; j < 8; j++){
		if(IRinfo[j] == 0){
			break;
		}
	}

	//First, we have to figure out 
	//length of the black 
	int length = j-k;

	return length;
}

int get_prev(){
	int k = 0;
	for(k = 0; k < 8; k++){
		if(prevSensor[k] == 1){
			break;
		}
	}

	//Get last black index + 1 -> j
	int j = 0; 
	for(j = k + 1; j < 8; j++){
		if(prevSensor[j] == 0){
			break;
		}
	}

	//First, we have to figure out 
	//length of the black 
	int length = j-k;
	return length;
}

//prev length >= 4 && 0 or 7th sensor is black --> 
void no_road(){
	printf("noroad\n");
	int prev_length = get_prev();
	if(prev_length >= 4){
		if(prevSensor[7] == 1){
			rotate_to_left(90);
			//jikjin jjogem?
			Clock_Delay1ms(250);
			return;
		}
		else if(prevSensor[0] == 1){
			rotate_to_right(90);
			//jikjin??
			Clock_Delay1ms(250);
		}
	}
	else
		return;
}

void robot_task(){
	//MOTOR WORK DONE HERE
	//DC_Motor_Interface(, , ,);
	int sensor_count = 0;

	int i = 0;	

	for(i = 0; i < 8; i++){
		printf("%d " , IRinfo[i]);
	}
	printf("\n");

	for(i = 0; i < 8; i++){
		if(IRinfo[i] == 1)
			sensor_count++;
	}

	// Get first black index -> k
	int k = 0;
	for(k = 0; k < 8; k++){
		if(IRinfo[k] == 1){
			break;
		}
	}

	//Get last black index + 1 -> j
	int j = 0; 
	for(j = k + 1; j < 8; j++){
		if(IRinfo[j] == 0){
			break;
		}
	}

	//First, we have to figure out 
	//length of the black 
	int length = j-k;
	if(k == 8){
		// there is no black
		length = 0;
	}
	// in normal case, sensor #3,#4 is black
	int a = j- 5;
	int b = k - 3;

	int speed = 1000;
	int shift_delta = 175;
	//DC_Motor_Interface(1, speed + shift_delta * a, speed - shift_delta * b)
	
	
	switch(length){
		case 0:
			count = 0;
			no_road();
			break;

		case 1:
			count = 0;
			DC_Motor_Interface(1, speed + shift_delta * a, speed - shift_delta * b);
			break;

		case 2:
			count = 0;
			DC_Motor_Interface(1, speed + shift_delta * a, speed - shift_delta * b);
			break;

		case 3:
			count = 0;
			break;

		case 4:
			count = 0;
			break;

		case 5:
			count = 0;
			break;

		case 6:
			count++;
			if(count > 10000)
				DC_Motor_Interface(1, 0, 0);
			break;

		case 7:
			count++;
			if(count > 10000)
				DC_Motor_Interface(1, 0, 0);
			break;

		case 8:
			count++;
			if(count > 10000)
				DC_Motor_Interface(1, 0, 0);
			break;

		default:

	}

	/*
	if(length >=6){
		count += 1;
		if(count > 3){
			DC_Motor_Interface(1,0,0);
			while(1){
				printf("spinning\n");
			}
		}
	}
	else{
		count = 0;
		if(length == 0){
			return;
			// i will not change motor control..
			// no black is on..
			// this logic is for old road.
		}
		else if(length == 2){
			return;
		}
		else if(length == 4){
			
		}
		else if(length == 5){

		}
	}*/

}

void TimerA2_Init(void(*task)(void), uint16_t period){
	TimerA2Task = task;
	TIMER_A2->CTL = 0x0280;
	TIMER_A2->CCTL[0] = 0x0010;
	TIMER_A2->CCR[0] = (period - 1);
	TIMER_A2->EX0 = 0x0005;
	NVIC->IP[3] = (NVIC->IP[3]&0xFFFFFF00) | 0x00000040;
	NVIC->ISER[0] = 0x00001000;
	TIMER_A2->CTL |= 0x0014;
}

void rotate_to_left(int data){
	DC_Motor_Interface(1, 0, 0);
	left_count = 0;
	DC_Motor_Interface(3, 0, 0);
	DC_Motor_Interface(4, 0, 0);
	DC_Motor_Interface(1, 2000, 2000);
	while(left_count < data * 2){

	}
	DC_Motor_Interface(1, 0, 0);
	DC_Motor_Interface(2, 0, 0);
	DC_Motor_Interface(4, 0, 0);
	DC_Motor_Interface(1, 300, 300);

}
void rotate_to_right(int data){
	DC_Motor_Interface(1, 0, 0);
	left_count = 0;
	DC_Motor_Interface(2, 0, 0);
	DC_Motor_Interface(5, 0, 0);
	DC_Motor_Interface(1, 2000, 2000);
	while(left_count < data * 2){

	}
	DC_Motor_Interface(1, 0, 0);
	DC_Motor_Interface(2, 0, 0);
	DC_Motor_Interface(4, 0, 0);
	DC_Motor_Interface(1, 300, 300);

}

void robot(){
	int i = 0;
	int j = 0;
	int sensor = 0;
	int flag[8];

	while(i < 8){
		flag[i] = -1;
		i++;
	}

	while(1){
	        for(j = 0; j < 8; j++){
	            P5->OUT |= 0x08;
	            P9->OUT |= 0x04;
	            P7->DIR = 0xFF;
	            P7->OUT = 0xFF;

	            Clock_Delay1us(10);

	            P7->DIR = 0x00;

	            for(i = 0; i < 2000; i++){
	                sensor = P7->IN & (1<<(j));
	                if(!sensor){
	                    flag[j] = i;
	                    break;
	                }
	                Clock_Delay1us(1);
	            }

	            P5->OUT &= ~0x08;
	            P9->OUT &= ~0x04;
	            Clock_Delay1ms(10);

	        }

					for(i = 0; i < 8; i++){
						prevSensor[i] = IRinfo[i];
					}

	        for(i = 0; i < 8; ++i){
	            if(flag[i] < 700){
	                IRinfo[i] = 0; //white
	            }
	            else if(flag[i] < 1500){
	                IRinfo[i] = 1; //black
	            }
	            else{
	                IRinfo[i] = -1;
	            }
	        }
					robot_task();
	}
}

void timer_A3_capture_init(){
	P10->SEL0 |= 0x30;
	P10->SEL1 &= ~0x30;
	P10->DIR &= ~0x30;

	TIMER_A3->CTL &= ~0x0030;
	TIMER_A3->CTL = 0x0200;

	TIMER_A3->CCTL[0] = 0x4910;
	TIMER_A3->CCTL[1] = 0x4910;
	TIMER_A3->EX0 &= ~0x0007;

	NVIC->IP[3] = (NVIC->IP[3]&0x0000FFFF) | 0x40400000;
	NVIC->ISER[0] = 0x0000C000;
	TIMER_A3->CTL |= 0x0024;
}

void TA3_0_IRQHandler(){
	TIMER_A3->CCTL[0] &= ~0x0001;
	period_right = TIMER_A3->CCR[0] - first_right;
	first_right = TIMER_A3->CCR[0];
}

void TA3_N_IRQHandler(){
	TIMER_A3->CCTL[1] &= ~0x0001;
	//period_left = TIMER_A3->CCR[1] - first_left;
	//first_left = TIMER_A3->CCR[1];
	left_count++;
}


void main(void)
{
	InitF();
	//initial speed
	DC_Motor_Interface(1, 1500, 1500);
	robot();

}	
