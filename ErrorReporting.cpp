#include "ErrorReporting.h"
#include <string>
#include <stdio.h>

ErrorReporting::ErrorReporting(int* samplePointer, int* sampleIndexPointer):
	gpioHistory{0}{
	sample = samplePointer;
	sampleIndex = sampleIndexPointer;
	errorVec.reserve(MAX_ERROR_CODES);
	int errorIndex = 0;
	for(errorIndex = 0; errorIndex < MAX_ERROR_CODES; errorIndex++){
		errorCodes[errorIndex] = 0;
		errorVec[errorIndex] = 0;
	}
	currentErrorCodeIndex = 0;
}

int ErrorReporting::getErrorCode(int errorIndex) const{
	if(errorIndex < MAX_ERROR_CODES){
		return errorCodes[errorIndex];
	}
	return 0;
}

void ErrorReporting::setErrorCode(int errorCodeI, int errorCode){
	errorCodes[errorCodeI] = errorCode;
}

void ErrorReporting::setNextErrorCode(int errorCode, float frequency){
	//int errorCodeIndex = 0;
	//for(errorCodeIndex= 0; errorCodeIndex < MAX_ERROR_CODES; errorCodeIndex++){
	//	if (errorCodes[errorCodeIndex] == errorCode){
	//		return;
	//	}
	//}
	errorCodes[currentErrorCodeIndex] = errorCode;
	setGpioHistory(currentErrorCodeIndex);
	incrementErrorCodeIndex();
	errorVec[currentErrorCodeIndex] = new ErrorInfo(sample, *sampleIndex, errorCode, frequency);
}

void ErrorReporting::incrementErrorCodeIndex(){
	currentErrorCodeIndex++;
	if (currentErrorCodeIndex >= MAX_ERROR_CODES){
		currentErrorCodeIndex = 0;
	}
}

int ErrorReporting::findErrorCode(int errorCode) const{
	int errorIndex = 0;
	int found = 0;
	for(errorIndex = 0; errorIndex < MAX_ERROR_CODES; errorIndex++){
		if(errorCodes[errorIndex] == errorCode){
			found = 1;
			break;
		}
	}
	if(found != 1){
		errorIndex = -1;
	}
	return errorIndex;
}

int ErrorReporting::generateErrorCode(int testType, int problemDetail, int location, int problemType){
	int testTypeVal = testType <<  14;
	int problemDetailVal = problemDetail << 10;
	int locationVal =  location << 4;

	return testTypeVal | problemDetailVal | locationVal | problemType;
}

std::string ErrorReporting::generateErrorMessage(int errorCode){
	int testType = (errorCode & (0xF << 14)) >> 14;
	int problemDetail = (errorCode & (0xF << 10)) >> 10;
	int location = (errorCode & (0x3F << 4)) >> 4;
	int problemType = errorCode & 0xF;
	std::string testTypeString = "Fault found in";
	switch(testType){
		case 0x0: //No error
			testTypeString = "";
			break;
		case 0x1: //Static test
			testTypeString += " static test";
			break;
		case 0x2: //Dynamic test
			testTypeString += " dynamic test";
			break;

	}
	std::string problemDetailString = "";
//	printf("Problem detail: %d\n", problemDetail);
	std::string t = "Pin ";
	char numstr[21]; // enough to hold all numbers up to 64-bits
	sprintf(numstr, "%d", location);
	t = t + numstr;

	switch(problemDetail){
		case 0x0: //No detail
			break;
		case 0x1: //Pin level
			problemDetailString = t;//getPinName(location);
			break;
		case 0x2: //Coil terminal level
			problemDetailString = getCoilTerminalName(location);
			break;
		case 0x3: //Coil level
			problemDetailString = getCoilName(location);
			break;
		case 0x4: //Motor level
			problemDetailString = getMotorName(location);
			break;
	}
	std::string problemTypeString = "";
	switch(problemType){
		case 0x0: //No fault
			break;
		case 0x1: //Electrical short
			problemTypeString = " electrical short was found";
			break;
		case 0x2: //Mising component
			problemTypeString = " the component was missing";
			break;
		case 0x3: //coil short to self
			problemTypeString = " the coil is shorting itself";
			break;
		case 0x4://coil short to coil
			problemTypeString = " coil is shorting with another coil";
			break;
		case 0x5://coil short to coil
			problemTypeString = " open circuit was found";
			break;
	}
	return testTypeString + "," + problemDetailString + "," + problemTypeString;
}

std::string ErrorReporting::getPinName(int location){
	static std::string one( "Pin " + location);
	printf("location pin name:%d\n", location);
	printf("one: %s\n", one.c_str());
	return one;
}

std::string ErrorReporting::getCoilTerminalName(int location){
	std::string result = "";
	switch(location){
		case 0x0:
			result = "M0.A0";
			break;
		case 0x1:
			result = "M0.A1";
			break;
		case 0x2:
			result = "M0.B0";
			break;
		case 0x3:
			result = "M0.B1";
			break;
		case 0x4:
			result = "M1.A0";
			break;
		case 0x5:
			result = "M1.A1";
			break;
		case 0x6:
			result = "M1.B0";
			break;
		case 0x7:
			result = "M1.B1";
			break;
		case 0x8:
			result = "M2.A0";
			break;
		case 0x9:
			result = "M2.A1";
			break;
		case 0xA:
			result = "M2.B0";
			break;
		case 0xB:
			result = "M2.B1";
			break;
		case 0xC:
			result = "M3.A0";
			break;
		case 0xD:
			result = "M3.A1";
			break;
		case 0xE:
			result = "M3.B0";
			break;
		case 0xF:
			result = "M3.B1";
			break;
	}
	return "Coil terminal: " + result;
}

std::string ErrorReporting::getCoilName(int location){
	std::string result = "";
	switch(location){
		case 0x0:
			result = "M0.A";
			break;
		case 0x1:
			result = "M0.B";
			break;
		case 0x2:
			result = "M1.A";
			break;
		case 0x3:
			result = "M1.B";
			break;
		case 0x4:
			result = "M2.A";
			break;
		case 0x5:
			result = "M2.B";
			break;
		case 0x6:
			result = "M3.A";
			break;
		case 07:
			result = "M3.B";
			break;
	}
	return "Coil: " + result;
}

std::string ErrorReporting::getMotorName(int location){
	std::string result = "";
	switch(location){
		case 0x0:
			result = "M0";
			break;
		case 0x1:
			result = "M1";
			break;
		case 0x2:
			result = "M2";
			break;
		case 0x3:
			result = "M3";
			break;

	}
//	printf("location: %d\n", location);
	return "Motor: " + result;
}

void ErrorReporting::setGpioHistory(int errorID){
	int &sampleIndexRef = *sampleIndex;
	int gpioHistoryIndex = 0;
	for(gpioHistoryIndex = HISTORY_ITEMS - 1; gpioHistoryIndex >= 0; gpioHistoryIndex--){
		if(sampleIndexRef - gpioHistoryIndex >= 0){
			gpioHistory[(errorID * HISTORY_ITEMS) + gpioHistoryIndex] = sample[(sampleIndexRef - gpioHistoryIndex)];
		}else{
			 gpioHistory[(errorID * HISTORY_ITEMS) + gpioHistoryIndex] = 0x3;
		}
	}
}

void ErrorReporting::printGpioHistory(int errorID){
	for(int gpioHistoryIndex = 0; gpioHistoryIndex < HISTORY_ITEMS; gpioHistoryIndex++){
		printf("GpioHistory[%d]: %d\n", (errorID * HISTORY_ITEMS) + gpioHistoryIndex,gpioHistory[(errorID*HISTORY_ITEMS)+gpioHistoryIndex]);
	}

}

int ErrorReporting::getCurrentErrorCodeIndex() const{
	return currentErrorCodeIndex;
}
