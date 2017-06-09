#include "Tamper.h"
#include "Status.h"
//#include "graphics.h"
#include "LCD_C_Graphics.h"

#include "EventTypes.h"
//#include "RTCC.h"
#include "Logger.h" 
#include <stdint.h>
#include "errors.h"

#include "PLC/PLC.h"
#include "Schaduler.h"

extern MeterStatus status;
extern rtc_t rtcc;
extern Tamper_Count TamperCount;
extern unsigned long LastEventLogAddress;

extern uint16_t CoverRemovalTripDuration;

int tamper_InitError = 0;

#define UCtamperLogged 1
#define LCtamperLogged 2
#define MtamperLogged  4
#define ConftamperLogged 8

uint8_t tamperLoggedStatus;

volatile uint8_t UCTamperSchaduled = 0;
volatile uint8_t LCTamperSchaduled = 0;
volatile uint8_t MagneticTamperSchaduled = 0;

uint16_t UCTamperCounter = 0;
uint16_t LCTamperCounter = 0;
uint16_t MagneticTamperCounter = 0;
uint16_t NeutralTamperCounter = 0;

#define LC_tamperVisibility 1
#define UC_tamperVisibility 2
#define M_tamperVisibility 4
#define N_tamperVisibility 8
#define RF_tamperVisibility 16

uint8_t tamper_Visibility = 0;
uint16_t init_tamper_time=5000;
/**
 * Initializes the tamper module by 
 *      Make the Port Directions Input
 *      read last tamper status from EERPOM
 */
void initTamper()
{
    COVER_IF = 0;   //clear all pending interrupts as this might be noice
  	/*
	 * make all port tamper port directions as input
	 */
	UC_TamperDIR &= ~UC_TamperBIT;
	LC_TamperDIR &= ~LC_TamperBIT;
	Magnetic_TamperDIR &= ~Magnetic_TamperBIT;
        
    LC_IE |= LC_TamperBIT; //Rising edge is selected as trigger source
    UC_IE |= UC_TamperBIT;
    LC_IES &= ~LC_TamperBIT;
    UC_IES &= ~UC_TamperBIT;
        
    validation_arg_t validation_arg;
    validation_arg.validate = 0;
    read_from_eeprom(&status,(uint8_t *)0,getMeterStatus,&validation_arg);
}
/**
 * Initializes the tamper module by 
 *      Make the Port Directions Input
 *      read last tamper status from EERPOM
 * adds delay startup
 */
void initTampperSwitchs()
{
                                            
    ScaduleTask(initTamper,init_tamper_time,GENERAL_TASK);
    
}
/**
* checks the magnetic sensor and logs the magnetic tamper detected event.
*/
void CheckMagneticTamper()
{
       //check Magnetic Tamper
      //TODO. log magnetic error to EEPROM
        uint8_t z = Magnetic_TamperPIN;
        
	if(((z & Magnetic_TamperBIT) != Magnetic_TamperBIT)&&(status.MagneticTamperLoggedSatus!=1))
	{
		status.MangneticTamperStatus = 1;
                status.MagneticTamperLoggedSatus = 1;
                TamperCount.Magnetic_Count++;
                
                uint8_t temp82 = TamperCount.Magnetic_Count;
                uint8_t temp8 = MagneticTamperType;
                write_to_eeprom(&temp82,&temp8,setTamperCount); 
                
//                setTamperCount( MagneticTamperType, TamperCount.Magnetic_Count );
	}
        else
        {
		status.MangneticTamperStatus = 0;      
                status.MagneticTamperLoggedSatus = 0;
        }      
}
/**
* checks tamper status and updates the display accordingly.
*/

void UpdateTamperIndicators()
{
  
  uint8_t temp=0;
  uint8_t sc_task=0;
  if(status.LowerCoverRemovedTamperStatus == 1)
  {
    temp|=0x01;
    
    if((tamper_Visibility & LC_tamperVisibility) != LC_tamperVisibility)
    {
      ShowTamper_c(LC_TamperType);
      AddError(LowerCoverRemovedTamperError);
      tamper_Visibility|=LC_tamperVisibility;
      sc_task=1;
    }
  }
  else
  {
    if((tamper_Visibility & LC_tamperVisibility) == LC_tamperVisibility)
    {
     ClearTamper_c(LC_TamperType);
     RemoveError(LowerCoverRemovedTamperError);
     tamper_Visibility &= ~LC_tamperVisibility;
    }
  }
  
  
  if(status.UpperCoverRemovedTamperStatus == 1)
  {
    temp|=0x02;
    if((tamper_Visibility & UC_tamperVisibility) != UC_tamperVisibility)
    {
      ShowTamper_c(UC_TamperType);
      AddError(UpperCoverRemovedTamperError);
      tamper_Visibility|= UC_tamperVisibility;
      sc_task=1;
    }
  }
  else
  {
    if((tamper_Visibility & UC_tamperVisibility) == UC_tamperVisibility)
    {
      ClearTamper_c(UC_TamperType);
      RemoveError(UpperCoverRemovedTamperError);
      tamper_Visibility &= ~UC_tamperVisibility;
    }
  }
  
  
  CheckMagneticTamper();
  if(status.MangneticTamperStatus == 1)
  {
    temp|=0x04;
    if((tamper_Visibility & M_tamperVisibility) != M_tamperVisibility)
    {
       ShowTamper_c(MagneticTamperType);
       AddError(MangneticTamperError);
       tamper_Visibility|= M_tamperVisibility;
       sc_task=1;
    }
  }
  else
  {
    if((tamper_Visibility & M_tamperVisibility) == M_tamperVisibility)
    {
      ClearTamper_c(MagneticTamperType);
      RemoveError(MangneticTamperError);
      tamper_Visibility&=~M_tamperVisibility;
    }
  }
  
  
  if(status.NeutralTamperTimedoutStatus == 1)
  {
    temp|=0x08;
    if((tamper_Visibility & N_tamperVisibility) != N_tamperVisibility)
    {
      ShowTamper_c(NeutralTamperType);
      AddError(NeutralTamperError);
      tamper_Visibility|=N_tamperVisibility;
      sc_task=1;
    }
  }
  else
  {
     if((tamper_Visibility & N_tamperVisibility) == N_tamperVisibility)
     {
       ClearTamper_c(NeutralTamperType);
       RemoveError(NeutralTamperError);
       tamper_Visibility&=~N_tamperVisibility;
     }

  }
  
  if(status.energy_reverse_flow_tamper == 1)
  {
    temp|=0x10;
    if((tamper_Visibility & RF_tamperVisibility) != RF_tamperVisibility)
    {
      ShowTamper_c(ReverseConnactionTamperType);
      AddError(ReverseConnectionTamperError);
      tamper_Visibility|=RF_tamperVisibility;
      sc_task=1;
    }
  }
  else
  {
     if((tamper_Visibility & RF_tamperVisibility) == RF_tamperVisibility)
     {
       ClearTamper_c(ReverseConnactionTamperType);
       RemoveError(ReverseConnectionTamperError);
       tamper_Visibility&=~RF_tamperVisibility;
     }

  }
  
   if(sc_task ==1)
   {     
    #ifdef ALARM_ON_TAMPER     
         SecondBeeps();//initial beep when the tamper happens
         ScaduleTask(ToggleBuzzer2,MINUTE,TOOGLE_BUZZER_TASK); 
    #endif     
   }   
  
  
   if(temp!=0)
   {
     status.TamperDetectedStatus = 1;
   }
   else
   {
     status.TamperDetectedStatus = 0;
     CancelTask2(ToggleBuzzer2);
     BuzzerOff();
   }
}

/**
* checks if the lower cover is open and calls the LCTamperDetected to log the event
* @see LCTamperDetected
*/
void ProcessTamperLC()
{             
    uint16_t z=LC_TamperPIN;
    if((z & LC_TamperBIT) == LC_TamperBIT)//!=
    {
      LCTamperDetected();   
    }    
     LCTamperSchaduled = 0;
}
/**
* checks if the upper cover is open and calls UCTamperDetected to log the event
* @see UCTamperDetected
*/
void ProcessTamperUC()
{
     volatile int z=UC_TamperPIN;
    //Check Upper Cover Tamper
    if(( z & UC_TamperBIT) == UC_TamperBIT) //!=
    {
      UCTamperDetected();
    }
    UCTamperSchaduled = 0;
}
/**
* logs the upper coover tamper event
* @see ProcessTamperUC
*/
void UCTamperDetected()
{
	// log upper cover tamper event
    event_log l;
    l.event_code = UpperCoverRemovedTamperError;
    l.time_stamp = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
    l.checksum  =(getCheckSum(&(l.time_stamp.TimestampLow),4) + l.time_stamp.TimestampUp + l.event_code)&0xff;
    //l.value = 0;
    write_to_eeprom(&l,(uint8_t *)4,log_events);
    
    TamperCount.UpperCoverRemoved_Count++;
    tamperLoggedStatus |= UCtamperLogged;
    status.UpperCoverRemovedTamperStatus = 1;
    
    uint8_t temp82 = TamperCount.UpperCoverRemoved_Count;
    uint8_t temp8 = UC_TamperType;
    write_to_eeprom(&temp82,&temp8,setTamperCount); 
    
//    setTamperCount( UC_TamperType, TamperCount.UpperCoverRemoved_Count);
    
    //if(status.BattryStatus == 0)
//       logMeterStatus(&status);
     //write_to_eeprom(&status,(uint8_t *)0,logMeterStatus);
//      LogTamperStatus();
    status.write_status = 1;
    //BuzzerOn();
}
/**
* logs the low cover tamper event 
* @see ProcessTamperLC
*/
void LCTamperDetected()
{
     status.LowerCoverRemovedTamperStatus = 1;	
     //log lower cover removed tamper
     event_log l;
     l.event_code = LowerCoverRemovedTamperError;
     l.time_stamp = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
     l.checksum  =(getCheckSum(&(l.time_stamp.TimestampLow),4) + l.time_stamp.TimestampUp + l.event_code)&0xff;
     //l.value = 0;
     write_to_eeprom(&l,(uint8_t *)4,log_events);
     
     TamperCount.LowerCoverRemoved_Count ++;
     tamperLoggedStatus |= LCtamperLogged; 
     
     uint8_t temp82 = TamperCount.LowerCoverRemoved_Count;
     uint8_t temp8 = LC_TamperType;
     write_to_eeprom(&temp82,&temp8,setTamperCount); 
     
     //temp |= 0x08;
     //BuzzerOn();
   //send communication 
     //if(status.BattryStatus == 0)//extends EEPROM's life 
//     logMeterStatus(&status);
     //write_to_eeprom(&status,(uint8_t *)0,logMeterStatus);
//     LogTamperStatus();
     status.write_status = 1;
}
