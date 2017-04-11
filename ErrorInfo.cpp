#include "ErrorInfo.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#define MICROSECONDS 1000000.0

ErrorInfo::ErrorInfo(int* samples, int eIndex, int eCode, double frequency){
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

}

void ErrorInfo::dumpToFile(const char* filename){

	std::ofstream myfile;
        myfile.open(filename);

	std::vector<int> lastSeenPinAt;
	std::vector<int> localPins;
	lastSeenPinAt.reserve(32);
	localPins.reserve(32);
	std::string strBuf;

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
				int difference = 0;
				double hz = 0.0;
				int sampleFlag = 0;
                                myfile << "\t\t" << pinIndex;
				int pinMask = 1 << localPins[pinIndex];
				bool previousSampleNotSet =	(pinMask & sampleSnapshot->at(previousIndex)) == 0;
				bool nextSampleNotSet =	false;
				if(previousSampleNotSet || nextSampleNotSet){
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
