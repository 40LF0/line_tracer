/*
 * printf --> exists for testing
 */

#ifndef TACHOMETER_H_
#define TACHOMETER_H_

#include "msp.h"

class TACHOMETER{
private:
	uint16_t first_left;
	uint16_t first_right;
	uint16_t period_left;
	uint16_t period_right;

	void release();

public:
	TACHOMETER();
	~TACHOMETER()
	void Init();
  void HandlerInterrupt0();
	void HandlerInterruptN();
	void SetParam(int fl, int fr, int pl, int pr);
};

TACHOMETER::TACHOMETER():first_left(0), first_right(0),
												 period_left(0), period_right(0){
	Init();
	printf("TACHOMETER::TACHOMETER()\n");
}

TACHOMETER::~TACHOMETER(){
	release();
	printf("TACHOMETER::~TACHOMETER()\n");
}

void TACHOMETER::SetParam(int fl, int fr, int pl, int pr){
	first_left = fl;
	first_right = fr;
	period_left = pl;
	period_right = pr;
}

void TACHOMETER::HandlerInterrupt0(){
	TIMER_A3->CCTL[0] &= ~0x0001;
	period_right = TIMER_A3->CCR[0] - first_right;
	first_right = TIMER_A3->CCR[0];
}

void TACHOMETER::HandlerInterruptN(){
  TIMER_A3->CCTL[1] &= ~0x0001;
	period_left = TIMER_A3->CCR[1] - first_left;
	first_left = TIMER_A3->CCR[1];
}

void TACHOMETER::release(){
	printf("void TACHOMETER::release()\n");
}

void TACHOMETER::Init(){
	printf("void TACHOMETER::Init()\n");
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

#endif
