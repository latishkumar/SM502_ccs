
#include "firmware_loader.h"

#include <msp430.h> 
#include <stdint.h>
#include "../SM502/I2C2EEPROM.h"


/*define top of ram, this is where the last byte of reset vector is mapped to*/
#define TOP_OF_RAM          (0x9BFF)

/*start address of interrupt vector table in the RAM*/
#define RAM_INT_VECT_START_ADDR  (TOP_OF_RAM + 1 - sizeof(ram_int_vect_t))

/*
 * this is to declare the type of interrupt service routines in the ram
 * normally we use function pointers as a type of service routines, but since
 * we are now using CPUx(large memory model) , pointers will take more than two
 * bytes of memory(20bits), hence, conflicting whith the interrupt vector architecture
 * which should be in a lower 64k memory address.
 * */
typedef uint16_t isr_type_t;

/*
 * we only need user nonmaskable interrupt to detect and handle
 * power loss during firmware upgrade, so memory allocation in RAM of the first
 * 3 vector tables should be enough for our purpose
 */
typedef struct {
  isr_type_t user_nonmaskable_inter;
  isr_type_t system_nonmaskable_inter;
  isr_type_t reset;
}ram_int_vect_t;

/*the initial jamp address of the 3 interrupts which normally should not happed*/
#define UNDEFINED_ISR    0x0000

/*
 * put the interrupt vector tables in the specified RAM region
 */
#pragma location=RAM_INT_VECT_START_ADDR
ram_int_vect_t ram_int_vect = {
	UNDEFINED_ISR,
	UNDEFINED_ISR,
	UNDEFINED_ISR,
};

void firmware_upgrade_startup(){
	/*
	 * disable all maskable interrupts,
	 *backup system image before loader starts,
	 *configure the clock system and digital core
	 *supplies for lower values. finally initialize
	 *and call the firmware loader
	 */

}

#define CARRIAGE_RETURN 13


/*
 * converts ti-hex ascii characters into real bytes, and finally concatenate adjacent
 * nibbles to form a byte
 * this function expects even number of characters to be converted
 */
static void decode_hex_to_byte(uint8_t *in_ptr,uint8_t *out_ptr,unsigned int byte_len){

	unsigned int i;

	uint8_t h_nib, l_nib;

	for( i = 0; i < byte_len; i += 2)
	{
		h_nib = in_ptr[i] > '9' ? ((in_ptr[i] < 'a')?in_ptr[i] - 'A' + 10 : in_ptr[i] - 'a' + 10): in_ptr[i] - '0';
		l_nib = in_ptr[i+1] > '9' ? ((in_ptr[i+1] < 'a') ? in_ptr[i+1] - 'A' + 10 : in_ptr[i+1] - 'a' + 10): in_ptr[i+1] - '0';
		*out_ptr++ = (h_nib << 4 ) | l_nib;
	}
}

uint8_t row_data[34];
uint8_t machine_byte[16];  uint8_t byte_counter = 0;
unsigned char ch; uint32_t program_start_address;


extern void init_loader();

firmware_image_t firm_image = {
			.next_code_addr = CODE_ADDRESS_START,
			.next_section_addr = FIRMWARE_SECTION_ADDRESS_START,
			.no_sections = 0
};

firmware_sec_t firm_sec ={
		.backup_mem_addr = CODE_ADDRESS_START,
		.code_size = 0
};

firmware_image_t* fi = &firm_image;
firmware_sec_t* fs = &firm_sec;


void init_usb(){
	P1DIR|=BIT5;
	P1DIR&=~BIT4;
	P1SEL |=BIT4 | BIT5;
	UCA1CTLW0 |= UCSWRST; //reset for configuration
    UCA1CTLW0 &= ~UCPEN; //Disable Parity
    UCA1CTLW0 &= ~UCMSB;//LSB first  default
    UCA1CTLW0 &= ~UC7BIT;//8 -bit    default
    UCA1CTLW0 &= ~UCSPB;// one stop bit default
    UCA1CTLW0 |= UCMODE_0;//UART Mode default
    UCA1CTLW0 &= ~UCSYNC;//Asynchronous mode default
    UCA1CTL1  |= UCSSEL_3;//SMCLK is the clock source
  	UCA1CTLW1 |= UCGLIT_1;//50nsec glitch time

  	UCA1BRW_L = 18;//57600
	UCA1BRW_H = 0;
	UCA1MCTLW=0x4400|UCOS16|UCBRF_3;//oversampling baud-rate generation mode 0x4400 UCBRSx

  	/*UCA1BRW_L = 54;//19200
  	UCA1BRW_H = 0;
  	UCA1MCTLW=0xEE00|UCOS16|UCBRF_9;//oversampling baud-rate generation mode 0x4400 UCBRSx*/
    UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only
    UCA1CTLW0 &=~UCSWRST;
    UCA1IE |= UCRXIE;//|UCTXIE|UCRXIE;
}
#define MCLK_DEF 16   /* 16 MHz */
void init_firmware_loader()
{
	 init_usb();
}
void InitI2C();
volatile int download_finished;
void perform_low_battry_backup();

void receive_firmware_image()
{
	if(UCA1IFG & UCRXIFG)
	{
		ch = UCA1RXBUF;
		switch(ch)
		{
			case NEW_SEC_START:
				if(fi ->scan_state == SCANING_ADDRESS)
				{
					//mem_block_erase(FIRMWARE_ADDRESS_START,64);
					asm(" nop");
				}
				else
				{
					/*it must have been the end of previous section, hence, log the section structure*/
					fs ->program_mem_addr = program_start_address;
					/*calculate checksum*/
					eeprom_write(fs,fi ->next_section_addr,sizeof(firmware_sec_t));
					fi ->next_section_addr += sizeof(firmware_sec_t);
					fi ->no_sections ++;
				}
				fs ->code_size = 0;
				fs ->backup_mem_addr = fi ->next_code_addr;
				fi ->scan_state = SCANING_ADDRESS;
				break;
			case END_OF_IMAGE:
				fs ->program_mem_addr = program_start_address;
				/*calculate checksum*/
				eeprom_write(fs,fi ->next_section_addr,sizeof(firmware_sec_t));
				//fi ->next_section_addr += sizeof(firmware_sec_t);
				fi ->no_sections ++;
				eeprom_write(fi,FIRMWARE_ADDRESS_START,8);
				UCA1TXBUF = ch;
			//	__delay_cycles(10000);
				download_finished = 1;
				/*UCS_set_MCLK_1();
				PMM_set_vcore(0);
				AUXCTL0 = AUXKEY;
				AUXCTL2 |= AUX0LVL_1 * 5 + AUX1LVL_1 * 0 + AUX2LVL_1 * 0;*/
				perform_low_battry_backup();
				InitI2C();
				init_loader();
				return;
			case LINE_DELIMITER:

				if(fi ->scan_state == SCANING_ADDRESS)
				{
					if(byte_counter >= 5)
					{
						row_data[0] = '0';
						decode_hex_to_byte(row_data, machine_byte, byte_counter+1);
						program_start_address = (uint32_t)machine_byte[0]<<16 |(uint16_t)machine_byte[1]<<8 | (uint16_t)machine_byte[2];
					}
					else
					{
						decode_hex_to_byte(&row_data[1], machine_byte, byte_counter);
						program_start_address = (uint16_t)machine_byte[0]<<8 |(uint16_t)machine_byte[1];
					}
					fi ->scan_state = SCANING_CODE;
				}
				else
				{
					decode_hex_to_byte(&row_data[1], machine_byte, byte_counter);
					byte_counter >>= 1;    /*the hex ascii characters are merged to generate single byte*/
					eeprom_write(machine_byte, fi ->next_code_addr, byte_counter);
					fi ->next_code_addr += byte_counter;
					fs -> code_size += byte_counter;
				}
				byte_counter = 0;
				break;
			case SPACE_DELIMITER:
				break;
			case CARRIAGE_RETURN:
				break;

			default:
				row_data[++byte_counter] = ch;
		}
		UCA1TXBUF = ch;
	}
}








