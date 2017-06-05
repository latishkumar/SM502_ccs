/*
 * long_power_failures.c
 *
 *  Created on: May 15, 2017
 *      Author: abi
 */

#include "headers.h"
#include "long_power_failures.h"
#include "power_quality.h"
/*
 * Time threshold for long power failure
 * Default: 3 minutes
 */
uint16_t time_threshold_for_long_power_failure = 600; //time in second

/*
 * Threshold for long power failure
 * Default: 50,00 % of Vref (defined in Reference Voltage for power
 * quality measurement).
 */
uint16_t voltage_threshold_for_long_power_failure = 5600;

/*
 * Total time for all long power
 * failure from the origin
 */
uint16_t duration_of_all_long_power_failure = 1300;

uint16_t number_of_long_power_failures = 0; // Number of long power failures in all phases //use method to read from EERPOM

/*
 * callback function for Long Power Failure object
 */
void change_time_threshold_for_long_power_failure(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      time_threshold_for_long_power_failure = nv_2;

//      setTripDuration(&TimeThresholdLongPowerFaile,7);
      uint8_t temp8 = 7;
      write_to_eeprom(&time_threshold_for_long_power_failure,&temp8,setTripDuration);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = time_threshold_for_long_power_failure & 0xFF;
       datap++;
      *datap = (time_threshold_for_long_power_failure>>8) & 0xFF;
    }
}

/*
 * callback function for setting threshold value
 * for Long Power Failure
 */
void change_threshold_for_long_power_failure(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {

      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      voltage_threshold_for_long_power_failure = nv_2;
//      VoltageThresholdLongPowerFailePer = *datap++;
//      VoltageThresholdLongPowerFailePer |= ((*datap++)<<8);


      time_threshold_for_long_power_failure = nominal_voltage - ((voltage_threshold_for_long_power_failure * nominal_voltage)/100);

//      setNominalValues(&VoltageThresholdLongPowerFailePer,4 );
      uint8_t temp8 = 4;
      write_to_eeprom(&voltage_threshold_for_long_power_failure,&temp8,setNominalValues);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = voltage_threshold_for_long_power_failure & 0xFF;
       datap++;
      *datap = (voltage_threshold_for_long_power_failure>>8) & 0xFF;
    }
}

/*
 * Call back funtion for number of long power failures
 */
void change_number_of_long_power_failures(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {

      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);
      number_of_long_power_failures = nv_2;

//      NumberOfLongPowerFails = *datap++;
//      NumberOfLongPowerFails |= ((uint32_t)(*datap++)<<8);

      //logNumberofLongPowerFailes(ThrusholdVoltageSag);TODO . impliment shis
    }
    else if(data_direction == ATTR_READ)
    {

      *datap = number_of_long_power_failures & 0xFF;
       datap++;
      *datap = (number_of_long_power_failures>>8) & 0xFF;

    }
}
