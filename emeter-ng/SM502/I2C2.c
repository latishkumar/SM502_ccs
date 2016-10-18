/*
 * I2C2.c
 *
 *  Created on: May 21, 2013
 *      Author: Elec_Lab
 */



#include "I2C2.h"

void __delay(int i)
{
   for(;i>0;i--)
   {
     _NOP();
     _NOP();
     _NOP();
     _NOP();
   }
}

void I2C2Init()
{

  I2C2_SCL_HI;
  I2C2_SDA_HI;

}

uint8_t Slave_Address(uint8_t sn,uint8_t b16)
{
  #ifdef EEPROM_REV_2 
     return (0x50 | ((b16)&0x3) | (sn<<2));
  #else
      return (0x50 | ((b16<<2)&0x4) | (sn&0x01));
  #endif 
}
/*
 *the method writes 8 bit number to the EEPROM
 *The method does not end the write Process
 *c 8 bit number to write to EEPROM
 */
void I2C2WriteInt8(uint8_t c)
{
	uint8_t i=8;

	for(;i>=1;i--)
	{

           if(c & 0x80)
               I2C2_SDA_HI;
           else
               I2C2_SDA_LO;
           
            c <<= 1;
            
           __delay(I2C_Delay);
           I2C2_SCL_HI;
           __delay(I2C_Delay);
           I2C2_SCL_LO;
	}

}

/*
 * This method Reads 8 bit number from the EEPROM
 * the address to be read is the last address where reading or
 * writing stopped.
 *
 * LSB first
 */
uint8_t I2C2ReadInt8()
{
    uint8_t i=8;//,j=7;
    uint8_t read2=0;

    I2C2_SDA_HI;
    
    for(;i>=1;i--)
    {

       I2C2_SCL_HI;
       __delay(I2C_Delay);
       read2<<=1;

        if((I2C2_SDA_PIN & IICEEPROM_SDA_BIT) == IICEEPROM_SDA_BIT)
                read2|= 1;
        I2C2_SCL_LO;
        __delay(I2C_Delay);
    }
    return read2;
}


uint8_t result=0;
void I2C2RequestTx(uint8_t transmit,uint8_t slave_address)
{
	uint8_t tx=0;
	tx=slave_address<<1 | (~transmit & 0x01);

	I2C2GenerateStartCondition();

	I2C2WriteInt8(tx);
}


void I2C2NACK()
{

	I2C2_SCL_LO;
	__delay_cycles(25);
	I2C2_SDA_HI;
	__delay_cycles(25);
	I2C2_SCL_HI;
	__delay_cycles(25);
	I2C2_SCL_LO;
	__delay_cycles(25);

}


void I2C2ACK()
{

	I2C2_SCL_LO;
	__delay_cycles(25);
	I2C2_SDA_LO;
	__delay_cycles(25);
	I2C2_SCL_HI;
	__delay_cycles(25);
	I2C2_SCL_LO;
	__delay_cycles(25);

}


void I2C2GenerateStopCondition()
{

  I2C2_SDA_LO;
  __delay(I2C_Delay);
  I2C2_SCL_HI;
  __delay(I2C_Delay);
  I2C2_SDA_HI;
  __delay(I2C_Delay);
  I2C2_SCL_LO;


}


void I2C2GenerateStartCondition()
{
	  
	  I2C2_SDA_HI;
	  __delay(I2C_Delay);
	  I2C2_SCL_HI;
	  __delay(I2C_Delay);
	  I2C2_SDA_LO;
	  __delay(I2C_Delay);
	  I2C2_SCL_LO;
	  __delay(I2C_Delay);

}


uint8_t I2C2getACK()
{
	uint8_t result = 0;
        __delay(I2C_Delay);
	I2C2_SDA_HI; //make the data input
        __delay(I2C_Delay);
	I2C2_SCL_HI; //pull the clock low
	__delay(I2C_Delay);

	if((I2C2_SDA_PIN & IICEEPROM_SDA_BIT) == IICEEPROM_SDA_BIT)
		result= 0;
	else
		result = 1;
        __delay(I2C_Delay);
	I2C2_SCL_LO;
        __delay(I2C_Delay);

	return result;
}


void PollACK2(uint8_t slaveaddress)
{
	I2C2RequestTx(1,slaveaddress);
	while(I2C2getACK()!=1)
	{
		I2C2RequestTx(1,slaveaddress);
		__delay(I2C_Delay);
	}
}


void BussIdeal2()
{
	I2C2_SCL_HI;
	I2C2_SDA_HI;
}




