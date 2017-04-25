#include "PJ_RPI.h"
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "TestCase.h"
#include "Coil.h"
#include "Motor.h"
#include "MovingWindow.h"
#include "CoilTracker.h"
#include "Utility.h"
#include "ErrorReporting.h"
#include "LEDChar.h"
#include "LEDDriver.h"

#define ZEROES_SEEN 19
#define NREADS 32
void takeReadings(int nReads, int ReadResult[], int readmask);
int printReadings(int nReads, int ReadResult[]);
int* pinsToMask(int* outputPins, int* readPins, int* result);
std::vector<CoilTracker>* configureCoilTracker(ErrorReporting* errorPointer);
std::vector<Coil>* configureCoils(ErrorReporting* errorPointer);

std::vector<Motor>* configureMotors(ErrorReporting* errorPointer);
int retrieveExpectedMask(int pin, std::vector<Motor>* m);
int retrieveExpectedMaskCoils(int pin, std::vector<Coil>* c);
void setPullDown();
int showMenu();
int checkButtons();
int runDynamicTest(int readmask);
int runStaticTest();
int runOptoTest();
int main()
{
	//Map the virtual address to the physical pi registers
	if(map_peripheral(&gpio) == -1)
	{
       	 	printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
        	return -1;
    	}

	SetProgramPriority(10000);

	int readmask = generateGPIOReadMask();
	setPullDown();

	setAllPinsToInp();

	LEDDriver ledTest = LEDDriver();
	while(1){
		int menuOption = showMenu();
		float seconds;
		switch(menuOption){
			case 1:
				printf("Executing the static test\n");
				delayMicroseconds(100000);
				runStaticTest();
				break;
			case 2:
				printf("Executing the dynamic test\n");
				runDynamicTest(readmask);
				break;
			case 3:
				printf("Executing the opto test\n");
				delayMicroseconds(100000);
				runOptoTest();
				break;
			case 4:
				printf("Timing Test\n");
				struct timeval start;
        		        struct timeval stop;
	                	struct timeval total;
				gettimeofday(&start,NULL);
				gettimeofday(&stop,NULL);
				timersub(&stop, &start, &total);
				seconds = total.tv_usec / 1000000.0;
				printf("%f seconds passed\n", seconds);

				ledTest.driveDisplay("one");
				ledTest.driveDisplay("four");
				ledTest.driveDisplay("nine",1.0);
				ledTest.driveDisplay("test");
				ledTest.driveDisplay("dead");
				ledTest.driveDisplay("to ");

				ledTest.driveDisplay("    ");
				ledTest.driveDisplay(".   ");
				ledTest.driveDisplay("..  ");
				ledTest.driveDisplay("... ");
				ledTest.driveDisplay("....");
				ledTest.driveDisplay("... ");
				ledTest.driveDisplay("..  ");
				ledTest.driveDisplay(".   ");
				ledTest.driveDisplay("    ");

				break;
			default:
				printf("No action attatched to that button\n");
		}

	}

	return 0;

}

int runDynamicTest(int readmask){
	int sample[10000] = {0};
	int sampleIndex = 0;
	int dynamicReads = 1; //NREADS
	ErrorReporting e = ErrorReporting(sample, &sampleIndex);
	std::vector<Motor> *motors = configureMotors(&e);
	std::vector<Motor> &motor = *motors;
	//unsigned int pinTime[32] = {0};
	int exitCondition = 0;
	OUT_GPIO(6);
	printf("%d motors to check\n", motor.size());
	while(1)
	{
		int readResult[1] = {0};
		delayMicroseconds(100);

		takeReadings(dynamicReads, readResult, readmask);

		sample[sampleIndex] = bitwiseAverageArray(readResult, dynamicReads);

		int errorFlag = 0;
		for(unsigned int motorIndex = 0; motorIndex < motor.size(); motorIndex++){
			motor[motorIndex].updateState(sample[sampleIndex]);
			errorFlag |= motor[motorIndex].updateCoils(sample[sampleIndex]);
		}

		//if(errorFlag > 0){
		//	flashLED(100000, 0);
		//}
		int errorIndex = 0;
		for(errorIndex = 0; errorIndex < MAX_ERROR_CODES; errorIndex++){
			int errorCode = e.getErrorCode(errorIndex);
			if (errorCode != 0){
				if(e.errorVec[errorIndex] != 0){

					if((e.errorVec[errorIndex]->frequency > 30)){
						printf("freq: %f\n", ((e.errorVec[errorIndex])->frequency));
						//std::cout << e.errorVec[errorIndex]->frequency;
						printf("sampleIndex: %d\n", sampleIndex);
						//e.printGpioHistory(errorIndex);//index is auto incremented
						printf(e.generateErrorMessage(errorCode).c_str());
						printf("\nerrorCode: %x\n",errorCode);
						flashLED(100000, 0);
					}
					e.setErrorCode(errorIndex, 0);
					delete e.errorVec[errorIndex];
					e.errorVec[errorIndex] = 0;
				}

			}
		}
		sampleIndex++;
		if(sampleIndex > 10000) sampleIndex = 0;

		int exitRead = GPIO_READMULT(0xFFFFFFC);
		exitRead &= (1<<9)|(1<<18)|(1<<23)|(1<<26);
		if(exitRead  != 0){
			exitCondition = flashLED(0,1000);//checkButtons();
			if(exitCondition != 0){
				INP_GPIO(6);
				return exitCondition;
			}
		}

	}
	INP_GPIO(6);
	return 0;
}

int runOptoTest(){
	int sample[10000] = {0};
	int sampleIndex = 0;

	ErrorReporting e = ErrorReporting(sample, &sampleIndex);

	int exitCondition = 0;
	int pioPins[] = {15,24,22,11};

	int readmask = (1<<pioPins[0]) | (1<<pioPins[1]) | (1<<pioPins[2]) | (1<<pioPins[3]);
	printf("Readmask: %d\n", readmask);
	int lastReading = 0;
	int pioValues[4] = {0};
	int prevPioValues[4] = {0};
	int pullUpMask = (1<< 22) | (1<<11);
	setPullUp(pullUpMask);
	while(1)
	{
		delayMicroseconds(100);

		int readResult = GPIO_READMULT(readmask);

		//single out the pioPins
		int pioChanged = 0;
		for(int pioIndex = 0; pioIndex < 4; pioIndex++){
			int pioMask = (1<<pioPins[pioIndex]);
			pioValues[pioIndex] = ((readResult & pioMask) >> pioPins[pioIndex]);
			prevPioValues[pioIndex] = ((lastReading & pioMask) >> pioPins[pioIndex]);

			pioValues[pioIndex] = pioValues[pioIndex] > 0 ? 0: 1;
			prevPioValues[pioIndex] = prevPioValues[pioIndex] > 0 ? 0: 1;

			if(pioValues[pioIndex] != prevPioValues[pioIndex]){
				pioChanged = 1;
                        }

		}

		if (pioChanged ==1){
			printf("PIO values: %d%d%d%d\n", pioValues[0], pioValues[1], pioValues[2], pioValues[3]);
		}

		lastReading = readResult;

		//Check if any buttons have been pressed
		int exitRead = GPIO_READMULT(0xFFFFFFC);
		exitRead &= (1<<9)|(1<<18)|(1<<23)|(1<<26);
		if(exitRead  != 0){
			exitCondition = flashLED(0,1000);//checkButtons();
			if(exitCondition != 0){
				setPullDown();
				return exitCondition;
			}
		}

	}
	INP_GPIO(6);
	return 0;
}

int runStaticTest(){
	int sample[10000] = {0};
        int sampleIndex = 0;

	std::vector<TestCase> t;
	t.reserve(7);

	ErrorReporting e = ErrorReporting(sample, &sampleIndex);
	std::vector<Motor> *motors = configureMotors(&e);
	std::vector<Motor> &motor = *motors;

	int resulting[2];
	int* resultargs = resulting;
	int outputpins[32] = {6, 21};
	int readpins[32] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26, 27};
	int readmask = generateGPIOReadMask();
	int groundPins[] = {20, 9, 11, 15, 22, 24, 23};

	for (unsigned int pinIndex= 0; pinIndex < 7; pinIndex++){
		outputpins[1] = groundPins[pinIndex];
		resultargs  = pinsToMask(outputpins, readpins, resultargs);
		int outputmask = resultargs[0];
		int expectedMask = 1<<20 | 1<<9 | 1<<11 | 1<<15 | 1<<22 | 1<<24;//0;//retrieveExpectedMask(pinIndex,motors);
//		if((linkConnection == 1) || (groundPins[pinIndex] == 23)){
//			expectedMask = 1<<23;
//		}
		printf("expected: %d, %d\n",expectedMask,groundPins[pinIndex]);
		t.push_back(TestCase(outputmask, expectedMask));
		printf("Expected result for test %d: %d\n", groundPins[pinIndex], t[pinIndex].getExpectedResult());
	}

	setAllPinsToInp();
	int capPin = 1 << 6;
	int a0ReadResult[NREADS] = {0};
	int a1ReadResult[NREADS] = {0};
	int b0ReadResult[NREADS] = {0};
	int b1ReadResult[NREADS] = {0};
	int* readArray[4] = {a0ReadResult, a1ReadResult, b0ReadResult, b1ReadResult};

	int motorFlag = 0;
	int exitCondition = 0;
	printf("Got to end of init\n");

	while(1){

		volatile int READ_OK = 0;

		GPIO_CLR = 0x7FFFFFC;
		int retryCount = 0;

		//Test Motors
		for (unsigned int motorIndex = 0; motorIndex < motor.size(); motorIndex++){
			int checkNext = 0;
			int coilPins[4];
			int* tempCoilPins = motor[motorIndex].getCoilTracker(0)->getCoil()->getPins();
//			printf("%d, %d\n",tempCoilPins[0], tempCoilPins[1]);
			coilPins[0] = tempCoilPins[0];
			coilPins[1] = tempCoilPins[1];

			int* tempCoilPins2 = motor[motorIndex].getCoilTracker(1)->getCoil()->getPins();

			coilPins[2] = tempCoilPins2[0];
			coilPins[3] = tempCoilPins2[1];

			int pinIndex = 0;
			for(pinIndex = 0; (checkNext == 0) && (pinIndex < 4); pinIndex++){

				delayMicroseconds(50);

				setOutputs(capPin | 1 << coilPins[pinIndex]);
				int* arr = readArray[pinIndex];
				GPIO_SET = capPin | 1 << coilPins[pinIndex];
				takeReadings(NREADS,arr,readmask);
				GPIO_CLR = capPin | 1 << coilPins[pinIndex];
				INP_GPIO(coilPins[pinIndex]);
				READ_OK = printReadings(NREADS, readArray[pinIndex]);


				if (READ_OK != 1){
                 	        	if(retryCount < 2){
                        			pinIndex--;
						retryCount++;
                                	}else{
	                                	checkNext = 1; //Skip the other coils/pins as we can't check that they are there
					}
                        	}else{
	                        	retryCount = 0;
//					printf("Read ok\n");
				}

			}
			if(checkNext == 0){
//				printf("Checking\n");
				motorFlag += motor[motorIndex].testMotor(readArray[0], coilPins[0], readArray[1], coilPins[1], readArray[2], coilPins[2], readArray[3], coilPins[3]);
        	               	if (motorFlag != 0){
				//	int exitCode = flashLED(100000, 0);
					motorFlag = 0;
				}

			}

		}

		int errorIndex = 0;
		int errorsPresent = 0;
		int errorcount = 0;
		for(errorIndex = 0; errorIndex < MAX_ERROR_CODES; errorIndex++){
	        	int errorCode = e.getErrorCode(errorIndex);
       		        if (errorCode != 0){
               			printf(e.generateErrorMessage(errorCode).c_str());
	                        printf("\nerrorCode: %x\n",errorCode);
        	               	e.setErrorCode(errorIndex, 0);
				if(((errorCode & 0xF) != MISSING_COMPONENT) && (errorsPresent == 0)){
					delayMicroseconds(100);
					LEDDriver ledTest = LEDDriver();
					std::string s = "";
					for(int i = 0; i < 4; i++){
						int mask = 0xf << (i*4);
						int value = (errorCode & mask) >> (i*4);
						switch(value){
							case 0:
								s = "0" + s;
								break;
							case 1:
								s = "1" + s;
								break;
							case 2:
								s = "2" + s;
								break;
							case 3:
								s = "3" + s;
								break;
							case 4:
								s = "4" + s;
								break;
							case 5:
								s = "5" + s;
								break;
							case 6:
								s = "6" + s;
								break;
							case 7:
								s = "7" + s;
								break;
							case 8:
								s = "8" + s;
								break;
							case 9:
								s = "9" + s;
								break;
							case 10:
								s = "A" + s;
								break;
							case 11:
								s = "B" + s;
								break;
							case 12:
								s = "C" + s;
								break;
							case 13:
								s = "D" + s;
								break;
							case 14:
								s = "E" + s;
								break;
							case 15:
								s = "F" + s;
								break;


						}
					}
					ledTest.driveDisplay(s, 0.5);
					flashLED(100000, 0);
					errorsPresent = 1;
				}
				errorcount++;
                     	}
                }
		if(errorcount != 0){
			printf("ErrorCodeCount:%d\n", errorcount);
		}

		int exitRead = GPIO_READMULT(0xFFFFFFC);
                exitRead &= (1<<9)|(1<<18)|(1<<23)|(1<<26);
                if(exitRead  != 0){
                        exitCondition = flashLED(0,1000);//checkButtons();
                        if(exitCondition != 0){
                                return exitCondition;
                        }
                }

//		for (int a = 0; a < 7; a ++){
//			delayMicroseconds(100);
//
//			setOutputs(t[a].getOutputMask() );
//			GPIO_SET = t[a].getOutputMask() ;
//
//			takeReadings(NREADS, t[a].readResult, readmask);
//			GPIO_CLR = t[a].getOutputMask();
//			INP_GPIO(a);
//
//			READ_OK = printReadings(NREADS, t[a].readResult);
//
//			if (READ_OK != 1){
//				if(retryCount < 2){
//					a--;
//					retryCount++;
//				}
//				continue;
//			}
//			retryCount = 0;
//			READ_OK = t[a].compareAll();
//			exitCondition = flashLED(0,1000);
//			if(exitCondition != 0){
//				return exitCondition;
//			}
//
//			if(READ_OK != 1){
//				printf("Failed outputmask: %d\n",t[a].getOutputMask());
//				exitCondition = flashLED(100000,1000);
//				if(exitCondition != 0){
//					return exitCondition;
//				}
//				//GPIO_SET = 1<<6;
//				//delayMicroseconds(100000);
//				//GPIO_CLR = 1<<6;
//			}
//		}
//
	}
	return 0;
}

void takeReadings(int nReads, int ReadResult[], int readmask){

	int readcount = 0;
	for(readcount = 0; readcount < nReads; readcount++){

		ReadResult[readcount] = GPIO_READMULT(readmask);
	}
}

int printReadings(int nReads, int ReadResult[]){
	volatile int pinmask = (1 << 6);
	int readcount = 0;
	int capacitorBitCount = 0;
	for(readcount = 0; readcount < nReads; readcount++){
		if(ReadResult[readcount]!= 0){
		}
		capacitorBitCount += ((pinmask & ReadResult[readcount]) > 0) ? 0 : 1;
	}
	return (capacitorBitCount > ZEROES_SEEN) ? 1 : 0;
}

int*  pinsToMask(int* outputPins, int* readPins, int* result)
{
	int i = 0;
        int outputmask = 0;
        int readmask = 0;
        for (i = 0; i < 32; i++) {
		outputmask |= outputPins[i] == 0 ? 0 : 1 << outputPins[i];
                readmask |= readPins[i] == 0? 0: 1 << readPins[i];
        }
	result[0] = outputmask;
	result[1] = readmask;
	return result;
}

std::vector<Coil>* configureCoils(ErrorReporting* errorPointer){
	static std::vector<Coil> c;
	c.erase(c.begin(), c.end());

	c.reserve(8);
	c.push_back(Coil(2,3,"M2.A",M2A,errorPointer));//
        c.push_back(Coil(4,12,"M0.A",M0A,errorPointer));//
        c.push_back(Coil(14,16,"M0.B",M0B,errorPointer));//
        c.push_back(Coil(17,27,"M2.B",M2B,errorPointer));//
        c.push_back(Coil(10,5,"M3.A",M3A,errorPointer));//
        c.push_back(Coil(19,13,"M3.B",M3B,errorPointer));//
        c.push_back(Coil(21,25,"M1.A",M1A,errorPointer));//
        c.push_back(Coil(7,8,"M1.B",M1B,errorPointer));//
	return &c;

}

std::vector<CoilTracker>* configureCoilTracker(ErrorReporting* errorPointer){
	static std::vector<CoilTracker> mP;
	mP.erase(mP.begin(), mP.end());

	mP.reserve(8);
	mP.push_back(CoilTracker(2,3,"M2.A",M2A,errorPointer, 32));//
	mP.push_back(CoilTracker(4,12,"M0.A",M0A,errorPointer,32));//
	mP.push_back(CoilTracker(14,16,"M0.B",M0B,errorPointer,32));//
	mP.push_back(CoilTracker(17,27,"M2.B",M2B,errorPointer,32));//
	mP.push_back(CoilTracker(10,5,"M3.A",M3A,errorPointer,32));//
	mP.push_back(CoilTracker(19,13,"M3.B",M3B,errorPointer,32));//
	mP.push_back(CoilTracker(21,25,"M1.A",M1A,errorPointer,32));//
	mP.push_back(CoilTracker(7,8,"M1.B",M1B,errorPointer,32));//

	return &mP;
}

std::vector<Motor>* configureMotors(ErrorReporting* errorPointer){
	static std::vector<Motor> motorArray;
	motorArray.erase(motorArray.begin(), motorArray.end());
	motorArray.reserve(4);

	motorArray.push_back(Motor(4,12,"M0.A",M0A,14,16,"M0.B",M0B, errorPointer));
	motorArray.push_back(Motor(21,25,"M1.A",M1A,7,8,"M1.B",M1B, errorPointer));
	motorArray.push_back(Motor(2,3,"M2.A",M2A,17,27,"M2.B",M2B, errorPointer));
	motorArray.push_back(Motor(10,5,"M3.A",M3A,19,13,"M3.B",M3B, errorPointer));

	return &motorArray;
}

int retrieveExpectedMask(int pin, std::vector<Motor>* m){
//	int pairIndex = 0;
//	std::vector<Coil> &p = *pairs;
	std::vector<Motor> &motorVec = *m;
	int result = 0;
	int motorIndex = 0;
	for(motorIndex = 0; motorIndex < 4; motorIndex++){
		int trackerIndex = 0;
		for(trackerIndex = 0; trackerIndex < 2; trackerIndex++){
			result = motorVec[motorIndex].getCoilTracker(trackerIndex)->getCoil()->getExpectedMask(pin);
			if (result != 0){
	                       	printf("result: %d\n",result);
                	        result |= 1<<pin;
        	                return result;
	                }
		}
	}
	//Not a motor pairing
	//GPIO20 = GND
	//GND-> GPIO15, GPIO22, GPIO9, GPIO5, GPIO24
	//20,15
	//20,22
	//20,2
	//20,5
	//20,24
	switch(pin){
		case 20:
		case 9:
		case 11:
		case 15:
		case 22:
		case 24:
			return	(1<<20 | 1<<11 |1 << 9 | 1<<15 | 1<<22 | 1<<24);
	}
	return 1<<pin;
}
int retrieveExpectedMaskCoils(int pin, std::vector<Coil>* c){
//	int pairIndex = 0;
//	std::vector<Coil> &p = *pairs;
	std::vector<Coil> &coilVec = *c;
	int coilIndex = 0;
	for(coilIndex = 0; coilIndex < 8; coilIndex++){
		int result = coilVec[coilIndex].getExpectedMask(pin);
		if (result != 0){
                       	printf("result: %d\n",result);
               	        result |= 1<<pin;
       	                return result;
                }
	}
	//Not a motor pairing
	//GPIO20 = GND
	//GND-> GPIO15, GPIO22, GPIO9, GPIO5, GPIO24
	//20,15
	//20,22
	//20,2
	//20,5
	//20,24
	switch(pin){
		case 20:
		case 9:
		case 11:
		case 15:
		case 22:
		case 24:
			return	(1<<20 | 1<<11 |1 << 9 | 1<<15 | 1<<22 | 1<<24);
	}
	return 1<<pin;
}

int showMenu(){
	printf("Waiting for a test to be selected\n");
	printf("Options: \n");
	printf("\t1.Static Test Oryx4/8\n");
	printf("\t2.Static Test Oryx Nano\n");
	printf("\t3.Dynamic Test\n");

	setAllPinsToInp();

	int buttonPressed = checkButtons();
	printf("Option selected: %d\n", buttonPressed);
	return buttonPressed;
}

int checkButtons(){
	int storedResult = 0;
	const int gpio9 = 1<<9;
	const int gpio18 = 1<<18;
	const int gpio23 = 1<<23;
	const int gpio26 = 1<<26;

	int pinMask = gpio9 | gpio18 | gpio23 | gpio26;

	while(1){
		int readResult = GPIO_READMULT(pinMask);
		if(readResult!= 0){
//			printf("Something detected %x\n", readResult);
			delayMicroseconds(5000);
			readResult = GPIO_READMULT(pinMask);
			if(readResult != 0){
				storedResult = readResult;
//				printf("Something detected after 5 ms %x\n",readResult);
				do {
					readResult= GPIO_READMULT(pinMask);
					delayMicroseconds(100);
				}while (readResult != 0);
				break;
			}
		}
	}

	int gpioPin = storedResult & pinMask;
	int returnValue = 0;
	switch(gpioPin){
		case gpio9://0x200: //GPIO9
			returnValue = 1;
			break;
		case gpio18: //0x40000: //GPIO18
			returnValue = 2;
			break;
		case gpio23: //0x800000: //GPIO23
			returnValue = 3;
			break;
		case gpio26: //0x4000000: //GPIO26
			returnValue = 4;
			break;
//		default:
//			printf("Press fewer buttons\n");
	}
	return returnValue;
}
