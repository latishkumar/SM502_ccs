/*
 * firmware_loader.h
 *
 *  Created on: Nov 11, 2016
 *      Author: Getaneh
 */

#ifndef FIRMWARE_LOADER_H_
#define FIRMWARE_LOADER_H_

#include<stdint.h>

#define FIRMWARE_ADDRESS_START (unsigned long)0x00040000ul//0x3E0000ul//0x000AC000ul
#define FIRMWARE_SECTION_ADDRESS_START 0x3E0008ul//0x000AC008ul
#define CODE_ADDRESS_START  0x3E0038 //0x000AC038ul
#define SCANING_ADDRESS 0
#define SCANING_CODE 1
#define ACK_CODE 0x23
#define NEW_SEC_START	0x40  /*@ character*/
#define	END_OF_IMAGE	0x71  /*q character*/
#define SPACE_DELIMITER	0x20  /*space character*/
#define LINE_DELIMITER	0x0A  /*newline character*/

#define MAX_RECV_SIZE           129
#define START_COMMUNICATION     0x78
#define END_COMMUNICATION       0xFF
#define ACK_COMMAND             0x49
#define NACK_COMMAND            0x53

typedef struct firmware_section{
	uint32_t program_mem_addr;
	uint32_t backup_mem_addr;
	uint32_t code_size;
	uint16_t checksum;
}firmware_sec_t;

typedef struct firmware_image{
	uint16_t no_sections;
	uint32_t next_code_addr;
	uint32_t next_section_addr;
	uint16_t scan_state;
	uint16_t checksum;
}firmware_image_t;

typedef enum{
    IDLE = 0,
    RECV_EXPECTED_SIZE,
    RECV_DATA,
}state_t;

typedef struct{
    state_t recv_state;
    uint8_t expected_size;
    uint8_t byte_count;
    uint8_t buffer[MAX_RECV_SIZE];
}usb_recv_t;

usb_recv_t usb_recv;

void init_firmware_loader();

#endif /* FIRMWARE_LOADER_H_ */
