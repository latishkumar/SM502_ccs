/*
 * power_quality.c
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */
/*
 * Reference Voltage for power
 * quality measurement (sags and
 * swells) and for long power fail
 */
#include "headers.h"
#include "power_quality.h"
#include "long_power_failures.h"
uint16_t nominal_voltage = 230;//V

uint16_t max_vrms = 280;  // maximum vrms voltage the meter support in V

uint8_t min_vrms = 170;   // minimum vrms voltage the meter starts functioning

uint8_t  max_irms = 80;   // maximum irms current the meter support in A

uint16_t under_voltage_trip_duration =  10; // Time threshold for the detection of voltage sags

uint16_t over_voltage_trip_duration =  1;

uint16_t voltage_sag_threshold = 3000; //Default: 7,00% of decrement over Vref.

uint16_t voltage_swell_threshold = 1500; //Default: 7,00% of increment over Vref.

/*
 * Callback function for reference voltage
 */
void change_reference_voltage(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      nominal_voltage = nv_2;

      //TODO. check this is with in the hardware maximmu limit
      //if(Nominal_Voltage <= AbsoluteMaxVRMS){

      max_vrms = nominal_voltage +( ((voltage_swell_threshold/100) * nominal_voltage)/100);
      min_vrms = nominal_voltage - (((voltage_sag_threshold/100) * nominal_voltage)/100);
      ThresholdLongPowerFaile = nominal_voltage - (((voltage_threshold_for_long_power_failure/100) * nominal_voltage)/100);
      setNominalValues(&nominal_voltage,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = nominal_voltage & 0xFF;
       datap++;
      *datap = (nominal_voltage>>8) & 0xFF;
    }
}

/*
 * Call back function for time threshold for
 * voltage sag
 */
void change_time_threshold_voltage_sag(void *data,int data_direction)
{
      uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {

      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      under_voltage_trip_duration = nv_2;

//      UnderVoltageTripDuration = *datap++;
//      UnderVoltageTripDuration |= ((*datap++)<<8);

      setTripDuration(&under_voltage_trip_duration,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = under_voltage_trip_duration & 0xFF;
       datap++;
      *datap = (under_voltage_trip_duration>>8) & 0xFF;
    }
}

/*
 * call back function for voltage sag threshold
 */
void change_threshold_voltage_sag(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      voltage_sag_threshold= nv_2;
//      ThrusholdVoltageSagPer = *datap++;
//      ThrusholdVoltageSagPer |= ((*datap++)<<8);


      min_vrms = nominal_voltage - ((voltage_sag_threshold * nominal_voltage)/100);
      setMinValues(&voltage_sag_threshold,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = voltage_sag_threshold & 0xFF;
       datap++;
      *datap = (voltage_sag_threshold>>8) & 0xFF;
    }
}

/*
 * Call back function for time threshold for voltage swell
 */
void change_time_threshold_voltage_swell(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
            uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      over_voltage_trip_duration = nv_2;

      //write_TimeThreshold_OverVoltage(Nominal_Voltage);
      setTripDuration(&over_voltage_trip_duration,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = over_voltage_trip_duration & 0xFF;
       datap++;
      *datap = (over_voltage_trip_duration>>8) & 0xFF;
    }
}

/*
 * Callback function for voltage swell threshold
 */
void change_threshold_voltage_swell(void *data,int data_direction)
{

        uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {

      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      voltage_swell_threshold = nv_2;
//      ThrusholdVoltageSwellPer = *datap++;
//      ThrusholdVoltageSwellPer |= ((*datap++)<<8);


      max_vrms = nominal_voltage +( (voltage_swell_threshold * nominal_voltage)/100);

      setMaxValues(&voltage_swell_threshold,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = voltage_swell_threshold & 0xFF;
       datap++;
      *datap = (voltage_swell_threshold>>8) & 0xFF;
    }
}
