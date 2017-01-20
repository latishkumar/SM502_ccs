#include <msp430.h> 
#include <stdint.h>

#include "../../firmware_loader/firmware_loader.h"
#include "../usb_firmware_loader/firmware_loader/I2C2EEPROM.h"
#include "../usb_firmware_loader/firmware_loader/PMM.h"
#include "../usb_firmware_loader/firmware_loader/UCS.h"
#include "../usb_firmware_loader/LCD_C.h"
int data = 0;
volatile int sec_flag = 0;
volatile int download_finished = 0;
void InitI2C();
//void read_eeprom(uint8_t* res,uint32_t start_address ,  uint32_t read_length);
void read_eeprom(uint8_t* buff, uint32_t start_address, uint16_t size);
uint8_t data_buffer[35]={
		0xEB,	0x10,	0x20,	0x30,	0x40,	0x50,	0x60,	0x70,
		0x81,	0x95,	0xa1,	0xb1,	0xc1,	0xd1,	0xe1,	0xf1,
		0x12,	0x02,	0x32,	0x26,	0x52,	0x42,	0x72,	0x62,
		0x93,	0x83,	0xb3,	0xa3,	0xd3,	0xc3,	0xf3,	0xef,
};
//uint8_t read_mem(uint8_t* buff, uint32_t start_addres, uint16_t size);
//uint8_t write_mem(uint8_t* buff, uint32_t start_addres, uint16_t size);
#define FIRMWARE_START_ADDRES 0x0000

uint8_t res22[472],res1[472],res33[32],res4[32],res5[32];
void test_eeprom_read_write_function();
/*
 * main.c
 */
volatile uint32_t add=0x38;
//void test_slave_address();
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P8DIR |= BIT4 | BIT6;
    P8OUT |= BIT4;
    P8OUT |= BIT6;

    init_firmware_loader();
    init_lcd_c();
    all_segments_off();
   // InitI2C();
// all_segments_on();
   //__bis_SR_register(GIE);
  /* for(;;){
	   eeprom_read(res1,add,471);
	   InitI2C();
	   read_eeprom(res22,add,471);
	   int i;
	   for(i=0;i<471;i++){
		   if(res1[i]!= res22[i]){
			   asm(" nop");
		   }
	   }
	 //  test_slave_address();
   }
*/
    __bis_SR_register(GIE);
    TA1CCTL0 |= CCIE;
    TA1CCR0 = 16760;
    TA1CTL = TASSEL__ACLK|MC__UP|TACLR;
    //();
    for(;;)
    {

    	// test_eeprom_read_write_function();
    	if(sec_flag)
    	{
			sec_flag = 0;
          // P8OUT ^= BIT4;
    	   P8OUT ^= BIT6;
    	}
    	if(sec_flag==3){
    		test_eeprom_read_write_function();
    	}
    }
	return 0;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void time_ISR(){
	sec_flag = 1;
}

void EEPROM2Init();

void test_eeprom_read_write_function()
{
	volatile int tmp;
	    if(eeprom_write(data_buffer,0x0020,32)){
	    	data_buffer[0]=0xAA;
	    	if(eeprom_write(data_buffer,0x1020,32));
	    	data_buffer[0]=0xBB;
	    	if(eeprom_write(data_buffer,0x2020,32));
	    	data_buffer[0]=0xCC;
	    	if(eeprom_write(data_buffer,0x3020,32));
	    	data_buffer[0]=0xDD;
	    	if(eeprom_write(data_buffer,0x4020,32));
	    	//mem_read(res2,0x3000,32);
	    	//mem_read(res1,0x2000,32);

	    	InitI2C();
	    	read_eeprom(res1,0x0020,32);
	    	read_eeprom(res22,0x1020,32);
	    	read_eeprom(res33,0x2020,32);
	    	read_eeprom(res4,0x3020,32);
	    	read_eeprom(res5,0x4020,32);
	    		int i;
	    		for(i=1;i<32;i++)
	    		{
	    			if(data_buffer[i] != res22[i])//!= res1[i] !=res33[i] != res4[i] != res5[i])
	    			{
	    				tmp=55;
	    			}
	    			if(data_buffer[i] != res1[i])
					{
						tmp=55;
					}
	    			if(data_buffer[i] !=res33[i])// !=res33[i] != res4[i] != res5[i])
					{
						tmp=55;
					}
	    			if(data_buffer[i] !=res4[i])// !=res33[i] != res4[i] != res5[i])
					{
						tmp=55;
					}
	    			if(data_buffer[i] !=res5[i])// !=res33[i] != res4[i] != res5[i])
					{
						tmp=55;
					}
	    		}
	    	}
}
/*uint32_t Address = 0x3FFEE;
uint8_t slaveAddress = 0;
uint8_t Slave_Address(uint8_t sn,uint8_t b16);
void test_slave_address()
{
	uint8_t AddHigh = Address>>16;
	slaveAddress = Slave_Address((AddHigh&0x04)>>2,AddHigh & 0x3);
}*/

