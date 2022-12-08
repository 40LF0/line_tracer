#include "msp.h"
#include "Clock.h" //48MHz
#include <stdint.h>
#include <stdio.h>

/**
 * main.c
 */

int IRinfo[8];

int milisec;
int sec;
int InitFlag;
int sensorVal;
void (*TimerA2Task)(void);
void task();

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

void systick_init(){
	//processor clock is 48MHz
	SysTick->LOAD = 0x00FFFFFF;
	SysTick->CTRL = 0x00000005;
}

void systick_wait1ms(){
	milisec++;
	SysTick->LOAD = 0xFFFF;
	SysTick->VAL = 0;
	while((SysTick->CTRL & 0x00010000) == 0) {}
	InitFlag=0;
}

void systick_wait1s(){
	int i;
	int count = 1000;
	for(i = 0; i < count; ++i){
		systick_wait1ms();
	}
	InitFlag=0;
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

void InitF(){
	Clock_Init48MHz();
	led_init();
	switch_init();
	systick_init();
	motor_init();
	IRsensor_init();
  TimerA2_Init(task, 10000);

	timer_A3_capture_init();
	InitFlag=1;
}

void delay_brightness(){
	int delay = 1;
	while(1){
		if(delay >= 10000)
			delay = 1;

		turn_on_led(0x02);
		Clock_Delay1ms(10000-delay);
		turn_off_led();
		Clock_Delay1ms(delay);

		delay+=100;
	}
	InitFlag=0;
}

void display_time(){
	int sw1; //switch declaration
	if (InitFlag == 1){
		systick_wait1ms();

		while(1){
			sw1 = P1->IN & 0x02;

			if(milisec>1000){
				sec++;
				milisec = 0;
			}
			//if(!sw1) -> switch pressed
			if(!sw1){
				printf("current time = %ds %dms\n" , sec, milisec);
			}
			systick_wait1ms();
		}
	}
	InitFlag=0;
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

void use_IRSensor(){

	int sensor;

	while(1){
		P5->OUT |= 0x08;
		P9->OUT |= 0x04;

		P7->DIR = 0xFF;
		P7->OUT = 0xFF;

		Clock_Delay1us(10);

		P7->DIR = 0x00;

		Clock_Delay1us(1000);

		sensor = P7->IN & 0x10;

		if(sensor){
			P2->OUT |= 0x01;
		}
		else
			P2->OUT &= ~0x07;

		P5->OUT &= ~0x08;
		P9->OUT &= ~0x04;

		printf("testing...\n");

		Clock_Delay1ms(10);
	}
}

void wait_constantIR(){
	int sensor;
	while(1){
		P5->OUT |= 0x08;
		P9->OUT |= 0x04;

		P7->DIR = 0xFF;
		P7->OUT = 0xFF;

		Clock_Delay1us(10);

		P7->DIR = 0x00;

		int i;
		for(i = 0; i < 10000; i++){
			sensor = P7->IN & 0x10;
			if(!sensor){
				printf("Timing Constant: %d\n", i);
				sensorVal = i;
				break;
			}
			Clock_Delay1us(1);
		}

		P5->OUT &= ~0x08;
	  P9->OUT &= ~0x04;

		Clock_Delay1ms(10);
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

void task(){
//MOTOR WORK DONE HERE
	//DC_Motor_Interface(, , ,);
	int k = 0;
	for(k = 0; k < 8; ++k){
		if(IRinfo[k] == 1){
			break;
		}
	}
	int j = 0; 
	for(j = k + 1; j < 8; ++j){
		if(IRinfo[j] == 0){
			break;
		}
	}
	if(j == 0){
		j = 8;	
	}
	if((j-k) >= 4){
		//need to stop
		exit(1);
	}
	else{
    int a = j- 4;
		int b = k - 3;
		DC_Motor_Interface(1, 500 + 150*a, 500 - 150*b);
	}
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

void testtestmove(){
	Clock_Init48MHz();
	systick_init();
	motor_init();
	IRsensor_init();

	int sensor;
	while(1){
		P5->OUT |= 0x08;
	  P9->OUT |= 0x04;
    P7->DIR = 0xFF;
	  P7->OUT = 0xFF;

	  Clock_Delay1us(10);

	  P7->DIR = 0x00;
    int i;
    for(i = 0; i < 10000; i++){
      sensor = P7->IN & 0x10;
	    if(!sensor){
	      printf("Timing Constant: %d\n", i);
	      break;
	    }
	    Clock_Delay1us(1);
	  } 

	  P5->OUT &= ~0x08;
	  P9->OUT &= ~0x04;
	  Clock_Delay1ms(10); 

		if(i > 700){
			DC_Motor_Interface(2, 0, 0);
			DC_Motor_Interface(4, 0, 0);
			DC_Motor_Interface(1, 500, 500);
			systick_wait1ms();
		}
		else{
			DC_Motor_Interface(3, 0, 0);
			DC_Motor_Interface(5, 0, 0);
			DC_Motor_Interface(1, 700, 700);
			systick_wait1ms();
		}
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

uint32_t get_left_rpm(){
	return 2000000 / period_left;
}

int is_IR_sensor_discharge(int n){
	if(n < 1 || n > 8){
		return -1;
	}
	return P7->IN & (1 << (n-1));
}

void IR_sensor_discharge_time_table(){
	int flag[8];
	int i = 0;
	int j = 0;
	while(i < 8){
		flag[i] = -1;
		++i;
	}

	flag[0] = 650;
	
	P5->OUT |= 0x08;
	P9->OUT |= 0x04;

	P7->DIR = 0xFF;

	P7->OUT = 0xFF;

	Clock_Delay1us(10);

	P7->DIR = 0x00;

	for(i = 0; i < 10000; ++i){
		for(j = 1; j < 8; ++j){
			if(flag[j] == -1){
				int sensor = is_IR_sensor_discharge(j);
				if(!sensor){
					flag[j] = i;
				}
			}
		}
	}
	for(i = 0; i < 7; ++i){
		if(flag[i] < 700){
			IRinfo[i] = 0; //white
		}
		else if(flag[i] < 1500){
			IRinfo[i] = 1; //black
		}
		else{
			IRinfo[i] = -1; //error
		}
	}
	for(i = 0; i < 8; i++){
		printf("%d " , flag[i]);
	}

	printf("\n");

	Clock_Delay1us(1);
	P5->OUT &= ~0x08;
	P9->OUT &= ~0x04;
}

void main(void)
{
	InitF();
	//initial speed
	DC_Motor_Interface(1, 1000, 1000);
	while(1){
		IR_sensor_discharge_time_table();
	}
}	
