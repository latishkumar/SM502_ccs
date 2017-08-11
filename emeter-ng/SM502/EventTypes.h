/*
 * EventTypes.h
 *
 *  Created on: Apr 16, 2013
 *      Author: Donayam
 */


#ifndef EVENTTYPES_H_
#define EVENTTYPES_H_

//This file contains the event code for the available event types in the meter
//1 byte is given to all available event codes in the meter
//this implies that up to 128 event types can be supported
//from 0x00 to 0xff



//Tamper Event codes
//#define MangneticTamperEventCode 0x1
//#define UpperCoverRemovedTamperEventCode 0x2
//#define LowerCoverRemovedTamperEventCode 0x3
//#define NeutralTamperEventCode 0x04
//#define ReverseCurrentEventCode 0x05
//#define ConfigrationPortTamperEventCode 0x06


//Configuration and Calibration
#define ConfigrationDetected  0x50
#define MeterCalibrated 0x51
//Standard event types
//Power condition events
#define PowerOut  0xA0
#define PowerUp   0xA1
#define FIRST_BOOT                        0xB0
#define RTC_INVALID_TIME_AFTER_CORRECTED  0xB1
#define INCORRECT_TIMESTAMP               0xB2
#define RTC_INVALID_TIME_BEFORE_CORRECTED 0xB3
#define BACKUP_RESTORED_FROM_FLASH_LB     0XB4
#define BACKUP_RESTORED_FROM_EEPROM       0XB5
#define BACKUP_RESTORED_FROM_FLASH_NB     0XB6
#define BACKUP_RESTORATION_FAILED         0XB7
#define BOR_RESET_EVENT                   0xB8

//#define LowVoltage 0xA2
//#define HighVoltage 0xA3
//#define OverCurrent 0xA4
//#define OverFrequency 0xA6
//#define UnderFrequency 0xA5   
//#define ActivePowerExceded 0xA7
#define LongPowerFailer 0xA8



//Meter Connect Event
#define MeterTariffConnected 0xC0
#define MeterConfigrationConnected 0xC1
#define MeterCommandConnected 0xC2
#define MeterConnectAfterRestart 0xC3
//Charging Events
#define MeterTariffCharged 0xD0

//Meter Disconnect Event
#define MeterCommandDisconnect 0xD1
#define MeterTariffDisconnected 0xD2  
#define MeterOverPowerDisconnect 0xD3 
#define MeterOverCurrentDisconnect 0xD4   
#define MeterOverVoltageDisconnect 0xD5   
#define MeterConfigrationDisconnect 0xD6
#define MeterDisconnectAfterRestart 0xD7

#define EEPROM_Formatted 0xE0
#define CLOCK_PROGRAMMED 0xE2
#define TAMPER_EVENT_CLEARED 0xE3
   
//Communication event
#define BEGIN_COMMUNICATION_PLC_PORT 1
#define END_COMMUNICATION_PLC_PORT   2
#define BEGIN_COMMUNICATION_OPTICAL_PORT 3
#define END_COMMUNICATION_OPTICAL_PORT   4
#define BEGIN_COMMUNICATION_SERIAL_PORT 5
#define END_COMMUNICATION_SERIAL_PORT   6
#endif /* EVENTTYPES_H_ */
