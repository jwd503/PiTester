#ifndef COIL_H
#define COIL_H
#include <string>
#include "ErrorReporting.h"

class Coil{
	private:
		int pins[2];
		int getOther(int pin);
		int coilState;
		int previousCoilState;
		ErrorReporting* e;
	public:
		std::string name;
		Coil(int pin1, int pin2, std::string name1, int location1, ErrorReporting* errorPointer);
		int* getPins();
		int getExpectedMask(int pin);
		int getCoilState();
		int getPreviousCoilState();
		int setCoilState(int gpioReading);
		int inValidState();
		int location;

};
#endif
