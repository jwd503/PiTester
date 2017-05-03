#include "ErrorInfo.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <time.h>

#define MICROSECONDS 1000000.0

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

	int problemDetail = (errorCode & (0xF << 10)) >> 10;
	int location = (errorCode & (0x3F << 4)) >> 4;
	int problemType = errorCode & 0xF;
	ledOut.reserve(3);
	ledOut.push_back("cant");
	ledOut.push_back("find");
	ledOut.push_back("location");
	switch(problemType){
		case ELECTRICAL_SHORT:
			ledOut.at(0) = "SC  ";
			break;
		case OPEN_CIRCUIT:
			ledOut.at(0) = "OC  ";
			break;
		case MISSING_COMPONENT:
			ledOut.at(0) = "NA  ";
			break;
		case COIL_TO_COIL:
			ledOut.at(0) = "C2C ";
			break;
		case COIL_SHORT_TO_SELF:
			ledOut.at(0) = "CSC ";
			break;
	}
	switch(problemDetail){
		case PIN_LEVEL:
			ledOut.at(1) = "Pin ";
			switch(location){
				case 2:
					ledOut.at(2) = "DB14";
					break;
				case 3:
					ledOut.at(2) = "DB15  ";
					break;
				case 4:
					ledOut.at(2) = "DB1   ";
					break;
				case 5:
					ledOut.at(2) = "DB22  ";
					break;
				case 6:
					ledOut.at(2) = "DB19  ";
					break;
				case 7:
					ledOut.at(2) = "DB13  ";
					break;
				case 8:
					ledOut.at(2) = "DB12  ";
					break;
				case 9:
					ledOut.at(2) = "DB20  ";
					break;
				case 10:
					ledOut.at(2) = "DB23  ";
					break;
				case 11:
					ledOut.at(2) = "DB21  ";
					break;
				case 12:
					ledOut.at(2) = "DB2   ";
					break;
				case 13:
					ledOut.at(2) = "DB24  ";
					break;
				case 14:
					ledOut.at(2) = "DB4   ";
					break;
				case 15:
					ledOut.at(2) = "DB5   ";
					break;
				case 16:
					ledOut.at(2) = "DB3   ";
					break;
				case 17:
					ledOut.at(2) = "DB16  ";
					break;
				case 18:
					ledOut.at(2) = "DB6   ";
					break;
				case 19:
					ledOut.at(2) = "DB25  ";
					break;
				case 20:
					ledOut.at(2) = "DB7   ";
					break;
				case 21:
					ledOut.at(2) = "DB10  ";
					break;
				case 22:
					ledOut.at(2) = "DB18  ";
					break;
				case 23:
					ledOut.at(2) = "DB8   ";
					break;
				case 24:
					ledOut.at(2) = "DB9   ";
					break;
				case 25:
					ledOut.at(2) = "DB11  ";
					break;
				case 26:
					ledOut.at(2) = "DB26  ";
					break;
				case 27:
					ledOut.at(2) = "DB17  ";
					break;
			}
			break;
		case MOTOR_LEVEL:
			ledOut.at(1) = "MOTO";
			switch(location){
				case 0:
					ledOut.at(2) = "   0";
					break;
				case 1:
					ledOut.at(2) = "   1";
					break;
				case 2:
					ledOut.at(2) = "   2";
					break;
				case 3:
					ledOut.at(2) = "   3";
					break;

			}
			break;
		case COIL_LEVEL:
			ledOut.at(1) = "coil";

			switch(location){
				case 0:
					ledOut.at(2) = "nn0A";
					break;
				case 1:
					ledOut.at(2) = "nn0B";
					break;
				case 2:
					ledOut.at(2) = "nn1A";
					break;
				case 3:
					ledOut.at(2) = "nn1B";
					break;
				case 4:
					ledOut.at(2) = "nn2A";
					break;
				case 5:
					ledOut.at(2) = "nn2B";
					break;
				case 6:
					ledOut.at(2) = "nn3A";
					break;
				case 7:
					ledOut.at(2) = "nn3B";
					break;
			}

			break;
		case COIL_TERMINAL_LEVEL:
			ledOut.at(1) = "coil";

			switch(location){
				case 0:
					ledOut.at(2) = " 0A1";
					break;
				case 1:
					ledOut.at(2) = " 0A2";
					break;
				case 2:
					ledOut.at(2) = " 0B1";
					break;
				case 3:
					ledOut.at(2) = " 0B2";
					break;
				case 4:
					ledOut.at(2) = " 1A1";
					break;
				case 5:
					ledOut.at(2) = " 1A2";
					break;
				case 6:
					ledOut.at(2) = " 1B1";
					break;
				case 7:
					ledOut.at(2) = " 1B2";
					break;
				case 8:
					ledOut.at(2) = " 2A1";
					break;
				case 9:
					ledOut.at(2) = " 2A2";
					break;
				case 10:
					ledOut.at(2) = " 2B1";
					break;
				case 11:
					ledOut.at(2) = " 2B2";
					break;
				case 12:
					ledOut.at(2) = " 3A1";
					break;
				case 13:
					ledOut.at(2) = " 3A2";
					break;
				case 14:
					ledOut.at(2) = " 3B1";
					break;
				case 15:
					ledOut.at(2) = " 3B2";
					break;
			}

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
