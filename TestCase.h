#ifndef TESTCASE_H
#define TESTCASE_H
#ifndef NREADS
	#define NREADS 32
#endif
#include "ErrorReporting.h"
class TestCase{
	private:
		int expectedResult;
		int outputMask;
	public:
		int readResult[NREADS];
		ErrorReporting* e;
		TestCase(int outputMask, int expectedResult, ErrorReporting* e);
		void setReads(int nReads);
		int compareAll();
		int compareOne(int id);
		int getOutputMask() const;
		int getExpectedResult() const;
};

#endif
