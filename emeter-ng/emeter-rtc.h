

#ifndef RTC_H
#define RTC_H


/**
* initialize the rtc module 
*/
void init_rtc();

/**
* set the rtc to new time 
* @param time. The time to adjust the rtc to 
*/
void adjust_rtc(rtc_t *time);

/**
* @return Day of the week. from 0 to 6, sunday to monday
* @param datetime:- the date time to get the day of the week for 
*/
int get_weekday(rtc_t *datetime);

/**
* checks wheather the passed time is in the valid range of ddate and time 
*/
uint8_t is_time_valid(const rtc_t *time_temp_var);

/**
* get the value of the hardware time 
* @param rtc -> the time returned from the hardware
*/
void getHardwareTime(rtc_t *rtc);

#endif
