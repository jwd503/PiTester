#include "LEDDriver.h"
#include "LEDChar.h"
#include "string.h"
#include <map>

const uint8_t LEDDriver::sevsegascii_table[] = {
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, SEGN, SEGN,
	SEGN, SEGN, 0x80, SEGN,
	0x3F, 0x06, 0x5B, 0x4F, // 0 1 2 3
	0x66, 0x6D, 0x7D, 0x07, // 4 5 6 7
	0x7F, 0x6F, SEGN, SEGN, // 8 9
	SEGN, SEGN, SEGN, SEGN,
	SEGN, 0x77, 0x7C, 0x39, //   A B C
	0x5E, 0x79, 0x71, 0x3D, // D E F G
	0x74, 0x30, 0x1E, SEGN, // H I J _
	0x38, 0x37, 0x54, 0x3F, // L M N O
	0x73, 0x67, 0x50, 0x6D, // P Q R S
	0x78, 0x3E, 0x1C, SEGN, // T U V _
	SEGN, 0x6E, 0x5B, SEGN, // _ Y Z
	SEGN, SEGN, SEGN, SEGN,
	SEGN, 0x77, 0x7C, 0x39, //   A B C
	0x5E, 0x79, 0x71, 0x3D, // D E F G
	0x74, 0x30, 0x1E, SEGN, // H I J _
	0x38, 0x37, 0x54, 0x3F, // L M N O
	0x73, 0x67, 0x50, 0x6D, // P Q R S
	0x78, 0x3E, 0x1C, SEGN, // T U V _
	SEGN, 0x6E, 0x5B, SEGN, // _ Y Z
	SEGN, SEGN, SEGN, SEGN
};
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
//		std::string s(1,word[charIndex]);
		LEDChars[charIndex].setSegments(sevsegascii_table[(int)word[charIndex]]);
		//if the following condition is met the key was found in the mapping
//		if(charSet.find(s) != charSet.end()){
//			printf("word[%d]: %x\n", charIndex, charSet.at(s));
			//Set the LEDs segments to display to the value from the mapping
//			LEDChars[charIndex].setSegments(charSet.at(s));
//		}else{
			//Couldn't find a mapping for this character so set blank instead
//			LEDChars[charIndex].setSegments(0);
//		}

	}
}

void LEDDriver::setDisplay(){
	for(int segmentIndex = 0; segmentIndex < 8; segmentIndex++){
		int ledSelect = 0;
		int segmentSelected = 0;
		LEDChars[0].setSegmentSelect(segmentIndex);
		segmentSelected = LEDChars[0].segmentSelect;
		for (int ledIndex = 0; ledIndex < 4; ledIndex++){
			int segmentSelectInd = ((LEDChars[ledIndex].segments & (1<<segmentIndex)) > 0);
			if (segmentSelectInd != 0){
				ledSelect |= LEDChars[ledIndex].ledSelect;
			}
		}
		int delay = 10;
		GPIO_SET = segmentSelected;
		delayMicroseconds(delay);
		GPIO_SET = 1<<18;
		GPIO_CLR = 1<<18;
		GPIO_CLR = segmentSelected;

		GPIO_SET = ledSelect;
		delayMicroseconds(delay);
		GPIO_SET = 1<<18;
		GPIO_CLR = 1<<18;
		GPIO_CLR = ledSelect;
		delayMicroseconds(1000);
	}

}

void LEDDriver::driveDisplay(const std::string word){
	float duration = 0.3;
	int iterations = 125 * duration;
	decodeWord(word);
	setOutputs((1<<26) | (1<<23) | (1<<6) | (1<<9) |(1<<18));
	for(int i = 0; i < iterations; i++){
		setDisplay();
	}
	LEDChars[0].clearRegister();

}

void LEDDriver::driveDisplay(const std::string word, float duration){
	int iterations = 125 * duration;
	decodeWord(word);
	setOutputs((1<<26) | (1<<23) | (1<<6) | (1<<9) |(1<<18));
	for(int i = 0; i < iterations; i++){
		setDisplay();
	}
	LEDChars[0].clearRegister();

}
