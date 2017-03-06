#ifndef TESTCASE_H
#define TESTCASE_H
#ifndef NREADS
	#define NREADS 32
#endif
class TestCase{
	private:
		int expectedResult;
		int outputMask;
	public:
		int readResult[NREADS];

		TestCase(int outputMask, int expectedResult);
		void setReads(int nReads);
		int compareAll();
		int compareOne(int id);
		int getOutputMask();
		int getExpectedResult();
};

#endif
