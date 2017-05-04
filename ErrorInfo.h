#include <vector>
#include <string>

//Problem type
#define ELECTRICAL_SHORT 1
#define MISSING_COMPONENT 2
#define COIL_SHORT_TO_SELF 3
#define COIL_TO_COIL 4
#define OPEN_CIRCUIT 5

//ProblemDetail Constants
#define PIN_LEVEL 1
#define COIL_TERMINAL_LEVEL 2
#define COIL_LEVEL 3
#define MOTOR_LEVEL 4

class ErrorInfo{
	private:
		std::vector<int> *sampleSnapshot;
		int errorIndex;
		int errorCode;
		static const std::string pType[];
		static const std::string pinLocation[];
		static const std::string coilLocation[];
		static const std::string coilTerminalLocation[];
		static const std::string motorLocation[];

	public:
		ErrorInfo(int* samples, int eIndex, int eCode, float frequency);
		~ErrorInfo();
		void generateLEDOut();

		std::vector<std::string> ledOut;
		void dumpToFile(const char* filename);
		int getErrorCode();
		float frequency;

};


