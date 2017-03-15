#ifndef COILTRACKER_H
#define COILTRACKER_H
#include "MovingWindow.h"
#include "Coil.h"
#include "ErrorReporting.h"

#include <string>
#include <vector>

class CoilTracker{
	private:
		MovingWindow window;
		Coil coil;
		ErrorReporting* e;
	public:
		unsigned int pinCount[2];
		unsigned int storedPinCount[2];
		MovingWindow states;

		CoilTracker(int pin1, int pin2, std::string name,int location, ErrorReporting* errorPointer, int size);
		int getCurrentValue();
		int updateCoil(int gpioReading);
		Coil* getCoil();
		int getValue(int index);

};
#endif
