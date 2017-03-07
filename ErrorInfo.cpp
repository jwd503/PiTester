#include "ErrorInfo.h"
#include <vector>
#include <fstream>
#include <iostream>

#define TO_MICROSECONDS 1/1000000.0

ErrorInfo::ErrorInfo(int* samples){
	sampleSnapshot =  new std::vector<int>;
	sampleSnapshot->insert(sampleSnapshot->begin(), samples, samples + 9999);
}

void ErrorInfo::dumpToFile(const char* filename){

	std::ofstream myfile;
	myfile.open("sampleLog.txt");
	int lastSeenPinAt[32] = {0};
	int flag = 0;
	for (int sampleIndex = 0; sampleIndex < 10000; sampleIndex++){
		if (sampleSnapshot->at(sampleIndex) != 0) {
			myfile << sampleIndex << ": ";
			for(int pin = 2; pin < 28; pin++){
				if( 1<< pin & sampleSnapshot->at(sampleIndex)){
					int sampleFlag = 0;
					myfile << "\t\t" << pin;
					if (sampleIndex > 0){
						double hz = 0;
						int difference = 0;
						//Check if the previous entry had this pin on, finds first in sequence
						if((1<< pin & sampleSnapshot->at(sampleIndex -1)) == 0){
							if(lastSeenPinAt[pin] != 0){
								difference = ((sampleIndex - lastSeenPinAt[pin]) * 100);
								hz = difference* TO_MICROSECONDS;
							}
							lastSeenPinAt[pin] = sampleIndex;
							myfile << " " << difference  << "us elapsed  aprox hz:" << hz << "\n";
							sampleFlag = 1;

						}else if(sampleIndex < 9999){
							//Check if the next entry had this pin on, finds last in sequence
							if((1 << pin & sampleSnapshot->at(sampleIndex)) == 0){
								if(lastSeenPinAt[pin] != 0){
									difference = ((sampleIndex - lastSeenPinAt[pin]) * 100);
									hz = difference * TO_MICROSECONDS;
								}
								//outputs duration of the pulse
								myfile << " " << difference  << "us elapsed  aprox hz:" << hz << "\n";
								sampleFlag = 1;
								lastSeenPinAt[pin] = sampleIndex;
							}
						}
					}
					if (sampleFlag == 0) myfile << "\n";
				}
			}
			myfile << "\n";
			flag = 0;
		}else{ //No pins set to 1
			if(flag == 0){
				myfile << "=========================================================================\n";
				flag = 1;
			}
		}
	}
}
