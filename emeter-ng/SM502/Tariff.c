#include "Tariff.h"
#include "Logger.h"
//#include "RTCC.h"
#include "Status.h"
#include<stdint.h>
//#include "../emeter-structs.h"

uint8_t billingSchema = PRE_PAID;


Public_Holiday_t holidays[MAX_HOLIDAYS];

//Schedule_timeslot_t todaysTimeslot[4];
CurrentBalance Current_balance;
CurrentBalance Balance_PostPaid;


//long MinimumTariff = 2000; //the minimum tariff allowed to use the meter, //not used 

uint32_t low_balance_temp;
//extern RTCtime time;
extern rtc_t rtcc;
extern MeterStatus status;
extern  struct phase_parms_s chan1;

uint32_t TempLastEnergyValue;

uint8_t TariffError =0;
uint32_t TotalKWHRSinceLastBillingPeriod = 0;

DailyProfileEntry todaysRates;
uint32_t rates[40] = {2730,3564,4993,5500,5666,5880,6943,6943,6943,6943,
                      2730,3564,4993,5500,5666,5880,6943,6943,6943,6943,
                      2730,3564,4993,5500,5666,5880,6943,6943,6943,6943,
                      2730,3564,4993,5500,5666,5880,6943,6943,6943,6943
                     }; //according to the current EEPCO ratting 1/1/2014

uint32_t power_brakes[10]={50,100,200,300,400,500,100000,100000,100000,100000};//current ratting 
uint32_t ConsumptionSinceLastBilling=0;

//extern __monitor void perform_low_battry_backup();

int32_t lowtariff_temp =0;

//TODO: implement methods that use this variables 
rtc_t  Current_billing_Period_start;
rtc_t  Estimated_billing_period_end; 
uint8_t Current_billing_Duration_in_days = 30;
uint8_t Current_billing_Duration_Counter = 1;

/**
* initializes the tariff module 
*               read the current balance from EEPROM
*               load the holidays form EEPROM
*               Load today's tariff rate
*               
*/
void initTariff()
{
    validation_arg_t validation_arg;
    validation_arg.validate = 0;
    read_from_eeprom(&Current_balance,(uint8_t *)0,getCurrentBalance,&validation_arg);
        
    //load public holidays 
    uint8_t i=0;
    for(;i < MAX_HOLIDAYS;i++)
    {
      getPublicHoliday(i,&holidays[i]);
    }
    
    //load tariff rates 
    LoadTodaysTariff(rtcc.day);
    
    //load the power brake points 
    for(i=0;i<10;i++)
    {
      getPowerBreaks(&power_brakes[i],&i );
    }     
}

/**
* calclated the billing based on the day, tariff rate, and consumption according to the current EEU billing schema
*/
void CalculateBilling()
{                           
         
#ifndef KIT
      
   if(billingSchema == POST_PAID)
      CalculateTariff_New(&Balance_PostPaid.balance,&TempLastEnergyValue,phase->active_energy_import,&ConsumptionSinceLastBilling);
   else        
      CalculateTariff_New(&Current_balance.balance,&TempLastEnergyValue,phase->active_energy_import,&ConsumptionSinceLastBilling);
#endif 
       
}

uint8_t getRateStartIndex(const uint32_t *rates_start_array10, unsigned long EnergySinceLastBillingCuttoff)
{
  uint8_t i =0;
   for(;i<10;i++)
   {
      if(*(rates_start_array10++)>EnergySinceLastBillingCuttoff)
      {
          return i;
      }
   }
   return 9;//return the highest rate index
}
/**
* helper method for CalculateBilling
* @param CurrentBalance: the current balance
* @param TempLastEnergy: the last energy value that is billed 
* @param CurrentEnergy_Import_Reading: the current energy import reading
* @param ConsumptionSinceLastBilling: the total consumption of the meter since the begging of this month
*/
void CalculateTariff_New(int32_t *CurrentBalance, unsigned long *TempLastEnergy,unsigned long CurrentEnergy_Import_Reading,
                         unsigned long *ConsumptionSinceLastBilling)
{
    int32_t NetEnergyReading = CurrentEnergy_Import_Reading - *TempLastEnergy;
    if(NetEnergyReading >= 1000)
    {
        int32_t temp = NetEnergyReading / 1000; //avoid floating point artimatic by calculating only for even number of Thousands
        *TempLastEnergy  += NetEnergyReading + ((CurrentEnergy_Import_Reading%1000));//WH +  //was chan1.consumed_active_energy
        unsigned long charge = 0;
        uint8_t tariffslot=0;

        tariffslot = GetCurrentMinuteTariffTimeSlot();  //on which 4 slots of the day the tariff is found

        while(temp > 0) //it is nearly impossible for temp to be greater than 1
        {
            ++(*ConsumptionSinceLastBilling);
            uint8_t rateIndex = getRateStartIndex(power_brakes,*ConsumptionSinceLastBilling );
            charge = rates[rateIndex+(tariffslot*10)];

            if(billingSchema == PRE_PAID)
                *CurrentBalance -= charge;
            else
                *CurrentBalance += charge;
            temp--;
        }
        write_to_eeprom(&Current_balance,(uint8_t *)0,setCurrentBalance);
        write_to_eeprom(&TempLastEnergyValue,(uint8_t *)0,setTempLastEnergyValue);
        write_to_eeprom(&ConsumptionSinceLastBilling,(uint8_t *)0,setConsumptionSinceLastBilling);
                
        if(billingSchema == PRE_PAID)
            CheckTariff(power_brakes,ConsumptionSinceLastBilling);
        else
            status.LowTariff = 0;
    }
}

/**
* Switches the billing shema from pre padi to post paid or viseversa
*@param schema : PRE_PAID
*               POST_PAID
*/
void switch_billing_schema(uint8_t schema)
{
   if(billingSchema != schema)
   {
       if(schema == PRE_PAID)
       {
          //TODO. log schema changed event 
          //handle the current pre paid and post paid balances
          //remove low balance errors 
          status.LowTariff = 0;
//          setBillingSchema(PRE_PAID);
          uint8_t bs = PRE_PAID;
          write_to_eeprom(&bs,(uint8_t *)0,setBillingSchema);
       }
       else if (schema == POST_PAID)
       {         
//          setBillingSchema(POST_PAID);
          uint8_t bs = POST_PAID;
          write_to_eeprom(&bs,(uint8_t *)0,setBillingSchema);          
       }     
   }
 
}
/**
* reads and loads todays billing profile from EEPROM
* @param day_id : 
*         1:- weekdays  monday...friday
          2:- weekends  saturday, and sunday
          3:- special days,  for special days tables 
*
* @param entry the value read
*/
void loadDailyProfile(uint8_t day_id,DailyProfileEntry *entry)
{
   uint8_t scheduleNumberStart = (day_id-1) * 4;
   uint8_t i=scheduleNumberStart,j=0;
   uint8_t end = i+4;
   for(;i<end;i++,j++)
   {
      getTariffSchaduleTimeslot(i,&entry->timeslots[j]);    
   }   
   entry->day_id = day_id;   
}
/* day_id 1:- weekdays  monday...friday
          2:- weekends  saturday, and sunday
          3:- special days,  for special days tables 
*/
void loadDailyProfilePassive(uint8_t day_id,DailyProfileEntry *entry)
{
   uint8_t scheduleNumberStart = (day_id-1) * 4;
   uint8_t i=scheduleNumberStart,j=0;
   uint8_t end = i+4;
   for(;i<end;i++,j++)
   {
      getTariffSchaduleTimeslotPassive(i,&entry->timeslots[j]);    
   }   
   entry->day_id = day_id;   
}

/**
* @see loadDailyProfilePassive
*/
void writeDailyProfilePassive_dlms(DailyProfileEntry *DayProfile)
{
   uint8_t scheduleNumberStart = (DayProfile->day_id) * 4;
   uint8_t i=scheduleNumberStart,j=0;
   uint8_t end = i+4;
   for(;i<end;i++,j++)
   {  
     write_to_eeprom(&DayProfile->timeslots[i],&i,setTariffSchaduleTimeslotPassive);
   }   
   DayProfile->collected_params_count = 0;//reset the collected entry count
}
/**
* loads todays profile to EEPROM
* day from 0 to 6 for sunday to monday
*/
void LoadTodaysTariff(int day)
{   
    uint8_t z=0;
    uint8_t holiday=0;
    for(;z<MaxHolidays;z++)
    {
      if(rtcc.day == holidays[z].day && rtcc.month == holidays[z].month && rtcc.year == holidays[z].year)
      {
        holiday=1;
        break;
      }
    }
    
    z=0;

    uint8_t day_id = 0;
    if(holiday == 1)          //holiday 
    {
       day_id = 3;
    }
    else 
    {
        z= weekday();
        if(z == 0 || z == 6) // sunday or saturday weekend 
        {
          day_id = 2;
        }
        else                 // normal day
        {
          day_id = 1;
        } 
    }
         
    loadDailyProfile(day_id,&todaysRates);
    //load tariff rates 
    uint8_t w;
    uint8_t w_end;
    uint8_t j=0;
    uint8_t rate_start = (day_id-1)*40;
    w=rate_start;
    w_end = rate_start+40;
    uint8_t add;
    for(;w<w_end;w++,j++)
    {
      add = rate_start + w;
       getRate(&rates[j],&add); 
    }   
    
}
/**
* get the tariff slot that should be used for the current hour and minute based on 
* the configured time slots
*/
int GetCurrentMinuteTariffTimeSlot()
{   
    uint8_t hour = rtcc.hour;
    uint8_t minute = rtcc.minute;
    
    if( (todaysRates.timeslots[0].start_Hour < hour ||( todaysRates.timeslots[0].start_Hour == hour && todaysRates.timeslots[0].start_minute <= minute))        
       && (todaysRates.timeslots[1].start_Hour > hour ||( todaysRates.timeslots[1].start_Hour == hour&& todaysRates.timeslots[1].start_minute >= minute)))
    {
        return 0; //the first time slot in the schedule 
    }
    else if( (todaysRates.timeslots[1].start_Hour < hour ||( todaysRates.timeslots[1].start_Hour == hour && todaysRates.timeslots[1].start_minute <= minute))        
       && (todaysRates.timeslots[2].start_Hour > hour ||( todaysRates.timeslots[2].start_Hour == hour && todaysRates.timeslots[2].start_minute >= minute)))
    {
        return 1; //the second time slot in the schedule 
    }
    else if( (todaysRates.timeslots[2].start_Hour < hour ||( todaysRates.timeslots[2].start_Hour == hour && todaysRates.timeslots[2].start_minute <= minute))        
       && (todaysRates.timeslots[3].start_Hour > hour ||( todaysRates.timeslots[3].start_Hour == hour && todaysRates.timeslots[3].start_minute >= minute)))
    {
        return 2; //the third time slot in the schedule 
    }
    else
        return 3; //this is the last time slot in the schedule    
}
/**
* method called at the end of every day
*               loads todays tariff rate
*               incriments the number of days in this month
*/
void per_day_activity()
{
      LoadTodaysTariff(rtcc.day);
      Current_billing_Duration_Counter++;
      

}

/**
* Activity to be called at the end of every month
* creates billing log 
* ends the billing period and starts new one 
* reset all billing variables 
*/
void per_month_activity()
{ 
   ConsumptionSinceLastBilling = 0;   
   if(compare_time(&rtcc,&Estimated_billing_period_end)>0 && Current_billing_Duration_Counter >= Current_billing_Duration_in_days )
   {

     Current_billing_Period_start.year = rtcc.year;
     Current_billing_Period_start.month = rtcc.month;
     Current_billing_Period_start.day = rtcc.day;
     Current_billing_Period_start.hour = rtcc.hour;
     Current_billing_Period_start.minute = rtcc.minute;
     Current_billing_Period_start.second = rtcc.second;
     
     Estimated_billing_period_end.second = Current_billing_Period_start.second;
     Estimated_billing_period_end.minute = Current_billing_Period_start.minute;
     Estimated_billing_period_end.hour =   Current_billing_Period_start.hour;
     
     Estimated_billing_period_end.day = Current_billing_Period_start.day + Current_billing_Duration_in_days;
     //TOOD. rearrange this correctly 
     Current_billing_Duration_Counter = 1;
   }
   
   EnergyBillingCutoffDayLog l;
   l.ActiveEnergy_QI = phase->readings.active_energy_Q1;
   l.ActiveEnergy_QII = phase->readings.active_energy_Q2;
   l.ActiveEnergy_QIII = phase->readings.active_energy_Q3;
   l.ActiveEnergy_QIV = phase->readings.active_energy_Q4;
   
   l.balance = Current_balance.balance;
   l.ReActiveEnergy_QI = phase->readings.reactive_energy_Q1;
   l.ReActiveEnergy_QII = phase->readings.reactive_energy_Q2;
   l.ReActiveEnergy_QIII = phase->readings.reactive_energy_Q3;
   l.ReActiveEnergy_QIV = phase->readings.reactive_energy_Q4;
   
   l.BillingPeriodStart = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
   l.BillingPeriodEnd =   getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);//TODO correct this
   
   logBillingCutoffDateLog(&l);
   LoadTodaysTariff(rtcc.day); //may be this is not required      
}
/*
* checks if the current balance is bellow the minimum tariff rate 
* @param power_breaks :- ethiopian power levls where billing rate is changed 
* @param consumtionSinceLastBilling: this months consumption      
*    returns 1: if less than the minimum tariff 
*            0: otherwise 
*/

int CheckTariff(const uint32_t *power_brakes,unsigned long *ConsumptionSinceLastBilling)
{
  if(billingSchema == PRE_PAID)
  { // Low tariff exists only in PRE_PAID meters
    
    uint8_t tariffslot = GetCurrentMinuteTariffTimeSlot();  //on which 4 slots of the day the tariff is found 
    uint8_t rateIndex = getRateStartIndex(power_brakes,*ConsumptionSinceLastBilling);
    int32_t charge = rates[rateIndex+(tariffslot*10)];
     
    if(Current_balance.balance >= charge)
    {
      status.LowTariff = 0;
      return 0;
    }
    else
    {
      status.LowTariff = 1;
      lowtariff_temp = Current_balance.balance;
      return 1;
    }
  }
  else
  {
    status.LowTariff  = 0 ;
    return 1;
  }
}
