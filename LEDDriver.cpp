#include "LEDDriver.h"
#include "LEDChar.h"
#include "string.h"
#include <map>
std::map<std::string, int> LEDDriver::charSet = {
        {"0", 0x3F},
        {"1", 0x06},
        {"2", 0x5B},
        {"3", 0x4F},
        {"4", 0x66},
        {"5", 0x6D},
        {"6", 0x7D},
        {"7", 0x07},
        {"8", 0x7F},
        {"9", 0x67},
        {"A", 0x77},
        {"b", 0x7C},
        {"C", 0x39},
        {"d", 0x5E},
        {"E", 0x79},
        {"F", 0x71}
};

LEDDriver::LEDDriver(){
	for(int ledIndex=0; ledIndex < 4; ledIndex++){
		//Initialise all the LEDs as blank
		LEDChars.push_back(LEDChar(ledIndex, 0));
	}
}

void LEDDriver::decodeWord(std::string word){
	//This only checks the first 4 chars of the string parsed as there are only 4 LED displays
	for(int charIndex = 0; charIndex < 4; charIndex++){
		//Fetch the char and cast as a string
		std::string s(1,word[charIndex]);
		//if the following condition is met the key was found in the mapping
		if(charSet.find(s) != charSet.end()){
			printf("word[%d]: %x\n", charIndex, charSet.at(s));
			//Set the LEDs segments to display to the value from the mapping
			LEDChars[charIndex].setSegments(charSet.at(s));
		}else{
			//Couldn't find a mapping for this character so set blank instead
			LEDChars[charIndex].setSegments(0);
		}

	}
}
