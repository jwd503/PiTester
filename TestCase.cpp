#include "TestCase.h"
#include <stdio.h>
TestCase::TestCase(int outputMask, int expectedResult):
	readResult{0}{
	this->outputMask = outputMask;
	this->expectedResult = expectedResult;
}


int TestCase::compareAll(){
	int i = 0;
	int result = 0;
	//Skip the first result as signal propagation errors show up here
	for(i = 8; i < NREADS-6; i++){
		int tempResult = compareOne(i);
		if(tempResult != 0){
			printf("Read no:%d\n", i);
		}
		result |= tempResult;
	}

	return (result == 1)? 0: 1;
}

int TestCase::compareOne(int id){
	if(id < NREADS){
//		printf("observed: %d, expected: %d\n", (readResult[id] & ~(1 << 6)) ,(expectedResult  & ~(1 << 6)));
		int mask = ~(1<<6);// & ~(1<<2) & ~(1<<3);
		if ((readResult[id] & mask) == (expectedResult  & mask)){
			return 0;
		} else{
			printf("observed: %d, expected: %d\n", (readResult[id] & ~(1 << 6)) ,(expectedResult  & ~(1 << 6)));

			return 1;
		}
	} else{
		return 1;
	}
}

int TestCase::getOutputMask() const{
	return outputMask;
}

int TestCase::getExpectedResult() const{
	return expectedResult;
}
