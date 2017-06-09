/*
 * manufacturer_specific_objects.c
 *
 *  Created on: May 26, 2017
 *      Author: abi
 */

#include "headers.h"
#include "manufacturer_specific_objects.h"
#include "self_diagnosis.h"
int16_t alarm_status;
/*
 * Method function for reset alarm object
 */
void reset_alarms(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  uint8_t type = *data++;//type is 18
  uint16_t x=0;
  x= *data << 8;
  x |= *(++data);//x is the tamper type from 1 --- 4
  status.write_tamper_status = 1;
  *response_len = 0;
  switch(x)
  {
    case 1:
        status.UpperCoverRemovedTamperStatus = 0;
        break;
    case 2:
        status.LowerCoverRemovedTamperStatus = 0;
        break;
    case 3:
        status.MangneticTamperStatus = 0;
        break;
    case 4:
        status.NeutralTamperStatus = 0;
        break;
    default:
        break;
  }
  status.write_tamper_status = 0;
}

/*
 * Call back function for reset alarm object
 */
void reset_persistent_events(void *data, int data_direction)
{
	if(data_direction == ATTR_WRITE)
	{
		status.write_tamper_status = 1;
		int16_t nv_2 = 0;
		uint8_t *ptr2 = data;
		nv_2 = *(ptr2+1);
	    nv_2 |= ((*(ptr2))<<8);
		switch(nv_2)
		{
			case 1:
				status.UpperCoverRemovedTamperStatus = 0;
				break;
			case 2:
				status.LowerCoverRemovedTamperStatus = 0;
				break;
			case 3:
				status.MangneticTamperStatus = 0;
				break;
			case 4:
				status.NeutralTamperStatus = 0;
				break;
			default:
				break;
		  }
		  status.write_tamper_status = 0;
	}
	else if(data_direction == ATTR_READ)
	{
	    uint8_t *datap =  data;
	    *datap = alarm_status & 0xFF;
		datap++;
		*datap = (alarm_status>>8) & 0xFF;
	}
}

extern void updateCalibrationFactor(int16_t ErrorPercent,uint8_t type);
/*
 * Call back function for power scaler calibration
 */
void calibrate_power_scaler(void *data, int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      int16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);
      updateCalibrationFactor(nv_2,0);
    }
    else if(data_direction == ATTR_READ)
    {
        int16_t tmp_power_scale_factor = nv_parms.seg_a.s.chan1.current.P_scale_factor[0];
        *datap = tmp_power_scale_factor & 0xFF;
         datap++;
        *datap = (tmp_power_scale_factor>>8) & 0xFF;
    }
}

/*
 * Callback function for current scaler calibration
 */
void calibrate_current_scaler(void *data, int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      int16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);
      updateCalibrationFactor(nv_2,1);
    }
    else if(data_direction == ATTR_READ)
    {
        int16_t tmp_power_scale_factor = nv_parms.seg_a.s.chan1.current.I_rms_scale_factor;
        *datap = tmp_power_scale_factor & 0xFF;
         datap++;
        *datap = (tmp_power_scale_factor>>8) & 0xFF;
    }
}
/*
 * Callback function for voltage scaler caliberation
 */
void calibrate_voltage_scaler(void *data, int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      int16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);
      updateCalibrationFactor(nv_2,2);
    }
    else if(data_direction == ATTR_READ)
    {
        int16_t tmp_power_scale_factor = nv_parms.seg_a.s.chan1.V_rms_scale_factor;
        *datap = tmp_power_scale_factor & 0xFF;
         datap++;
        *datap = (tmp_power_scale_factor>>8) & 0xFF;
    }
}
/*
 * Reset methods for power scaler
 */
void calibrate_power(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  uint8_t type = *data++;//type is 18
  int16_t x=0;
  x= *data << 8;
  x |= *(++data);//x is the tamper type from 0 --- 3

  updateCalibrationFactor(x,0);

}

/*
 * Reset methods for current scaler
 */
void calibrate_current(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  uint8_t type = *data++;//type is 18
  int16_t x=0;
  x= *data << 8;
  x |= *(++data);//x is the tamper type from 0 --- 3

  updateCalibrationFactor(x,1);


}
/*
 * Reset methods for voltage sclaer
 */
void calibrate_voltage(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  uint8_t type = *data++;//type is 18
  int16_t x=0;
  x= *data << 8;
  x |= *(++data);//x is the tamper type from 0 --- 3
//  x -=  1;
//  x= ~x;
  updateCalibrationFactor(x,2);

}

extern uint32_t power_brakes[];
extern uint32_t ConsumptionSinceLastBilling;
extern int CheckTariff(const uint32_t *power_brakes,unsigned long *ConsumptionSinceLastBilling);
/*
 * Method for incrementing balance
 */
void increment_balance(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    uint8_t type = *data++;//type is 6
    uint32_t x=0;
    uint32_t temp=0;
    temp = *data;
    x  = temp << 24;
    temp = (*(++data));
    x |= temp<<16;//x is the tamper type from 0 --- 3
    temp = (*(++data));
    x |= temp<<8;
    x |= *(++data);
    type++;
    Current_balance.balance += x;
    write_to_eeprom(&Current_balance,(uint8_t *)0,setCurrentBalance);
    CheckTariff(power_brakes,&ConsumptionSinceLastBilling);
    *response_len = 0;
}
/*
 * Method for decrementing balance
 */
void decrement_balance(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    uint8_t type = *data++;//type is 6
    uint32_t x=0;
    uint32_t temp=0;
    temp = *data;
    x  = temp << 24;
    temp = (*(++data));
    x |= temp<<16;//x is the tamper type from 0 --- 3
    temp = (*(++data));
    x |= temp<<8;
    x |= *(++data);
    type++;
    Current_balance.balance -= x;
    write_to_eeprom(&Current_balance,(uint8_t *)0,setCurrentBalance);
    CheckTariff(power_brakes,&ConsumptionSinceLastBilling);
    *response_len = 0;
}
