#ifndef COIL_H
#define COIL_H
#include <string>
#include "ErrorReporting.h"

class Coil{
	private:
		int pins[2];
		int getOther(int pin) const;
		int coilState;
		int previousCoilState;
		ErrorReporting* e;
	public:
		std::string name;
		Coil(int pin1, int pin2, std::string name, int location1, ErrorReporting* errorPointer);
		int* getPins();
		int getExpectedMask(int pin) const;
		int getCoilState() const;
		int getPreviousCoilState() const;
		int setCoilState(int gpioReading);
		int inValidState() const;
		int location;

};
#endif
