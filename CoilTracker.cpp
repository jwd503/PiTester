#include "CoilTracker.h"
#include "ErrorReporting.h"

#include <stdio.h>

CoilTracker::CoilTracker(int pin1, int pin2, const std::string name, int location, ErrorReporting* errorPointer, int size):
	window(size),
	coil(pin1, pin2, name, location, e),
	states(size){
	e = errorPointer;
	frequency = 0.0;
}

int CoilTracker::updateCoil(int gpioReading){

	int coilStateChanged = coil.setCoilState(gpioReading);
	int coilState = coil.getCoilState();

	if( coilStateChanged == 1){
		window.incrementIndex();
		window.setCurrentValue(0);
		states.incrementIndex();
		states.setCurrentValue(coilState);

	}else{
		window.incrementCurrentValue();

		//Int wraparound prevention takes 33mins to get here sampling at 10khz and no state change
		if(window.getCurrentValue() > 2000000000) {
			window.setCurrentValue(0);
		}
	}
	if (coil.inValidState() == 0){
		int errorCode = e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coil.location, COIL_TO_COIL);
		e->setNextErrorCode(errorCode, frequency);
	}
	return coilStateChanged;

}

int CoilTracker::getCurrentValue() const{
	return window.getCurrentValue();
}

int CoilTracker::getValue(int index) const{
	return window.getValue(index);
}

Coil* CoilTracker::getCoil(){
	return &coil;
}
