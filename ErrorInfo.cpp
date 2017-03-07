#include "ErrorInfo.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#define MICROSECONDS 1000000.0

ErrorInfo::ErrorInfo(int* samples, int eIndex, int eCode){
	sampleSnapshot =  new std::vector<int>;
	sampleSnapshot->insert(sampleSnapshot->begin(), samples, samples + 10000);

	errorIndex = eIndex;
	errorCode = eCode;
	std::string filename = "logs/";
	char numstr[21]; // enough to hold all numbers up to 64-bits
        sprintf(numstr, "%d", errorCode);
	filename = filename + numstr;
	dumpToFile(filename.c_str());

        int problemDetail = (errorCode & (0xF << 10)) >> 10;
        int location = (errorCode & (0x3F << 4)) >> 4;
	printf("problemdetail: %d, location: %d\n",problemDetail, location);
	switch(problemDetail){
		case 0x0: //No detail
                        break;
                case 0x1: //Pin level
                        appendPinLevel(location);
                        break;
                case 0x2: //Coil terminal level
			appendCoilTerminalLevel(location);
                        break;
                case 0x3: //Coil level
			appendCoilLevel(location);
                        break;
                case 0x4: //Motor level
			appendMotorLevel(location);
                        break;
	}

	frequency = calculateFrequency();

}

void ErrorInfo::dumpToFile(const char* filename){

	std::ofstream myfile;
	myfile.open(filename);
	int lastSeenPinAt[32] = {0};
	int flag = 0;
	printf("Size: %d\n",sampleSnapshot->size());
	for (int sampleIndex = 0; sampleIndex < sampleSnapshot->size(); sampleIndex++){
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
								hz = MICROSECONDS / difference;
							}
							lastSeenPinAt[pin] = sampleIndex;
							myfile << " " << difference  << "us elapsed  aprox hz:" << hz << "\n";
							sampleFlag = 1;

						}else if(sampleIndex < 9999){
							//Check if the next entry had this pin on, finds last in sequence
							if((1 << pin & sampleSnapshot->at(sampleIndex)) == 0){
								if(lastSeenPinAt[pin] != 0){
									difference = ((sampleIndex - lastSeenPinAt[pin]) * 100);
									hz = MICROSECONDS / difference;
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

int ErrorInfo::calculateFrequency(){

	std::vector<int> lastSeenPinAt;
	std::vector<double> pinFrequency;
	std::vector<int> pinDifference;
	lastSeenPinAt.reserve(pins.size());
	pinFrequency.reserve(pins.size());
	pinDifference.reserve(pins.size());

	for(int pinIndex = 0; pinIndex < pins.size(); pinIndex++){
		lastSeenPinAt[pinIndex] = 0;
		pinFrequency[pinIndex] = 0;
		pinDifference[pinIndex] = 0;
        }

	int startIndex = errorIndex + 1;
	bool fullLoop = false;
	int sampleIndex = startIndex;
	while(fullLoop != true){
		int nextIndex = (sampleIndex + 1) % (sampleSnapshot->size() - 1);
		int previousIndex =  (sampleIndex - 1) < 0 ? (sampleSnapshot->size() - 1): (sampleIndex -1);
		int pinIndex = 0;
		for(pinIndex = 0; pinIndex < pins.size(); pinIndex++){
//			pinDifference[pinIndex] = 0;
			if( 1<<pins[pinIndex] & sampleSnapshot->at(sampleIndex)){
				if( (1<<pins[pinIndex] & sampleSnapshot->at(previousIndex)) == 0){
					if(lastSeenPinAt[pinIndex] != 0){
						pinDifference[pinIndex] = ((sampleIndex - lastSeenPinAt[pinIndex]) * 100);
						if(sampleIndex < lastSeenPinAt[pinIndex]){
							pinDifference[pinIndex] = (((sampleSnapshot->size() -1) - lastSeenPinAt[pinIndex]) + sampleIndex) * 100;
						}

						pinFrequency[pinIndex] = MICROSECONDS / pinDifference[pinIndex];
					}
					lastSeenPinAt[pinIndex] = sampleIndex;
				}
			}
		}
		sampleIndex = nextIndex;
		fullLoop = sampleIndex == startIndex;
	}

	for(int pinIndex = 0; pinIndex < pins.size(); pinIndex++){

		printf("pinFreq[%d]: %f, %i, %i\n", pinIndex, pinFrequency[pinIndex], pinDifference[pinIndex], pins[pinIndex]);

	}

}

void ErrorInfo::appendPinLevel(int location){
	pins.reserve(1);
	pins.push_back(location);
}

void ErrorInfo::appendCoilTerminalLevel(int location){
	pins.reserve(1);
	switch(location){
		case 0x0:
                        pins.push_back(4); // M0.A0
                        break;
                case 0x1:
                        pins.push_back(12);// M0.A1
                        break;
                case 0x2:
			pins.push_back(16);// M0.B0
                        break;
                case 0x3:
                        pins.push_back(14);// M0.B1
                        break;
                case 0x4:
			pins.push_back(21);// M1.A0
                        break;
                case 0x5:
                        pins.push_back(25);// M1.A1
                        break;
                case 0x6:
                        pins.push_back(8); // M1.B0
                        break;
                case 0x7:
                        pins.push_back(7); // M1.B1
                        break;
                case 0x8:
                        pins.push_back(2); // M2.A0
                        break;
                case 0x9:
                        pins.push_back(3); // M2.A1
                        break;
                case 0xA:
                        pins.push_back(17);// M2.B0
                        break;
                case 0xB:
                        pins.push_back(27);// M2.B1
                        break;
                case 0xC:
                        pins.push_back(5); // M3.A0
                        break;
                case 0xD:
                        pins.push_back(10);// M3.A1
                        break;
                case 0xE:
			pins.push_back(13);// M3.B0
			break;
		case 0xF:
			pins.push_back(19);// M3.B1
			break;
	}
}

void ErrorInfo::appendCoilLevel(int location){
	pins.reserve(2);
	switch(location){
                case 0x1:
                        pins.push_back(4); // M0.A0
                        pins.push_back(12);// M0.A1
                        break;
                case 0x0:
                        pins.push_back(16);// M0.B0
                        pins.push_back(14);// M0.B1
                        break;
                case 0x3:
                        pins.push_back(21);// M1.A0
                        pins.push_back(25);// M1.A1
                        break;
                case 0x2:
                        pins.push_back(8); // M1.B0
                        pins.push_back(7); // M1.B1
                        break;
                case 0x5:
                        pins.push_back(2); // M2.A0
                        pins.push_back(3); // M2.A1
                        break;
                case 0x4:
                        pins.push_back(17);// M2.B0
                        pins.push_back(27);// M2.B1
                        break;
        	case 0x7:
                        pins.push_back(5); // M3.A0
                        pins.push_back(10);// M3.A1
                        break;
                case 0x6:
                        pins.push_back(13);// M3.B0
                        pins.push_back(19);// M3.B1
                        break;
        }
}

void ErrorInfo::appendMotorLevel(int location){
	pins.reserve(4);
	switch(location){
                case 0x0:
                        pins.push_back(4); // M0.A0
                        pins.push_back(12);// M0.A1
                        pins.push_back(16);// M0.B0
                        pins.push_back(14);// M0.B1
                        break;
                case 0x1:
                        pins.push_back(21);// M1.A0
                        pins.push_back(25);// M1.A1
                        pins.push_back(8); // M1.B0
                        pins.push_back(7); // M1.B1
                        break;
                case 0x2:
                        pins.push_back(2); // M2.A0
                        pins.push_back(3); // M2.A1
                        pins.push_back(17);// M2.B0
                        pins.push_back(27);// M2.B1
                        break;
        	case 0x3:
                        pins.push_back(5); // M3.A0
                        pins.push_back(10);// M3.A1
                        pins.push_back(13);// M3.B0
                        pins.push_back(19);// M3.B1
                        break;
        }
}


ErrorInfo::~ErrorInfo(){
	delete sampleSnapshot;
}
