#include "LEDChar.h"
#include "PJ_RPI.h"

LEDChar::LEDChar(int index, int segments){
	segmentSelect = 0;
	ledSelect = 0;
	switch(index){
		case 0:
			ledSelect = 1<<9;
			break;
		case 1:
			ledSelect = 1<<23;
			break;
		case 2:
			ledSelect = 1<<6;
			break;
		case 3:
			ledSelect = 1<<26;
			break;
	}
	this->segments = segments;
	colon = false;
}

void LEDChar::setSegmentSelect(int segment){
	//GPIO26 controls the colon
	//GPIO6 and 23 swapped from the original diagram
	//0 GPIO26
	//1 GPIO26 + GPIO9
	//2 GPIO26 + GPIO6
	//3 GPIO26 + GPIO9 + GPIO6
	//4 GPIO26 + GPIO23
	//5 GPIO26 + GPIO23 + GPIO9
	//6 GPIO26 + GPIO23 + GPIO6
	//7 GPIO26 + GPI23 + GPIO6 + GPIO9

	segmentSelect = 0;
	if(colon == true) segmentSelect = 1 << 26;

	switch(segment){
		case 0:	//A Y0 Top
			//segmentSelect = 0;
			break;
		case 1:	//B Y1 Top right
			segmentSelect |= gpio9;
			break;
		case 2:	//C Y2 Bottom right
			segmentSelect |= gpio23;
			break;
		case 3:	//D Y3 Bottom
			segmentSelect |= gpio9 | gpio23;
			break;
		case 4:	//E Y4 Botom left
			segmentSelect |= gpio6;
			break;
		case 5:	//F Y5 Top Left
			segmentSelect |= gpio6 | gpio9;
			break;
		case 6:	//G Y6 Middle
			segmentSelect |= gpio6 | gpio23;;
			break;
		case 7:	//DP Y7 DP
			segmentSelect |= gpio6 | gpio23 | gpio9;
			break;
	}
}

int LEDChar::getLEDPins(){
	return gpio9 | gpio23 | gpio6 | (1<<26);
}

void LEDChar::clearRegister(){
	//set the shift register pins to outputs
	INP_GPIO(9);
	OUT_GPIO(9);
	INP_GPIO(18);
	OUT_GPIO(18);
	INP_GPIO(6);
	OUT_GPIO(6);
	INP_GPIO(23);
	OUT_GPIO(23);
	INP_GPIO(26);
	OUT_GPIO(26);

	//Reset the shift register values
	GPIO_CLR = getLEDPins();
	GPIO_SET = 1<<18;
	GPIO_CLR = 1<<18;
	GPIO_SET = 1<<18;
	GPIO_CLR = 1<<18;

	//Set the pins back to input mode
	INP_GPIO(9);
	INP_GPIO(18);
	INP_GPIO(6);
	INP_GPIO(23);
	INP_GPIO(26);

}

int LEDChar::decode(int bit){
	//segments 0-127 1 bit per segment
	if (segments & (1<<bit)){
		setSegmentSelect(bit);
		return segmentSelect;
	}
	segmentSelect = 0;
	return 0;
}

void LEDChar::setSegments(int segments){
	this->segments = segments;
}
