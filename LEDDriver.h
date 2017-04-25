#include <vector>
#include "string.h"
#include <map>
#include "LEDChar.h"
#include "PJ_RPI.h"
#include "Utility.h"
class LEDDriver{
	//Uses GPIO 26,23,29,9 to write data
	//GPIO 18 to clock data in
	//Requires 2 writes to fully select the segment/led display to fire

	private:
		std::vector<LEDChar> LEDChars;
		static std::map<std::string,int> charSet;
		#define SEGN 0x00
		#define SEGU 0x08 //unknown way of display this character
		static const uint8_t sevsegascii_table[];
		void setDisplay();

	public:
		LEDDriver();
		void driveDisplay(std::string word);
		void driveDisplay(std::string word, float duration);
		void decodeWord(std::string word);

};
