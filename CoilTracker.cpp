#include "CoilTracker.h"
#include "ErrorReporting.h"

#include <stdio.h>

CoilTracker::CoilTracker(int pin1, int pin2, std::string name, int location, ErrorReporting* errorPointer, int size):
	window(size),
	states(size),
	coil(pin1, pin2, name, location, e){
	e = errorPointer;
}

int CoilTracker::updateCoil(int gpioReading){

	int coilStateChanged = coil.setCoilState(gpioReading);

	if( coilStateChanged == 1){
		window.incrementIndex();
		window.setCurrentValue(0);
		states.incrementIndex();
		states.setCurrentValue(coil.getCoilState());
	}else{
		window.incrementCurrentValue();
	}

	if (coil.inValidState() == 0){
		e->setNextErrorCode(e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coil.location, COIL_TO_COIL));
//		printf("There is probably something wrong with coil: %s, state:%d\n",coil.name.c_str(), coil.getCoilState());
	}
	return coilStateChanged;

}

int CoilTracker::getCurrentValue(){
	return window.getCurrentValue();
}

Coil* CoilTracker::getCoil(){
	return &coil;
}
