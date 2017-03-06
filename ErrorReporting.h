#ifndef ERRORREPORTING_H
#define ERRORREPORTING_H

#include <string>
#define MAX_ERROR_CODES 32

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
#define ELECTRICAL_SHORT 1
#define MISSING_COMPONENT 2
#define COIL_SHORT_TO_SELF 3
#define COIL_TO_COIL 4

class ErrorReporting{
	private:
		int errorCodes[MAX_ERROR_CODES];
		std::string errorMessage[MAX_ERROR_CODES];
		static int availableCodes[32];
		static std::string availableMessages[32];

		void setupErrorMapping();
		int currentErrorCodeIndex;
		void incrementErrorCodeIndex();
	public:
		ErrorReporting();
		int getErrorCode(int errorIndex);
		void  setNextErrorCode(int errorCode);
		int findErrorCode(int errorCode);
		std::string getErrorMessage(int  errorIndex);
		void setErrorMessage(std::string message, int errorIndex);
		std::string lookupErrorCode(int errorCode);
		int generateErrorCode(int testType, int problemDetail, int location, int problemType);
		std::string generateErrorMessage(int errorCode);

		std::string getMotorName(int location);
		std::string getPinName(int location);
		std::string getCoilTerminalName(int location);
		std::string getCoilName(int location);
		void setErrorCode(int errorCodeI, int errorCode);

};

#endif
