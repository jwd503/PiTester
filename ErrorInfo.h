#include <vector>
class ErrorInfo{
	private:
		std::vector<int> *sampleSnapshot;
		int errorIndex;
		int errorCode;

	public:
		ErrorInfo(int* samples, int eIndex, int eCode, double frequency);
		~ErrorInfo();

		void dumpToFile(const char* filename);
		double frequency;

};


