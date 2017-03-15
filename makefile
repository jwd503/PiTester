CC = g++
CXXFLAGS = -g3 -std=c++1y
OBJECTS = main.o PJ_RPI.o TestCase.o Coil.o MovingWindow.o CoilTracker.o Motor.o ErrorReporting.o ErrorInfo.o

LDIR =../lib

LIBS=-lrt


main : $(OBJECTS)
	$(CC) $(CXXFLAGS) $(LIBS) $(OBJECTS) -o main

%.o : %.c
	$(CC) $(CXXFLAGS) -c $<

clean:
	rm *.o
	rm main
