#include <vector>
class ErrorInfo{
	private:
		std::vector<int> *sampleSnapshot;
		std::vector<int> pins;
		int errorIndex;
		int errorCode;
		void appendPinLevel(int location);
		void appendCoilTerminalLevel(int location);
		void appendCoilLevel(int location);
		void appendMotorLevel(int location);
	public:
		ErrorInfo(int* samples, int eIndex, int eCode, double frequency);
		~ErrorInfo();

		void dumpToFile(const char* filename);
		int calculateFrequency();
		double frequency;

};


