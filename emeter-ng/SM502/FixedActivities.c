#include <stdint.h>
#include "Logger.h"
#include "Status.h"
#include "Relay.h"
#include "Tamper.h"
#include "battery_monitor.h"
#include "LCD_C.h"
#include "LCD_C_Graphics.h"
#include <stdio.h>
#include "self_diagnosis.h"
#include "disconnect_control_and_log.h"
#include "hourly_load_profile.h"
//under voltage and current ranges 
//extern uint8_t MIN_IRMS ;
extern uint8_t MIN_VRMS ;
extern uint8_t MIN_Frequency ;

extern uint16_t Nominal_Voltage ;//V
extern uint8_t Basic_Current ; //A
extern uint8_t Nominal_Frequency ;//HZ


extern uint16_t ActivePowerTripDuration    ; //in seconds 
extern uint16_t OverCurrentTripDuration    ;
extern uint16_t over_voltage_trip_duration    ;
extern uint16_t under_voltage_trip_duration   ;
extern uint16_t UnderFrequencyTripDuration ;
extern uint16_t OverFrequencyTripDuration  ;
extern uint8_t CoverRemovalTripDuration    ;

 uint8_t ActivePowerTimer   ;
 uint8_t OverCurrentTimer   ;
 uint8_t OverVoltageTimer   ;
 uint8_t UnderVoltageTimer  ;
 uint8_t UnderFrequencyTimer;
 uint8_t OverFrequencyTimer ;
 uint16_t LongPowerFailTimer ;
 
extern  MeterStatus status; 
volatile uint8_t secondCounter =0;

extern volatile uint8_t connect_counter;
extern volatile uint8_t start_connect_timeout; 
extern volatile uint8_t reset_timeout ;
extern volatile uint8_t reset_counter ;
extern volatile uint8_t reset_relay_required ;

//TODO. write cosem object for this 
uint8_t EnergyLoggingTime = 15; //Minute 


extern uint16_t time_threshold_for_long_power_failure;
extern uint16_t NeutralTamperTripDuration;
extern uint32_t Current_Thrushold;

extern Tamper_Count TamperCount;

uint16_t neutral_tamper_trip_counter;
uint8_t low_bat_backup_time = 5;

extern uint8_t output_state;

extern void perform_low_battry_backup(); //was __monitor func
extern uint16_t number_of_long_power_failures;
extern rtc_t activePassiveCalenderTime;


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
   uint8_t tmp;
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
          //Log fraud event to the EEPROM
          event_log l;
          //l.value = phase->readings.active_power;
          //l.EventGroup = 1;
          l.event_code = ReverseCurrentError;
          l.time_stamp = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
          l.checksum  =(getCheckSum(&(l.time_stamp.TimestampLow),4) + l.time_stamp.TimestampUp + l.event_code)&0xff;
          tmp = 4;
		  write_to_eeprom(&l,&tmp,log_events);
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
    	   //log disconnect event
    	   disconnect_event_log l;
    	   l.event_code = ActivePowerExcededError;
    	   l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
    	   l.disconnect_control_status = control_state;
    	   l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
    	   // l.value = phase->readings.active_power;
    	   // l.EventGroup = 1;
    	   tmp = 9;
    	   write_to_eeprom(&l,&tmp,log_events);
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
           //log power quality event
           event_log l;
           l.event_code = OverCurrentError;
           l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
           l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
           //l.value = phase->readings.I_rms;
           tmp = 5;
           write_to_eeprom(&l,&tmp,log_events);
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
     if(OverVoltageTimer >= over_voltage_trip_duration)
      {
         status.OverVoltageTimedOutStatus = 1;
           //disconnect meter 
         if(status.OverVoltageLoggedStatus != 1)
         {
        	 //log power quality event
             event_log l;
             l.event_code = HighVoltageError;
             l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
             l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
             //l.value = phase->readings.V_rms;
             tmp = 5;
             write_to_eeprom(&l,&tmp,log_events);
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
     if(UnderVoltageTimer >= under_voltage_trip_duration)
     {
    	 status.UnderVoltageTimedOutStatus = 1;
         //disconnect meter
         if(status.UnderVoltageLoggedStatus != 1)
         {
        	 // log power quality event
             event_log l;
             l.event_code = LowVoltageError;
             l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
             l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
             //l.value = phase->readings.V_rms;
             tmp = 5;
             write_to_eeprom(&l,&tmp,log_events);
             AddError(LowVoltageError);
             //TX event
             status.UnderVoltageLoggedStatus = 1;
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
     if(LongPowerFailTimer >= time_threshold_for_long_power_failure)
     {
       status.LongPowerFaileTimedOutStatus = 1;
       if(status.LongPowerFailLoggedStatus != 1)
       {
    	   // log power quality event
           event_log l;
           l.event_code = LongPowerFailer;
           l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
           l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
	       //l.value = phase->readings.V_rms;
           tmp = 5;
           write_to_eeprom(&l,&tmp,log_events);
           status.LongPowerFailLoggedStatus = 1;
           AddError(LongPowerFailer);
           number_of_long_power_failures++;
           //logNumberOfLongPowerFailes(&number_of_long_power_failures);TODO. impliment this
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
			   event_log l;
			   l.event_code = OverFrequencyError;
			   l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
			   l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
			   //l.value = phase->readings.frequency;
			   AddError(OverFrequencyError);
			   tmp = 5;
			   write_to_eeprom(&l,&tmp,log_events);
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
        	   // log power quality event
        	   event_log l;
        	   l.event_code = UnderFrequencyError;
        	   l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
        	   l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
        	   //l.value = phase->readings.frequency;
        	   tmp = 5;
        	   write_to_eeprom(&l,&tmp,log_events);
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
    	  // log fraud event
    	  event_log l;
    	  l.event_code = TAMPER_EVENT_CLEARED;
    	  l.time_stamp = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
    	  l.checksum  =(getCheckSum(&(l.time_stamp.TimestampLow),4) + l.time_stamp.TimestampUp + l.event_code)&0xff;
    	  // l.value = 0;
    	  tmp = 4;
    	  write_to_eeprom(&l,&tmp,log_events);
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
	int a;
	rtc_t t;
	getHardwareTime(&t);
	a = t.minute;
	while(a>0)
	{
		a -= EnergyLoggingTime;
	}
	if(a == 0) //capture hourly energy log
	{
		obj_load_profile_capture((uint8_t *)0,(uint16_t)0,(uint8_t *)0,(uint16_t *)0);
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

