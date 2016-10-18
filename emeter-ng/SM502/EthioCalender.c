

#include "EthioCalender.h"
#include <math.h>

/**
* division opeator
*/
uint32_t quotient(uint64_t i,uint64_t j)
{
  return (uint32_t)floor(i/j);
}
/**
* module operator 
*/
uint16_t mod(uint64_t i,uint64_t j)
{
   return i - (j * quotient(i,j));
}
/*
* @param GC -> Time in gregorian calender to convert to Judian number 
* @return the Judian number of a grigorian calender scaled by 10000
*         
*/
uint64_t JulianDayNumber(const rtc_t *GC)
{
   uint64_t w = GC->year;
     w *= 367;

   uint16_t z = (7*(GC->year + ((GC->month+9)/12)))/4;
   uint16_t v = ((275*GC->month)/9) + GC->day;
   float time_h = GC->hour + ((GC->minute)/60.0f);
   float temp = time_h/24.0f;
   uint64_t x = 17210140000 ;//+ (temp) ;//1721013.5 + 0.5 * 10000
   
    uint8_t sign = 0;
    float zz = (100.0f*GC->year) + GC->month - 190002;
    if(zz < 0.5f)
      sign = 1;
    
    if(sign == 0)//positive 
      temp -=0.5f;
    else 
      temp +=0.5f;
    uint32_t temp2 = (uint32_t)(temp * 10000);  
    x +=((w-z+v)*10000) + temp2;
    
    return x;      
}
/**
*   the method converts a judian number to Ethiopian calender date 
*       
*   @param  JulianDayNumber : the date time in Julian number
*   @EC      the converted date time in ethiopian calender    
*/
void EthiopianCalender(uint64_t JulianDayNumber,rtc_t *EC)
{  
    uint64_t r = mod(((JulianDayNumber - Joffset)/10000), 1461);
    uint64_t n = ((mod(r,365))+(365*quotient(r,1460)));
    
    uint64_t year  = (4 * quotient((JulianDayNumber - Joffset), 1461)) + quotient(r,365) - quotient(r,1460);
             year/=10000;
             
    uint8_t  month = (quotient(n,30)) +1;
    uint8_t day = mod(n,30) + 1;
    
    EC->year = year;
    EC->month = month;
    EC->day = day;
  
}
/**
* changes date in GC to Ethiopian calender 
* @param GC -> pointer to time in gregorian calender 
* @param EC -> pointer to time in Ethiopian Calender 
*/
void toEthioCalender(const rtc_t *GC,rtc_t *EC)
{
   uint64_t JD = JulianDayNumber(GC);
   EthiopianCalender(JD,EC);
}