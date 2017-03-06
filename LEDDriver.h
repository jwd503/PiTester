#include <vector>

class LEDDriver{
	//Uses GPIO 26,23,29,9 to write data
	//GPIO 18 to clock data in
	//Requires 2 writes to fully select the segment/led display to fire

	private:
		std::vector<LEDChar>
	public:
		void setSegmentSelect(int segment, int charIndex);
		void driveDisplay();

};
