#ifndef MOTOR_H
#define MOTOR_H
#include <string>
#include "CoilTracker.h"
#include "ErrorReporting.h"
#include "TestCase.h"
class Motor{
	private:
		CoilTracker coilTracker[2];
		int lastFiredIndex;
		int lastFiredCount;
		int errorCount;
		int lastErrorFlag;
		int sameFireCount;
		ErrorReporting* e;
	public:
		Motor(int pin1, int pin2, std::string name1,int location1, int pin3, int pin4, std::string name2, int location2, ErrorReporting* errorPointer);
		CoilTracker* getCoilTracker(int index);
		int checkLastFired(int firedIndex);
		int updateLastFired(int firedIndex);
		int checkCoils(int gpioReading);
		int updateCoils(int gpioReading);
		int testMotor(int coilA1Reading[], int pin1, int coilA2Reading[], int pin2, int coilB1Reading[], int pin3, int coilB2Reading[], int pin4);
		int compareOne(int value, Coil* testCoil, int pin);
};

#endif
