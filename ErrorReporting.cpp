#include "ErrorReporting.h"
#include <string>
#include <stdio.h>

ErrorReporting::ErrorReporting(){
	int errorIndex = 0;
	for(errorIndex = 0; errorIndex < MAX_ERROR_CODES; errorIndex++){
		errorCodes[errorIndex] = 0;
		errorMessage[errorIndex] = "";
	}
	setupErrorMapping();
	currentErrorCodeIndex = 0;
};

int ErrorReporting::getErrorCode(int errorIndex){
	if(errorIndex < MAX_ERROR_CODES){
		return errorCodes[errorIndex];
	}
	return 0;
}

void ErrorReporting::setErrorCode(int errorCodeI, int errorCode){
	errorCodes[errorCodeI] = errorCode;
}

void ErrorReporting::setNextErrorCode(int errorCode){
	int errorCodeIndex = 0;
	for(errorCodeIndex= 0; errorCodeIndex < MAX_ERROR_CODES; errorCodeIndex++){
		if (errorCodes[errorCodeIndex] == errorCode){
			return;
		}
	}
	errorCodes[currentErrorCodeIndex] = errorCode;
	incrementErrorCodeIndex();
}

void ErrorReporting::incrementErrorCodeIndex(){
	currentErrorCodeIndex++;
	if (currentErrorCodeIndex >= MAX_ERROR_CODES){
		currentErrorCodeIndex = 0;
	}
}

int ErrorReporting::findErrorCode(int errorCode){
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

void ErrorReporting::setErrorMessage(std::string message, int errorIndex){
	errorMessage[errorIndex] = message;
}

void ErrorReporting::setupErrorMapping(){
	int testType[16];
	testType[0] = 0x0; //No error
	testType[1] = 0x1; //Static
	testType[2] = 0x2; //Dynamic

	int problemDetail[16];
	problemDetail[0] = 0x0; //No detail
	problemDetail[1] = 0x1; //pin level
	problemDetail[2] = 0x2; //coil terminal level
	problemDetail[3] = 0x3; //coil level
	problemDetail[4] = 0x4; //motor level

	int location[32];
	location[0] = 0x00; //pin 0	/ M0.A0	/ M0.A	/ M0
	location[1] = 0x01; //pin 1	/ M0.A1	/ M0.B	/ M1
	location[2] = 0x02; //pin 2	/ M0.B0	/ M1.A	/ M2
	location[3] = 0x03; //pin 3	/ M0.B1	/ M1.B	/ M3
	location[4] = 0x04; //pin 4	/ M1.A0	/ M2.A
	location[5] = 0x05; //pin 5	/ M1.A1	/ M2.B
	location[6] = 0x06; //pin 6	/ M1.B0	/ M3.A
	location[7] = 0x07; //pin 7	/ M1.B1	/ M3.B
	location[8] = 0x08; //pin 8	/ M2.A0	/
	location[9] = 0x09; //pin 9	/ M2.A1	/
	location[10] = 0x0A; //pin 10	/ M2.B0	/
	location[11] = 0x0B; //pin 11	/ M2.B1	/
	location[12] = 0x0C; //pin 12	/ M3.A0	/
	location[13] = 0x0D; //pin 13	/ M3.A1	/
	location[14] = 0x0E; //pin 14	/ M3.B0	/
	location[15] = 0x0F; //pin 15	/ M3.B1	/
	location[16] = 0x10; //pin 16
	location[17] = 0x11; //pin 17
	location[18] = 0x12; //pin 18
	location[19] = 0x13; //pin 19
	location[20] = 0x14; //pin 20
	location[21] = 0x15; //pin 21
	location[22] = 0x16; //pin 22
	location[23] = 0x17; //pin 23
	location[24] = 0x18; //pin 24
	location[25] = 0x19; //pin 25
	location[26] = 0x1A; //pin 26
	location[27] = 0x1B; //pin 27

	int problemType[32];
	problemType[0] = 0x0; //No problemn
	problemType[1] = 0x1; //Electrical short
	problemType[2] = 0x2; //Missing Component
	problemType[3] = 0x3; //

	//availableCodes[0] = 0x0;
	//availableMessages[0] = "No Error";

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

