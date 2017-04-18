#include "LEDChar.h"
#include "PJ_RPI.h"

LEDChar::LEDChar(int index, int segments){
	switch(index){
		case 0:
			ledSelect = 1<<9;
			break;
		case 1:
			ledSelect = 1<<20;
			break;
		case 2:
			ledSelect = 1<<23;
			break;
		case 3:
			ledSelect = 1<<26;
			break;
		default:
			ledSelect = 0;
			break;
	}
	this->segments = segments;
}

void LEDChar::setSegmentSelect(int segment){
	//0 GPIO26
	//1 GPIO26 + GPIO9
	//2 GPIO26 + GPIO20
	//3 GPIO26 + GPIO9 + GPIO20
	//4 GPIO26 + GPIO23
	//5 GPIO26 + GPIO23 + GPIO9
	//6 GPIO26 + GPIO23 + GPIO20
	//7 GPIO26 + GPI23 + GPIO20 + GPIO9

	segmentSelect = 1 << 26;

	switch(segment){
		case 0:	//A Y0 Top
			//segmentSelect = 0;
			break;
		case 1:	//B Y1 Top right
			segmentSelect |= gpio9;
			break;
		case 2:	//C Y2 Bottom right
			segmentSelect |= gpio20;
			break;
		case 3:	//D Y3 Bottom
			segmentSelect |= gpio9 | gpio20;
			break;
		case 4:	//E Y4 Botom left
			segmentSelect |= gpio23;
			break;
		case 5:	//F Y5 Top Left
			segmentSelect |= gpio23 | gpio9;
			break;
		case 6:	//G Y6 Middle
			segmentSelect |= gpio23 | gpio20;;
			break;
		case 7:	//DP Y7 DP
			segmentSelect |= gpio23 | gpio20 | gpio9;
			break;
	}
}

int LEDChar::getLEDPins(){
	return gpio9 | gpio23 | gpio20 | (1<<26);
}

void LEDChar::clearRegister(){
	INP_GPIO(9);
	OUT_GPIO(9);
	INP_GPIO(18);
	OUT_GPIO(18);
	INP_GPIO(20);
	OUT_GPIO(20);
	INP_GPIO(23);
	OUT_GPIO(23);
	INP_GPIO(26);
	OUT_GPIO(26);

	GPIO_CLR = getLEDPins();
	GPIO_SET = 1<<18;
	GPIO_CLR = 1<<18;
	GPIO_SET = 1<<18;
	GPIO_CLR = 1<<18;
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
