#include "CoilTracker.h"
#include "ErrorReporting.h"

#include <stdio.h>

CoilTracker::CoilTracker(int pin1, int pin2, std::string name, int location, ErrorReporting* errorPointer, int size):
	window(size),
	states(size),
	coil(pin1, pin2, name, location, e){
	e = errorPointer;
	pinCount[0] = 0;
	pinCount[1] = 0;
	prevCoilState = 0;
	storedPinCount[0] = 0;
	storedPinCount[1] = 0;
}

int CoilTracker::updateCoil(int gpioReading){

	int coilStateChanged = coil.setCoilState(gpioReading);
	int coilState = coil.getCoilState();

	if( coilStateChanged == 1){
		window.incrementIndex();
		window.setCurrentValue(0);
		states.incrementIndex();
		states.setCurrentValue(coilState);
		if(0x1 & coilState && ((0x1 & prevCoilState) == 0)) {
			storedPinCount[0] = pinCount[0];
			pinCount[0] = 1;
		}
		if(0x2 & coilState && ((0x2 & prevCoilState) == 0)) {
			storedPinCount[1] = pinCount[1];
			pinCount[1] = 1;
		}
	}else{
		window.incrementCurrentValue();
		pinCount[0]++;
		pinCount[1]++;

		//Int wraparound prevention takes 33mins to get here sampling at 10khz and no state change
		if(window.getCurrentValue() > 2000000000) {
			window.setCurrentValue(0);
		}
	}
	prevCoilState = coilState;
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
