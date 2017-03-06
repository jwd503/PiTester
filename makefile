CC = g++
CFLAGS = -g -O3 -std=c++1y
OBJECTS = main.o PJ_RPI.o TestCase.o Coil.o MovingWindow.o CoilTracker.o Motor.o ErrorReporting.o

main : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o main

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o
	rm main
