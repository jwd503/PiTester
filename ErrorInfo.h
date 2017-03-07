#include <vector>
class ErrorInfo{
	private:
		std::vector<int> *sampleSnapshot;
		std::vector<int> pins;
		int errorLocation;
		int errorCode;
	public:
		ErrorInfo(int* samples);

		void dumpToFile(const char* filename);
		int calculateFrequency();

};
