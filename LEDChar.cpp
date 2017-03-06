#include "LEDChar.h"

LEDChar::LEDChar(int index){
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
		case default:
			ledSelect = 0;
			break;
	}
}

LEDChar::setSegmentSelect(int segment){
	//0 GPIO26
	//1 GPIO26 + GPIO9
	//2 GPIO26 + GPIO20
	//3 GPIO26 + GPIO9 + GPIO20
	//4 GPIO26 + GPIO23
	//5 GPIO26 + GPIO23 + GPIO9
	//6 GPIO26 + GPIO23 + GPIO20
	//7 GPIO26 + GPI23 + GPIO20 + GPIO9
	int gpio9 = 1 << 9;
	int gpio20 = 1 << 20;
	int gpio23 = 1 << 23;

	segmentSelect = 1 << 26;

	switch(segment){
		case 1:	//A Y0 Top
			//segmentSelect = 0;
			break;
		case 2:	//B Y1 Top right
			segmentSelect |= gpio9;
			break;
		case 3:	//C Y2 Bottom right
			segmentSelect |= gpio20;
			break;
		case 4:	//D Y3 Bottom
			segmentSelect |= gpio9 | gpio20;
			break;
		case 5:	//E Y4 Botom left
			segmentSelect |= gpio23;
			break;
		case 6:	//F Y5 Top Left
			segmentSelect |= gpio23 | gpio9;
			break;
		case 7:	//G Y6 Middle
			segmentSelect |= gpio23 | gpio20;;
			break;
		case 8:	//DP Y7 DP
			segmentSelect |= gpio23 | gpio20 | gpio9;
			break;
	}
}

