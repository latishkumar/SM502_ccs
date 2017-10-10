/*
 * identification_numbers.c
 *
 *  Created on: May 12, 2017
 *      Author: Eliab Erango
 */
#include "headers.h"
#include "identification_numbers.h"

/*Device ID 1
 * E-meter Serial Number
 */
uint8_t Meter_Sr_No[] =
{
    10,'1','1','0','0','0','0','0','0','3','4'
};
/*Device ID 2
 * manuf_code[2]+model_code[2]+manuf_year[2]
 */
uint8_t Device_ID_2[] =
{
   6,'d','v','0','1','1','3'
};

/*Device ID 3
 * equipment_type[10]+Protoc ol_ID[8]
 */
uint8_t Device_ID_3[] =
{
   18,'c','o','n','t','a','d','o','r',' ',' ','D','L','M','S','0','1','0','6'
};

/*Device ID 4
 * Tariff Contract Profile Number [10]+ calendar schedules identifier[10]
 */
uint8_t Device_ID_4[] =
{
   20,'0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9'
};

/*Device ID 5
 * FLAG_ID (meter manufacturer)[3] + E-meter serial number[10]
 */
uint8_t Device_ID_5[] =
{
   13,'d','V','s','A','B','C','D','1','2','3','4','5','6'
};

/*Device ID 6
 * Multicast communication identifier
*/
uint8_t Device_ID_6[] =
{
   24,'0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','1','2','3','4'
};

/* Device ID 7
 * Firmware Version
*/
uint8_t Device_ID_7[] =
{
   5,'V','0','1','0','2'    //v1.2
};

/* Device ID 8
 * Scroll Display Mode
 * Possible values = A,B,C,D,...Z
 */
uint8_t Device_ID_8[] =
{
   1,'A'
};
uint8_t Manufacturer_Name[] = //TODO check this
{
    20, 'd','V','e','n','t','u','s',' ','T','e','c','h','n','o','l','o','g','i','e','s'
};
//Callback function for Device ID 1 object
void change_meter_serial_number(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;

//      for(;i<=Meter_Sr_No[0];i++)
//        Meter_Sr_No[i] = *datap++;
      if(length == Meter_Sr_No[0])
      {
        memcpy(&(Meter_Sr_No[1]),datap,length);
        //setDeviceIDs(Meter_Sr_No,0);
        uint8_t temp8 = 0;
        write_to_eeprom(&Meter_Sr_No,&temp8,setDeviceIDs);
      }
      else
      {
        //some kind of error
      }
    }
    else if(data_direction == ATTR_READ)
    {
      memcpy(data,Meter_Sr_No,Meter_Sr_No[0]+1);
    }
}

//Callback function for Device ID 2 object
void change_device_id2(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;

//      for(;i<=Device_ID_2[0];i++)
//        Device_ID_2[i] = *datap++;
      if(length < Device_ID_2[0])
      {
        memcpy(Device_ID_2,data,length+1);
//        setDeviceIDs(Device_ID_2,2);
        uint8_t temp8 = 2;
        write_to_eeprom(&Device_ID_2,&temp8,setDeviceIDs);
      }
      else
      {
      }

    }
    else if(data_direction == ATTR_READ)
    {
      memcpy(datap,Device_ID_2,Device_ID_2[0]+1);

    }
}

//Callback function for Device ID 3 object
void change_device_id3(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
    if(data_direction == ATTR_WRITE)
    {
       uint8_t length = *datap++;

//      for(;i<=Device_ID_3[0];i++)
//        Device_ID_3[i] = *datap++;
      if(length < Device_ID_3[0])
      {
        memcpy(Device_ID_3,data,length+1);
//        setDeviceIDs(Device_ID_3,3);
        uint8_t temp8 = 3;
        write_to_eeprom(&Device_ID_3,&temp8,setDeviceIDs);
      }
      else
      {
      }

    }
    else if(data_direction == ATTR_READ)
    {
      //memcpy(datap,Device_ID_3,Device_ID_3[0]+1);
    }
}

//Callback function for Device ID 4 object
void change_device_id4(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=0;
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;

//      for(;i<=Device_ID_4[0];i++)
//        Device_ID_4[i] = *datap++;

      if(length < Device_ID_4[0])
      {
        memcpy(Device_ID_4,data,length+1);
//        setDeviceIDs(Device_ID_4,4);
        uint8_t temp8 = 4;
        write_to_eeprom(&Device_ID_4,&temp8,setDeviceIDs);
      }
      else
      {
      }

    }
    else if(data_direction == ATTR_READ)
    {
      memcpy(datap,Device_ID_4,Device_ID_4[0]+1);
    }
}

//Callback function for Device ID 5 object
void change_device_id5(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;

//      for(;i<=Device_ID_5[0];i++)
//        Device_ID_5[i] = *datap++;

      if(length < Device_ID_5[0])
      {
        memcpy(Device_ID_5,data,length+1);
//        setDeviceIDs(Device_ID_5,5);
        uint8_t temp8 = 5;
        write_to_eeprom(&Device_ID_5,&temp8,setDeviceIDs);
      }
      else
      {
      }

    }
    else if(data_direction == ATTR_READ)
    {
      memcpy(datap,Device_ID_5,Device_ID_5[0]+1);
    }
}

//Callback function for Device ID 6 object
void change_device_id6(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;

    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;
//      for(;i<=Device_ID_6[0];i++)
//        Device_ID_6[i] = *datap++;

      if(length < Device_ID_6[0])
      {
        memcpy(Device_ID_6,data,length+1);
//        setDeviceIDs(Device_ID_6,6);
        uint8_t temp8 = 6;
        write_to_eeprom(&Device_ID_6,&temp8,setDeviceIDs);
      }
      else
      {
      }
    }
    else if(data_direction == ATTR_READ)
    {
      memcpy(datap,Device_ID_6,Device_ID_6[0]+1);
    }
}

//Callback function for Device ID 7 object
void change_device_id7(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;

    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;
//      for(;i<=Device_ID_7[0];i++)
//        Device_ID_7[i] = *datap++;

      if(length < Device_ID_7[0])
      {
        memcpy(Device_ID_7,data,length+1);
//        setDeviceIDs(Device_ID_7,7);
        uint8_t temp8 = 7;
        write_to_eeprom(&Device_ID_7,&temp8,setDeviceIDs);
      }
      else
      {
      }
    }
    else if(data_direction == ATTR_READ)
    {
      memcpy(datap,Device_ID_7,Device_ID_7[0]+1);
    }
}
//Callback function for Device ID 8 object
void change_device_id8(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;

    if(data_direction == ATTR_WRITE)
    {
     uint8_t length = *datap++;
//      for(;i<=Device_ID_8[0];i++)
//        Device_ID_8[i] = *datap++;
      if(length < Device_ID_8[0])
      {
        memcpy(Device_ID_8,data,length+1);
//        setDeviceIDs(Device_ID_8,8);
        uint8_t temp8 = 8;
        write_to_eeprom(&Device_ID_8,&temp8,setDeviceIDs);
      }
      else
      {
      }
    }
    else if(data_direction == ATTR_READ)
    {
      memcpy(datap,Device_ID_8,Device_ID_8[0]+1);
    }
}
