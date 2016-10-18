#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include <emeter-toolkit.h>
#include "../emeter-rtc.h"

#define NumberOfMinutesPerYear 529200uL
#define NumberOfMinutesPerMonth 43200uL
#define NumberOfMinutesPerDay 1440


/*
 *	Parity	Year	Month	Date	Hour	Minute	Second
 *	7 bit	7 bit	4 bit	5 bit	5 bit	6 bit	6 bit
					5 Byte
 *
 */
typedef struct
{
	unsigned long TimestampLow; // 4 byte
	uint8_t TimestampUp;   //1 byte
}TimeStump;

/**
*  Calculates the time difference between 2 date Time structures in minute 
*  @param time1 the first time 
*  @param time2 the second time 
*  @return the Time difference in minute between the two dates 
*/
int64_t getTimeDifferenceInMinutes(const rtc_t *time1,const rtc_t *time2);


/**
*   Compares two time structures 
*   @param time0 the first time sturcture 
*   @param time1 the second time structure 
*   @return 1  if time0 comes after time    
*           0  if time0 is the same as time 1
*           -1 if time0 comes before time 1
*/
int8_t compare_time(const rtc_t *time0,const rtc_t *time);

/**
*  converts a long number of maximum 9 digits to its individual digits in ascii form
*  @param number:- the number to convert to ascii form 
*  @param output_buffer:- array of char to put the solution in 
*  @param outputlength:- the number of valid digits in the number
*      
*  @return 1 if conversion was done and puts the result in the output_buffer
*          0 if no conversion was done  and put o in the output_buffer 
*/
uint8_t longToString_c(unsigned long number,uint8_t *output_buffer,uint8_t buff_length,int *outputlength);

/**
* uncompresses compressed rtc_t struct 
* @param t : the comperssed time structure 
* @return rtc_t: returns the uncompressed rtc_t struct 
*/
rtc_t  getTime(TimeStump *t); 
/**
*  calculates the sum of individual bytes of number or array
*  @see getTimeStamp
*  @param number: poiter to an array or long number 
*  @param length: the number of bytes in contained in the number pointer
*  @return returns the sum of each 8 byte values
*/
uint32_t getCheckSum(void *number,uint8_t length);


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
TimeStump getTimeStamp(uint16_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t minute,uint8_t second);


typedef struct 
{  
  
  uint32_t last_entry_no;//the last entry to be checked for read
  
  uint32_t start_entry; //search start entry number 
  uint32_t end_entry;  //search enc entry enumber 
  void *search_data;  //the data to search 
  void *current_data;  //the current read data
  
//  void (*getComapareItem)(void *Item,void *current_data);
  int8_t(*PeakItem)(void *Item,unsigned long entry_number);  //get item method
  int8_t(*Compare)(void *Item1, void *Item2); //compare method
  
}Search_t;

/*
*   Generic binary search implementation. The algorithm can do binary search in any kind of sorted list.
*   
*   @param search_item : description of the search criteria     
*   @returns 
*      0:  if the item is not found. The last item tested can be found in the last_entry_no of the search_item struct           
*     xx: the index of the item in the list 
*   
*/
uint32_t search_nearest_log(Search_t *search_item);

//uint32_t search_nearest_energy(Search_t *serach_item);
#endif 