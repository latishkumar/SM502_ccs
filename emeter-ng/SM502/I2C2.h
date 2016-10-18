/*
 * I2C2.h
 *
 *  Created on: May 21, 2013
 *      Author: Donayam
 */

#ifndef _I2C2_H_
#define _I2C2_H_

#include <msp430.h>
#include <stdint.h>




#define IICEEPROM_SDA_BIT BIT1





#define I2C_Delay 10
#define I2C2_SCL_IN  (P2DIR &= ~BIT0)
#define I2C2_SCL_OUT (P2DIR |=BIT0)
#define I2C2_SCL_P_HI (P2OUT |=BIT0)
#define I2C2_SCL_P_LO (P2OUT &=~BIT0)

#define I2C2_SDA_IN (P2DIR &= ~BIT1)
#define I2C2_SDA_OUT (P2DIR |=BIT1)
#define I2C2_SDA_P_HI (P2OUT|=BIT1)
#define I2C2_SDA_P_LO (P2OUT&=~BIT1)

#define I2C2_SCL_HI  (I2C2_SCL_IN,I2C2_SCL_P_HI)  //Oct 23,2015 - 10:53[M.G] Original #define I2C2_SCL_HI  (I2C2_SCL_OUT,I2C2_SCL_P_HI)
#define I2C2_SCL_LO  (I2C2_SCL_OUT,I2C2_SCL_P_LO)   
        
#define I2C2_SDA_HI  (I2C2_SDA_IN,I2C2_SDA_P_HI)//[M.G] original #define I2C2_SDA_HI  (I2C2_SDA_OUT,I2C2_SDA_P_HI)
#define I2C2_SDA_LO  (I2C2_SDA_OUT,I2C2_SDA_P_LO)


#define I2C2_SDA_PIN P2IN //BIT1

void I2C2Init();

uint8_t Slave_Address(uint8_t sn,uint8_t b16);
/**
 * The method writes 8 bit number to the EEPROM
 * The method does not end the write Process
 * @param c:- 8 bit number to write to EEPROM
 */
void I2C2WriteInt8(uint8_t c);

/**
 * This method Reads 8 bit number from the EEPROM
 * the address to be read is the last address where reading or
 * writing stopped.
 */

uint8_t I2C2ReadInt8();

/**
 *  sends tx request to the I2C device
 * @param transmit 
 *               1 for write 
 *               0 for read
* @param slave_address :- address of the slave device
 *
 */
void I2C2RequestTx(uint8_t transmit,uint8_t slave_address);
/**
* send ACK to device 
*/
void I2C2ACK();
/**
* send NACK to device
*/
void I2C2NACK();
/**
* generate stop condition
*/
void I2C2GenerateStopCondition();
/**
* genreates start condition
*/
void I2C2GenerateStartCondition();

/**
* get ACK byte from slave
*/
uint8_t I2C2getACK();
/**
* wait for ACK
*/
void PollACK2(uint8_t slaveaddress);
/**
* sets the I2C bus to idel mode 
*/
void BussIdeal2();


#endif /* _I2C2_H_ */
