/*
 * prime_plc_setup.c
 *
 *  Created on: May 17, 2017
 *      Author: Eliab Erango
 */
#include "headers.h"
#include "prime_plc_setup.h"

uint8_t plc_firmware_Version[] =
{
  12,'1','.','3','.','6','0','b','.','8','2','2','4'//'0','0','0','7','0','0'//,'0','1','0','0','0','9','0','0','0','1'16
};

uint16_t plc_vendor_id = 1234;

uint8_t plc_product_id = 234;

uint8_t mac_address[]={6,0x30,0x30,0x30,0x30,0x30,0x30}; //6 octet string -EUI48
/*
 * Callback function for PLC firmware version
 */
void get_set_plc_firmware_version(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      datap++;
      //setMacAddress(datap,(uint8_t *)0);
      memcpy(&plc_firmware_Version[1],datap,plc_firmware_Version[0]);
    }
    else if(data_direction == ATTR_READ)
    {
       memcpy(datap,plc_firmware_Version,plc_firmware_Version[0]+1);
    }
}

/*
 * Callback function for PLC MAC address
 */

void get_set_mac_address(void *data,int data_direction)
{
	uint8_t *datap =  data;
	if(data_direction == ATTR_WRITE)
	{
	  datap++;
	  setMacAddress(datap,(uint8_t *)0);
	  //datap++;
	  memcpy(&mac_address[1],datap,6);
	}
	else if(data_direction == ATTR_READ)
	{
	   memcpy(datap,mac_address,7);
//	   uint8_t  *datap2= (mac_address+1);
//	   memcpy(datap2,EUI,6);
//	   memcpy(datap,mac_address,7);
	}
	/*uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      datap++;
      setMacAddress(datap,(uint8_t *)0);
      //datap++;
      //memcpy(EUI,datap,6);
      memcpy(mac_address+1,datap,6);
    }
    else if(data_direction == ATTR_READ)
    {
      // uint8_t  *datap2= (mac_address + 1);
     //  memcpy(datap2,EUI,6);
       memcpy(datap,mac_address,7);///TODO
    }*/
}
