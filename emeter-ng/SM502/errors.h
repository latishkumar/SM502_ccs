/*
 * EventTypes.h
 *
 *  Created on: Apr 16, 2013
 *      Author: Donayam
 */


#ifndef ERRORS_H_
#define ERRORS_H_
#include <stdint.h>
//This file contains the event code for all the available event types in the meter
//1 byte is given to all available event codes in the meter
//this implies that up to 128 event types can be supported
//from 0x00 to 0xff


//Tamper Errors 
#define MangneticTamperError 0x1
#define UpperCoverRemovedTamperError 0x2
#define LowerCoverRemovedTamperError 0x3
#define NeutralTamperError 0x04

#define ReverseCurrentError 0x05
#define ConfigrationPortTamperError 0x06
#define ReverseConnectionTamperError 0x07
#define CurrentWithNoVoltageError 0x08


//power errors
#define LowVoltageError 0xA2
#define HighVoltageError 0xA3
#define OverCurrentError 0xA4
#define OverFrequencyError 0xA6
#define UnderFrequencyError 0xA5   
#define ActivePowerExcededError 0xA7

//#define EnergyOverflowError 0xA8
  
//hardware Errors 
#define EEPROM1_Error  0xB1

#define EEPROM2_Error  0xB2
#define PLC_Error      0xB3
#define LCD_Error      0xB4

#define RTC_Error      0xB5
#define RTC_Reset_Error 0xB6
#define LowBattry      0xB7

#define LowBalanceError 0xC4


/**
*   Sets the values of the Error list array to no error indicator sign i.e "#"
*/
void init_ErrorList();
/**
*   Add an error to the Error list array
*   @param Error_No The Error number to add to the error list 
*   @retrun 0 if the error list buffer is full and the current error cannot be added 
*           1 if the error is already  in the list or if it is added sucessfully  
*/
uint8_t AddError(uint8_t Error_No);


/**
*  Removes an error from the Errors list array
*  @param Error_No The Error number to remove from the list of errors 
*  @return   1 removes the error and returns 1 whethere the error was there or not   
*/
uint8_t RemoveError(uint8_t Error_No); 


/**
*    returns the index of the current error number that is displayed on the LCD
*/
uint8_t getCurrentErrorNumber();
/**
* method written to provide the next error number from the error list when called 
* @returns 
* 0: if there are no more errors to display 
* 1: if there is error to display 
*
* @param Error:- the error number to display next 
*/
uint8_t getNextError(uint8_t *Error);

#endif /* EVENTTYPES_H_ */
