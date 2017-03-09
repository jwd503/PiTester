#include "MovingWindow.h"

MovingWindow::MovingWindow(int size){
	data.reserve(size);
	int i = 0;
	for(i = 0; i < size; i++){
		data.push_back(0);
	}
	windowIndex = 0;
}

int MovingWindow::getWindowIndex(){
	return windowIndex;
}

int MovingWindow::getPreviousWindowIndex(){
	int index = windowIndex - 1;
	if(windowIndex - 1 < 0){
		index = data.capacity() - 1;
	}
	return index;
}

int MovingWindow::getValue(int index){
	return data[index];
}

void MovingWindow::incrementIndex(){
	windowIndex++;
	windowIndex = windowIndex > data.size() ? 0: windowIndex;
}

void MovingWindow::setCurrentValue(int value){
	data[windowIndex] = value;
}

int MovingWindow::getCurrentValue(){
	return data[windowIndex];
}

void MovingWindow::incrementCurrentValue(){
	data[windowIndex]++;
}
