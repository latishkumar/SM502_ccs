/*

This file contains the definition for the meter status
including the following

All tampers :- defined in EventTypes.h 
TamperDetected:- overall status of the tamper flags 


MeterDisconnected:- whether the meter is connected to the grid or not 
AutoDisconnectEnabled:- whether the meter can disconnect automatically or not

NewLog:-  new energy log is available status

PowerFailDetected:- when the Power fails 
CommunicationDetected:- if message is received from PLC

ConfigrationDetected:- when the meter is to be reconfigured from the USB or RS232 

KeyReleased:- when the key on the meter is pressed or released 

LoggingTimeIsUp:- status indicating that the 15minute logging time is up 

AutoTransmissionTimeIsUp:- this is used to indicate it is time to meter's power 
                           reading to the central server.                             
*/


#ifndef STATUS_H
#define STATUS_H
#include<stdint.h>

//#include "EventTypes.h"

typedef struct
{

  volatile uint8_t MangneticTamperStatus:1; 
  volatile uint8_t UpperCoverRemovedTamperStatus:1;
  volatile uint8_t LowerCoverRemovedTamperStatus:1;
  volatile uint8_t NeutralTamperStatus:1;
 // volatile uint8_t NeutralTamperTimedoutStatus:1;
  volatile uint8_t MagneticTamperLoggedSatus:1;
  
  volatile uint8_t TamperDetectedStatus:1;
  volatile uint8_t ConfigrationPortTamperStatus:1;
//  uint8_t ReverseConnectionTamperStatus:1;
  
  volatile uint8_t MeterDisconnected:1;
  volatile uint8_t DisconnectCommandRecived:1;
  volatile uint8_t AutoDisconnectEnabled:1; 

  volatile uint8_t ConnectCommandRecived:1;
//  int NewLog:1;

  volatile uint8_t LowTariff:1;
  volatile uint8_t PowerFailDetected:1;

  volatile uint8_t LongPowerFailStatus:1;
  
  volatile uint8_t PLCCommunicationDetected:1;
  volatile uint8_t CommunicationDetected:1;
  volatile uint8_t ConfigurationDetectedStatus:1;

  volatile uint8_t KeyReleased:1;
  volatile uint8_t KeyPressed ;
  
  //volatile uint8_t LoggingTimeIsUp:1;

  volatile uint8_t AutoTransmissionTimeIsUp:1;

  volatile uint8_t DisplayState;//the current state of the display,i.e displaying active energy, reactive energy ,RMS V,RMS C etc...
  volatile uint8_t DisplayUpdateRequired:1;//used to indicate that the power and message area of the LCD needs to be updated
  volatile uint8_t UpdateTime:1;//to indicate the Time on the LCD needs to be Updated
  volatile uint8_t UpdateDate:1;//to indicate the Date on the LCD needs to be updated
  volatile uint8_t DisplayMode;  //0 Auto mode 
                        //1 Manual mode 
                       
  
//  uint8_t RTCResetToDefaultStatus:1;
  volatile uint8_t RTCResetToDefaultStatusAddedToErrorList:1;
  volatile uint8_t RTC_StatusAddedToErrorList:1;
  
  
  volatile uint8_t OverVoltageStatus:1;
  volatile uint8_t OverVoltageTimedOutStatus:1;
  
  volatile uint8_t UnderVoltageStatus:1;
  volatile uint8_t UnderVoltageTimedOutStatus:1;
  
  volatile uint8_t UnderFreqStatus:1;
  volatile uint8_t UnderFreqTimedOutStatus:1;
  
  volatile uint8_t OverFreqStatus:1;
  volatile uint8_t OverFreqTimedOutStatus:1;
  
  volatile uint8_t OverCurrentStatus:1;
  volatile uint8_t OverCurrentTimedOutStatus:1;
  
  volatile uint8_t ActivePowerExcededStatus:1;
  volatile uint8_t ActivePowerExcededTimedOutStatus:1;
  
  volatile uint8_t ActivePowerExcededLoggedStatus:1;
  volatile uint8_t OverCurrentLoggedStatus:1;
  volatile uint8_t OverVoltageLoggedStatus:1;
  volatile uint8_t UnderVoltageLoggedStatus:1;
  volatile uint8_t LongPowerFailLoggedStatus:1;
  volatile uint8_t OverFreqLoggedStatus:1;
  volatile uint8_t UnderFreqLoggedStatus:1;
  volatile uint8_t LongPowerFaileTimedOutStatus:1;
  /*
   1 :Connected 
   0 :Disconnected
  */
  volatile uint8_t RelayStatus:2;
  volatile uint8_t BattryStatus;
  
  volatile uint8_t energy_reverse_flow_tamper:1;
  volatile uint8_t energy_reverse_flow_logged_status:1;
  volatile uint8_t LogEnergy:1;
  volatile uint8_t LogEnergyLogged:1;
  
  volatile uint8_t SecondElapsed:1;
  volatile uint8_t MiuteElapsed:1;
  volatile uint8_t DayChanged:1;
  volatile uint8_t MontheChagned:1;
  volatile uint8_t write_tamper_status:1;
  volatile uint8_t write_status:1;
  
  volatile uint8_t energy_log_overlapped:1;
  volatile uint8_t task_exec_finished:1;
  volatile uint8_t daily_snapshot_energy_overlapped:1;
  volatile uint8_t standard_event_log_overlapped:1;
  volatile uint8_t fraud_event_log_overlapped:1;
  volatile uint8_t power_qual_event_log_overlapped:1;
  volatile uint8_t common_event_log_overlapped:1;
  volatile uint8_t firmware_event_log_overlapped:1;
  volatile uint8_t synchronization_event_log_overlapped:1;
  volatile uint8_t disconnect_event_log_overlapped:1;
  //uint8_t Power_out;
//  uint8_t rtc_init:1;
  
}MeterStatus;

//#define M_status_default {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

typedef enum{
     SYSTEM_INITIALIZING=0,
     SYSTEM_RUNNING,
     SYSTEM_IN_CRITICAL_ERROR

}System_Status_type;

#define Absolute_MAX_VRMS   300
#define Absolute_MAX_IRMS   70 

#define Absolute_MAX_FREQUENCY  57  //the max tolarable frequency error 
#define Absolute_MIN_FREQUENCY  42  

#define MAX_ENERGY_READING  9999999 //the maximum value of energy that can be read
#define MAX_PHASE_ANGLE_ERROR  5   //the maximum phase angle error that can be compensated 

#endif

