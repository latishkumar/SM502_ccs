/*
 * Tariff.h
 *
 *  Created on: Apr 29, 2013
 *      Author: Donayam
 */

#ifndef TARIFF_H_
#define TARIFF_H_

#define MaxTariffRates 16
#define MaxHolidays 24
#define MaxScheduleTimeslots 4

#include <stdint.h>
   
#define POST_PAID 0
#define PRE_PAID  1 
   
/*
 *tariff_Type :- weekend
 *		 weekdays
 *		 Sunday
 *               public holidays
 *start_Hour  :- the hour the tariff is started
 *
 *start_minute:- the minute the tariff is started
 *
 */



/*
 * tariff is stored as 32 bit number
 * this supports 4 digit after decimal and
 * this supports up to (429496.7296) for a single tariff
 *
 *
 * Let us store a total of 4 tariffs per day for weekdays
 * let us store a total of 4 tariffs per day for Holidays
 * let us store a total of 4 tariffs for Sunday
 * let us store a total of 4 tariffs for Saturday
 * atotal of 28 tariff rates 
 *
*/




/*
 * every day will have 4 schedules
 * each schedule will need  (8 byte)???????????//
 *
 * a Total of 3(1weekend + 1weakday +1special_day_profile) * 4sch/day * 8 byte/schedule = 96 byte
 * which is 0.75 page   
 *
 * let us make this  1 pages 
 *  
 * 
 *  
 */

//one season 
//week profile, 2 weakeds and wekdays 
//day profile, 4 day profile    
//special day profile 
   
typedef struct
{
  uint8_t start_minute;//6 bit                                                      1 byte 
  uint8_t start_Hour;//5 bit                                                        1 byte 
  
  uint16_t rate_values_start_pointer;//rate number                            //    2 possible values 
  
  uint32_t checksum; //8 bit checksum                                               4 byte 
  
}Schedule_timeslot_t;


#define Schedule_timeslot_init {0,0,0,0}



typedef struct{
   Schedule_timeslot_t timeslots[4];
//   uint32_t rates[40];
   uint8_t day_id;   
   uint8_t collected_params_count; // used only for writting purpose 
}DailyProfileEntry;


/*
  3 schadeles with up to 10 power switching points  with 4 scadules/day 
  3*10*4 = 120 

  = 120 * 4byte = 480byte , 4page 
    
  extern uint32_t billing_rates[120]; 
  uint8_t rates_start_values_array[10];
*/



/*
 *24 holidays
 *each holiday will need  3 byte
 *so 24 holidays will need a total of 72 byte
 *
 *let make this one page for easy of configuration i.e 128 byte
 */




typedef struct
{

   uint16_t year; //7 bit
   uint8_t month;//4 bit
   uint8_t day;  //5 bit
   uint8_t Checksum;// 8 bit checksum
   
}Public_Holiday_t;

#define MAX_HOLIDAYS  24
#define Init_public_Holiday {0,0,0,0}

/*
 *	 ==>balance is 4byte number, the actual balance scaled by 10000, this supports a maximum balance of 429496.7296birr
 * 			which is more than expected to be required.
 *	 ==>Sum check is the sum of each byte of the balance used as a error checking.
 */
typedef struct
{
	int32_t balance; //The current amount of money the user has  4byte
	uint8_t checksum; // error byte for the balance	1byte
        
}CurrentBalance;


extern CurrentBalance Current_balance;
/**
* initializes the tariff module 
*               read the current balance from EEPROM
*               load the holidays form EEPROM
*               Load today's tariff rate
*               
*/
void initTariff();


/**
* reads and loads todays billing profile from EEPROM
* @param day_id : 
*         1:- weekdays  monday...friday
          2:- weekends  saturday, and sunday
          3:- special days,  for special days tables 
*
* @param entry the value read
*/
void loadDailyProfile(uint8_t day_id,DailyProfileEntry *entry);
/**
* loads the passive daily profile from EEPROM
* @see loadDailyProfile
*/
void loadDailyProfilePassive(uint8_t day_id,DailyProfileEntry *entry);
/**
* @see loadDailyProfilePassive
*/
void writeDailyProfilePassive_dlms(DailyProfileEntry *DayProfile);
/**
* loads todays profile to EEPROM
* day from 0 to 6 for sunday to monday
*/
void LoadTodaysTariff(int day);
/**
* calclated the billing based on the day, tariff rate, and consumption according to the current EEU billing schema
*/
void CalculateBilling();
/**
* helper method for CalculateBilling
* @param CurrentBalance: the current balance
* @param TempLastEnergy: the last energy value that is billed 
* @param CurrentEnergy_Import_Reading: the current energy import reading
* @param ConsumptionSinceLastBilling: the total consumption of the meter since the begging of this month
*/
void CalculateTariff_New(int32_t *CurrentBalance, unsigned long *TempLastEnergy,unsigned long CurrentEnergy_Import_Reading,
                         unsigned long *ConsumptionSinceLastBilling);
/**
* get the tariff slot that should be used for the current hour and minute based on 
* the configured time slots
*/
int GetCurrentMinuteTariffTimeSlot();


/**
* Switches the billing shema from pre padi to post paid or viseversa
*@param schema : PRE_PAID
*               POST_PAID
*/
void switch_billing_schema(uint8_t schema);

/*
* checks if the current balance is bellow the minimum tariff rate 
* @param power_breaks :- ethiopian power levls where billing rate is changed 
* @param consumtionSinceLastBilling: this months consumption      
*    returns 1: if less than the minimum tariff 
*            0: otherwise 
*/
int CheckTariff(const uint32_t *power_brakes,unsigned long *ConsumptionSinceLastBilling);
/**
* returns the start index for the current users billing profile 
* @param rates_start_array10: the configured rate switching energy values array
* @param EnergySinceLastBillingCuttoff: consumed energy since last billing
*/
uint8_t getRateStartIndex(const uint32_t *rates_start_array10, unsigned long EnergySinceLastBillingCuttoff);
#endif /* TARIFF_H_ */
