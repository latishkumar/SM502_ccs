#include <msp430.h> 
#include <stdint.h>
#include "firmware_loader.h"
#include "I2C2EEPROM.h"

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
 */
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

/*the initial jamp address of the 3 interrupts which normally should not happen*/
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


uint8_t get_checksum(void* data, uint32_t no_bytes)
{
    volatile uint8_t sum = 0;
	uint8_t* data_ptr = (uint8_t*) data;
	while(no_bytes > 0){
		sum += *data_ptr++;
		no_bytes--;
	}
	return(~sum);
}


void InitI2C();
extern void start_loader(uint32_t image_metadata_addr, uint32_t machine_code_addr);

uint32_t prog_addr = FIRMWARE_ADDRESS_START;

uint8_t recv_byte;
void perform_flash_backup(uint8_t backup_type);

void receive_firmware_image()
{
	if(UCA1IFG & UCRXIFG){
		recv_byte = UCA1RXBUF;
		switch(usb_recv.recv_state){
		case IDLE:
			if(recv_byte == START_COMMUNICATION){
				prog_addr = FIRMWARE_ADDRESS_START;
				UCA1TXBUF = ACK_COMMAND;
				prog_addr = FIRMWARE_ADDRESS_START;
				usb_recv.recv_state = RECV_EXPECTED_SIZE;
			}
			break;
		case RECV_EXPECTED_SIZE:
			if(recv_byte == END_COMMUNICATION){
				//end of image transmission
				//start bootloader
				usb_recv.recv_state = IDLE;
				//UCS_set_MCLK_1();
				//PMM_set_vcore(0);
				uint32_t number_of_sec = 0;
                perform_flash_backup(0x04); //Low battery backup
				eeprom_read(&number_of_sec,FIRMWARE_ADDRESS_START + 7,4);
				InitI2C();
				start_loader(FIRMWARE_ADDRESS_START + 7, FIRMWARE_ADDRESS_START + 18 + (12*number_of_sec) );
			}else{
				usb_recv.expected_size = recv_byte;
				usb_recv.byte_count = 0;
				usb_recv.recv_state = RECV_DATA;
			}
			break;
		case RECV_DATA:
			usb_recv.buffer[usb_recv.byte_count++] = recv_byte;
			if(usb_recv.byte_count >= usb_recv.expected_size){
				if(get_checksum(usb_recv.buffer, usb_recv.byte_count)!=0){
					prog_addr = FIRMWARE_ADDRESS_START;
					UCA1TXBUF = NACK_COMMAND;
					usb_recv.recv_state = IDLE;
				}else{
					usb_recv.byte_count -= 1; //we don't want to store checksum
					eeprom_write(usb_recv.buffer, prog_addr, usb_recv.byte_count);
					prog_addr += usb_recv.byte_count;
					usb_recv.byte_count = 0;
					UCA1TXBUF = ACK_COMMAND;
					usb_recv.recv_state = RECV_EXPECTED_SIZE;
				}
			}
			break;
		default:
			break;
		}
	}
}








