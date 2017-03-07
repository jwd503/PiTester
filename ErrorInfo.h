#include <vector>
class ErrorInfo{
	private:
		std::vector<int> *sampleSnapshot;
		std::vector<int> pins;
		int errorIndex;
		int errorCode;
		int frequency;
		void appendPinLevel(int location);
		void appendCoilTerminalLevel(int location);
		void appendCoilLevel(int location);
		void appendMotorLevel(int location);
	public:
		ErrorInfo(int* samples, int eIndex, int eCode);
		~ErrorInfo();

		void dumpToFile(const char* filename);
		int calculateFrequency();

};
