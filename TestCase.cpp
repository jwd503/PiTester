#include "TestCase.h"
#include <stdio.h>
TestCase::TestCase(int outputMask, int expectedResult, int ignoreMask, ErrorReporting* errorPointer):
	readResult{0}{
	this->ignoreMask = ignoreMask;
	this->outputMask = outputMask;
	this->expectedResult = expectedResult;
	e = errorPointer;
}


int TestCase::compareAll(){
	int i = 0;
	int result = 0;
	//Skip the first result as signal propagation errors show up here
	for(i = 8; i < NREADS; i++){
		int tempResult = compareOne(i);
		result |= tempResult;
	}
	int pin = outputMask & ~(1<<6);
	pin = pin & 0x7FFFFFC;//pins 2-26
	int pinCounter = 0;
	bool pinFound = false;
	for(int pinIndex = 0; pinIndex< 32; pinIndex++){
		if( (pin & (1<<pinIndex)) != 0){
			pinFound = true;
			break;
		}else{
			pinCounter++;
		}
	}

	if ((outputMask != 0) && ((outputMask & ~(1<<6)) == 0)){
		//pin 6
		pinFound = true;
		pinCounter = 6;
	}

	if(((result & 0x1) != 0) && pinFound) { //Short circuit
		int errorCode = e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pinCounter, ELECTRICAL_SHORT);
		e->setNextErrorCode(errorCode, 100.0);

	}

	if(((result & 0x2) != 0) && pinFound) { //Open circuit
		int errorCode = e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pinCounter, OPEN_CIRCUIT); //Change to OC code
		e->setNextErrorCode(errorCode, 100.0);

	}

	return result  != 0  ? 1: 0;
}

int TestCase::compareOne(int id){
	if(id >= NREADS){ //invalid id provided
		return 1;
	}
	int mask = ~(1<<6);

	//Result ok
	int maskedInput = readResult[id] & mask;
	int maskedOutput = expectedResult & mask;

	if ((maskedInput|ignoreMask) == (maskedOutput|ignoreMask)){
		return 0;
	} else{
		printf("observed: %d, expected: %d\n", (readResult[id] & ~(1 << 6)) ,(expectedResult  & ~(1 << 6)));
		if(readResult[id] == outputMask){
			return 2;//Open circuit
		}else{
			return 1;//Short circuit
		}
	}
}

int TestCase::getOutputMask() const{
	return outputMask;
}

int TestCase::getExpectedResult() const{
	return expectedResult;
}
