

#ifndef UI_H
#define UI_H
#include <msp430.h>
#include <stdint.h>
//p2.6 and p2.7


#ifdef KIT
#define KeyPort P2IN //p6in
#define KeyDir  P2DIR //p6dir
#define KeyBit  BIT6  //bit0


#define BuzzerPort  P8OUT
#define BuzzerDir   P8DIR
#define BuzzerSEL   P8SEL
#define BuzzerBit   BIT5


//p1.7 and p1.6
#define LedRedPort P1OUT //p9out
#define LedRedDir  P1DIR //p9dir
#define LedRedBit  BIT7  //bit3


#define LedGreenPort P1OUT //p9out
#define LedGreenDir  P1DIR //p9dir
#define LedGreenBit  BIT6  //bit2

#define LedEnergyPort P9OUT
#define LedEnergyDir  P9DIR
#define LedEnergyBit  BIT1



#define configurationRS232Port   P3OUT 
#define configurationUSBPort     P3OUT

#define configurationRS232Pin   P3IN 
#define configurationUSBPin     P3IN
//#define ConfigrationOpticalPort P3OUT

#define configurationRS232Dir    P3DIR
#define configurationUSBDir      P3DIR
//#define ConfigrationOpticalDir  P3DIR

#define configurationRS232Bit    BIT1
#define configurationUSBBit      BIT0
//#define ConfigrationOpticalBit  BIT1



#else


#define KeyPort P9IN //P2IN 
#define KeyDir  P9DIR//P2DIR 
#define KeyBit  BIT0 //BIT6  


#define BuzzerPort  P8OUT
#define BuzzerDir   P8DIR
#define BuzzerSEL   P8SEL
#define BuzzerBit   BIT5


//p1.7 and p1.6
#define LedRedPort P8OUT //p9out
#define LedRedDir  P8DIR //p9dir
#define LedRedBit  BIT6  //bit3


//#define LedGreenPort P8OUT //p9out
//#define LedGreenDir  P8DIR //p9dir
//#define LedGreenBit  BIT5  //bit2

#define LedEnergyPort P8OUT
#define LedEnergyDir  P8DIR
#define LedEnergyBit  BIT4



#define configurationRS232Port   P3OUT 
#define configurationUSBPort     P3OUT
//#define ConfigrationOpticalPort P3OUT

#define configurationRS232Pin   P3IN 
#define configurationUSBPin     P3IN
//#define ConfigrationOpticalPort P3OUT

#define configurationRS232Dir    P3DIR
#define configurationUSBDir      P3DIR
//#define ConfigrationOpticalDir  P3DIR

#define configurationRS232Bit    BIT1
#define configurationUSBBit      BIT0
//#define ConfigrationOpticalBit  BIT1

#endif 

/**
* initilizes ports for key pad and buzzor
*/
void InitUI();
/**
* checks if key has been pressed
*/
uint8_t testKey();
/**
* if key is perssed, process the result 
*/
void processKey();

/**
* alarms a single beep sound on the buzzor
*/
void BeepBuzzer();
/**
* turns the buzzor on
*/
void BuzzerOn();
/**
* turns the buzzor off 
*/
void BuzzerOff();
/**
* toggles the buzzors status. if it is on it turns it off otherwise it will turn it on
*/
void ToggleBuzzer();
/**
* routine created to toogle the bezzor authomatically
*/
void ToggleBuzzer2();
/**
* creates a beep sound on the bezzor for specified amount of time at an interval of second
*/
void SecondBeeps();
/**
* turns on the Energy pulse LED
*/
void EnergyLedOn();
/**
* turns off the Energy pulse LED
*/
void EnergyLedOff();
/**
* turns on the red led on the bord
*/
void RedLedOn();
/**
* turns of the red led on the bord
*/
void RedLedOff();

/*[M.G] Restored this function definition from previous version */
void checkConfigrationPortsStatus();


/**
* turns the red led on or off based on the systems status
*/
void UpadateErrorIndicators();

#endif
