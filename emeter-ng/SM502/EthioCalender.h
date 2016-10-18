



#ifndef EthioCalender_h
#define EthioCalender_h

#include "Logger.h"


#define Joffset (17238560000u) // scaled by 10000
/**
* changes date in GC to Ethiopian calender 
* @param GC -> pointer to time in gregorian calender 
* @param EC -> pointer to time in Ethiopian Calender 
*/
void toEthioCalender(const rtc_t *GC,rtc_t *EC);

/*
* @param GC -> Time in gregorian calender to convert to Judian number 
* @return the Judian number of a grigorian calender scaled by 10000
*         
*/
uint64_t JulianDayNumber(const rtc_t * GC);


/**
*   the method converts a judian number to Ethiopian calender date 
*       
*   @param  JulianDayNumber : the date time in Julian number
*   @EC      the converted date time in ethiopian calender    
*/
void EthiopianCalender(uint64_t JulianDayNumber,rtc_t *EC);

#endif 