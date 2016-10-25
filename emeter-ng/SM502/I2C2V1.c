/*
 * I2C2.c
 *
 *  Created on: May 21, 2013
 *      Author: Elec_Lab
 */



#include "I2C2.h"

#define I2C2_SDA BIT1


void I2C2Init()
{

  I2C2_SCL_OUT;
  I2C2_SDA_OUT;

}

uint8_t Slave_Address(uint8_t sn,uint8_t b16)
{
  return (0x50 | ((b16<<2)&0x4) | (sn&0x01));
}
/*
 *the method writes 8 bit number to the EEPROM
 *The method does not end the write Process
 *c 8 bit number to write to EEPROM
 */
void I2C2WriteInt8(uint8_t c)
{
	uint8_t i=8;
        uint8_t j=7;//0;
	uint8_t mask=0x80;//1;
	uint8_t send=0;
	for(;i>=1;i--)
	{

	   I2C2_SCL_LO;
	   __delay_cycles(5);
       send = (c & mask)>>j;

       if(send==1)
    	   I2C2_SDA_HI;
       else
    	   I2C2_SDA_LO;

       __delay_cycles(6);

       I2C2_SCL_HI;
       j--;//j++
       mask >>= 1;
       //__delay_cycles(200);
       //__delay_cycles(1);

       __delay_cycles(5);
	}
	I2C2_SCL_LO;
}

/*
 * This method Reads 8 bit number from the EEPROM
 * the address to be read is the last address where reading or
 * writing stopped.
 *
 * LSB first
 */
uint8_t read2=0;
uint8_t I2C2ReadInt8()
{
   I2C2_SDA_IN;
    uint8_t i=8,j=7;
    uint16_t result=0;

     read2=0;


    for(;i>=1;i--)
    {

       I2C2_SCL_LO;
       __delay_cycles(20);
       I2C2_SCL_HI;
       __delay_cycles(25);

        if((P2IN & I2C2_SDA) == I2C2_SDA)
                result= 1;
        else
                result = 0;

       read2 |= (result)<<j;

       j--;
       __delay_cycles(20);

    }

    I2C2_SCL_LO;
    I2C2_SDA_OUT;
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

  //I2C2_SDA_OUT;
  I2C2_SCL_LO;
  __delay_cycles(2);

  I2C2_SDA_LO;
  __delay_cycles(25);
  I2C2_SCL_HI;
  __delay_cycles(25);
  I2C2_SDA_HI;
  __delay_cycles(30);
  I2C2_SCL_LO;
  __delay_cycles(25);

}


void I2C2GenerateStartCondition()
{
	  //I2C2_SCL_LO;
	  I2C2_SDA_HI;
	  __delay_cycles(25);
	  I2C2_SCL_HI;
	  __delay_cycles(50);
	  I2C2_SDA_LO;
	  __delay_cycles(30);
	  I2C2_SCL_LO;
	  __delay_cycles(30);

}


uint8_t I2C2getACK()
{
	uint8_t result = 0;
	I2C2_SDA_IN; //make the data input

	I2C2_SCL_LO; //pull the clock low

	__delay_cycles(50);

	I2C2_SCL_HI; //make the clock high

	__delay_cycles(50);

	if((P2IN & I2C2_SDA) == I2C2_SDA)
		result= 0;
	else
		result = 1;

	I2C2_SCL_LO;
    __delay_cycles(25);

    I2C2_SDA_OUT;

	return result;
}


void PollACK2(uint8_t slaveaddress)
{
	I2C2RequestTx(1,slaveaddress);
	while(I2C2getACK()!=1)
	{
		I2C2RequestTx(1,slaveaddress);
		__delay_cycles(10);
	}
}


void BussIdeal2()
{
	I2C2_SCL_HI;
	I2C2_SDA_HI;
}





///*
// * I2C2.c
// *
// *  Created on: May 21, 2013
// *      Author: Elec_Lab
// */
//
//
//
//#include "I2C2.h"
//
//
//
//void I2C2Init()
//{
//
//  I2C2_SCL_OUT;
//  I2C2_SDA_OUT;
//
//}
//
//uint8_t Slave_Address(uint8_t sn,uint8_t b16)
//{
//  return (0x50 | ((b16<<2)&0x4) | (sn&0x01));
//}
///*
// *the method writes 8 bit number to the EEPROM
// *The method does not end the write Process
// *c 8 bit number to write to EEPROM
// */
//void I2C2WriteInt8(uint8_t c)
//{
//	uint8_t i=8;
//        uint8_t j=7;//0;
//	uint8_t mask=0x80;//1;
//	uint8_t send=0;
//	for(;i>=1;i--)
//	{
//
//	   I2C2_SCL_LO;
//	   __delay_cycles(Write_delay);
//       send = (c & mask)>>j;
//
//       if(send==1)
//    	   I2C2_SDA_HI;
//       else
//    	   I2C2_SDA_LO;
//
//       __delay_cycles(Write_delay);
//
//       I2C2_SCL_HI;
//       j--;//j++
//       mask >>= 1;
//       //__delay_cycles(200);
//       //__delay_cycles(1);
//
//       __delay_cycles(Write_delay);
//	}
//	I2C2_SCL_LO;
//}
//
///*
// * This method Reads 8 bit number from the EEPROM
// * the address to be read is the last address where reading or
// * writing stopped.
// *
// * LSB first
// */
//uint8_t read2=0;
//uint8_t I2C2ReadInt8()
//{
//        I2C2_SDA_IN;
//	uint8_t i=8,j=7;
//        uint16_t result=0;
//
//	 read2=0;
//
//
//	for(;i>=1;i--)
//	{
//
//	   I2C2_SCL_LO;
//       __delay_cycles(Read_delay);
//       I2C2_SCL_HI;
//       __delay_cycles(Read_delay);
//
//     	if((P2IN & I2C2_SDA) == I2C2_SDA)
//   		result= 1;
//     	else
//   		result = 0;
//
//       read2 |= (result)<<j;
//
//       j--;
//       __delay_cycles(Read_delay);
//
//	}
//
//	I2C2_SCL_LO;
//	I2C2_SDA_OUT;
//	return read2;
//}
//
//
//uint8_t result=0;
//void I2C2RequestTx(uint8_t transmit,uint8_t slave_address)
//{
//	uint8_t tx=0;
//	tx=slave_address<<1 | (~transmit & 0x01);
//
//	I2C2GenerateStartCondition();
//
//	I2C2WriteInt8(tx);
//}
//
//
//void I2C2NACK()
//{
//
//	I2C2_SCL_LO;
//	__delay_cycles(Read_delay);
//	I2C2_SDA_HI;
//	__delay_cycles(Read_delay);
//	I2C2_SCL_HI;
//	__delay_cycles(Read_delay);
//	I2C2_SCL_LO;
//	__delay_cycles(Read_delay);
//
//}
//
//
//void I2C2ACK()
//{
//
//	I2C2_SCL_LO;
//	__delay_cycles(Read_delay);
//	I2C2_SDA_LO;
//	__delay_cycles(Read_delay);
//	I2C2_SCL_HI;
//	__delay_cycles(Read_delay);
//	I2C2_SCL_LO;
//	__delay_cycles(Read_delay);
//
//}
//
//
//void I2C2GenerateStopCondition()
//{
//
//        //I2C2_SDA_OUT;
//        I2C2_SCL_LO;
//        __delay_cycles(Read_delay);
//
//        I2C2_SDA_LO;
//        __delay_cycles(Read_delay);
//        I2C2_SCL_HI;
//        __delay_cycles(Read_delay);
//        I2C2_SDA_HI;
//        __delay_cycles(Read_delay);
//        I2C2_SCL_LO;
//        __delay_cycles(Read_delay);
//
//}
//
//
//void I2C2GenerateStartCondition()
//{
//	  //I2C2_SCL_LO;
//	  I2C2_SDA_HI;
//	  __delay_cycles(Read_delay);
//	  I2C2_SCL_HI;
//	  __delay_cycles(Read_delay);
//	  I2C2_SDA_LO;
//	  __delay_cycles(Read_delay);
//	  I2C2_SCL_LO;
//	  __delay_cycles(Read_delay);
//
//}
//
//
//uint8_t I2C2getACK()
//{
//	uint8_t result = 0;
//	I2C2_SDA_IN; //make the data input
//
//	I2C2_SCL_LO; //pull the clock low
//
//	__delay_cycles(Read_delay);
//
//	I2C2_SCL_HI; //make the clock high
//
//	__delay_cycles(Read_delay);
//
//	if((P2IN & I2C2_SDA) == I2C2_SDA)
//		result= 0;
//	else
//		result = 1;
//
//	I2C2_SCL_LO;
//        __delay_cycles(Read_delay);
//
//        I2C2_SDA_OUT;
//	return result;
//}
//
//
//void PollACK2(uint8_t slaveaddress)
//{
//	I2C2RequestTx(1,slaveaddress);
//	while(I2C2getACK()!=1)
//	{
//		I2C2RequestTx(1,slaveaddress);
//		__delay_cycles(Read_delay);
//	}
//}
//
//
//void BussIdeal2()
//{
//	I2C2_SCL_HI;
//	I2C2_SDA_HI;
//}
//
