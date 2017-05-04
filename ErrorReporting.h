#ifndef ERRORREPORTING_H
#define ERRORREPORTING_H

#include "ErrorInfo.h"
#include <string>
#include <queue>
#define MAX_ERROR_CODES 32
#define HISTORY_ITEMS 50

#define NONE 0
//TestType Constants
#define STATIC_TEST 1
#define DYNAMIC_TEST 2

//ProblemDetail Constants
#define PIN_LEVEL 1
#define COIL_TERMINAL_LEVEL 2
#define COIL_LEVEL 3
#define MOTOR_LEVEL 4

//Coil ID
#define M0A 0
#define M0B 1
#define M1A 2
#define M1B 3
#define M2A 4
#define M2B 5
#define M3A 6
#define M3B 7

//Problem type
//#define ELECTRICAL_SHORT 1
//#define MISSING_COMPONENT 2
//#define COIL_SHORT_TO_SELF 3
//#define COIL_TO_COIL 4
//#define OPEN_CIRCUIT 5

class ErrorReporting{
	private:
		int gpioHistory[HISTORY_ITEMS*MAX_ERROR_CODES];

		void incrementErrorCodeIndex();
		int* sample;
		int* sampleIndex;
	public:
		ErrorReporting(int* samplePointer, int* sampleIndexPointer);
		int getCurrentErrorCodeIndex() const;
		void  setNextErrorCode(int errorCode, float frequency);
		static int generateErrorCode(int testType, int problemDetail, int location, int problemType);
		static std::string generateErrorMessage(int errorCode);

		static std::string getMotorName(int location);
		static std::string getPinName(int location);
		static std::string getCoilTerminalName(int location);
		static std::string getCoilName(int location);

		void setGpioHistory(int errorID);
		void printGpioHistory(int errorID);
		std::queue<ErrorInfo*> errorVec;
		int currentErrorCodeIndex;

};

#endif
