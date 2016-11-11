

#include "utilities.h"
#include <string.h>
/**
  Compare two rtc_t structures
* @param time0:- the first time to compare 
* @param time :- the second time to compare
* if time0 comes after time 
*    return 1
* else if time0 == time
*    return 0
* else 
*    return -1
*     
*/
int8_t compare_time(const rtc_t *time0,const rtc_t *time)
{  
    if(time0->year > time->year)
    {
      return 1;
    }
    else if(time0->year == time->year)
    {
     if(time0->month > time->month)
     {
       return 1;
     }
     else if(time0->month == time->month)
     {
       if(time0->day > time->day)
       {
         return 1;
       }
       else if(time0->day == time->day)
       {
         if(time0->hour > time->hour)
         {
               return 1;
         }
         else if(time0->hour == time->hour)
         {
           if(time0->minute > time->minute)
           {
               return 1;
           }
           else if(time0->minute == time->minute)
           {
                if(time0->second > time->second)
                {
                  return 1;
                }
                else if(time0->second == time->second)
                {
                  return 0;
                }
                else 
                  return -1;
           }
           else 
           {
             return -1;
           }
         }
         else
         {
           return -1;
         }
       }
       else
       {
         return -1;
       }
     }
     else
     {
       return -1;
     }
    }
    else
    {
      return -1;
    }
}

/**
*  Calculates the time difference between 2 date Time structures in minute 
*  @param time1 the first time 
*  @param time2 the second time 
*  @return the Time difference in minute between the two dates 
*/
int64_t getTimeDifferenceInMinutes(const rtc_t *time1,const rtc_t *time2)
{
    int8_t yMinDiff = (time1->year - time2->year);   
    int8_t mMinDiff = (time1->month  - time2->month);         
    int8_t dMinDiff = (time1->day - time2->day);
    int8_t hMinDiff = (time1->hour - time2->hour);
    int8_t minMinDiff = (time1->minute - time2->minute);
    
    int64_t TimeDiffInMinutes = (NumberOfMinutesPerYear*yMinDiff) + (mMinDiff * NumberOfMinutesPerMonth) + 
                                 (dMinDiff * NumberOfMinutesPerDay) +(hMinDiff * 60) + minMinDiff;
   
    
    return TimeDiffInMinutes>0?TimeDiffInMinutes:0;
}


/*
* algoritm to change decimal number to character array filled with its digits 
* @param number : the decimal number
* @param output_buffer: output buffer of character array 
* @param : the buffer length 
* @param : outputlength :- the number of digits in the input number 
*               i.e the length of filled memoris in the output_buffer
*/
uint8_t longToString_c(unsigned long number,uint8_t *output_buffer,uint8_t buff_length,int *outputlength)
{
        //long remainder = 0;
  	//int i = -1;
	uint8_t num[]={0,0,0,0,0,0,0,0,0 }; //Assuming that we are going to write a number with maximum of 8 digits
          
        uint32_t mask=0x80000000;
        uint32_t c_bit=0;
        uint16_t num_bits=0;
        uint8_t stCount=0;
        if(number == 0 )
        {
          *outputlength=0;
          //return 0;
        }
        else
        {
          while(mask !=0)
          {
        	  int j;
            for(j=0;j<buff_length;j++)
             {
                if(num[j] >=5)
                  num[j] +=3;
             }
                        
             c_bit = number&mask;
             
             int i;
             for(i=buff_length-1;i>0;i--)
             {
               num[i]<<=1;
               num[i]&=0xf;
               num[i] |= ((num[i-1]&0x8)>>3);             
             }
              
             num[0]<<=1;
             num[0]&=0x0f;
             if(c_bit == mask)
             {
               if(stCount==0)
               {
                 stCount=1;
               }
                num[0]|= 1;  
             }
             mask>>=1;    
             if(stCount == 1)
               num_bits++;
          }
          
        }
        //num[0]&=0xf;
//	while (number != 0) {
//		i++;
//		remainder = number % 10;
//		number = (unsigned long) number / 10;
//		num[i] = remainder;              
//	}
        
         *outputlength = buff_length;
         int k;
         for(k=buff_length-1;k>=0;k--){
            if(num[k] !=0)
            {
              break;
            }
            *outputlength-=1;
         } 
        
        memcpy(output_buffer,num,*outputlength);

       
        return 1;
}

/**
* uncompresses compressed rtc_t struct 
* @param t : the comperssed time structure 
* @return rtc_t: returns the uncompressed rtc_t struct 
*/
rtc_t  getTime(TimeStump *t) 
{
  rtc_t temp;
  temp.second = (t->TimestampLow & 0x3F); //6bit 
  temp.minute = (t->TimestampLow & 0xFC0)>>6;
  temp.hour   = (t->TimestampLow & 0x1F000)>>12;
  temp.day = (t->TimestampLow & 0x3E0000) >>17;
  temp.month = (t->TimestampLow & 0x3E00000)>>22;
  temp.year = (t->TimestampLow & 0xF8000000)>>26;   /*[m.g] was ...>>27 changed to ....>>26 to fix Bug #5637 */
  temp.year |=(t->TimestampUp << 6);
  //temp.year |= 2000; // make the year 2000 and something /*[m.g] No real reason to do this. commented out while testing for Bug #5637 */
  
  
  if(!is_time_valid(&temp))
  {
    temp.year = 1;
    temp.month = 1;
    temp.day = 1;
    temp.hour = 1;
    temp.minute = 1;
    temp.second = 1;
    temp.isValid = 0;
  }
  else
    temp.isValid = 1;
  
  return temp;
}

/**
*  The method computes a compressed version of rtc_t structrue      
*  @see getTime
*  @param year  
*  @param month  
*  @param date  
*  @param hour  
*  @param minute
*  @param second
*  @return returns the compressed verion of the timestump
*/
TimeStump getTimeStamp(uint16_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t minute,uint8_t second)
{
  TimeStump t;
  unsigned long temp;
  unsigned long temp2;
  temp = second & 0x1F;   /* copy 6 bits for seconds */
  temp |= (minute<<6);    /* copy 6 bits for minutes */
  
  temp2=hour;
  temp |=((temp2&0x1F)<<12); /* copy 5 bits for hour */

  temp2= date;
  temp |=((temp2&0x1F)<<17); /* copy 5 bits for date */

  temp2=month;
  temp |=((temp2&0x0F)<<22); /* copy 4 bits for month */

  temp2=year;
  temp|=(temp2<<26);   /* copy lowest 6 bits of year. [m.g] was ...<<27 changed to ...<<26 to fix Bug #5637 bug */

  temp2>>=6;

  t.TimestampLow = temp;
  t.TimestampUp  = (uint8_t)temp2;       /* copy uper 8 bits of year */

  return t;
}

/**
*  calculates the sum of individual bytes of number or array
*  @see getTimeStamp
*  @param number: poiter to an array or long number 
*  @param length: the number of bytes in contained in the number pointer
*  @return returns the sum of each 8 byte values
*/
uint32_t getCheckSum(void *number,uint8_t length)
{
  uint8_t *val = (uint8_t *)number;
  int i=0;
  uint32_t sum=0;
  
  for(;i<length;i++)
  {
    sum = sum + *val++;
  }
  
  return sum;
  
}        

uint32_t mid_entry = 0;
int8_t res=0;
/*
*   Generic binary search implementation. The algorithm can do binary search in any kind of sorted list.
*   
*   @param search_item : description of the search criteria     
*   @returns 
*      0:  if the item is not found. The last item tested can be found in the last_entry_no of the search_item struct           
*     xx: the index of the item in the list 
*   
*/
uint32_t search_nearest_log(Search_t *search_item)
{
   search_item->last_entry_no = search_item->start_entry;
   
   while(search_item->start_entry <= search_item->end_entry)
   {
       mid_entry = search_item->start_entry + ((search_item->end_entry - search_item->start_entry)>>1);
       search_item->PeakItem(search_item->current_data, (mid_entry));//*search_item->SingleItemSize+ search_item->StartAddress
       res = search_item->Compare(search_item->current_data,search_item->search_data);
       
       if(res == 0)
       {
          return mid_entry;
       }
       else if(res>0)
       {
         search_item->end_entry   = mid_entry - 1;        
       }
       else 
       {
         search_item->start_entry = mid_entry + 1; 
       }
       
       search_item->last_entry_no = mid_entry;
   }   
   return 0;
}

