#include "MovingWindow.h"

MovingWindow::MovingWindow(int size){
	data.reserve(size);
	for(int i = 0; i < size; i++){
		data.push_back(0);
	}
	windowIndex = 0;
}

int MovingWindow::getWindowIndex() const{
	return windowIndex;
}

int MovingWindow::getPreviousWindowIndex() const{
	int index = windowIndex - 1;
	if(windowIndex - 1 < 0){
		index = data.capacity() - 1;
	}
	return index;
}

int MovingWindow::getValue(int index) const{
	return data[index];
}

void MovingWindow::incrementIndex(){
	windowIndex++;
	windowIndex = (((unsigned int)windowIndex >= data.size()) ? 0: windowIndex);
}

void MovingWindow::setCurrentValue(int value){
	if((unsigned int)windowIndex < data.size()){
		data[windowIndex] = value;
	}
}

int MovingWindow::getCurrentValue() const{
	return data[windowIndex];
}

void MovingWindow::incrementCurrentValue(){
	if((unsigned int)windowIndex < data.size()){
		data[windowIndex]++;
	}
}
