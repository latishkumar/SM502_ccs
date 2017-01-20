/*
 * eeprom_read_interface.c
 *
 *  Created on: Dec 26, 2016
 *      Author: user
 */
#include <msp430f6736.h>
#include <stdint.h>

/*
 * read_eeprom.c
 *
 *  Created on: Dec 26, 2016
 *      Author: Ella
 */
#define I2C_PORT_SEL  P2SEL
#define I2C_PORT_OUT  P2OUT
#define I2C_PORT_REN  P2REN
#define I2C_PORT_DIR  P2DIR
#define SDA_PIN       BIT1                  // UCB0SDA pin
#define SCL_PIN       BIT0                  // UCB0SCL pin
#define SCL_CLOCK_DIV 0x12                  // SCL clock devider
void InitI2C()
{
  I2C_PORT_SEL |= SDA_PIN | SCL_PIN;        // Assign I2C pins to USCI_B0
  //P2DIR		   |= SDA_PIN | SCL_PIN;
  // Recommended initialisation steps of I2C module as shown in User Guide:
  UCB0CTLW0  |= UCSWRST;                      // Enable SW reset
  UCB0CTLW0  |= UCMST | UCMODE_3 | UCSYNC;     // I2C Master, synchronous mode
  UCB0CTLW0 |= UCSSEL_2 | UCTR ;     // Use SMCLK, TX mode, keep SW reset
  UCB0BR0 = 41;//SCL_CLOCK_DIV;                  // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA  = 0x50;          // define Slave Address
                                            // In this case the Slave Address
                                            // defines the control byte that is
                                        // sent to the EEPROM.
  UCB0I2COA0 = 0x01A5;                       // own address.
  UCB0CTLW0 &= ~UCSWRST;                     // Clear SW reset, resume operation

  if (UCB0STATW & UCBBUSY)                   // test if bus to be free
  {                                         // otherwise a manual Clock on is
                                            // generated
    I2C_PORT_SEL &= ~SCL_PIN;               // Select Port function for SCL
    I2C_PORT_OUT &= ~SCL_PIN;               //
    I2C_PORT_DIR |= SCL_PIN;                // drive SCL low
    I2C_PORT_SEL |= SDA_PIN + SCL_PIN;      // select module function for the
                                            // used I2C pins
  };
}


void read_eeprom_2(uint8_t* res,uint32_t start_address ,  uint32_t read_length)
{
  uint16_t Add =  start_address & 0xffff;
  while (UCB0STAT & UCBBUSY);                // wait until I2C module has
                                            // finished all operations
  // Write Address first

   UCB0CTLW0 |= UCTR;   // UCTR=1 => Transmit Mode (R/W bit = 0)
   UCB0CTLW0 |=UCTXSTT;   // start condition generation
   while(UCB0CTLW0 & UCTXSTT);                // Start condition sent?
   while(!(UCTXIFG0 & UCB0IFG));
   UCB0IFG &= ~UCTXIFG0;                   // Clear USCI_B0 TX int flag
   UCB0TXBUF = (Add >> 8);						//transmit high byte of the address
   while(!(UCTXIFG0 & UCB0IFG));
   UCB0IFG &= ~UCTXIFG0;                   // Clear USCI_B0 TX int flag
   UCB0TXBUF = Add;//(uint16_t)(start_address & 0x00ff);
   while(!(UCTXIFG0 & UCB0IFG));
   UCB0IFG &= ~UCTXIFG0;                   // Clear USCI_B0 TX int flag

   // Read Data byte
   UCB0CTLW0 &= ~UCTR;                        // UCTR=0 => Receive Mode (R/W bit = 1)
   UCB0CTLW0 |= UCTXSTT;                      // I2C start condition
   while(UCB0CTLW0 & UCTXSTT);                // Start condition sent?
   int i;
   for(i = 0; i < read_length; i++) // read data bytes
     {
	  while(!(UCRXIFG0 & UCB0IFG));
	  *(res + i) = UCB0RXBUF;
     }

   UCB0CTLW0 |= UCTXSTP;                      // I2C stop condition
   while(UCB0CTLW0 & UCTXSTP);                // Ensure stop condition got sent
}







