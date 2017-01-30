/*
 * tester.c
 *
 *  Created on: Jan 20, 2017
 *      Author: eliab
 */
//#include <emeter-toolkit.h>
#include <stdint.h>
#include "SM502/utilities.h"


typedef struct
{
	uint32_t RTCHigh;
	uint32_t RTCLow;
}back_t;

back_t ts1;
extern rtc_t rtcc;
TimeStump ts;

rtc_t time;
TimeStump getTimeStamp(uint16_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t minute,uint8_t second);
void time_get_test()
{
	__bic_SR_register(GIE);
	while(1){
	//when power fails
	ts = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
	ts1.RTCHigh = ts.TimestampUp & 0x000000ff;
	ts1.RTCLow = ts.TimestampLow;

	//when power on
	TimeStump t = {ts1.RTCLow,(uint8_t)ts1.RTCHigh};
	time =  getTime(&t);
}
}
