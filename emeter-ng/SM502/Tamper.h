/*
*/


#ifndef TAMPER_H
#define TAMPER_H

#include <msp430.h>
#include <stdint.h>


#define UC_TamperDIR P1DIR//P2DIR
#define LC_TamperDIR P1DIR////P2DIR
#define Magnetic_TamperDIR P2DIR
//#define ConfigrationPort_TamperDIR P2DIR

#define UC_TamperPIN P1IN//P2IN
#define LC_TamperPIN P1IN//P2IN
#define Magnetic_TamperPIN P2IN
//#define ConfigrationPort_TamperPIN P2IN

#define UC_TamperBIT  BIT0//BIT7
#define LC_TamperBIT  BIT1//BIT3
#define Magnetic_TamperBIT BIT7//BIT4
//#define ConfigrationPort_TamperBIT BIT5

#define UC_TamperSEL P1SEL
#define LC_TamperSEL P1SEL//P2SEL

#define UC_IE P1IE//P2IE
#define LC_IE P1IE//P2IE

#define UC_IES P1IES//P2IES
#define LC_IES P1IES//P2IES
#define LC_SEL P1SEL
#define UC_SEL P1SEL 
#define COVER_IF P1IFG

#define CTIV P2IV
#define MINUTE 60000

enum
{
	 MagneticTamperType = 0,
	 UC_TamperType = 1,
	 LC_TamperType = 2,
	 NeutralTamperType = 3,
	 EarthTamperType = 4,
	 ReverseConnactionTamperType = 5,
	 MeterBypassTamperType = 6,
	 ConfigrationPortTamperType =7
};



/**
 * Initializes the tamper module by 
 *      Make the Port Directions Input
 *      read last tamper status from EERPOM
 */
void initTampperSwitchs();



/**
* checks tamper status and updates the display accordingly.
*/
void UpdateTamperIndicators();
/**
* checks the magnetic sensor and logs the magnetic tamper detected event.
*/
void CheckMagneticTamper();
/**
* checks if the upper cover is open and calls UCTamperDetected to log the event
* @see UCTamperDetected
*/
void ProcessTamperUC();
/**
* checks if the lower cover is open and calls the LCTamperDetected to log the event
* @see LCTamperDetected
*/
void ProcessTamperLC();
/**
* logs the low cover tamper event 
* @see ProcessTamperLC
*/
void LCTamperDetected();
/**
* logs the upper coover tamper event
* @see ProcessTamperUC
*/
void UCTamperDetected();

#endif
