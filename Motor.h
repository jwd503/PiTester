#ifndef MOTOR_H
#define MOTOR_H
#include <string>
#include "CoilTracker.h"
#include "ErrorReporting.h"
#include "TestCase.h"
#include <sys/time.h>

class Motor{
	private:
		CoilTracker coilTracker[2];
		int lastFiredIndex;
		int lastFiredCount;
		int errorCount;
		int lastErrorFlag;
		int sameFireCount;
		float frequency;
		ErrorReporting* e;
		double pinMeanCount[4];
		std::string name;
		struct timeval start;
		struct timeval stop;
		struct timeval total;

		typedef struct
		{
			struct timeval start;
			struct timeval stop;
			struct timeval total;
			unsigned int passedStates;
			float frequency;
		}timing;
		timing motorTiming[5];
		unsigned int currentTiming;
		enum State {
			STATE_IDLE	= 0,
			STATE_ONE	= 1,
			STATE_TWO	= 2,
			STATE_THREE	= 3,
			STATE_FOUR	= 4,
		};
		enum Input {
			IN_NONE		= 0,
			IN_ONE		= 1,
			IN_TWO		= 2,
			IN_THREE	= 4,
			IN_FOUR		= 8,
		};

		typedef struct
		{
			enum State source;
			enum State target;
			enum Input input;
		} StateTransition;

		static constexpr StateTransition transitionTable[16] = {
			// Motor starting
			{ STATE_IDLE,	STATE_ONE,	IN_ONE	},
			{ STATE_IDLE,	STATE_TWO,	IN_TWO	},
			{ STATE_IDLE,	STATE_THREE,	IN_THREE},
			{ STATE_IDLE,	STATE_FOUR,	IN_FOUR	},

			// Motor rotations
			{ STATE_FOUR,	STATE_ONE,	IN_ONE	},
			{ STATE_ONE,	STATE_THREE,	IN_THREE},
			{ STATE_THREE,	STATE_TWO,	IN_TWO	},
			{ STATE_TWO,	STATE_FOUR,	IN_FOUR	},

			{ STATE_FOUR,	STATE_TWO,	IN_TWO	},
			{ STATE_TWO,	STATE_THREE,	IN_THREE},
			{ STATE_THREE,	STATE_ONE,	IN_ONE	},
			{ STATE_ONE,	STATE_FOUR, 	IN_FOUR	},

			// Motor stopping
			{ STATE_ONE,	STATE_IDLE,	IN_NONE	},
			{ STATE_TWO,	STATE_IDLE,	IN_NONE	},
			{ STATE_THREE,	STATE_IDLE,	IN_NONE	},
			{ STATE_FOUR,	STATE_IDLE,	IN_NONE	},

		};
		State currentState;
		int passedStates;
		int stateCount;
		int revCount;

	public:
		Motor(int pin1, int pin2, std::string name1,int location1, int pin3, int pin4, std::string name2, int location2, ErrorReporting* errorPointer);
		CoilTracker* getCoilTracker(int index);
		int checkLastFired(int firedIndex);
		void updateLastFired(int firedIndex);
		int checkCoils(int gpioReading);
		int updateCoils(int gpioReading);
		int testMotor(int coilA1Reading[], int pin1, int coilA2Reading[], int pin2, int coilB1Reading[], int pin3, int coilB2Reading[], int pin4);
		int compareOne(int value, Coil* testCoil, int pin);
		void updateState(int pinValues);
};

#endif
