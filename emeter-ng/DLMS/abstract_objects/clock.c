/*
 * clock.c
 *
 *  Created on: May 12, 2017
 *      Author: abi
 */
#include "headers.h"
#include "clock.h"
#include "self_diagnosis.h"
int16_t clock_time_zone = 0x1234;    //  time zone
uint8_t clock_status = 0;           //  status
uint8_t daylights_savings_begin[] =  // daylights savings begin
{
  12,INJECT16(2010), MARCH, 20, TUESDAY, 15, 30, 45, 0xFF, INJECT16(120), 0x00,
};

uint8_t daylights_savings_end[] =    // daylights savings end
{
  12,INJECT16(2010), MARCH, 20, TUESDAY, 15, 30, 45, 0xFF, INJECT16(120), 0x00,
};

int8_t clock_dst_deviation = 60;    // daylights savings deviation

int8_t clock_dst_enabled = 0;       // daylights savings enabled
uint8_t clock_base = 1;             // internal crystal

uint8_t max_allowed_time_shift = 30;   // Maximum allowed time shift without registration of a time shift event

uint16_t clock_time_shift_invalid_lim = 600; // Maximum allowed time shift without setting IV measurement status to 1

uint8_t last_clock_synchronized_date_time[] = // Date (corrected) of last clock synchronization / setting
{
  12,'Y','Y','M','M','D','D','H','H','M','M','S','S'
};
uint8_t day_of_week;
void get_date_time_string(void *data, int direction)
{
  if(direction == ATTR_READ)
  {
     uint8_t dw = RTCDOW;//get_weekday(&rtcc);
     uint8_t temp[] = {12,INJECT16(rtcc.year), rtcc.month, rtcc.day, dw , rtcc.hour, rtcc.minute, rtcc.second, 0xFF, INJECT16(120), 0x00};
     memcpy(data,temp,13);
  }
  else{

   //TODO. Test this
     rtc_t temp;
     uint8_t *datap = data;
     uint8_t utemp;
     utemp = *datap++;//length
     utemp = *datap++;
     temp.year = *datap++;
     temp.year |= (utemp<<8);
     temp.month = *datap++;
     temp.day = *datap++;
     day_of_week = *datap++; // dow of the week
     temp.hour = *datap++;
     temp.minute = *datap++;
     temp.second = *datap++;

     hardware_status.RTCResetToDefaultStatus = 0;// && rtc is not reset to default
     hardware_status.RTC_Status = 1; // rtc is now ok
     // log synchronization event
     time_bound_event_log l;
     l.event_code = CLOCK_PROGRAMMED;
     l.begin_time_stamp = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second); //former clock
     adjust_rtc(&temp,1);
     l.end_time_stamp = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second); //after synchronization
     l.checksum  =(getCheckSum(&(l.end_time_stamp.TimestampLow),4) + l.end_time_stamp.TimestampUp + l.event_code)&0xff;
     uint8_t tmp = 8;
     write_to_eeprom(&l,&tmp,log_events);
  }

}
