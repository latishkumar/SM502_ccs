#include <stdint.h>
#include <stdlib.h>

#include <msp430.h>

//#include <emeter-toolkit.h>
#include "SM502/Logger.h" /*PATH CORRECTION*/
//#include "emeter-structs.h"
#include "SM502/Status.h"
#include "SM502/PLC/PLC.h"
#include "emeter-rtc.h"
#include "SM502/self_diagnosis.h"

#if !defined(NULL)
#define NULL    (void *) 0
#endif


#include "SM502/Status.h"

rtc_t rtc_temp_var;
rtc_t default_time = {2015,1,16,6,25,0,10,1};
rtc_t rtcc;
rtc_t rtc_global_temp;
rtc_t rtc_backup;

extern MeterStatus status;
extern System_Status_type SystemStatus;
extern HardwareStatus hardware_status;
extern const int8_t month_lengths[13];

volatile uint8_t rtc_init = 0;
volatile uint8_t lpc = 0;
volatile uint8_t rtc_init_counter=0;

void init_rtc()
{
   /* Basic timer setup */
   /* Set ticker to 32768/(256*256) */
   RTCPS0CTL = RT0IP_7;                                 //   256
   RTCPS1CTL = RT1IP_6;// + RT1PSIE;                    //  /128 / 256 = / 32768 -> sec interval
  
   if (!check_rtc_sumcheck())
   {
       RTCCTL0_H = RTCKEY_H;                            // Unlock RTC
       if (RTCCTL0 & RTCOFIFG)                          // init if RTCOFIFG is set
       {
          RTCCTL13 = RTCBCD+RTCHOLD+RTCMODE+RTCTEV_0;   // Init RTC
          RTCCTL0_L &= ~RTCOFIFG_L;                     // Clear Flag
          RTCCTL13 &= ~RTCHOLD;                         // Enable RTC
        }
        else
        {
          RTCCTL13 = RTCBCD+RTCMODE+RTCTEV_0;           // Init RTC
        }
        RTCCTL0_L |= RTCRDYIE_L;
        RTCCTL0_H = 0;                                  // LOCK RTC
    }
}

int bump_rtc_backup(void)
{
    /* First check the consistency of the current rtc setting. If it is inconsistent,
       (i.e. has a bad sumcheck) do not bump it. */
//    if (!check_rtc_sumcheck())
//        return RTC_INCONSISTENT;
    if (++rtc_backup.second < 60)
    {
        set_rtc_sumcheck();
        return RTC_CHANGED_SECOND;
    }
    rtc_backup.second = 0;
    if (++rtc_backup.minute < 60)
    {
        set_rtc_sumcheck();
        return RTC_CHANGED_MINUTE;
    }
    rtc_backup.minute = 0;
    if (++rtc_backup.hour < 24)
    {
        set_rtc_sumcheck();
        return RTC_CHANGED_HOUR;
    }
    rtc_backup.hour = 0;
    if ((rtc_backup.month == 2  &&  (rtc_backup.year & 3) == 0  &&  rtc_backup.day < 29)//every 4 years february is 29 days 
        ||
        rtc_backup.day < month_lengths[rtc_backup.month]) //else february is 28 days 
    {
        ++rtc_backup.day;
        set_rtc_sumcheck();
        return RTC_CHANGED_DAY;
    }
    rtc_backup.day = 1;
    if (++rtc_backup.month <= 12)
    {
        set_rtc_sumcheck();
        return RTC_CHANGED_MONTH;
    }
    rtc_backup.month = 1;
    ++rtc_backup.year;
    //set_rtc_sumcheck();
    return RTC_CHANGED_YEAR;
}


uint8_t is_time_valid(const rtc_t *time_temp_var)
{
        if(time_temp_var->year >= 4095 || time_temp_var->month>12 || time_temp_var->day > month_lengths[time_temp_var->month] || time_temp_var->hour > 23 || time_temp_var->minute > 59 || time_temp_var->second > 59)
        {
          return 0;
        }
        return 1;
}

int get_weekday(rtc_t *datetime)
{    
    /* This works for years 2000-2099 */
    /* Result is  1=Monday, 7=Sunday,etc. */
    /* Allow for the day of the month */
    uint8_t i;
    uint8_t days = datetime->day + 6;
    /* Allow for the months to date this year... */
    for (i = 1;  i < datetime->month;  i++)
        days += month_lengths[i];
    /* ...with a little offset if we are early in a leap year */
    if ((datetime->year & 0x03) == 0  &&  datetime->month <= 2)
        days--;
    /* Allow for the years... */
    days += datetime->year;
    /* ...and a little extra for the leap years */
    days += (datetime->year >> 2);
    days %= 7;
    return days;
}

//Set the Current System Time to time. this method handles the hardware and all software timers that need to be corrected with the hardware
extern uint8_t day_of_week;
void adjust_rtc(rtc_t *time)
{
    RTCCTL0_H = RTCKEY_H;                       // Unlock RTC
    RTCCTL13 = RTCBCD+RTCHOLD+RTCMODE+RTCTEV_0; // Init RTC
    //RTCCTL13 |=RTCHOLD;
    if(time->second != 0xFF)
    {
        BIN2BCD = time->second;
        RTCSEC  = BIN2BCD;
        rtc_temp_var.second    = time->second;
        rtc_global_temp.second = rtc_temp_var.second;
        rtc_backup.second      = rtc_temp_var.second;
        rtcc.second            = rtc_temp_var.second;
    }
    if(time->minute != 0xFF)
    {
        BIN2BCD = time->minute;
        RTCMIN  = BIN2BCD;
        rtc_temp_var.minute    = time->minute;
        rtc_global_temp.minute = rtc_temp_var.minute;
        rtc_backup.minute      = rtc_temp_var.minute;
        rtcc.minute            = rtc_temp_var.minute;
    }
    if(time->hour != 0xFF)
    {
        BIN2BCD = time->hour;
        RTCHOUR = BIN2BCD;
        rtc_temp_var.hour    = time->hour;
        rtc_global_temp.hour = rtc_temp_var.hour;
        rtc_backup.hour      = rtc_temp_var.hour;
        rtcc.hour            = rtc_temp_var.hour;
    }
    if(day_of_week != 0xFF)
    {
        BIN2BCD = day_of_week;
        RTCDOW  = BIN2BCD;
    }
    if(time->day != 0xFF)
    {
        BIN2BCD = time->day;
        RTCDAY  =  BIN2BCD;
        rtc_temp_var.day    = time->day;
        rtc_global_temp.day = rtc_temp_var.day;
        rtc_backup.day      = rtc_temp_var.day;
        rtcc.day            = rtc_temp_var.day;
    }
    if(time->month != 0xFF)
    {
        BIN2BCD = time->month;
        RTCMON  = BIN2BCD;
        rtc_temp_var.month    = time->month;
        rtc_global_temp.month = rtc_temp_var.month;
        rtc_backup.month      = rtc_temp_var.month;
        rtcc.month            = rtc_temp_var.month;
    }
    if(time->year != 0xFF)
    {
        BIN2BCD = time->year;
        RTCYEAR = BIN2BCD;
        rtc_temp_var.year    = time->year;
        rtc_global_temp.year = rtc_temp_var.year;
        rtc_backup.year      = rtc_temp_var.year;
        rtcc.year            = rtc_temp_var.year;
    }

    RTCCTL13 &= ~RTCHOLD;  // Enable RTC
    RTCCTL0_H = 0;         // LOCK RTC

    status.UpdateDate = 1;
    status.UpdateTime = 1;
}

void getHardwareTime(rtc_t *rtc)
{
            BCD2BIN = RTCSEC;
            rtc->second = BCD2BIN;// (x & 0xf) + (10 * (x>>4));
            
            //x = RTCMIN;
            BCD2BIN = RTCMIN;
            rtc->minute = BCD2BIN;//(x & 0xf) + (10 *(x>>4));
        //    rtcc.minute =  rtc_backup.minute%60;
            
            //x = RTCHOUR;
            BCD2BIN = RTCHOUR;
            rtc->hour = BCD2BIN;//(x & 0xf) + ((x >>4)*10);
            
            //x = RTCDAY;
            BCD2BIN = RTCDAY;
            rtc->day = BCD2BIN;//(x & 0xf) + (10 *(x>>4)); 
            
            //x = RTCMON;
            BCD2BIN = RTCMON;
            rtc->month = BCD2BIN;//(x&0xf) +((x>>4)*10);
         
            
            //rtcc.year = RTCYEAR;
            BCD2BIN = RTCYEAR;
            rtc->year = BCD2BIN ;//(rtcc.year &0xf)TimeStump +(((rtcc.year>>4)&0x0f)*10) + (((rtcc.year>>8)&0x0f)*100)+((rtcc.year>>12)&0x0f)*1000;
            
}

//ISR(RTC, one_second_ticker)
#pragma vector=RTC_VECTOR
__interrupt void one_second_ticker(void)
//void one_second_ticker(void)
{
    switch(RTCIV)
    {
        case RTCIV_RTCRDYIFG://RTCIV_RT1PSIFG:
                         
            getHardwareTime(&rtc_temp_var);
            
            //This is used by the PLC Initializing Routine to do some waiting
            if(plc_state  == INITIALIZING)
            {
               plc_counter ++;
            }
            if(rtc_init == 0)//is the first interrupt after power reset, we need to verify the consistency of the hardware rtc registers
            {
            	rtc_init_counter++;
                if(rtc_init_counter == 3)
                {
                	rtc_init = 1;
                    rtc_init_counter = 0;
                }
             
                //      temp_init_rtc=0;
                //     }
                //status.rtc_init = 1;
				if(!is_time_valid(&rtc_temp_var)) //the hardware RTC registers are not fine so reset the time. to fixed set point in time
				{                                 //this will be replaced by the RTC diagnosis if the last time stored in the EEPROM is valid
					hardware_status.RTCResetToDefaultStatus = 1;
					adjust_rtc(&default_time);
					//TODO. //log event to synchronization
				}
				else
				{
					rtcc.year = rtc_temp_var.year;
					rtcc.month = rtc_temp_var.month;
					rtcc.day = rtc_temp_var.day;
					rtcc.hour = rtc_temp_var.hour;
					rtcc.minute = rtc_temp_var.minute;
					rtcc.second = rtc_temp_var.second;
				}

		    }
            else
            {
            	rtcc.year = rtc_temp_var.year;
                rtcc.month = rtc_temp_var.month;
                rtcc.day = rtc_temp_var.day;
                rtcc.hour = rtc_temp_var.hour;
                rtcc.minute = rtc_temp_var.minute;
                rtcc.second = rtc_temp_var.second;

                if(SystemStatus==SYSTEM_INITIALIZING) // if the System is initializing
                      return;
                   
                bump_rtc_backup();

                if(rtc_global_temp.minute != rtcc.minute)
                {
                	//per minute activity
                    rtc_global_temp.minute = rtcc.minute;
//                  per_minute_activity();//run activities that needs to be executed every minute
                    status.MiuteElapsed = 1;
                }

				if(rtc_global_temp.hour != rtcc.hour)
				{
					//per hour activity
				    rtc_global_temp.hour = rtcc.hour;
				    status.UpdateDate = 1;
				    if(rtcc.hour == 0)
				    {
				        status.DayChanged = 1;
				    }
				}
				if(rtc_global_temp.day != rtcc.day)
				{
				   rtc_global_temp.day = rtcc.day;
				   //per day activity
				   if(hardware_status.RTCResetToDefaultStatus != 1 && hardware_status.RTC_Status != 0)
					 status.DayChanged=1;
//                          per_day_activity();//run activities that needs to be executed every day
				}
				if(rtc_global_temp.month != rtcc.month)
				{
				   //per month activity
				   rtc_global_temp.month = rtcc.month;
				   if(hardware_status.RTCResetToDefaultStatus != 1 && hardware_status.RTC_Status != 0)
					 status.MontheChagned = 1;
//                          per_month_activity(); //run activities that needs to be executed every month
				}

				rtc_global_temp.year = rtcc.year;
            }
                  
		   //Since RTC will be running from them moment it is initialized, we need to wait for the
		   //other modules to finish initializing before doing any thing that depends on them
		   if(SystemStatus==SYSTEM_INITIALIZING) // if the System is initializing
			  return;

			status.UpdateTime = 1;
			//status.UpdateDate = 1;

			if( operating_mode == OPERATING_MODE_NORMAL)
			{
			  #if defined(USE_WATCHDOG)
			  kick_watchdog();
			  #endif
			}
			else if(operating_mode == OPERATING_MODE_POWERFAIL)
			{
			  lpc++;
			}
		   status.SecondElapsed = 1;
		   set_rtc_sumcheck();
                                           
      break;
      default:
        	break;
    }
}

