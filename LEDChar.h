#ifndef LEDCHAR_H
#define LEDCHAR_H
#include <string.h>
#include <map>
class LEDChar{
	private:
		int ledSelect;
		static const int gpio9 = 1 << 9;
        	static const int gpio20 = 1 << 20;
	        static const int gpio23 = 1 << 23;
		static std::map<std::string, int> charSet;
		int segmentSelect;

	public:
		LEDChar(int index, int segments);
		void setSegmentSelect(int segment);
		void setSegments(int segments);
		int getLEDPins();
		void clearRegister();
		int decode(int bit);
		int segments;

};
#endif
