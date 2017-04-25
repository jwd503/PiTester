#ifndef LEDCHAR_H
#define LEDCHAR_H
#include <string.h>
#include <map>
#include "Utility.h"
class LEDChar{
	private:
		static const int gpio9 = 1 << 9;
        	static const int gpio23 = 1 << 23;
	        static const int gpio6 = 1 << 6;
		static std::map<std::string, int> charSet;
		bool colon;

	public:

		LEDChar(int index, int segments);
		void setSegmentSelect(int segment);
		void setSegments(int segments);
		int getLEDPins();
		void clearRegister();
		int decode(int bit);
		int segments;
		int ledSelect;
		int segmentSelect;

};
#endif
