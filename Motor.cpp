#include "Motor.h"
#include <stdio.h>
#include "ErrorReporting.h"

#define FIRE_COUNT 4
#define WINDOW_SIZE 32
Motor::Motor(int pin1, int pin2, std::string name1, int location1, int pin3, int pin4, std::string name2,int location2, ErrorReporting* errorPointer) :
	coilTracker({{pin1, pin2, name1, location1, errorPointer, WINDOW_SIZE},{pin3, pin4, name2, location2, errorPointer, WINDOW_SIZE}}){
	lastFiredIndex = 0;
	lastFiredCount = 0;
	sameFireCount = 0;
	errorCount = 0;
	lastErrorFlag = 0;
	e = errorPointer;
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

int Motor::updateLastFired(int firedIndex){
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
			e->setNextErrorCode(e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coilTracker[0].getCoil()->location, COIL_TO_COIL));
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
			if(errorCount >= 4){
				errorFlag = 1;
				e->setNextErrorCode(e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coilTracker[1].getCoil()->location, COIL_SHORT_TO_SELF));
				printf("Coil: %s has fired %d times in a row, possible short on coil: %s\n", coilTracker[0].getCoil()->name.c_str(), lastFiredCount, coilTracker[1].getCoil()->name.c_str());
			}
		}else{
			//reset error flag
			lastErrorFlag = 0;
//			if(lastErrorFlag == 1){
//				errorCount = 0;
//			}
		}
		sameFireCount = 0;
	}else if(coilStateChanged[1] == 1){ //coil2 state changed
//		printf("Coil:%s states, state1: %d, state2:%d\n", coilTracker[1].getCoil()->name.c_str(), coilTracker[0].getCoil()->getCoilState(), coilTracker[1].getCoil()->getCoilState());

		updateLastFired(1);
//		checkLastFired(1);
		int coilStatus = checkLastFired(1);
		if(coilStatus != 0){
//			e->setNextErrorCode(e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coilTracker[0].getCoil()->location, COIL_SHORT_TO_SELF));
//			printf("Coil: %s has fired %d times in a row, possible short on coil: %s\n", coilTracker[1].getCoil()->name.c_str(), lastFiredCount, coilTracker[0].getCoil()->name.c_str());
//			lastFiredCount = 0;
			if(lastErrorFlag == 2){
				errorCount++;
			}else{
				lastErrorFlag = 2;
				errorCount = lastFiredCount;
			}
			if(errorCount >= 4 ){
				e->setNextErrorCode(e->generateErrorCode(DYNAMIC_TEST, COIL_LEVEL, coilTracker[0].getCoil()->location, COIL_SHORT_TO_SELF));
				printf("Coil: %s has fired %d times in a row, possible short on coil: %s\n", coilTracker[1].getCoil()->name.c_str(), lastFiredCount, coilTracker[0].getCoil()->name.c_str());
				errorFlag = 2;
			}
		}else{
			//reset error flag
			lastErrorFlag = 0;
//			if(lastErrorFlag == 2){
//				errorCount = 0;
//			}
		}

		//	errorCount = 0;
		//}
		sameFireCount = 0;

	}
	return errorFlag;
}

int Motor::testMotor(int coilA1Reading[], int pin1, int coilA2Reading[], int pin2, int coilB1Reading[], int pin3, int coilB2Reading[], int pin4){
	int flag = 0;
	int counter = 0;
	int a1Result = 0, a2Result = 0, b1Result = 0, b2Result = 0;
	int i = 0;
	for(i = 8; i < NREADS - 12; i++){
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
		e->setNextErrorCode(e->generateErrorCode(STATIC_TEST, MOTOR_LEVEL, coilLocation /2, MISSING_COMPONENT));
		flag = 1;
	}else{

		if(a1Result != 0){
//			printf("a1 result: %d\n", a1Result);
			e->setNextErrorCode(e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pin1, ELECTRICAL_SHORT));
			flag = 2;
		}

		if(a2Result != 0){
//			printf("a2 result: %d\n", a2Result);
			e->setNextErrorCode(e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pin2, ELECTRICAL_SHORT));
			flag = 3;
		}

		if(b1Result != 0){
//			printf("b1 result: %d\n", b1Result);
			e->setNextErrorCode(e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pin3, ELECTRICAL_SHORT));
			flag = 4;
		}

		if(b2Result != 0){
//			printf("b2 result: %d, pin4: %d\n", b2Result, pin4);
			e->setNextErrorCode(e->generateErrorCode(STATIC_TEST, PIN_LEVEL, pin4, ELECTRICAL_SHORT));
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
		if (value == (1 << pin)){
			//Potentially open circuit
			return 2;
		}else {
//			printf("observed: %d, expected: %d, pin: %d\n", (value & ~(1 << 6)) ,(expectedResults  & ~(1 << 6)), pin);
			return 1;
		}
	}

}
