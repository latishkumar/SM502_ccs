//--------------------------------------------------------------------------
//
//  Software for MSP430 based e-meters.
//
//  THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
//  REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY, 
//  INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS 
//  FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR 
//  COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE. 
//  TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET 
//  POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY 
//  INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR 
//  YOUR USE OF THE PROGRAM.
//
//  IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL, 
//  CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY 
//  THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT 
//  OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM. 
//  EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF 
//  REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS 
//  OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF 
//  USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S 
//  AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF 
//  YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS 
//  (U.S.$500).
//
//  Unless otherwise stated, the Program written and copyrighted 
//  by Texas Instruments is distributed as "freeware".  You may, 
//  only under TI's copyright in the Program, use and modify the 
//  Program without any charge or restriction.  You may 
//  distribute to third parties, provided that you transfer a 
//  copy of this license to the third party and the third party 
//  agrees to these terms by its first use of the Program. You 
//  must reproduce the copyright notice and any other legend of 
//  ownership on each copy or partial copy, of the Program.
//
//  You acknowledge and agree that the Program contains 
//  copyrighted material, trade secrets and other TI proprietary 
//  information and is protected by copyright laws, 
//  international copyright treaties, and trade secret laws, as 
//  well as other intellectual property laws.  To protect TI's 
//  rights in the Program, you agree not to decompile, reverse 
//  engineer, disassemble or otherwise translate any object code 
//  versions of the Program to a human-readable form.  You agree 
//  that in no event will you alter, remove or destroy any 
//  copyright notice included in the Program.  TI reserves all 
//  rights not specifically granted under this license. Except 
//  as specifically provided herein, nothing in this agreement 
//  shall be construed as conferring by implication, estoppel, 
//  or otherwise, upon you, any license or other right under any 
//  TI patents, copyrights or trade secrets.
//
//  You may not use the Program in non-TI devices.
//
//  File: app_server_msgs.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: app_server_msgs.c,v 1.12 2013/01/30 05:36:32 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "cosem.h"
#include "obis.h"
#include "iec62056_46_link.h"
#include "msgs.h"
#include "packing.h"
#include "config.h"
#include "server_msgs.h"
#include "disconnect_control_and_log.h"
#define FALSE 0
#define TRUE (!FALSE)

#include "Logger.h"
#include "utilities.h"
#include "emeter-rtc.h"
#include "identification_numbers.h"

extern rtc_t rtcc;
extern uint8_t active_quadrant;
typedef struct{
  rtc_t preset_time;
  rtc_t validity_interval_start;
  rtc_t validity_interval_end;

}preset_adjusting_t;
preset_adjusting_t preset_adj_time;

typedef struct 
{
  daily_energy_log_t l;
  uint16_t logEntryNo;

}daily_current_entry_to_tx;
typedef struct
{
  hourly_energy_log_t l;
  uint16_t logEntryNo;

}hourly_current_entry_to_tx;
typedef struct
{
  EventLog l;
  uint16_t logEntryNo;
}currentEventToTX;
typedef struct
{
  event_log l;
  uint16_t logEntryNo;
}current_event_to_tx;
typedef struct
{
  time_bound_event_log l;
  uint16_t logEntryNo;
}current_time_bound_event_to_tx;
typedef struct
{
  firmware_event_log l;
  uint16_t logEntryNo;
}current_firmware_event_to_tx;
typedef struct
{
  disconnect_event_log l;
  uint16_t logEntryNo;
}current_disconnect_event_to_tx;

typedef struct{
  Public_Holiday_t Holiday;
  uint8_t holiday_no;
  uint8_t day_id;
}PublicHolidayEntry;

/*typedef struct{
    rtc_t execution_time;
    uint16_t control_script_type;        
}DisconnectControlScript;

//TODO .write this to EEPROM 
DisconnectControlScript disconnect_script;*///n
hourly_current_entry_to_tx current_hourly_energy_log_entry = {init_hourly_energy_log};
daily_current_entry_to_tx current_daily_energy_log_entry = {init_daily_energy_log};
uint8_t currentEnergyLogEntryData[12];

currentEventToTX currentEventLogEntry = {init_EventLog,0};
uint8_t currentEventLogEntryData[12];
current_event_to_tx current_event_log = {init_common_event_logs,0};
current_time_bound_event_to_tx current_time_bound_event_log = {init_time_bounded_event_logs,0};
current_firmware_event_to_tx current_firmware_event_log = {init_time_bounded_event_logs, 0};
current_disconnect_event_to_tx current_disconnect_event_log = {init_disconnect_event_log,0}; ;
PublicHolidayEntry currentHoliday = {Init_public_Holiday,0,3};
uint8_t currentHoliday_date[5];

DailyProfileEntry  currentDayProfile={Schedule_timeslot_init,0};
uint8_t dayProfileTime1[4];
uint8_t dayProfileTime2[4];
uint8_t dayProfileTime3[4];
uint8_t dayProfileTime4[4];

//extern void adjust_rtc(rtc_t *time);
extern rtc_t required_last_entry_time;
void load_hourly_energy_to_ram(unsigned int entry_no)
{
    get_hourly_energy_profile(&current_hourly_energy_log_entry.l,entry_no);
    current_hourly_energy_log_entry.logEntryNo = entry_no;
    if(access_selector == 1) //if this is selective access by range
    {
        rtc_t time_last = getTime(&current_hourly_energy_log_entry.l.timestamp);
        if(time_last.isValid==0)
        {
            msg_info.entries_remaining = 1;
            return;
        }
    }
    //format the time stump and copy it to the buffer
    rtc_t time = getTime(&current_hourly_energy_log_entry.l.timestamp);
    uint8_t currentEnergyLogEntryData2[] = {
    INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00,
    };
    memcpy(currentEnergyLogEntryData,currentEnergyLogEntryData2,12);
}

void load_daily_energy_to_ram(unsigned int entry_no)
{
	get_daily_snapshot_energy_profile(&current_daily_energy_log_entry.l,entry_no);
	current_daily_energy_log_entry.logEntryNo = entry_no;

	if(access_selector == 1) //if this is selective access by range
	{
		rtc_t time_last = getTime(&current_daily_energy_log_entry.l.timestamp);
		if(time_last.isValid == 0)
		{
			msg_info.entries_remaining = 1;
			return;
		}
	}
	//format the timestamp and copy it to the buffer
	rtc_t time = getTime(&current_daily_energy_log_entry.l.timestamp);
	uint8_t currentEnergyLogEntryData2[] = {
	INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00,
	};
	memcpy(currentEnergyLogEntryData,currentEnergyLogEntryData2,12);
}
void loadEventToRam(unsigned int entry_no)
{
          getEvent2(&currentEventLogEntry.l,entry_no);
          currentEventLogEntry.logEntryNo = entry_no;
           
           if(access_selector == 1) //if this is selective access by range
           {
             rtc_t time_last = getTime(&currentEventLogEntry.l.timeStump);
             if(time_last.isValid==0)
             {
               msg_info.entries_remaining = 1;
               return;
             }
             int8_t com3 = compare_time(&required_last_entry_time,&time_last);
             if(com3<0)//time_last comes after required_last_entry_time
              {
                   msg_info.entries_remaining = 1;
              }
           }
          //formate the timestump and copy it to the buffer
          rtc_t time = getTime(&currentEventLogEntry.l.timeStump);
          uint8_t currentEventLogEntryData2[] = {
            INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00,
          };
          memcpy(currentEventLogEntryData,currentEventLogEntryData2,12);
}
void load_fraud_event_to_ram(unsigned int entry_no)
{
    get_fraud_event(&current_event_log.l,entry_no);
    current_event_log.logEntryNo = entry_no;
    if(access_selector == 1) //if this is selective access by range
    {
         rtc_t time_last = getTime(&current_event_log.l.time_stamp);
         if(time_last.isValid==0)
         {
               msg_info.entries_remaining = 1;
               return;
         }
         int8_t com3 = compare_time(&required_last_entry_time,&time_last);
         if(com3<0)//time_last comes after required_last_entry_time
         {
               msg_info.entries_remaining = 1;
         }
    }
    //format the time stamp and copy it to the buffer
    rtc_t time = getTime(&current_event_log.l.time_stamp);
    uint8_t currentEventLogEntryData2[] = {
            INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00,
          };
    memcpy(currentEventLogEntryData,currentEventLogEntryData2,12);
}
void load_common_event_to_ram(unsigned int entry_no)
{
    get_common_event(&current_event_log.l,entry_no);
    current_event_log.logEntryNo = entry_no;
    if(access_selector == 1) //if this is selective access by range
    {
         rtc_t time_last = getTime(&current_event_log.l.time_stamp);
         if(time_last.isValid==0)
         {
               msg_info.entries_remaining = 1;
               return;
         }
         int8_t com3 = compare_time(&required_last_entry_time,&time_last);
         if(com3<0)//time_last comes after required_last_entry_time
         {
               msg_info.entries_remaining = 1;
         }
    }
    //format the time stamp and copy it to the buffer
    rtc_t time = getTime(&current_event_log.l.time_stamp);
    uint8_t currentEventLogEntryData2[] = {
            INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00,
          };
    memcpy(currentEventLogEntryData,currentEventLogEntryData2,12);
}
void load_power_qual_event_to_ram(unsigned int entry_no)
{
    get_power_qual_event(&current_event_log.l,entry_no);
    current_event_log.logEntryNo = entry_no;
    if(access_selector == 1) //if this is selective access by range
    {
         rtc_t time_last = getTime(&current_event_log.l.time_stamp);
         if(time_last.isValid==0)
         {
               msg_info.entries_remaining = 1;
               return;
         }
         int8_t com3 = compare_time(&required_last_entry_time,&time_last);
         if(com3<0)//time_last comes after required_last_entry_time
         {
               msg_info.entries_remaining = 1;
         }
    }
    // format the time stamp and copy it to the buffer
    rtc_t time = getTime(&current_event_log.l.time_stamp);
    uint8_t currentEventLogEntryData2[] = {
            INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00,
          };
    memcpy(currentEventLogEntryData,currentEventLogEntryData2,12);
}
void load_firmware_event_to_ram(unsigned int entry_no)
{
    get_firmware_event(&current_firmware_event_log.l,entry_no);
    current_firmware_event_log.logEntryNo = entry_no;
    if(access_selector == 1) //if this is selective access by range
    {
         rtc_t time_last = getTime(&current_firmware_event_log.l.time_stamp);
         if(time_last.isValid==0)
         {
               msg_info.entries_remaining = 1;
               return;
         }
         int8_t com3 = compare_time(&required_last_entry_time,&time_last);
         if(com3<0)//time_last comes after required_last_entry_time
         {
               msg_info.entries_remaining = 1;
         }
    }
    //format the time stamp and copy it to the buffer
    rtc_t time = getTime(&current_firmware_event_log.l.time_stamp);
    uint8_t currentEventLogEntryData2[] = {
            INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00,
          };
    memcpy(currentEventLogEntryData,currentEventLogEntryData2,12);
}
/*
 * @param type: 0 - former clock/before synchronization
 * 				1 - after synchronization
 */
void load_synchronization_event_to_ram(unsigned int entry_no,uint8_t type)
{
    get_synchronization_event(&current_time_bound_event_log.l,entry_no);
    current_time_bound_event_log.logEntryNo = entry_no;
    if(access_selector == 1) //if this is selective access by range
    {
    	rtc_t time_last;
    	if(type == 0)
    	{
    		 time_last = getTime(&current_time_bound_event_log.l.begin_time_stamp);
    	}
    	else if( type == 1)
    	{
    		 time_last = getTime(&current_time_bound_event_log.l.end_time_stamp);
    	}
         if(time_last.isValid==0)
         {
               msg_info.entries_remaining = 1;
               return;
         }
         int8_t com3 = compare_time(&required_last_entry_time,&time_last);
         if(com3<0)//time_last comes after required_last_entry_time
         {
               msg_info.entries_remaining = 1;
         }
    }
    //format the time stamp and copy it to the buffer
    rtc_t time;
    if(type == 0)
	{
		 time = getTime(&current_time_bound_event_log.l.begin_time_stamp);
	}
	else if( type == 1)
	{
		 time = getTime(&current_time_bound_event_log.l.end_time_stamp);
	}
    uint8_t currentEventLogEntryData2[] = {
            INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00,
          };
    memcpy(currentEventLogEntryData,currentEventLogEntryData2,12);
}
void load_disconnect_event_to_ram(unsigned int entry_no)
{
    get_disconnect_event(&current_disconnect_event_log.l,entry_no);
    current_disconnect_event_log.logEntryNo = entry_no;
    if(access_selector == 1) //if this is selective access by range
    {
         rtc_t time_last = getTime(&current_disconnect_event_log.l.time_stamp);
         if(time_last.isValid==0)
         {
               msg_info.entries_remaining = 1;
               return;
         }
         int8_t com3 = compare_time(&required_last_entry_time,&time_last);
         if(com3<0)//time_last comes after required_last_entry_time
         {
               msg_info.entries_remaining = 1;
         }
    }
    //format the time stamp and copy it to the buffer
    rtc_t time = getTime(&current_disconnect_event_log.l.time_stamp);
    uint8_t currentEventLogEntryData2[] = {
            INJECT16(time.year), time.month, time.day, ((time.day - 1) > 0)?(time.day - 1):0,time.hour, time.minute,time.second, 0xFF, INJECT16(120), 0x00, 
          };
    memcpy(currentEventLogEntryData,currentEventLogEntryData2,12);
}
void loadCurrentHoliday(uint8_t entry_no)
{
     uint8_t z = getPublicHoliday(entry_no,&currentHoliday.Holiday);
     uint8_t vvv[] = {INJECT16(currentHoliday.Holiday.year), currentHoliday.Holiday.month,currentHoliday.Holiday.day,(currentHoliday.Holiday.day-1)%7 };
                     memcpy(currentHoliday_date,vvv,5);
     currentHoliday.holiday_no = entry_no;
}


/* day_id 1:- weekdays  monday...friday
          2:- weekends  saturday, and sunday
          3:- special days,  for special days tables 
*/
void loadDailyProfile_dlms(uint8_t day_id)
{
   uint8_t j;
   loadDailyProfile(day_id,&currentDayProfile);
   
   for(j=0;j<4;j++)
   {
      uint8_t xxx[] = {currentDayProfile.timeslots[j].start_Hour, currentDayProfile.timeslots[j].start_minute, 0xff, 0xff};
      if(j==0)        
        memcpy(dayProfileTime1,xxx,4);
      else if(j==1)
        memcpy(dayProfileTime2,xxx,4);
      else if(j==2)
        memcpy(dayProfileTime3,xxx,4);
      else if(j==3)
        memcpy(dayProfileTime4,xxx,4);    
   }   
   currentDayProfile.day_id = day_id;   
}

/* day_id 1:- weekdays  monday...friday
          2:- weekends  saturday, and sunday
          3:- special days,  for special days tables 
*/
void loadDailyProfilePassive_dlms(uint8_t day_id)
{
   uint8_t j;
   loadDailyProfilePassive(day_id,&currentDayProfile);
   
   for(j=0;j<4;j++)
   {
      uint8_t xxx[] = {currentDayProfile.timeslots[j].start_Hour, currentDayProfile.timeslots[j].start_minute, 0xff, 0xff};
      if(j==0)        
        memcpy(dayProfileTime1,xxx,4);
      else if(j==1)
        memcpy(dayProfileTime2,xxx,4);
      else if(j==2)
        memcpy(dayProfileTime3,xxx,4);
      else if(j==3)
        memcpy(dayProfileTime4,xxx,4);    
   }   
   currentDayProfile.day_id = day_id;   
}

typedef struct
{
    unsigned char Load_Date[12];  
    uint32_t IRrms; 
    uint32_t IYrms; 
    uint32_t IZrms; 
    uint16_t VRrms; 
    uint16_t VYrms; 
    uint16_t VBrms; 
    uint32_t E_Active; 
    uint32_t E_Reactive_Lag; 
    uint32_t E_Reactive_Lead; 
    uint32_t E_Apparent; 
} sLoad_Profile;

union{
sBilling_Profile Billing_Profile;
sLoad_Profile Load_Profile;
};

extern uint16_t Vrms_R;
extern uint16_t Vrms_Y; 
extern uint16_t Vrms_B; 
extern uint32_t Irms_R;
extern uint32_t Irms_Y; 
extern uint32_t Irms_B;
extern uint16_t Mains_Freq;
extern int16_t sPF_R;
extern int16_t sPF_Y; 
extern int16_t sPF_B; 
extern int16_t sPF_Avg;
extern int32_t P_Reactive;
extern int32_t P_Active; 
//extern uint32_t P_Apparent;
extern uint32_t Temp_Uint32; 

extern uint32_t image_size;

int64_t get_numeric_item(int item)
{
    int64_t val;
    unsigned int entry_no = 0;
    switch (item)
    {
    case ITEM_TAG_IMAGE_SIZE:
      val = image_size;
      break;
    case ITEM_TAG_IR:   //instantaneous profile , done test
      val = phase->readings.I_rms;      
    break;
    case ITEM_TAG_IY:        
    break;

    case ITEM_TAG_VR:                     
      val = phase->readings.V_rms;        
    break;

    case ITEM_TAG_PFR:     //instantanious profile , done test                                    
      val = phase->readings.power_factor;        
    break;

    case ITEM_TAG_PF_TOTAL:      //instantanious profile , done test                              
      val = phase->readings.power_factor;      
    break;
    case ITEM_TAG_FREQUENCY:      //instantanious profile , done test                             
      val = phase->readings.frequency;
    break;
    case ITEM_TAG_KVA_TOTAL:      //instantanious profile , done test                             
      val = phase->readings.reactive_power;        
    break;
    case ITEM_TAG_KW_TOTAL:       //instantanious profile , done test                             
      val = phase->readings.active_power;        
    break;
    case ITEM_TAG_KVAR_TOTAL:   break;       //   val = P_Apparent; changed by E.E need to be implemented
    
    case ITEM_TAG_CUM_KWH_TOTAL:  //instantanious profile , done test 
      val = phase->import_active_energy_QI_QIV;
    break;
    
    case ITEM_TAG_RI_PLUS:
      val = phase->consumed_reactive_energy_QI;
      break;
    case ITEM_TAG_RC_PLUS:
      val = phase->consumed_reactive_energy_QII;
      break;
    case ITEM_TAG_RI_MINUS:
      val = phase->consumed_reactive_energy_QIII;
      break;
    case ITEM_TAG_RC_MINUS:
      val = phase->consumed_reactive_energy_QIV;
      break;
    
    case ITEM_TAG_ACTIVE_ENERGY_PLUS:
      val = phase->import_active_energy_QI_QIV;
      break;
    case ITEM_TAG_ACTIVE_ENERGY_MINUS:
      val = phase->export_active_energy_QII_QIII;
      break;
    
    case ITEM_TAG_ACTIVE_POWER_IMPORT:
        if(active_quadrant ==1||active_quadrant==4)
        {
          val = phase->readings.active_power;
        }
        else
          val = 0;
      break;
    case ITEM_TAG_ACTIVE_POWER_EXPORT:
        if(active_quadrant ==2||active_quadrant==3)
        {
          val = labs(phase->readings.active_power);
        }
        else
          val = 0;
      break;
    case ITEM_TAG_REACTIVE_POWER_IMPORT:
        if(active_quadrant ==1||active_quadrant==2)
        {
          val = (phase->readings.reactive_power);
        }
        else
          val = 0;
      break;
    case ITEM_TAG_REACTIVE_POWER_EXPORT:
        if(active_quadrant ==3||active_quadrant==4)
        {
          val = labs(phase->readings.reactive_power);
        }
        else
          val = 0;
      break;
      
    case ITEM_TAG_CUM_KVAR_LAGH_TOTAL:    val = 42;        break;
    case ITEM_TAG_CUM_KVAR_LEADH_TOTAL:   val = 42;        break;
    case ITEM_TAG_CUM_KVAH_TOTAL:         val = 42;        break;
    case ITEM_TAG_NUM_POWER_OFFS:         val = 0;        break;
    case ITEM_TAG_CUM_POWER_OFF_DURATION: val = 42;        break;
    case ITEM_TAG_CUM_TAMPER_COUNT:       val = 0;        break;
    case ITEM_TAG_CUM_MD_RESET_COUNT:     val = 0;        break;
    case ITEM_TAG_CUM_PROGRAMMING_COUNT:  val = 0;        break;
    case ITEM_TAG_KW_MAX_DEMAND:          val = 42;        break;
    case ITEM_TAG_KVA_MAX_DEMAND:         val = 42;        break;
        
    case ITEM_TAG_EVENT_GROUP_SE:
         entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(currentEventLogEntry.logEntryNo == entry_no)
          {
            
          }
          else
          {
              loadEventToRam(entry_no);
          }
          val = currentEventLogEntry.l.EventGroup;
      
      break;
    case ITEM_TAG_EVENT_DATA:
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(currentEventLogEntry.logEntryNo == entry_no)
          {
            
          }
          else
          {
              loadEventToRam(entry_no);
          }
          val = currentEventLogEntry.l.value;      
      break;
    
    case ITEM_TAG_EVENT_CODE_SE://standard event
          entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(currentEventLogEntry.logEntryNo == entry_no)
          {
            val = 0;
          }
          else
          {
              loadEventToRam(entry_no);
          }
          val = currentEventLogEntry.l.EventCode;
    break;
    case ITEM_TAG_EVENT_CODE_FE: // fraud event
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(current_event_log.logEntryNo == entry_no)
        {
        }
        else
        {
            load_fraud_event_to_ram(entry_no);
        }
        val = current_event_log.l.event_code;
       break;
    case ITEM_TAG_EVENT_CODE_CE: // common event
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(current_event_log.logEntryNo == entry_no)
        {
        }
        else
        {
            load_common_event_to_ram(entry_no);
        }
        val = current_event_log.l.event_code;
        break;
    case ITEM_TAG_EVENT_CODE_PQE: // power quality event
		entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
		if(current_event_log.logEntryNo == entry_no)
		{
		}
		else
		{
			load_power_qual_event_to_ram(entry_no);
		}
		val = current_event_log.l.event_code;
		break;
    case ITEM_TAG_EVENT_CODE_FRME: // firmware event
               entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	   if(current_firmware_event_log.logEntryNo == entry_no)
	   {
	   }
	   else
	   {
		   load_firmware_event_to_ram(entry_no);
	   }
	   val = current_firmware_event_log.l.event_code;
	   break;
    case ITEM_TAG_EVENT_CODE_SYNE: // synchronization event
	   entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	   if(current_time_bound_event_log.logEntryNo == entry_no)
	   {
	   }
	   else
	   {
		   load_synchronization_event_to_ram(entry_no,0);
	   }
	   val = current_time_bound_event_log.l.event_code;
	   break;
    case ITEM_TAG_EVENT_CODE_DE: // disconnect control event
           entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	   if(current_disconnect_event_log.logEntryNo == entry_no)
	   {
	   }
	   else
	   {
		   load_disconnect_event_to_ram(entry_no);
	   }
	   val = current_disconnect_event_log.l.event_code;
	   break;
     case ITEM_TAG_STATUS_DE: // disconnect control event
	   entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	   if(current_disconnect_event_log.logEntryNo == entry_no)
	   {
	   }
	   else
	   {
		   load_disconnect_event_to_ram(entry_no);
	   }
	   val = current_disconnect_event_log.l.disconnect_control_status;
	   break;
// Hourly energy profile
    case ITEM_TAG_ACTIVE_ENERGY_IMPORT_LP_1: //done test
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(current_hourly_energy_log_entry.logEntryNo == entry_no)
        {
        }
        else
        {
            load_hourly_energy_to_ram(entry_no);
        }
        val = current_hourly_energy_log_entry.l.inc_active_import_energy;
        break;

    case ITEM_TAG_REACTIVE_ENERGY_QI_LP_1:  //done test
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(current_hourly_energy_log_entry.logEntryNo == entry_no)
        {
        }
        else
        {
            load_hourly_energy_to_ram(entry_no);
        }
        val = current_hourly_energy_log_entry.l.inc_reactive_energy_QI;
        break;

    case ITEM_TAG_REACTIVE_ENERGY_QIV_LP_1:  //done test
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(current_hourly_energy_log_entry.logEntryNo == entry_no)
        {
        }
        else
        {
            load_hourly_energy_to_ram(entry_no);
        }
        val = current_hourly_energy_log_entry.l.inc_reactive_energy_QIV;
        break;
    case ITEM_TAG_ACTIVE_ENERGY_EXPORT_LP_1:  //done test
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(current_hourly_energy_log_entry.logEntryNo == entry_no)
        {
        }
        else
        {
            load_hourly_energy_to_ram(entry_no);
        }
        val = current_hourly_energy_log_entry.l.inc_active_export_energy;
        break;
     case ITEM_TAG_REACTIVE_ENERGY_QII_LP_1:  //done test
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(current_hourly_energy_log_entry.logEntryNo == entry_no)
        {
        }
        else
        {
            load_hourly_energy_to_ram(entry_no);
        }
        val = current_hourly_energy_log_entry.l.inc_reactive_energy_QII;
        break;
      case ITEM_TAG_REACTIVE_ENERGY_QIII_LP_1:  //done test
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(current_hourly_energy_log_entry.logEntryNo == entry_no)
        {
        }
        else
        {
            load_hourly_energy_to_ram(entry_no);
        }
        val = current_hourly_energy_log_entry.l.inc_reactive_energy_QIII;
        break;
// Get Billing Profile Values
    case ITEM_TAG_AE_IM_BI: val=Billing_Data[0].ActiveImport;  break;
    case ITEM_TAG_AE_EX_BI: val =Billing_Data[0].ActiveExport; break;

    case ITEM_TAG_RE_QI_BI: val=Billing_Data[0].ReactiveQI;   break;
    case ITEM_TAG_RE_QII_BI: val=Billing_Data[0].ReactiveQII;   break;
    case ITEM_TAG_RE_QIII_BI: val=Billing_Data[0].ReqctiveQIII;   break;
    case ITEM_TAG_RE_QIV_BI: val=Billing_Data[0].ReqctiveQIV;   break;
    case ITEM_TAG_MAX_DEMAND_BI: val =Billing_Data[0].MAX_Demand; break;
    
//    case ITEM_TAG_BALANCE_BI: val=Billing_Profile.Cumm_Energy_KWh_TZ7;   break;
    
    case ITEM_TAG_DAY_ID: 
          entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(currentDayProfile.day_id == entry_no)
          {
            
          }
          else
          {
             loadDailyProfile_dlms(entry_no);
          }
          
           val = currentDayProfile.day_id;
    break;
    
    
    case ITEM_TAG_DAY_PROFILE_SCRIPT_ID1:     
          entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(currentDayProfile.day_id == entry_no)
          {
            
          }
          else
          {
             loadDailyProfile_dlms(entry_no);
          }
          
           val = currentDayProfile.timeslots[0].rate_values_start_pointer;
        
    break;
    case ITEM_TAG_DAY_PROFILE_SCRIPT_ID2: 
          entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(currentDayProfile.day_id == entry_no)
          {
            
          }
          else
          {
             loadDailyProfile_dlms(entry_no);
          }
          
           val = currentDayProfile.timeslots[1].rate_values_start_pointer;
         
      break;  
     case ITEM_TAG_DAY_PROFILE_SCRIPT_ID3: 
          entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(currentDayProfile.day_id == entry_no)
          {
            
          }
          else
          {
             loadDailyProfile_dlms(entry_no);
          }
          
           val = currentDayProfile.timeslots[2].rate_values_start_pointer;
         
      break; 
     case ITEM_TAG_DAY_PROFILE_SCRIPT_ID4: 
          entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(currentDayProfile.day_id == entry_no)
          {
            
          }
          else
          {
             loadDailyProfile_dlms(entry_no);
          }
          
           val = currentDayProfile.timeslots[3].rate_values_start_pointer;
         
      break;
      
      case ITEM_TAG_DAY_ID_PASSIVE: 
            entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
            if(currentDayProfile.day_id == entry_no)
            {
              
            }
            else
            {
               loadDailyProfilePassive_dlms(entry_no);
            }
            
             val = currentDayProfile.day_id;
      break;
    
    
      case ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID1:     
            entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
            if(currentDayProfile.day_id == entry_no)
            {
              
            }
            else
            {
               loadDailyProfilePassive_dlms(entry_no);
            }
            
             val = currentDayProfile.timeslots[0].rate_values_start_pointer;
    
      break;
      case ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID2: 
            entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
            if(currentDayProfile.day_id == entry_no)
            {
              
            }
            else
            {
               loadDailyProfilePassive_dlms(entry_no);
            }
            
             val = currentDayProfile.timeslots[1].rate_values_start_pointer;
     
      break;  
      case ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID3: 
            entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
            if(currentDayProfile.day_id == entry_no)
            {
              
            }
            else
            {
               loadDailyProfilePassive_dlms(entry_no);
            }
            
             val = currentDayProfile.timeslots[2].rate_values_start_pointer;
     
      break; 
      case ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID4: 
            entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
            if(currentDayProfile.day_id == entry_no)
            {
              
            }
            else
            {
               loadDailyProfilePassive_dlms(entry_no);
            }
            
             val = currentDayProfile.timeslots[3].rate_values_start_pointer;
     
      break;
      
    //special days list
      case ITEM_TAG_SPECIAL_DAY_INDEX:
            entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
            if(currentHoliday.holiday_no == entry_no)
            {
            }
            else
            {
              loadCurrentHoliday( entry_no );
            }
            val = currentHoliday.holiday_no;
      break;
      case ITEM_TAG_SPECIAL_DAY_DAY_ID:
            entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
            if(currentHoliday.holiday_no == entry_no)
            {
              
            }
            else
            {
              loadCurrentHoliday( entry_no );
            }
            val = currentHoliday.day_id;
      break;    
      case ITEM_TAG_DISCONNECT_CONTROL_SCRIPT_SELECTOR:

            val = disconnect_script.control_script_type;
            val |= disconnect_script.control_script_type>>8;
      break;
/*
 * Daily snapshot/Daily absolute enegry profile/Load profile with recording period 2
 */
      case ITEM_TAG_CUM_KWH_TOTAL_LP_2:

    	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
          if(current_daily_energy_log_entry.logEntryNo == entry_no)
		  {

		  }
          else
          {
        	  load_daily_energy_to_ram(entry_no);
          }
          val = current_daily_energy_log_entry.l.active_import_energy;
          break;

      case ITEM_TAG_REACTIVE_ENERGY_QI_LP_2:

    	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
    	  if(current_daily_energy_log_entry.logEntryNo == entry_no)
    	  {

    	  }
    	  else
    	  {
    		  load_daily_energy_to_ram(entry_no);
    	  }
    	  val = current_daily_energy_log_entry.l.reactive_energy_QI;
    	  break;

      case ITEM_TAG_REACTIVE_ENERGY_QIV_LP_2:

    	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
    	  if(current_daily_energy_log_entry.logEntryNo == entry_no)
    	  {

    	  }
    	  else
    	  {
    		  load_daily_energy_to_ram(entry_no);
    	  }
    	  val = current_daily_energy_log_entry.l.reactive_energy_QIV;
    	  break;
/*******/
      default:
    	  val=0;
    	  break;
    }
    return val;
}
/*- End of function --------------------------------------------------------*/
extern uint8_t image_signature[];
extern uint8_t image_ident[];
int get_string_item(uint8_t *buf, int len, int item)
{
    unsigned int entry_no;
    uint8_t xxx[] = {12, 30, 45, 255};
    
    uint8_t currentTime[]= {INJECT16(rtcc.year), (rtcc.month),rtcc.day,rtcc.day-1, rtcc.hour, rtcc.minute, rtcc.second, 0xFF, INJECT16(120), 0x00};

    uint8_t zzz[] = {INJECT16(2010), MARCH, 20, TUESDAY, 15, 30, 45, 0xFF, INJECT16(120), 0x00};

    switch (item)
    {
    case  ITEM_TAG_IMAGE_IDENTIFICATION:     
       memcpy(buf, image_ident, len); 
      break;
      
    case  ITEM_TAG_IMAGE_SIGNATURE:
      memcpy(buf, image_signature, len);       
      break;
      
    case ITEM_TAG_NEW_CALENDER_ACTIVATION_TIME_CONTRACT1:
      break;
//    case ITEM_TAG_CURRENT_TIME: 
//      memcpy(buf, xxx, len); 
//      return len;
    case ITEM_TAG_CURRENT_DATETIME: // current instantanious values , done test
      memcpy(buf, currentTime, len); 
    return len;
    case ITEM_TAG_CUM_LAST_MD_EVENT_DATETIME: memcpy(buf, zzz, len); return len;

// Load Profile Data-period 1(hourly load profile
    case ITEM_TAG_DATETIME_LP_1: // done, test
      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      if(current_hourly_energy_log_entry.logEntryNo == entry_no)
      {

      }
      else
      {
          load_hourly_energy_to_ram(entry_no);
      }
      memcpy(buf, currentEnergyLogEntryData, len);
      return len;

    case ITEM_TAG_DATETIME_SE: // standard event
      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      if(currentEventLogEntry.logEntryNo == entry_no)
      {
      }
      else
      {
          loadEventToRam(entry_no);
      }
      memcpy(buf, currentEventLogEntryData, len);
      return len;
    case ITEM_TAG_DATETIME_FE: // fraud event
          entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	  if(current_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_fraud_event_to_ram(entry_no);
	  }
	  memcpy(buf, currentEventLogEntryData, len);
	  return len;
    case ITEM_TAG_DATETIME_CE: // common event
	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	  if(current_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_common_event_to_ram(entry_no);
	  }
	  memcpy(buf, currentEventLogEntryData, len);
	  return len;
    case ITEM_TAG_DATETIME_PQE: // power quality event
	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	  if(current_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_power_qual_event_to_ram(entry_no);
	  }
	  memcpy(buf, currentEventLogEntryData, len);
	  return len;
    case ITEM_TAG_DATETIME_FRME: // firmware event
	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	  if(current_firmware_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_firmware_event_to_ram(entry_no);
	  }
	  memcpy(buf, currentEventLogEntryData, len);
	  return len;

	case ITEM_TAG_FORMER_FIRMWARE: // firmware event-former firmware version
	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	  if(current_firmware_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_firmware_event_to_ram(entry_no);
	  }
	  memcpy(buf, &Device_ID_7[1], len);
	  return len;

    case ITEM_TAG_ACTIVE_FIRMWARE: // firmware event-active firmware version
	  entry_no = msg_info.start_entry + (msg_info.num_entries-msg_info.entries_remaining);
	  if(current_firmware_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_firmware_event_to_ram(entry_no);
	  }
	  memcpy(buf, current_firmware_event_log.l.active_firmware, len);
	  return len;

    case ITEM_TAG_DATETIME_SYNE1: // synchronization event-former clock
	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	  if(current_time_bound_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_synchronization_event_to_ram(entry_no,0);
	  }
	  memcpy(buf, currentEventLogEntryData, len);
	  return len;

	case ITEM_TAG_DATETIME_SYNE2: // synchronization event -clock after synchronization
	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	  if(current_time_bound_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_synchronization_event_to_ram(entry_no,1);
	  }
	  memcpy(buf, currentEventLogEntryData, len);
	  return len;

    case ITEM_TAG_DATETIME_DE: // disconnect control event
	  entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
	  if(current_disconnect_event_log.logEntryNo == entry_no)
	  {
	  }
	  else
	  {
		  load_disconnect_event_to_ram(entry_no);
	  }
	  memcpy(buf, currentEventLogEntryData, len);
	  return len;
// Billing Data
    case ITEM_TAG_BILLING_DATETIME_MAX_CURR:
      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
//       tempBuff = &(Billing_Profile.Bill_Date[0]);
//      memcpy(tempBuff,(Billing_Data+(entry_no-1)*sizeof(sBilling_Profile)),sizeof(sBilling_Profile));
      memcpy(buf, Billing_Data[entry_no-1].Bill_Date, len);
      break;
    case ITEM_TAG_BILLING_DATETIME_MAX_BI:
      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      //memcpy(buf,(Billing_Data+(entry_no-1)*sizeof(sBilling_Profile)),sizeof(sBilling_Profile));
      memcpy(buf,  Billing_Data[entry_no-1].MAX_DE_DATE, len);
      break;
       
    case ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME: memcpy(buf, xxx ,len); return len;
    case ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY: memcpy(buf, xxx ,len); return len;
    
    //special days entry
    case ITEM_TAG_SPECIAL_DAY_DATE:
        entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
        if(currentHoliday.holiday_no == entry_no)
        {
        }
        else
        {
          loadCurrentHoliday( entry_no );
        }
        
        memcpy(buf,currentHoliday_date,len);
        return len;
      break;

// Daily snapshot profile(Load profile with period 2)
    case ITEM_TAG_DATETIME_LP_2:
    	 entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
		 if(current_daily_energy_log_entry.logEntryNo == entry_no)
		 {
		  //memcpy(buf, currentEnergyLogEntryData, len);//currentEnergyLogEntryData
		 }
		 else
		 {
		   load_daily_energy_to_ram(entry_no);
		 }
		 memcpy(buf, currentEnergyLogEntryData, len);//currentEnergyLogEntryData
		return len;
    default:   memset(buf, 0, len); return len;
    }
    return 0;
}
/*- End of function --------------------------------------------------------*/
uint8_t quarters[] ={0,7,22,37,60};
                  // 0 15 30 45 60
void adjust_to_quarter(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    uint8_t temp;
    temp = rtcc.minute;
    uint8_t i=0;
    for(;i<5;i++)
    {
      if(temp<quarters[i])
      {
        temp=(i-1)*15;
        return;
      }
    }
    
    rtc_t time = {rtcc.year,rtcc.month,rtcc.day,rtcc.hour,temp,rtcc.second,0 };
    adjust_rtc(&time,0);    
}
/*- End of function --------------------------------------------------------*/

void adjust_to_measuring_period(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{

    
    
}
/*- End of function --------------------------------------------------------*/
extern const int8_t  month_lengths[];
void adjust_to_minute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  rtc_t time = {rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second};
     if(time.second >=30)
     {
       time.second++;
       if(time.second>=60)
       {
         time.minute++;
         time.second = 0;
         if(time.minute >= 60)
         {
           time.minute = 0;
           time.hour++;
           if(time.hour >=24)
           {
             time.hour = 0;
             time.day++;
                if ((rtc_backup.month == 2  &&  (rtc_backup.year & 3) == 0  &&  rtc_backup.day >= 29)//every 4 years february is 29 days 
                          ||
                     rtc_backup.day >= month_lengths[rtc_backup.month]) //else february is 28 days 
                 {
                   time.month++;
                   time.day = 1;
                   if(time.month > 12)
                   {
                     time.month = 1;
                     time.year++;
                   }
                 }
           }
           
         }
       }
     }

      adjust_rtc(&time,0);

  
}
/*- End of function --------------------------------------------------------*/


void adjust_to_preset_time(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
   if(compare_time(&preset_adj_time.preset_time,&preset_adj_time.validity_interval_start)>=0 &&
      compare_time(&preset_adj_time.preset_time,&preset_adj_time.validity_interval_end)>=0)
   {
       //adjust the rtc to the preset_time 
       adjust_rtc(&preset_adj_time.preset_time,1);
        
   }
}
/*- End of function --------------------------------------------------------*/


void preset_adjusting_time(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  
//    rtc_t preset_time,validity_interval_star,validity_interval_end;
    preset_adj_time.preset_time.year  =  *data;
    preset_adj_time.preset_time.year |=  *(++data);
    preset_adj_time.preset_time.month =  *(++data);
    preset_adj_time.preset_time.day = *(++data);
    ++data; //day of the week     
    preset_adj_time.preset_time.hour = *(++data);
    preset_adj_time.preset_time.minute = *(++data);
    preset_adj_time.preset_time.second = *(++data);
    ++data;
    ++data;
    ++data;
    ++data;
    
    
     preset_adj_time.validity_interval_start.year  =  *(++data);
     preset_adj_time.validity_interval_start.year |=  *(++data);
     preset_adj_time.validity_interval_start.month =  *(++data);
     preset_adj_time.validity_interval_start.day = *(++data);
    ++data; //day of the week     
     preset_adj_time.validity_interval_start.hour = *(++data);
     preset_adj_time.validity_interval_start.minute = *(++data);
     preset_adj_time.validity_interval_start.second = *(++data);
    ++data;
    ++data;
    ++data;
    ++data;
    
    
     preset_adj_time.validity_interval_end.year  =  *(++data);
     preset_adj_time.validity_interval_end.year |=  *(++data);
     preset_adj_time.validity_interval_end.month =  *(++data);
     preset_adj_time.validity_interval_end.day = *(++data);
    ++data; //day of the week     
     preset_adj_time.validity_interval_end.hour = *(++data);
     preset_adj_time.validity_interval_end.minute = *(++data);
     preset_adj_time.validity_interval_end.second = *(++data);
    ++data;
    ++data;
    ++data;
    ++data;
  
    
    
}
/*- End of function --------------------------------------------------------*/

//TODO. Finish this,
void shift_time(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    int32_t amount  = 0;
    uint32_t temp = 0;  
     
    amount = *data++;   
    
    temp = *data++;
    amount |= temp<<8;
    
    temp = *data++;
    amount |= (temp<<16);
    
    temp = *data++;
    amount  |= (temp<<24);
  
    uint8_t dir =0; // 0, move the time forward 1, move the time backward
    
    if(amount < 0)
    {
      temp = (~amount) + 1; 
      dir = 1;
    }
    else 
      temp = amount;
    
    
    uint8_t s_min = temp/60;
    uint8_t s_sec = temp%60;
    
    rtc_t time = {rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second,0};
    
    if(dir ==0)
    {
       time.second += s_sec;
       if(time.second >=60)
       {
         time.minute ++;
         time.second  =0;
         
         time.minute += s_min;
         if(time.minute >= 60)
         {
           time.minute = 0;
           time.hour++;
           if(time.hour >=24)
           {
             time.hour = 0;
             time.day++;
                if ((rtc_backup.month == 2  &&  (rtc_backup.year & 3) == 0  &&  rtc_backup.day >= 29)//every 4 years february is 29 days 
                          ||
                     rtc_backup.day >= month_lengths[rtc_backup.month]) //else february is 28 days 
                 {
                   time.month++;
                   time.day = 1;
                   if(time.month > 12)
                   {
                     time.month = 1;
                     time.year++;
                   }
                 }
           }
         }
       }
      
    }
    else if(dir == 1)
    {
       if(time.second < s_sec )
       {
         int8_t m_temp = time.minute;
         m_temp -= 1;
         time.second += (60 - s_sec);
         
         m_temp -= s_min;
         
         if(m_temp < 0)
         {
            time.minute = 60 + m_temp;
            
            m_temp = time.hour -1;
            if(m_temp < 0)
            {
                time.hour = 24 + m_temp;
                
                m_temp = time.day - 1;
                
                if(m_temp < 1) //set day to the previous month last day 
                {
                  //TODO. finish this 
                  //time.day = month_length[rtcc.month - 1];
                  
                }
                
                
            }else
            {
              time.hour = m_temp; 
            }
            
            
         }
         else
         {
           time.minute = m_temp;
         }
       }
    }
         
     adjust_rtc(&time,0);    
}
       

/*- End of function --------------------------------------------------------*/

void security_activate(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/

void global_key_transfer(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/


/*- End of function --------------------------------------------------------*/

void change_hls_secret(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  if(data_len!=18)
  {
    *response_len=0xffff; 
  }
  else
  {
    
    *response_len=0; 
  }     
}
void change_hls_secret_NS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  if(data_len!=18)
  {
    *response_len=0xffff; 
  }
  else
  {
    *response_len=0; 
  }     
}
void change_hls_secret_LS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  if(data_len!=18)
  {
    *response_len=0xffff; 
  }
  else
  {
    
    *response_len=0; 
  }     
}
void change_hls_secret_HS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  if(data_len!=18)
  {
    *response_len=0xffff; 
  }
  else
  {
    
    *response_len=0; 
  }     
}
/*- End of function --------------------------------------------------------*/

void add_object(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_lena)
{
}
/*- End of function --------------------------------------------------------*/

void remove_object(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/

/*void billing_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}*///n
/*- End of function --------------------------------------------------------*/

void tariff_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/
/*void GlobalReset_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    if(*data == 1) // parameter reset this is the script-ID in the coresponding script table 
    {
        //TODO. reset all the configuration parameters to the default status, 
        // change the EEPROM Formatted indicator 
        // and restart
    }
    else if(*data == 2) //data reset
    {
      //TODO. reset the Meter logged data's event,load profile and billing profile.
    }
}*///n
/*void Disconnect_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  //test this method and try to use the data_len argument 
    uint16_t methodNumber = *data;
             methodNumber |= ((uint16_t)(*(data++)))<< 8;
             
     if(methodNumber == 1)
     {
        status.DisconnectCommandRecived = 1;       
     }
     else if(methodNumber == 2)
     {
        status.ConnectCommandRecived = 1;
     }
}*///n

/*void insert_special_day(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
void delete_special_day(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}*///n
/*void remote_reconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    status.ConnectCommandRecived = 1;
    status.DisconnectCommandRecived = 0;
}*///n
/*void remote_disconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
   status.DisconnectCommandRecived = 1;
   status.ConnectCommandRecived = 0;
}*///n

void pre_remote_disconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
void pre_remote_reconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}

/*void activate_passive_calendar(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}*///n
/*- End of function --------------------------------------------------------*/

void Obj_Instant_Profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/

void Obj_Instant_Profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/

void Obj_Instant_Scaler_Profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/

void Obj_Instant_Scaler_Profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/

/*void Obj_Load_Profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 = EnergyLogAddress_Start;
      uint8_t temp8=0;
      LastEnergyLogAddress = EnergyLogAddress_Start;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);      
      write_to_eeprom(&temp8,(uint8_t *)0,setEnergyOverlapFlag); 
  
}*///n
/*- End of function --------------------------------------------------------*/

void Obj_Load_Profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*void Obj_Event_Log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 =EventLogAddress_Start;
      uint8_t temp8=1;
      LastEventLogAddress = EventLogAddress_Start;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);      
      write_to_eeprom(&temp8,(uint8_t *)0,setEventOverlapFlag);   
}*///n
/*- End of function --------------------------------------------------------*/

/*void Obj_Event_Log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}*///n

/*- End of function --------------------------------------------------------*/

void Obj_Load_Scaler_Profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/

void Obj_Load_Scaler_Profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*- End of function --------------------------------------------------------*/
/*- End of file ------------------------------------------------------------*/

void set_string_item(uint8_t item, uint8_t *data, uint16_t len)
{
//  unsigned int entry_no;
  switch(item)
  {
    //Activity Calendar 
     case ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_1: 
       
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      
      currentDayProfile.timeslots[0].start_Hour = *data++;
      currentDayProfile.timeslots[0].start_minute = *data++;
      currentDayProfile.collected_params_count++;        
      
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      }

    break;
    case ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_2:  
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      
      currentDayProfile.timeslots[1].start_Hour = *data++;
      currentDayProfile.timeslots[1].start_minute = *data++;
      currentDayProfile.collected_params_count++;        
      
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      }
    break;
    case ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_3:  
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      
      currentDayProfile.timeslots[2].start_Hour = *data++;
      currentDayProfile.timeslots[2].start_minute = *data++;
      currentDayProfile.collected_params_count++;        
      
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      } 
      break;
    case ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_4:  
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      
      currentDayProfile.timeslots[3].start_Hour = *data++;
      currentDayProfile.timeslots[3].start_minute = *data++;
      currentDayProfile.collected_params_count++;        
      
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      }
    break;
    case ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME:
          break;
    case ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY:
          break;
          
    case ITEM_TAG_DISCONNECTCONTROL_SCRIPT_EXE_DAY:
         disconnect_script.execution_time.year = *data++;
         disconnect_script.execution_time.year |= (*data++<<8);
         disconnect_script.execution_time.month = *data++;
         disconnect_script.execution_time.day = *data++;
         //disconnect_script.execution_time.dayoftheweek = *data++;
      break;
     case ITEM_TAG_DISCONNECTCONTORL_SCRIPT_EXE_TIME:
         disconnect_script.execution_time.hour   = *data++;
         disconnect_script.execution_time.minute = *data++;
         disconnect_script.execution_time.second = *data++;
         //disconnect_script.execution_time.hundredths= *data++;
      break;      
    default:
         break;
  }          
} 
void set_numeric_item(uint8_t item, uint8_t *data, uint16_t len)
{
  uint16_t x;
//  unsigned int entry_no;
  switch(item)
  {     
    //Activity calendar
    case ITEM_TAG_DAY_ID_PASSIVE: 
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      
      currentDayProfile.day_id = *data++;      
      currentDayProfile.collected_params_count++;        
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      }

    break;
    
    
    case ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID1:     
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      
      currentDayProfile.timeslots[0].rate_values_start_pointer = *data++;      
      currentDayProfile.collected_params_count++;        
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      }
    
    break;
    case ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID2: 
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      
      currentDayProfile.timeslots[1].rate_values_start_pointer = *data++;      
      currentDayProfile.collected_params_count++;        
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      }
     
      break;  
     case ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID3: 
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      currentDayProfile.timeslots[2].rate_values_start_pointer = *data++;      
      currentDayProfile.collected_params_count++;        
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      }
     
      break; 
     case ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID4: 
//      entry_no = msg_info.start_entry+(msg_info.num_entries-msg_info.entries_remaining);
      
      currentDayProfile.timeslots[3].rate_values_start_pointer = *data++;      
      currentDayProfile.collected_params_count++;        
      if(currentDayProfile.collected_params_count == 9)
      {
        writeDailyProfilePassive_dlms(&currentDayProfile);
      }
     
      break;
     case ITEM_TAG_DISCONNECT_CONTROL_SCRIPT_SELECTOR:
         x = *data++;
        x  |= (uint16_t)(*data++)<<8;
        disconnect_script.control_script_type = x;
        
      break;

    default:
         break;
  }        
}
