/*
 * I2C2.h
 *
 *  Created on: May 21, 2013
 *      Author: Elec_Lab
 */

#ifndef I2C2_H_
#define I2C2_H_

#include <msp430.h>
#include <stdint.h>
/*
 * The address of the First EEPROM
 */

//#define Slave_Address(sn,b16)  (0x50 | ((b16<<2)&0x4) | ((sn&0x01)))
//#define Slave_Address1  (80) //0x50

/*
 * The address of the Second EEPROM
 */
//#define Slave_Address2  81 //0x51

#define Read_delay 20
#define Write_delay 20

#define I2C2_SCL_IN (P2DIR &= ~BIT0)
#define I2C2_SCL_OUT (P2DIR |=BIT0)
#define I2C2_SCL_HI (P2OUT |=BIT0)
#define I2C2_SCL_LO (P2OUT &=~BIT0)

#define I2C2_SCL BIT0

#define I2C2_SDA_IN (P2DIR &= ~BIT1)
#define I2C2_SDA_OUT (P2DIR |=BIT1)
#define I2C2_SDA_HI (P2OUT|=BIT1)
#define I2C2_SDA_LO (P2OUT&=~BIT1)

#define I2C2_SDA BIT1

void I2C2Init();

uint8_t Slave_Address(uint8_t sn,uint8_t b16);
/*
 *the method writes 8 bit number to the EEPROM
 *The method does not end the write Process
 *c 8 bit number to write to EEPROM
 */
void I2C2WriteInt8(uint8_t c);

/*
 * This method Reads 8 bit number from the EEPROM
 * the address to be read is the last address where reading or
 * writing stopped.
 */

uint8_t I2C2ReadInt8();

/*
 *
 */
void I2C2RequestTx(uint8_t transmit,uint8_t slave_address);
void I2C2ACK();
void I2C2NACK();
void I2C2GenerateStopCondition();
void I2C2GenerateStartCondition();


uint8_t I2C2getACK();
void PollACK2(uint8_t slaveaddress);
void BussIdeal2();


#endif /* I2C2_H_ */
