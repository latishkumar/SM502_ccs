#include <stdint.h>
#include "Logger.h"
#include "Status.h"
#include "Relay.h"
#include "Tamper.h"
#include "BatteryMonitor.h"
#include "LCD_C.h"
#include "LCD_C_Graphics.h"
#include <stdio.h>
#include "self_diagnosis.h"

//under voltage and current ranges 
extern uint8_t MIN_IRMS ;
extern uint8_t MIN_VRMS ;
extern uint8_t MIN_Frequency ;

extern uint16_t Nominal_Voltage ;//V
extern uint8_t Basic_Current ; //A
extern uint8_t Nominal_Frequency ;//HZ


extern uint16_t ActivePowerTripDuration    ; //in seconds 
extern uint16_t OverCurrentTripDuration    ;
extern uint16_t OverVoltageTripDuration    ;
extern uint16_t UnderVoltageTripDuration   ;
extern uint16_t UnderFrequencyTripDuration ;
extern uint16_t OverFrequencyTripDuration  ;
extern uint8_t CoverRemovalTripDuration    ;

 uint8_t ActivePowerTimer   ;
 uint8_t OverCurrentTimer   ;
 uint8_t OverVoltageTimer   ;
 uint8_t UnderVoltageTimer  ;
 uint8_t UnderFrequencyTimer;
 uint8_t OverFrequencyTimer ;
 uint8_t LongPowerFailTimer ;
 
extern  MeterStatus status; 
volatile uint8_t secondCounter =0;

extern volatile uint8_t connect_counter;
extern volatile uint8_t start_connect_timeout; 
extern volatile uint8_t reset_timeout ;
extern volatile uint8_t reset_counter ;
extern volatile uint8_t reset_relay_required ;

//TODO. write cosem object for this 
uint8_t EnergyLoggingTime = 15; //Minute 


extern uint16_t TimeThresholdLongPowerFaile;
extern uint16_t NeutralTamperTripDuration;
extern uint32_t Current_Thrushold;

extern Tamper_Count TamperCount;

uint16_t neutral_tamper_trip_counter;
uint8_t low_bat_backup_time = 5;


extern __monitor void perform_low_battry_backup();
extern uint16_t NumberOfLongPowerFails; 
extern rtc_t activePassiveCalenderTime;

uint8_t minuteCounts = 0;
uint8_t low_bat_backup_min_counter;

extern uint8_t SelectedCalender;
extern HardwareStatus hardware_status;
#if defined(NEUTRAL_MONITOR_SUPPORT)
extern struct neutral_parms_s neutral_c;
#endif
/**
* compares the current difference between the phase and neutral lines and updates the system status
* @param phase_current:- the phase current 
* @param neutral_current:- the neutral current
* @param Thrushold:- the current comparision thrushold 
*                    this sould be current_thrushold is in percent scaled by 100
*                    e.g for 5% thrushold => 5 *100 = 500
* 
*/
void test_current_balance(float phase_current,float neutral_current,uint32_t Thrushold)
{

      phase_current = phase_current/1000;
      neutral_current = neutral_current/100; 
      if(phase_current < 1)
        phase_current =0;
      if(neutral_current < 1)
        neutral_current = 0;
        
      float currentDiff = (phase_current * Thrushold)/10000 ; //10000 current_thrushold is in percent scaled by 100      
      float difference = phase_current - neutral_current;
      
      if(difference > currentDiff)
      {
        status.NeutralTamperStatus = 1; //TODO uncomment this
        #if defined(Log_Current_Balance_Monitor)
           printf("Inbalance Ph=%f,Ne=%f , Cdif=%f,diff=%f\n",phase_current,neutral_current,currentDiff,difference);
        #endif 
      }
      else 
      {
        status.NeutralTamperStatus = 0;
      }
      
     if(status.NeutralTamperStatus == 1)
     {
        if(status.NeutralTamperTimedoutStatus == 0)
        {
          neutral_tamper_trip_counter++;
          if(neutral_tamper_trip_counter >= NeutralTamperTripDuration)
          {
            status.NeutralTamperTimedoutStatus = 1;
            TamperCount.Neutral_Count++;
            //setTamperCount(NeutralTamperType,TamperCount.Neutral_Count);
            
            uint8_t temp82 = TamperCount.Neutral_Count;
            uint8_t temp8 = NeutralTamperType;
            write_to_eeprom(&temp82,&temp8,setTamperCount);  
            
          }
        }               
     }
     else
     {
        neutral_tamper_trip_counter = 0; 
     }      
}

/** 
* used for Calender switching, from passive to active calender 
* this method is not used
*/

//uint8_t checkCalenderSwitchTime()
//{
//  if(compare_time(&activePassiveCalenderTime,&rtcc) == 1 )
//  {
//    return 1;
//  }
//  return 0;
//}

uint8_t calender_change_counter=0;
extern uint8_t OperatingMode;
/**
* function to handel every activity that is executed pre second basis
* includes ==> Relay monitoring 
*          ==> Led's monitoring 
*          ==> battery checking 
*          ==> calender swithcing checking 
*          ==> Event's timeout and logging 
*                       over power , over current, over voltage, over frequency, under frequency, under voltage, phase unbalance, tampering 

*/
void per_second_activity()
{
          
   secondCounter++;
   
  
   UpdateRelayStatus();      
   UpadateErrorIndicators();
   checkConfigrationPortsStatus();    /* [M.G] Un-commented this line Oct 22, 2015 10:23 */
   

   
       /*
      Check the battery status and update the status variable.
      we don't want to show the battery status to the user, so there 
      is no need to add it to the system errors list 
    */
   if(OperatingMode == 1){
      uint8_t test_temp22 = CheckBattery();
      Battery(test_temp22);
      if(test_temp22 < BATTRY_CRITICAL_LEVEL)
      {
        status.BattryStatus = 0; //bad status 
        
      }
      else 
        status.BattryStatus = 1; //good status 
   }
    
   if(status.energy_reverse_flow_tamper == 1)
   {
     if(status.energy_reverse_flow_logged_status == 0)//system initialized
      {
          //Log the Event to the EEPROM
          EventLog evl;
          evl.value = phase->readings.active_power;
          evl.EventGroup = 1;
          evl.EventCode = ReverseCurrentError;
          evl.timeStump = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
          evl.Checksum  =(getCheckSum(&(evl.timeStump.TimestampLow),4) + evl.timeStump.TimestampUp + evl.EventCode)&0xff;
//                                logEvent(&evl);
          write_to_eeprom(&evl,(uint8_t *)0,logEvent);
          status.energy_reverse_flow_logged_status = 1;
          
          AddError(ReverseCurrentError);
          
          return;
      }
   }
   else
   {
      RemoveError(ReverseCurrentError);
      status.energy_reverse_flow_tamper = 0;
      status.energy_reverse_flow_logged_status = 0;
   }

   
   calender_change_counter++;
   if(calender_change_counter>=15)
   {
             //this is just for test TODO. Delete this 
        status.UpdateDate = 1;
        if(SelectedCalender == AM)
        {
          SelectedCalender = AM_EN;
        }
        else 
        {
          SelectedCalender = AM;
        }
        calender_change_counter = 0;
        //end of this is just for test 
   }

   
   
//   if(OPERATING_MODE == OPERATING_MODE_NORMAL)
//   {
   if(status.ActivePowerExcededStatus == 1)
   {
     ActivePowerTimer++;
     if(ActivePowerTimer >= ActivePowerTripDuration)
     {
        //Disconnect meter due to Active Power Excedded time limitation 
       if(status.ActivePowerExcededLoggedStatus != 1)
       {
        //log the EVENT 
           EventLog l;
	   l.EventCode = ActivePowerExcededError;
	   l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	   l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
			+ l.timeStump.TimestampUp);
           l.value = phase->readings.active_power;
           l.EventGroup = 1;
	   //l.StartAddress = 0;	//last event log address
//	   logEvent(&l);  
           write_to_eeprom(&l,(uint8_t *)0,logEvent);
        //send event through PLC 
           AddError(ActivePowerExcededError);
         status.ActivePowerExcededLoggedStatus = 1;
          return;//Thinkg about this ?????
       }
     }
   }
   else
   {
     RemoveError(ActivePowerExcededError);
     status.ActivePowerExcededStatus = 0;
     status.ActivePowerExcededLoggedStatus = 0;
     ActivePowerTimer = 0;
   }
   
   //over current status 
   if(status.OverCurrentStatus == 1)
   {
      OverCurrentTimer++;
      if(OverCurrentTimer >= OverCurrentTripDuration)
      {
        status.OverCurrentTimedOutStatus = 1;
        if(status.OverCurrentLoggedStatus != 1)
        {
           //log event 
           EventLog l;
	   l.EventCode = OverCurrentError;
	   l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	   l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
			+ l.timeStump.TimestampUp);
	   //l.StartAddress = 0;	//last event log address
           l.value = phase->readings.I_rms;
//	   logEvent(&l);
           write_to_eeprom(&l,(uint8_t *)0,logEvent);
           //disconnect meter 
           //plc send     
           AddError(OverCurrentError);
           status.OverCurrentLoggedStatus = 1;
            return;//?????
        }
      }      
   }
   else
   {
     RemoveError(OverCurrentError);
     status.OverCurrentTimedOutStatus = 0;
     status.OverCurrentLoggedStatus = 0;
     OverCurrentTimer=0;
   }
   
   
   
   //over voltage 
   if(status.OverVoltageStatus == 1)
   {
     OverVoltageTimer++;
     if(OverVoltageTimer >= OverVoltageTripDuration)
      {
         status.OverVoltageTimedOutStatus = 1;
           //disconnect meter 
         if(status.OverVoltageLoggedStatus != 1)
         {
           //log event
           EventLog l;
	   l.EventCode = HighVoltageError;
	   l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	   l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
			+ l.timeStump.TimestampUp);
	    //l.StartAddress = 0;	//last event log address
           l.value = phase->readings.V_rms;
//	   logEvent(&l);  
           write_to_eeprom(&l,(uint8_t *)0,logEvent);
           status.OverVoltageLoggedStatus = 1;
           AddError(HighVoltageError);
           //TX event
           return;//?????
         }
         
      }
   }
   else
   {
     status.OverVoltageTimedOutStatus = 0;
     status.OverVoltageLoggedStatus = 0;
     OverVoltageTimer=0;
           RemoveError(HighVoltageError);
   }
   
   
   //under voltage 
   if(status.UnderVoltageStatus == 1)
   {
     UnderVoltageTimer++;
     if(UnderVoltageTimer >= UnderVoltageTripDuration)
     {
       status.UnderVoltageTimedOutStatus = 1;

         
           //disconnect meter
         if(status.UnderVoltageLoggedStatus != 1)
         {
           //log event 
           EventLog l;
	   l.EventCode = LowVoltageError;
	   l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	   l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
			+ l.timeStump.TimestampUp);
	    //l.StartAddress = 0;	//last event log address
           l.value = phase->readings.V_rms;
//	   logEvent(&l);
           write_to_eeprom(&l,(uint8_t *)0,logEvent);
           AddError(LowVoltageError);
           //TX event
           status.UnderVoltageLoggedStatus =1;
            return;//?????
         }
     }

   }
   else
   {
     status.UnderVoltageTimedOutStatus = 0;
     status.UnderVoltageLoggedStatus = 0;
     UnderVoltageTimer=0;
     RemoveError(LowVoltageError);
   }
   
   
   //long power failer 
   if(status.LongPowerFailStatus == 1)
   {
     LongPowerFailTimer++;
     if(LongPowerFailTimer >= TimeThresholdLongPowerFaile)
     {
       status.LongPowerFaileTimedOutStatus = 1;
       if(status.LongPowerFailLoggedStatus != 1)
       {
           //disconnect meter
           //log event 
           EventLog l;
	   l.EventCode = LongPowerFailer;
	   l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	   l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
			+ l.timeStump.TimestampUp);
	    //l.StartAddress = 0;	//last event log address
	   l.value = phase->readings.V_rms;
//           logEvent(&l);
           write_to_eeprom(&l,(uint8_t *)0,logEvent);
           status.LongPowerFailLoggedStatus = 1;
            AddError(LongPowerFailer);
           NumberOfLongPowerFails++;
           //logNumberOfLongPowerFailes(&NumberOfLongPowerFails);TODO. impliment this 
           
           //TX event
            return;//?????
       }
     }
   }
   else
   {
     RemoveError(LongPowerFailer);
     status.LongPowerFaileTimedOutStatus = 0;
     status.LongPowerFailLoggedStatus = 0;
     LongPowerFailTimer=0;
   }
   
   
   
   //over frequency status 
   if(status.OverFreqStatus == 1)
   {
     OverFrequencyTimer++;
     if(OverFrequencyTimer >= OverFrequencyTripDuration)
     {
           status.OverFreqTimedOutStatus=1;
           if(status.OverFreqLoggedStatus != 1)
           {
           //log event 
           EventLog l;
	   l.EventCode = OverFrequencyError;
	   l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	   l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
			+ l.timeStump.TimestampUp);
	   //l.StartAddress = 0;	//last event log address
           l.value = phase->readings.frequency;
//	   logEvent(&l);
           AddError(OverFrequencyError);
           write_to_eeprom(&l,(uint8_t *)0,logEvent);
           status.OverFreqLoggedStatus = 1;
            return;//?????
           }
     }
   }
   else
   {
     RemoveError(OverFrequencyError);
     status.OverFreqTimedOutStatus = 0;
     status.OverFreqLoggedStatus = 0;
     OverFrequencyTimer=0;
   }
   
   
   //under frequency logged status 
   if(status.UnderFreqStatus == 1)
   {
     UnderFrequencyTimer++;
     if(UnderFrequencyTimer >= UnderFrequencyTripDuration)
     {
           status.UnderFreqTimedOutStatus = 1;
           if(status.UnderFreqLoggedStatus != 1)
           {
           //log event 
           EventLog l;
	   l.EventCode = UnderFrequencyError;
	   l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	   l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
			+ l.timeStump.TimestampUp);
	   //l.StartAddress = 0;	//last event log address
           l.value = phase->readings.frequency;
//	   logEvent(&l);
           write_to_eeprom(&l,(uint8_t *)0,logEvent);
           //TX event
           AddError(UnderFrequencyError);
           status.UnderFreqLoggedStatus = 1;
            return;//?????
           }
     }
   }
   else
   {
     RemoveError(UnderFrequencyError);
     status.UnderFreqTimedOutStatus = 0;
     status.UnderFreqLoggedStatus = 0;
     UnderFrequencyTimer=0;
   }   
   
   
   

   /* CHECK RTC status and add them to Error List */
   if(hardware_status.RTCResetToDefaultStatus == 1)//&& status.RTCResetToDefaultStatusAddedToErrorList!=1
   {
     // AddError(RTC_Reset_Error);
   }
   else
   {
//     status.RTCResetToDefaultStatusAddedToErrorList= 0;
       RemoveError(RTC_Reset_Error);
   }
   
   if(hardware_status.RTC_Status == 0)
   {
     
   }
   else
   {
      RemoveError(RTC_Error);
   }
    
   
      //check for neutral tamper 
#ifdef NEUTRAL_MONITOR_SUPPORT
      test_current_balance(phase->readings.I_rms,neutral_c.readings.I_rms,Current_Thrushold);
#endif 
   

      UpdateTamperIndicators();
      
      
      if(start_connect_timeout == 1)
      {
        connect_counter++;
      }
      
      if(reset_relay_required ==1)
      {
        reset_counter++;  
        uint8_t t =  reset_timeout;
        if(reset_counter >= t)
        {
          ResetRelayPort();
        }
      }
      
      //if(checkCalenderSwitchTime() == 1)
      {
         //TODO. switch calender 
         //copy passive activity calendar to active activity calendar
      } 
      
      
      if(status.RelayStatus == 0) //verify relay is disconnected 
      {
         if(phase->readings.I_rms > 2)
         {
            
         }
      }
      
      if(status.write_status == 1)
      {
        write_to_eeprom(&status,(uint8_t *)0,logMeterStatus);
        status.write_status = 0;
      }
      
      if(status.write_tamper_status == 1)
      {
        write_to_eeprom(&status,(uint8_t *)0,logMeterStatus);
        status.write_tamper_status = 0;
        EventLog evl;
        evl.EventCode = TAMPER_EVENT_CLEARED;
        evl.timeStump = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
        evl.Checksum  =(getCheckSum(&(evl.timeStump.TimestampLow),4) + evl.timeStump.TimestampUp + evl.EventCode)&0xff;
        evl.value = 0;
//        logEvent(&evl);
        write_to_eeprom(&evl,(uint8_t *)0,logEvent);
      }
}




uint8_t elogMinCounter=0;
/**
*  function to handle every activity the is performed per minute basis. 
* i.e Energy profile data logging
*  
*/
void per_minute_activity(void)
{
  

    
    minuteCounts++;
    if(minuteCounts >= EnergyLoggingTime )
    {
        status.LoggingTimeIsUp = 1;
        minuteCounts = 0 ;
    }
 
    
    if(status.LogEnergyLogged == 1) //wait one minutes before the next power out detection starts working 
    {
      elogMinCounter++;
      if(elogMinCounter == 1)//the background process is allowed to log again after five minute
      {
        
        status.LogEnergyLogged = 0;
        elogMinCounter=0;
      }
    }
}

