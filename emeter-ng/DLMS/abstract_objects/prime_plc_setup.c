/*
 * prime_plc_setup.c
 *
 *  Created on: May 17, 2017
 *      Author: abi
 */
#include "headers.h"
#include "prime_plc_setup.h"

uint8_t plc_firmware_Version[] =
{
  6,'0','0','0','7','0','0'//,'0','1','0','0','0','9','0','0','0','1'16
};

uint16_t plc_vendor_id = 1234;

uint8_t plc_product_id = 234;

uint8_t mac_address[]={0,0xB,0,0,0,0,0x22}; //6 octect string -EUI48
/*
 * Callback function for PLC frimware version
 */
void get_set_firmware_version(void *data,int data_direction)
{

    if(data_direction == ATTR_WRITE)
    {
       int x=0;
       x++;
    }
    else if(data_direction == ATTR_READ)
    {
       int y=0;
       y++;
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
      memcpy(EUI,datap,6);
    }
    else if(data_direction == ATTR_READ)
    {
      // uint8_t  *datap2= (mac_address + 1);
     //  memcpy(datap2,EUI,6);
       memcpy(datap,mac_address,7);///TODO
    }
}
