#include "ErrorInfo.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <time.h>

#define MICROSECONDS 1000000.0

const std::string ErrorInfo::pType[] = {
	"",     "SC  ", "NA  ","CSC ",
	"C2C ", "OC  "
};
const std::string ErrorInfo::pinLocation[] = {
	"", 	"", 	"DB14", "DB15",
	"DB1 ", "DB22", "DB19",	"DB13",
	"DB12", "DB20",	"DB23", "DB21",
	"DB2 ",	"DB24", "DB4 ", "DB5 ",
	"DB3 ", "DB16", "DB6 ", "DB25",
	"DB7 ", "DB10", "DB18", "DB8 ",
	"DB9 ", "DB11", "DB26", "DB17"
};
const std::string ErrorInfo::coilLocation[] = {
	"nn0A", "nn0B", "nn1A", "nn1B",
	"nn2A", "nn2B", "nn3A", "nn3B"
};
const std::string ErrorInfo::coilTerminalLocation[]  = {
	" 0A1", " 0A2", " 0B1", " 0B2",
	" 1A1", " 1A2", " 1B1", " 1B2",
	" 2A1", " 2A2", " 2B1", " 2B2",
	" 3A1", " 3A2", " 3B1", " 3B2"
};
const std::string ErrorInfo::motorLocation[] = {
	"   0", "   1", "   2", "   3"
};


ErrorInfo::ErrorInfo(int* samples, int eIndex, int eCode, float frequency){
	//Take a snapshot of the last 10k samples before the error occured
	sampleSnapshot =  new std::vector<int>;
	sampleSnapshot->insert(sampleSnapshot->begin(), samples, samples + 10000);

	//Store the location of the error
	errorIndex = eIndex;
	errorCode = eCode;

	//Create a filename based on the erroCode triggered and dump the samples to file
	std::string filename = "logs/";
	char numstr[21]; // enough to hold all numbers up to 64-bits
	sprintf(numstr, "%x", errorCode);
	filename = filename + numstr;

	this->frequency = frequency;
	if (frequency > 69){
		dumpToFile(filename.c_str());
	}
	generateLEDOut();
}

void ErrorInfo::generateLEDOut(){
	int problemDetail = (errorCode & (0xF << 10)) >> 10;
	int location = (errorCode & (0x3F << 4)) >> 4;
	int problemType = errorCode & 0xF;
	ledOut.reserve(3);
	ledOut.push_back("cant");
	ledOut.push_back("find");
	ledOut.push_back("location");

	ledOut.at(0) = pType[problemType];
	switch(problemDetail){
		case PIN_LEVEL:
			ledOut.at(1) = "Pin ";
			ledOut.at(2) = pinLocation[location];
			break;
		case MOTOR_LEVEL:
			ledOut.at(1) = "MOTO";
			ledOut.at(2) = motorLocation[location];
			break;
		case COIL_LEVEL:
			ledOut.at(1) = "coil";
			ledOut.at(2) = coilLocation[location];
			break;

		case COIL_TERMINAL_LEVEL:
			ledOut.at(1) = "coil";
			ledOut.at(2) = coilTerminalLocation[location];
			break;
	}

}

int ErrorInfo::getErrorCode(){
	return errorCode;
}

void ErrorInfo::dumpToFile(const char* filename){

	std::ofstream myfile;
        myfile.open(filename);

	std::vector<int> lastSeenPinAt;
	std::vector<int> localPins;
	lastSeenPinAt.reserve(32);
	localPins.reserve(32);

	//initialise the starting values
	for(unsigned int pinIndex = 0; pinIndex < 32; pinIndex++){
		lastSeenPinAt[pinIndex] = 0;
		localPins[pinIndex] = pinIndex;
        }
	//Variables used to loop around the circular buffer of samples
	int startIndex = errorIndex + 1;
	bool fullLoop = false;
	int sampleIndex = startIndex;
	int flag = 0;

	//Keep going until we have completed a full loop
	while(fullLoop != true){
		int nextIndex = (sampleIndex + 1) % (sampleSnapshot->size() - 1);
		//Compress periods of no pins high such that the file isn't unnecessarily long
		if(sampleSnapshot->at(sampleIndex) == 0){ //No pins set to 1
                        if(flag == 0){
                                myfile << "\n=========================================================================\n\n";
                                flag = 1;
                        }
			sampleIndex = nextIndex;
	                fullLoop = sampleIndex == startIndex;
			continue;

                }else{
			myfile << sampleIndex<< ": ";
		}

		int previousIndex =  (sampleIndex - 1) < 0 ? (sampleSnapshot->size() - 1): (sampleIndex -1);
		for(int pinIndex = 2; pinIndex < 28; pinIndex++){
			//Check if current pin is set to a non 0 value
			if( 1<<localPins[pinIndex] & sampleSnapshot->at(sampleIndex)){
				int sampleFlag = 0;
                                myfile << "\t\t" << pinIndex;
				int pinMask = 1 << localPins[pinIndex];
				bool previousSampleNotSet =	(pinMask & sampleSnapshot->at(previousIndex)) == 0;
				bool nextSampleNotSet =	false;
				if(previousSampleNotSet || nextSampleNotSet){
					double hz = 0.0;
					int difference = 0;
					if(lastSeenPinAt[pinIndex] != 0){
						//Calculate an aproximate time that has elapsed since we last saw this pin go high(1)
						if(sampleIndex < lastSeenPinAt[pinIndex]){
							difference = (((sampleSnapshot->size() -1) - lastSeenPinAt[pinIndex]) + sampleIndex) * 100;
						} else{
							difference = ((sampleIndex - lastSeenPinAt[pinIndex]) * 100);
						}
						//Calculate a frequency from this time
						hz = MICROSECONDS / difference;
					}
					lastSeenPinAt[pinIndex] = sampleIndex;
					//append the time and calculate frequency to the file.
					myfile << "\t" << difference << "us hz:" << hz << "\n";
					sampleFlag = 1;
				}
				//This ensures that each pin is on a new line in the file
				if (sampleFlag == 0) myfile << "\n";
				flag = 0;
			}
		}
		//Check that we haven't looped back to the start
		sampleIndex = nextIndex;
		fullLoop = sampleIndex == startIndex;
	}
}

ErrorInfo::~ErrorInfo(){
	delete sampleSnapshot;
}
