#include "Coil.h"
#include "ErrorReporting.h"

#include <stdio.h>
#include <string>
#include <cstring>

Coil::Coil(int pin1, int pin2, const std::string name, int location1, ErrorReporting* errorPointer):
	name(name){
	pins[0] = pin1;
	pins[1] = pin2;
	coilState = 0;
	previousCoilState = 0;
	e = errorPointer;
	location = location1;

}

int* Coil::getPins(){
	return pins;
}

int Coil::getOther(int pin) const{
	//Returns -1 when no match found, pin number otherwise
	int pinIndex = -1;

	//Check if this instance contains the pin provided
	if(pins[0] == pin){
		pinIndex = 1;
	}else if(pins[1] == pin){
		pinIndex = 0;
	}

	if (pinIndex >= 0){
		//Pin was found so return the pin number
		return pins[pinIndex];
	}
	//No match found
	return -1;
}

int Coil::getExpectedMask(int pin) const{
	//Returns 0 if this instance is not paired with the one provided, mask otherwise
	int mask = 0;

 	//Fetch the other pin with which this coil is paired
	int otherPin = getOther(pin);

	if (otherPin >= 0){
		//Pin was found so return the mask corresponding with that pin
		mask = 1 << otherPin;
	}

	return mask;
}

int Coil::getCoilState() const{
	return coilState;
}

int Coil::getPreviousCoilState() const{
	return previousCoilState;
}

int Coil::setCoilState(int gpioReading){
	//Returns 0 if the coilState stays the same 1 otherwise
	int result = 0;

	//Create masks for the bits we are in
	int pin1Mask = 1 << pins[0];
	int pin2Mask = 1 << pins[1];

	//Mask out the bits we are interested in
	int pin1Bit = gpioReading & pin1Mask;
	int pin2Bit = gpioReading & pin2Mask;

	//Store the previous coilState
	previousCoilState = coilState;

	//Calculate the new coilState
	coilState = 0;
	coilState |= pin1Bit >> pins[0];	//bit 0 in coilState
	coilState |= pin2Bit >> (pins[1]-1);	//bit 1 in coilState

	//Check if the coilState has changed
	result = (previousCoilState == coilState)? 0:1;
	return result;
}

int Coil::inValidState() const{
	switch(coilState){
		case 0:	//No pins set
		case 1: //pin 1 set
		case 2: //pin 2 set
			return 1;
		case 3: //both pins set
			//illegal state
			return 0;
	}
	return 0;
}
