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
//				printf("Coil: %s has fired %d times in a row, possible short on coil: %s\n", coilTracker[0].getCoil()->name.c_str(), lastFiredCount, coilTracker[1].getCoil()->name.c_str());
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
//				printf("Coil: %s has fired %d times in a row, possible short on coil: %s\n", coilTracker[1].getCoil()->name.c_str(), lastFiredCount, coilTracker[0].getCoil()->name.c_str());
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
	int a1Result = 0, a2Result = 0, b1Result = 0, b2Result = 0;
	int i = 0;
	for(i = 12; i < NREADS - 12; i++){
		a1Result += compareOne(coilA1Reading[i], coilTracker[0].getCoil(), pin1);
		a2Result += compareOne(coilA2Reading[i], coilTracker[0].getCoil(), pin2);
		b1Result += compareOne(coilB1Reading[i], coilTracker[1].getCoil(), pin3);
		b2Result += compareOne(coilB2Reading[i], coilTracker[1].getCoil(), pin4);
		counter++;
	}
//	printf("%d,%d,%d,%d\n",pin1,pin2,pin3,pin4);
	counter *= 2;
//	printf("counter: %d\n",counter);
	if((a1Result == counter) && (a2Result == counter) && (b1Result == counter) && (b2Result == counter)){
		// All of the coils were missing
		int coilLocation = coilTracker[0].getCoil()->location;
		int errorCode = e->generateErrorCode(STATIC_TEST, MOTOR_LEVEL, coilLocation /2, MISSING_COMPONENT);
		e->setNextErrorCode(errorCode, frequency);
		flag = 1;
	}else{

		if(a1Result != 0){
//			printf("a1 result: %d\n", a1Result);
			int errorCode = e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pin1, ELECTRICAL_SHORT);
			e->setNextErrorCode(errorCode, frequency);
			flag = 2;
		}

		if(a2Result != 0){
//			printf("a2 result: %d\n", a2Result);
			int errorCode = e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pin2, ELECTRICAL_SHORT);
			e->setNextErrorCode(errorCode, frequency);
			flag = 3;
		}

		if(b1Result != 0){
//			printf("b1 result: %d\n", b1Result);
			int errorCode = e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pin3, ELECTRICAL_SHORT);
			e->setNextErrorCode(errorCode, frequency);
			flag = 4;
			for(int index =0; index < 8; index++){
				printf("reading[%d]: %d\n", index, coilB1Reading[index]);
			}

		}

		if(b2Result != 0){
//			printf("b2 result: %d, pin4: %d\n", b2Result, pin4);
			int errorCode = e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pin4, ELECTRICAL_SHORT);
			e->setNextErrorCode(errorCode, frequency);
			flag = 5;
		}
	}
	return flag;
}

int Motor::compareOne(int value, Coil* testCoil, int pin){
	int* coilPins = testCoil->getPins();
	int expectedResults = (1 << coilPins[0]) | (1 << coilPins[1]);
//	printf("observed: %d, expected: %d\n", (readResult[id] & ~(1 << 6)) ,(expectedResult  & ~(1 << 6)));
	int mask = ~(1<<6);
	if ((value & mask) == (expectedResults  & mask)){
		return 0;
	} else{
		printf("observed: %d, expected: %d, pin: %d\n", (value & ~(1 << 6)) ,(expectedResults  & ~(1 << 6)), pin);

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
//				float sum = motorTiming[4].frequency + motorTiming[1].frequency + motorTiming[2].frequency + motorTiming[3].frequency;
//				float avg = sum/4.0;
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
