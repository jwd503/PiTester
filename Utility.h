#ifndef UTILITY_H
#define UTILITY_H
#include "PJ_RPI.h"
#include <time.h>
#include <sys/time.h>

void delayMicroseconds (unsigned int howLong);
int flashLED(unsigned int duration, unsigned int delayBeforeRead);
void setPullDown();
void setPullDown(int mask);
void setPullUp(int mask);
void setPull(int mask, int direction);
int interrupts(int flag);


inline int flashLED(unsigned int duration, unsigned int delayBeforeRead){
	OUT_GPIO(6);
	int ledMask = 1 << 6;
	if(delayBeforeRead > 0){
		delayMicroseconds(delayBeforeRead);
		int result = GPIO_READMULT(0x4840200);
		if(result != 0){
			printf("Whoops\n");
			return 1;
		}
	}

	GPIO_SET = ledMask;
	delayMicroseconds(duration);
	GPIO_CLR = ledMask;
	return 0;
}

inline void delayMicrosecondsHard (unsigned int howLong)
 {
   struct timeval tNow, tLong, tEnd ;

   gettimeofday (&tNow, NULL) ;
   tLong.tv_sec  = howLong / 1000000 ;
   tLong.tv_usec = howLong % 1000000 ;
   timeradd (&tNow, &tLong, &tEnd) ;

   while (timercmp (&tNow, &tEnd, <))
     gettimeofday (&tNow, NULL) ;
}

inline void delayMicroseconds (unsigned int howLong)
 {
   struct timespec sleeper ;
   unsigned int uSecs = howLong % 1000000 ;
   unsigned int wSecs = howLong / 1000000 ;

   /**/ if (howLong ==   0)
     return ;
   else if (howLong  < 100)
     delayMicrosecondsHard (howLong) ;
   else
   {
     sleeper.tv_sec  = wSecs ;
     sleeper.tv_nsec = (long)(uSecs * 1000L) ;
     nanosleep (&sleeper, NULL) ;
   }
 }

inline void setPullDown(){
	setPull(0x7FFFFFC,1);
}

inline void setPullDownMasked(int mask){
	setPull(mask, 1);
}

inline void setPullUp(int mask){
	setPull(mask, 2);
}

inline void setPull(int mask, int direction){
        GPIO_PULL = direction;
        //wait 150 cycles
        delayMicroseconds(5);
        GPIO_PULLCLK0 = mask;
        //wait 150 cycles
        delayMicroseconds(5);
        GPIO_PULL = 0;
        delayMicroseconds(5);
        GPIO_PULLCLK0 = 0;
        delayMicroseconds(5);


}

inline int bitwiseAverageArray(int array[], int size){
	int bit = 0;
	int result = 0;
	int count[32] = {0};
	int arrayIndex = 0;

	for(arrayIndex = 0; arrayIndex < size; arrayIndex++){
		for(bit = 0; bit < 32; bit++){
			count[bit] += (array[arrayIndex] & 1 << bit) > 0? 1: 0;
		}
	}
	for(bit = 0; bit < 32; bit++){
		result |= ((count[bit] > ((size*2)/ 4)) ?1:0) << bit;
	}
	return result;
}

inline int generateGPIOReadMask(){
	int mask = 0;
	int readpins[32] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26, 27};
        int i = 0;
        //Generate a mask from the list of pins, used when reading from the gpio bank
        for (i = 0; i < 32; i++) {
                mask |= readpins[i] == 0? 0: 1 << readpins[i];
        }
	return mask;
}

inline void setAllPinsToInp(){
	 int pin = 0;
	//Set all pins to inputs
	for(pin=0; pin < 28; pin++){
		INP_GPIO(pin);
	}

}

inline int interrupts(int flag)
	{
	static unsigned int sav131 = 0;
	static unsigned int sav132 = 0;
	static unsigned int sav133 = 0;
	static unsigned int sav134 = 0;
	static unsigned int sav135 = 0;

	if(flag == 0)    // disable
	{
		if(sav132 != 0)
		{
		// Interrupts already disabled so avoid printf
			return(0);
		}

		if( (*(inter.addr+128) | *(inter.addr+129) | *(inter.addr+130)) != 0)
		{
			//printf("Pending interrupts\n");  // may be OK but probably
			//return(0);                       // better to wait for the
		}                                // pending interrupts to
                                       // clear
		sav131 = *(inter.addr+131);
		*(inter.addr+131) &= ~(1 << 6);
		sav134 = *(inter.addr+134);//IRQ_1);
		*(inter.addr+137) = sav134;
		sav132 = *(inter.addr+132);//IRQ_2);  // save current interrupts
		*(inter.addr+135) = sav132;  // disable active interrupts
		sav133 = *(inter.addr+133);//IRQ_BASIC);
		*(inter.addr+136) = sav133;
		sav135 = *(gpu.addr+0xC);
		*(gpu.addr+0xC) = sav135 | 15;
	}
	else            // flag = 1 enable
	{
		if(sav132 == 0)
		{
			printf("Interrupts not disabled\n");
			return(0);
		}
		*(inter.addr+131) = sav131;
		*(inter.addr+132) = sav132;    // restore saved interrupts
		*(inter.addr+133) = sav133;
		*(inter.addr+134) = sav134;
		*(gpu.addr+0xC)   = sav135;
		sav132 = 0;                 // indicates interrupts enabled
	}
	return(1);
}

inline void setOutputs(int outputmask){
        int pin = 0;
        int pinmask = 0;
        for(pin = 0; pin < 32; pin++){
                pinmask = 1 << pin;
                int result = outputmask & pinmask;
                if(result > 0){
                        INP_GPIO(pin);
                        OUT_GPIO(pin);
                }
        }
}

#endif
