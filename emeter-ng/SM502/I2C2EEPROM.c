/*
 * I2C2EEPROM.c
 *
 *  Created on: May 22, 2013
 *      Author: Donayam
 */

#include "I2C2EEPROM.h"
#include "I2C2.h"
#include <stdint.h>

/*
* The current read and write adress. when ever a write or read operation is sucessfull this addresses are incrimented by
* the amount of data written. this are used to avoid the unused memories in the b/n pages 
*/
unsigned long CurrentReadAddress; 
unsigned long CurrentWriteAddress;
unsigned long CurrentPage;
unsigned int CurrentSlaveAddress;
unsigned long CurrentReadPage;
/**
* Intialize the eeprom, by enabling the EEPROM PORTS For 
* input and output.
**/
void EEPROM2Init() {
	I2C2Init();
}
/**
* Disable the eeprom ports 
*/
void DisableEEPROM2PORT()
{
  I2C2_SDA_IN;
  I2C2_SCL_IN;
}
/**
* Writes 8 bit number to the eeprom after a write operation has already started by EEPROM2_WriteInt8
* @param c: character to be wrtten to the eeprom 
* @param end: indicator wheather to end the write cycle or not 
*             at the end of the write cycle the eeprom will transfer the data from 
*             temporary memory to the eeproms permanent memory
*             1. end the write cycle 
*             0. don't end the write sycle 
* @see  EEPROM2_WriteInt8
*/
uint8_t EEPROM2_WriteNextInt8(uint8_t c, uint8_t end) {
#ifdef EEPROM_REV_2
       unsigned long temppage = CurrentWriteAddress>>8;
#else
        unsigned long temppage = CurrentWriteAddress>>7;
#endif 
       if(temppage != CurrentPage)
       {
         //end the write sequency 
         if (I2C2getACK() == 1)
	      {
	       I2C2GenerateStopCondition();
          }
         //poll ack 
         PollACK2(CurrentSlaveAddress);
         //start a new write sequency, write the data
         return EEPROM2_WriteInt8(c,CurrentWriteAddress, end);
         
       }
	if (I2C2getACK() == 1) // check if the last data was written successfully
			{
		I2C2WriteInt8(c); // send the data
		if (end == 1)  //generate a stop condition
		{
			if (I2C2getACK() == 1)
			{
				I2C2GenerateStopCondition();
                                PollACK2(CurrentSlaveAddress);//this was missing 
				BussIdeal2();
			}
			else
			   return 0;
		}
	} else {
		return 0;
	}
       
        CurrentWriteAddress++;
	return 1;  //write was successful
}
uint8_t tempf=0;
/**
* Writes 8 bit number to the eeprom.
* @param c: caracter to write to the eeprom 
* @param Address: the address to write the data 
* @param end: indicator wheather to end the write cycle or not 
*             at the end of the write cycle the eeprom will transfer the data from 
*             temporary memory to the eeproms permanent memory
*             1. end the write cycle 
*             0. don't end the write sycle
*
*/
uint8_t EEPROM2_WriteInt8(uint8_t c,uint32_t Address, uint8_t end) {
	uint16_t Add = 0;
        uint8_t AddHigh = 0;
	Add =  (Address & 0xffff);
	//int slaveAddress;
        AddHigh = Address>>16;
        


        
        CurrentWriteAddress = Address;
        
        CurrentPage = CurrentWriteAddress>>7;
        #ifdef EEPROM_REV_2 
          CurrentPage = CurrentWriteAddress>>8;
          CurrentSlaveAddress = slave_address((AddHigh&0x04)>>2,AddHigh & 0x3);
        #else
          CurrentPage = CurrentWriteAddress>>7;
          CurrentSlaveAddress = slave_address((AddHigh&0x02)>>1,AddHigh & 0x01);
        #endif 
	//check for bit 17 to determine witch EEPROM to USE
//	if ((Address >> 16 ) != 0) {
//		CurrentSlaveAddress = Slave_Address2;
//	} else {
//		CurrentSlaveAddress = Slave_Address1;
//	}

	I2C2RequestTx(1, CurrentSlaveAddress);
	if (I2C2getACK() == 1) // check if slave is found
			{
		I2C2WriteInt8(Add >> 8); // send high byte of the address
		if (I2C2getACK() == 1)
		{
			I2C2WriteInt8((uint8_t)(Address & 0xff));  //send low byte of the address
		}
		else
			return 0;
		if (I2C2getACK() == 1)
			I2C2WriteInt8(c); // send the data
		else
			return 0;

		if (end == 1)  //generate a stop condition
				{
			if (I2C2getACK() == 1)
			{
				I2C2GenerateStopCondition();
                                PollACK2(CurrentSlaveAddress);//this was missing 
				BussIdeal2();
			}
			else
				return 0;
		}
	} else {
		return 0;
	}
        
        
        CurrentWriteAddress++;
	return 1;  //write was successful
}

/**
* Reads 8 bit number from the eeprom 
* @param Address: the address to the data to 
* @param end indicator wheather to end the write cycle or not 
*             at the end of the write cycle the eeprom will transfer the data from 
*             temporary memory to the eeproms permanent memory
*             1. end the write cycle 
*             0. don't end the write sycle
* @param data: the data read from the eeprom 
*/
uint8_t EEPROM2_ReadInt8(uint32_t Address, uint8_t end, uint8_t *data) {
	uint16_t Add =  Address & 0xffff;
	uint8_t slaveAddress = 0;
        uint8_t AddHigh = Address>>16;
        
        CurrentReadAddress = Address;
        
        
    #ifdef EEPROM_REV_2 
         CurrentReadPage = CurrentReadAddress>>8;
         slaveAddress = slave_address((AddHigh&0x04)>>2,AddHigh & 0x3);
    #else
         CurrentReadPage = CurrentReadAddress>>7;
         slaveAddress = slave_address((AddHigh&0x02)>>1,AddHigh & 0x01);
    #endif
//        slaveAddress = slave_address((AddHigh&0x02)>>1,AddHigh & 0x01);
        

	//check for bit 17  to determine witch EEPROM to USE
//	if ((Address >>16 ) != 0) {
//		slaveAddress = Slave_Address2;
//	} else {
//		slaveAddress = Slave_Address1;
//	}

	I2C2RequestTx(1, slaveAddress);
	if (I2C2getACK() == 1) {

		I2C2WriteInt8(Add >> 8);
		if (I2C2getACK() == 1)
			I2C2WriteInt8((uint16_t)(Address & 0xff));
		else
			return 0;

		if (I2C2getACK() == 1)
			I2C2RequestTx(0, slaveAddress);
		else
			return 0;

		if (I2C2getACK() == 1)
			*data = I2C2ReadInt8();
		else
			return 0;

		if (end == 1) {
			I2C2NACK();
			I2C2GenerateStopCondition();
			BussIdeal2();
		}

		//*data=ans;
	} else
		return 0;
	CurrentReadAddress++;
	return 1;
}

/**
* Reads 8 bit number from the eeprom after a read operation has been started by EEPROM2_ReadInt8
* @param end indicator wheather to end the write cycle or not 
*             at the end of the write cycle the eeprom will transfer the data from 
*             temporary memory to the eeproms permanent memory
*             1. end the write cycle 
*             0. don't end the write sycle
* @param data: the data read from the eeprom 
*/
uint8_t EEPROM2_ReadNextInt8(uint8_t end,  uint8_t *data) {
#ifdef EEPROM_REV_2  
    unsigned long temppage = CurrentReadAddress>>8;
#else 
    unsigned long temppage = CurrentReadAddress>>7;
#endif     
    if(temppage != CurrentReadPage)
    {
#ifdef EEPROM_REV_2
      if(temppage%1024 == 0) //change for EEPROM 2
#else
      if(temppage%512 == 0) //change for EEPROM 2
#endif              
       {
         //end the write sequence
    	 I2C2NACK();
    	 I2C2GenerateStopCondition();
         BussIdeal2();
         //start a new Read sequence, write the data         
         return EEPROM2_ReadInt8(CurrentReadAddress,end,data);
       }

    }
    CurrentReadPage = temppage;
	    I2C2ACK();
			*data = I2C2ReadInt8();
		if (end == 1) {
			I2C2NACK();
			I2C2GenerateStopCondition();
			BussIdeal2();
		}
		CurrentReadAddress++;
	return 1;
}

/**
* Reads 32 bit number from the eeprom after a read operation has been started by EEPROM2_ReadInt8
* @param end indicator wheather to end the write cycle or not 
*             at the end of the write cycle the eeprom will transfer the data from 
*             temporary memory to the eeproms permanent memory
*             1. end the write cycle 
*             0. don't end the write sycle
* @param data: the data read from the eeprom 
*/
uint8_t EEPROM2_ReadLong(uint32_t Address,uint8_t end,unsigned long *data)
{
	uint8_t x = 0;
	unsigned long temp=0;
	uint8_t z=0;
	z = EEPROM2_ReadInt8(Address,0,&x);
	if(z==0)
		return 0;
	*data = x;

	z = EEPROM2_ReadNextInt8(0,&x);
	temp=x;
	if(z==0)
		return 0;
	*data |= ((temp )<<8);

	z = EEPROM2_ReadNextInt8(0,&x);
	temp=x;
	if(z==0)
		return 0;
	*data |= (temp ) << 16;

	z = EEPROM2_ReadNextInt8(end,&x);
	temp=x;
	if(z==0)
		return 0;
	*data |= ((temp ))<<24;

	return 1;
}

/**
* Reads 8 bit number from the eeprom 
* @param Address: the address to the data to 
* @param end indicator wheather to end the write cycle or not 
*             at the end of the write cycle the eeprom will transfer the data from 
*             temporary memory to the eeproms permanent memory
*             1. end the write cycle 
*             0. don't end the write sycle
* @param data: the data read from the eeprom 
*/
uint8_t EEPROM2_ReadNextLong(uint8_t end,unsigned long *data)
{
	uint8_t x = 0;
	uint8_t z=0;
	z = EEPROM2_ReadNextInt8(0,&x);
	if(z==0)
		return 0;
	*data = x & 0xff;

	z = EEPROM2_ReadNextInt8(0,&x);
	if(z==0)
		return 0;
	*data |= (unsigned long)(x & 0xff)<<8;

	z = EEPROM2_ReadNextInt8(0,&x);
	if(z==0)
		return 0;
	*data |= ((unsigned long)(x & 0xff))<<16;

	z = EEPROM2_ReadNextInt8(end,&x);
	if(z==0)
		return 0;
	*data |= ((unsigned long)(x & 0xff))<<24;

	return 1;
}
/**
* Writes 8 bit number to the eeprom.
* @param c: caracter to write to the eeprom 
* @param Address: the address to write the data 
* @param end: indicator wheather to end the write cycle or not 
*             at the end of the write cycle the eeprom will transfer the data from 
*             temporary memory to the eeproms permanent memory
*             1. end the write cycle 
*             0. don't end the write sycle
*
*/
uint8_t EEPROM2_WriteLong(unsigned long c,uint32_t Address,uint8_t end)
{
	//int x = 0;
	uint8_t z=0;

	z= EEPROM2_WriteInt8((uint8_t)c,Address,0);
	if(z==0)
		return z;
        c>>=8;
	z= EEPROM2_WriteNextInt8((uint8_t)(c),0);
	if(z==0)
		return z;
        c>>=8;
	z= EEPROM2_WriteNextInt8((uint8_t)(c),0);
	if(z==0)
		return z;
        c>>=8;
	z= EEPROM2_WriteNextInt8((uint8_t)(c),end);
	if(z==0)
		return z;

	return 1;
}

/**
* Writes 8 bit number to the eeprom after a write operation has already started by EEPROM2_WriteInt8
* @param c: character to be wrtten to the eeprom 
* @param end: indicator wheather to end the write cycle or not 
*             at the end of the write cycle the eeprom will transfer the data from 
*             temporary memory to the eeproms permanent memory
*             1. end the write cycle 
*             0. don't end the write sycle 
* @see  EEPROM2_WriteInt8
*/
uint8_t EEPROM2_WriteNextLong(unsigned long c,uint8_t end)
{
	//int x = 0;
	uint8_t z=0;

	z= EEPROM2_WriteNextInt8((uint8_t)c,0);
	if(z==0)
		return z;

	z= EEPROM2_WriteNextInt8((uint8_t)(c>>8),0);
	if(z==0)
		return z;

	z= EEPROM2_WriteNextInt8((uint8_t)(c>>16),0);
	if(z==0)
		return z;

	z= EEPROM2_WriteNextInt8((uint8_t)(c>>24),end);
	if(z==0)
		return z;

	return 1;
}

/***
 * Functions below are added for usb firmware upgrade support
 */

uint8_t read_mem(uint8_t* buff, uint32_t start_addres, uint32_t size)
{
	uint8_t x = 0;
	uint8_t z=0;
	int i;
	z = EEPROM2_ReadInt8(start_addres,0,&x);

	if(z==0)
	{
		return 0;
	}

	*buff = x;

	for(i=1;i<size;i++)
	{
		if(i<size-1)
		{
			z = EEPROM2_ReadNextInt8(0,&x);
		}
		else
		{
			z = EEPROM2_ReadNextInt8(1,&x); //stop read cycle
		}
		if(z==0)
		{
			return 0;
		}
		*(buff + i) = x;
	}

	return 1;

}

void eeprom_read(void* res,uint32_t start_addres,uint32_t byte_length)
{
	read_mem((uint8_t*)res,start_addres,byte_length);
}

uint8_t write_mem(uint8_t* buff, uint32_t start_addres, uint32_t size)
{
	uint8_t z=0;
	z= EEPROM2_WriteInt8(*buff,start_addres,0);
	if(z==0)
	{
		return z;
	}
	int i;
	for(i=1;i<size;i++)
	{
		if(i<size-1)
		{
			z= EEPROM2_WriteNextInt8(*(buff+i),0);
		}
		else
		{
			z= EEPROM2_WriteNextInt8(*(buff+i),1);
		}
		if(z==0)
		{
			return z;
		}

	}

	return 1;

}
int eeprom_write(const void* data,uint32_t start_addres,uint32_t byte_length)
{
	return (write_mem((uint8_t*) data, start_addres, byte_length));
}
