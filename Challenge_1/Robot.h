#ifndef ROBOT_H_
#define ROBOT_H_

#include "msp.h"
#include "IR_sensor.h"
#include "motor.h"
#include "switch.h"
#include "tachometer.h"
#include "led.h"
#include "interrupt.h"
#include "control.h"
#include "Clock.h"
#include <iostream>
#include <stdio.h>

class Robot{
private:
	void release();
public:
	Robot();
	~Robot();
	void Init();

};

#endif
