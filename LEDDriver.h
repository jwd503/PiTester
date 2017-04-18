#include <vector>
#include "string.h"
#include <map>
#include "LEDChar.h"

class LEDDriver{
	//Uses GPIO 26,23,29,9 to write data
	//GPIO 18 to clock data in
	//Requires 2 writes to fully select the segment/led display to fire

	private:
		std::vector<LEDChar> LEDChars;
		static std::map<std::string,int> charSet;
	public:
		LEDDriver();
		void driveDisplay();
		void decodeWord(std::string word);

};
