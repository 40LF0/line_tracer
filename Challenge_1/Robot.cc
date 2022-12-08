#include "Robot.h"

Robot::Robot(){
	std::cout<<"Robot::Robot()\n";
}

Robot::~Robot(){
	release();
	std::cout<<"Robot::~Robot()\n";
}

void Robot::release(){
	std::cout<<"Robot::release()\n";
}

void Robot::Init(){
	std::cout<<"Robot::Init()\n";
	RSensor.IR_();
}
