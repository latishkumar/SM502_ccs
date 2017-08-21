;**************************************************************************************
; 			file: flash_programmer.asm
; 		    Created on: Nov 11, 2016
;   		Author: Getaneh
;***************************************************************************************

;***************************************************************************************
;						FLASH PROGRAMMER MODULE
;***************************************************************************************
; this file contains an MSP430 flash programmer which is used to store program machine
; instructions to the flash memory.
;usage: void flash_programmer(uint8_t* buff, uint8_t* addr_ptr, uint16_t size, uint16_t type)
; buff: points to the start of instruction byte to be programmed
; addr_ptr: points to the first address in program memory to be programmed
; size: total number of bytes to be programmed
; type: this indicates wheather block or word programming will be initiated - 0 for block 1 for word


; void read_mem(uint8_t* buff, uint32_t start_address, uint16_t size)
	.cdecls C, LIST, "msp430f6736.h"

FLASH_PROGRAMMER_LEN .equ	(FLASH_PROGRAMMER_ADDR_END - flash_programmer + 2)/2

	.global	FLASH_PROGRAMMER_ADDR_END
	.global FLASH_PROGRAMMER_LEN


	.text

flash_programmer:

L1:
	bit	#BUSY,&FCTL3						;check if flash is idle
	jnz L1
	mov.w	#FWPW,&FCTL3					;unlock flash memory
	bit.w	#1,R15                          ;check if memory block write or word write need to be initiated
	jnz		WORD_PROG
BLOCK_WRITE:
	mov.w	#FWPW+BLKWRT, &FCTL1			;enable block memory write
L2:
	mov.w	@R12+, 0(R13)
	mov.w	@R12+, 2(R13)
L3:
	bit.w	#WAIT, &FCTL3
	jz		L3
	incda	R13
	incda	R13
	sub.w	#4,R14
	jnz		L2
	jmp 	FINISH

WORD_PROG:
	mov.w	#FWPW+WRT, &FCTL1				;enable word memory write
L4:
	;mov.w	@R12+, 0(R13)
	mov.b   0(R12), 0(R13)
	inc		R12
L5:
	bit.w	#WAIT, &FCTL3
	jz		L5
	adda    #1, R13
	;incda   R13
	dec.w	R14
	jnz		L4

FINISH:
	mov.w	#FWPW, &FCTL1					;disable programming voltage
L6:
	bit		#BUSY, &FCTL3					;check if flash is still busy
	jnz		L6
	mov.w	#FWPW+LOCK, &FCTL3				;lock memory

FLASH_PROGRAMMER_ADDR_END:
	ret

	.end
