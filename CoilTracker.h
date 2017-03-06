#ifndef COILTRACKER_H
#define COILTRACKER_H
#include "MovingWindow.h"
#include "Coil.h"
#include "ErrorReporting.h"

#include <string>

class CoilTracker{
	private:
		MovingWindow window;
		MovingWindow states;
		Coil coil;
		ErrorReporting* e;
	public:
		CoilTracker(int pin1, int pin2, std::string name,int location, ErrorReporting* errorPointer, int size);
		int getCurrentValue();
		int updateCoil(int gpioReading);
		Coil* getCoil();
};
#endif
