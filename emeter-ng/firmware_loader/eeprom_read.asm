;**************************************************************************************
; 			file: eeprom_driver.asm
; 		    Created on: Nov 11, 2016
;   		Author: Eliab
;***************************************************************************************

;***************************************************************************************
;						EXTERNAL EEPROM DRIVER MODULE
;***************************************************************************************
; this file contains an external eeprom memory driver which is used to store firmware image
; the assembly routine simply reads the number of bytes requested starting from the start
; address supplied, and returns the result in the buffer location provided
; the equivalent c function prototype would be

; void read_eeprom(uint8_t* buff, uint32_t start_address, uint16_t size)
	.cdecls C, LIST, "msp430f6736.h"
	;.asg 	R12, buff
	;.asg 	R13, start_address
	;.asg 	R14, start_address_ms
	;.asg	R15, size
EEPROM_DRIVER_LEN		 .equ	(EEPROM_DRIVER_ADDR_END - read_eeprom+2)/2  ;memory driver instruction word length

	.global	EEPROM_DRIVER_ADDR_END
	.global EEPROM_DRIVER_LEN
	.global read_eeprom
	.text

read_eeprom:
;Set slave address
	bis.w   #80, R14
	mov.b   R14, &UCB0I2CSA
L1:	bit.b   #UCBBUSY,  &UCB0STATW 		;poll until I2C module finished all operations
	jne		L1
	bis.w	#16, &UCB0CTLW0			;set write bit
	bis.w	#2,  &UCB0CTLW0			;start condition generation
L2: bit.w	#2,  &UCB0CTLW0			;check if start condition is transmitted
	jne 	L2
L3: bit.w   #2,  &UCB0IFG
    jeq 	L3
    bic.w   #2,  &UCB0IFG
;Write Data Address
	mov	R13, R14
	swpb	R14
	mov.b	R14, R14
	mov.w	R14, &UCB0TXBUF
L4: bit.w   #2,  &UCB0IFG
	jeq		L4
	bic.w   #2,  &UCB0IFG
	mov.w	R13, &UCB0TXBUF 		;send the second high byte of the address
L5: bit.w   #2,  &UCB0IFG
    jeq 	L5
    bic.w   #2,  &UCB0IFG
;Read Data byte
    bic.w   #16, &UCB0CTLW0
    bis.w   #2,  &UCB0CTLW0
L6: bit.w	#2,  &UCB0CTLW0			;check if start condition is transmitted
	jne  	L6
L7: bit.w   #1,  &UCB0IFG
    jeq 	L7
    mov.b	&UCB0RXBUF, 0(R12) 		;store read byte in the memory
	inc.w	R12				   		;point to the next memory byte address
	dec.w	R15
	jnz     L7
	bis.w   #4,	 &UCB0CTLW0			;check if all required bytes are red
L8: bit.w   #4,  &UCB0CTLW0
	jne 	L8
	mov.b   &UCB0RXBUF, R13
EEPROM_DRIVER_ADDR_END:
    ret

    .end
