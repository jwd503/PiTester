CC = g++
CXXFLAGS = -std=c++1y -pedantic -Wall -Wextra -Wformat -Wno-missing-field-initializers -O3
OBJECTS = main.o PJ_RPI.o TestCase.o Coil.o MovingWindow.o CoilTracker.o Motor.o ErrorReporting.o ErrorInfo.o LEDChar.o LEDDriver.o

LDIR =../lib

LIBS=-lrt


main : $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LIBS) $(OBJECTS) -o main

%.o : %.cpp
	$(CC) $(CXXFLAGS) -c $<

debug : CXXFLAGS += -g3 -O0
debug : main

clean:
	rm *.o
	rm main
