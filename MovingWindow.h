#ifndef MOVINGWINDOW_H
#define MOVINGWINDOW_H
#include <vector>
class MovingWindow{
	private:
		int windowIndex;
		std::vector<int> data;
	public:
		MovingWindow(int size);
		void incrementIndex();
		int getWindowIndex();
		int getPreviousWindowIndex();
		void setCurrentValue(int value);
		void incrementCurrentValue();
		int getCurrentValue();
		int getValue(int index);

};

#endif
