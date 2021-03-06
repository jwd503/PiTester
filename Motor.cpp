#include "Motor.h"
#include <stdio.h>
#include "ErrorReporting.h"
#include <unistd.h>
#include <sys/time.h>

#define FIRE_COUNT 4
#define WINDOW_SIZE 32
constexpr Motor::StateTransition Motor::transitionTable[];

Motor::Motor(int pin1, int pin2, std::string name1, int location1, int pin3, int pin4, std::string name2,int location2, ErrorReporting* errorPointer) :
	coilTracker({{pin1, pin2, name1, location1, errorPointer, WINDOW_SIZE},{pin3, pin4, name2, location2, errorPointer, WINDOW_SIZE}}){
	lastFiredIndex = 0;
	lastFiredCount = 0;
	sameFireCount = 0;
	errorCount = 0;
	lastErrorFlag = 0;
	e = errorPointer;
	pinMeanCount[0] = 0;
	pinMeanCount[1] = 0;
	pinMeanCount[2] = 0;
	pinMeanCount[3] = 0;
	currentState = STATE_IDLE;
	name = name1 + name2;
	passedStates = 0;
	frequency = 0;
}

CoilTracker* Motor::getCoilTracker(int index){
	if((index > -1) && (index < 2)){
		return &coilTracker[index];
	}else{
		return 0;
	}
}

int Motor::checkLastFired(int firedIndex){
	if (lastFiredIndex == firedIndex){
		if (lastFiredCount <= FIRE_COUNT){
			//This coil has not fired too many times, all ok
			return 0;
		}else{
			//This coil has fired too many times in a row, problem with the other coil
			return 1;
		}
	}else{
		//Different coil has fired all ok
		return 0;
	}
}

void Motor::updateLastFired(int firedIndex){
	if (lastFiredIndex == firedIndex){
		lastFiredCount++;
	}else {
		lastFiredIndex = firedIndex;
		lastFiredCount = 1;
	}
}

int Motor::updateCoils(int gpioReading){

	int coilStateChanged[2] = {0};
	coilStateChanged[0] = coilTracker[0].updateCoil(gpioReading);
	coilStateChanged[1] = coilTracker[1].updateCoil(gpioReading);
	int errorFlag = 0;

	//Both states changed since last reading, given that we are sampling at ~10kHz probably something wrong
	if ((coilStateChanged[0] == coilStateChanged[1]) && (coilStateChanged[0] == 1)){
		int coilState[2] = {0};
		coilState[0] = coilTracker[0].getCoil()->getCoilState();
		coilState[1] = coilTracker[1].getCoil()->getCoilState();
		if(coilState[0] == coilState[1]){
			sameFireCount++;
		}else{
			updateLastFired(2);
		}
		if(sameFireCount > 3){
			int errorCode = e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coilTracker[0].getCoil()->location, COIL_TO_COIL);
			e->setNextErrorCode(errorCode, frequency);
			errorFlag = 4;
		}

	}else if(coilStateChanged[0] == 1){ //coil1 state changed

		updateLastFired(0);

		int coilStatus = checkLastFired(0);
		if(coilStatus != 0){

			if(lastErrorFlag == 1){
				errorCount++;
			}else{
				lastErrorFlag = 1;
				errorCount = lastFiredCount;
			}

			if (errorCount >= 2){
				errorFlag = 1;
				int errorCode = e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coilTracker[1].getCoil()->location, COIL_SHORT_TO_SELF);
				e->setNextErrorCode(errorCode, frequency);
			}

		}else{
			//reset error flag
			lastErrorFlag = 0;
		}
		sameFireCount = 0;
	}else if(coilStateChanged[1] == 1){ //coil2 state changed

		updateLastFired(1);
		int coilStatus = checkLastFired(1);
		if(coilStatus != 0){

			if(lastErrorFlag == 2){
				errorCount++;
			}else{
				lastErrorFlag = 2;
				errorCount = lastFiredCount;
			}

			if(errorCount >= 2 ){
				errorFlag = 2;
				int errorCode = e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coilTracker[0].getCoil()->location, COIL_SHORT_TO_SELF);
				e->setNextErrorCode(errorCode, frequency);
			}

		}else{
			//reset error flag
			lastErrorFlag = 0;
		}
		sameFireCount = 0;

	}
	return errorFlag;
}

int Motor::testMotor(int coilA1Reading[], int pin1, int coilA2Reading[], int pin2, int coilB1Reading[], int pin3, int coilB2Reading[], int pin4){
	int flag = 0;
	int counter = 0;

	int i = 0;
	int pins[] = {pin1, pin2, pin3, pin4};
	int pinResult[4] = {0};
	int pinTest[4] = {0};
	int pinErrorFlag[4] = {0};

	for(i = 12; i < NREADS - 12; i++){
		pinTest[0] = compareOne(coilA1Reading[i], coilTracker[0].getCoil(), pin1);
		pinTest[1] = compareOne(coilA2Reading[i], coilTracker[0].getCoil(), pin2);
		pinTest[2] = compareOne(coilB1Reading[i], coilTracker[1].getCoil(), pin3);
		pinTest[3] = compareOne(coilB2Reading[i], coilTracker[1].getCoil(), pin4);

		for(int pinIndex = 0; pinIndex < 4; pinIndex++){
			switch(pinTest[pinIndex]){
				case 0: //Result ok
					break;
				case 1: //Short circuit
					pinErrorFlag[pinIndex] |= 1;
					break;
				case 2: //Open circuit
					pinErrorFlag[pinIndex] |= 2;
					break;
			}
			pinResult[pinIndex] += pinTest[pinIndex];
		}
		counter++;
	}
	counter *= 2;

	if((pinResult[0] == counter) && (pinResult[1] == counter) && (pinResult[2] == counter) && (pinResult[3] == counter)){
		// All of the coils were missing
		int coilLocation = coilTracker[0].getCoil()->location;
		int errorCode = e->generateErrorCode(STATIC_TEST, MOTOR_LEVEL, coilLocation /2, MISSING_COMPONENT);
		e->setNextErrorCode(errorCode, frequency);
		flag = 1;
	}else{

		for(int pinIndex = 0; pinIndex < 4; pinIndex++){

			if((pinErrorFlag[pinIndex] & 1) > 0) { //Short circuit
				int errorCode = e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pins[pinIndex], ELECTRICAL_SHORT);
				e->setNextErrorCode(errorCode, frequency);
			}

			if((pinErrorFlag[pinIndex] & 2) > 0) { //Open circuit
				int errorCode = e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pins[pinIndex], OPEN_CIRCUIT); //Change to OC code
				e->setNextErrorCode(errorCode, frequency);
			}
			pinResult[pinIndex] += pinTest[pinIndex];

		}

	}
	return flag;
}

int Motor::compareOne(int value, Coil* testCoil, int pin){
	int* coilPins = testCoil->getPins();
	int expectedResults = (1 << coilPins[0]) | (1 << coilPins[1]);
	int mask = ~(1<<6);
	if ((value & mask) == (expectedResults  & mask)){
		return 0;
	} else{

		if (value == (1 << pin)){
			//Potentially open circuit
			return 2;
		}else {
			return 1;
		}
	}

}


void Motor::updateState(int gpioReading){
        int* coilP1 = coilTracker[0].getCoil()->getPins();
        int* coilP2 = coilTracker[1].getCoil()->getPins();
        int pins[] = {coilP1[0], coilP1[1], coilP2[0], coilP2[1]};

        //Calculate the state value which we can then use to check for a transition
        int stateValue = 0;
        for(int pinIndex = 0; pinIndex < 4; pinIndex++){
                int pinMask             = 1<<pins[pinIndex];
                int maskedValue         = pinMask & gpioReading;
                int normalisedValue     = maskedValue >> (pins[pinIndex] - pinIndex);
                stateValue |= normalisedValue;
        }

        for(int transitionIndex = 0; transitionIndex < 16; transitionIndex++){
		bool isCurrentState = this->transitionTable[transitionIndex].source == currentState;
		bool isThisInput = this->transitionTable[transitionIndex].input == stateValue;

		if(isCurrentState && isThisInput){
			int target =  this->transitionTable[transitionIndex].target;
			if(target != 0){//Moving to non IDLE state
				passedStates |= 1<<target;
				for(unsigned int timingIndex = 1; timingIndex < 5; timingIndex++){
					motorTiming[timingIndex].passedStates |= 1<<target;
				}
			}else{//Moving to IDLE state
				//reset the timers for the currentState
				motorTiming[currentState].frequency = 0;
				gettimeofday(&motorTiming[currentState].start, NULL);
				frequency = 0;
			}
			//Update currentState
	                currentState = this->transitionTable[transitionIndex].target;
			if(target != 0){
				//stop the timers and calculate a period/frequency
				gettimeofday(&motorTiming[currentState].stop, NULL);
				timersub(&motorTiming[currentState].stop, &motorTiming[currentState].start, &motorTiming[currentState].total);
				float time_taken = motorTiming[currentState].total.tv_sec + (motorTiming[currentState].total.tv_usec / 1000000.0);

				motorTiming[currentState].frequency = 1.0/ time_taken;
				float sum = motorTiming[1].frequency + motorTiming[2].frequency + motorTiming[3].frequency + motorTiming[4].frequency;
				float avg = sum/4.0;

				frequency = avg;
				coilTracker[0].frequency = avg;
				coilTracker[1].frequency = avg;

				//Begin the new timer
				gettimeofday(&motorTiming[currentState].start, NULL);

			}


                }

        }

}
