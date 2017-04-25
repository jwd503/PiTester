#ifndef MOVINGWINDOW_H
#define MOVINGWINDOW_H
#include <vector>
class MovingWindow{
	private:
		int windowIndex;
		std::vector<int> data;
	public:
		explicit MovingWindow(int size);
		void incrementIndex();
		int getWindowIndex() const;
		int getPreviousWindowIndex() const;
		void setCurrentValue(int value);
		void incrementCurrentValue();
		int getCurrentValue() const;
		int getValue(int index) const;

};

#endif
