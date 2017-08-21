
;----------------------------------------------------------------------------
;	File: loader.asm
;
;  Getaneh Likasa
;
;--------------------------------------------------------------------------
	.cdecls C, LIST, "msp430f6736.h"

	.define 128, BUFFERSIZE
	.define 0,	IMAGE_INFO_ADDR_OFFSET_L
	.define 2,  IMAGE_INFO_ADDR_OFFSET_H
	.define 4,  INST_START_ADDR_OFFSET_L
	.define 6,  INST_START_ADDR_OFFSET_H
	.define 8,  PROG_ADDR_OFFSET_L
	.define 10, PROG_ADDR_OFFSET_H
	.define 12, INST_BACKUP_ADDR_OFFSET_L
	.define 14, INST_BACKUP_ADDR_OFFSET_H
	.define 12, IMAGE_SECTION_SIZE

	.define	R4, eeprom_reader	           ;stores the entry address of memory reader routine on the stack
	.define R5,	programmer             ;stores the entry address of flash programmer routine on the stack
	.define R6,	loader             	   ;stores the entry address of loader routine on the stack
	.define R7, TOS                    ;points to top of stack
	.define R8, ADDR_PTR
	.define R9, SEC_COUNT_REG
	.define R10, CODE_COUNT_REG
	.define R12, BUF_PTR

	.define (LOADER_ADDR_END - firmware_loader + 2)/2, LOADER_LEN	   ;
	.define 0x00040000, FIRMWARE_ADDRESS_START
	.define 0x9BFA, UNMI_VECTOR_ADDRESS

	;instruction word lengths for each modules loaded into RAM
	.global EEPROM_DRIVER_ADDR_END
	.global EEPROM_DRIVER_LEN
	.global FLASH_PROGRAMMER_ADDR_END
	.global FLASH_PROGRAMMER_LEN
	.global POWER_HANDLER_ADDR_END
	.global POWER_HANDLER_LEN


;-----------------------------------------------------------------------------------------------------------------------------
;						LOADER INITIALIZER
;-----------------------------------------------------------------------------------------------------------------------------
	;void start_loader(uint32_t image_metadata_addr, uint32_t machine_code_addr)
	.global start_loader
	.text
;this routine is used to load all important modules to RAM, before the firmware loader starts
;4 modules are currently loaded to the RAM
;the external memory driver module is used for reading firmware images from external flash
;the flash programmer module is used to program msp430 flash
;power handler is used to handle the main power cut while firmware upgrading is under progress-this is
;essentially an interrupt service routine vectored from RAM interrupt vector space
;the loader module which is responsible for the overall firmware upgrade process
;finally the init_loader passes control the loader module in the RAM and firmware upgrade starts

start_loader:

	dint							;disable interrupts during firmware updating
	nop
	mov.w	#5A80h,&WDTCTL			;disable watchdog
	;-------------------------------------------------------------------------------------
	;copy to RAM important modules used to carry out firmware upgrade
	;-------------------------------------------------------------------------------------
	mova	#EEPROM_DRIVER_ADDR_END, R7      ;copy the last memory address(ret) of memory driver module
	mov.w	#EEPROM_DRIVER_LEN, R8 		   ;number of instruction words to copy into RAM
L1:
	push 	@R7						   ; copy instruction word into RAM
	decda	R7
	dec		R8
	jnz		L1
	mov		SP, eeprom_reader			 	   ;save memory driver start address to R8

	;copy flash programmer module to RAM
	mova	#FLASH_PROGRAMMER_ADDR_END, R7
	mov.w	#FLASH_PROGRAMMER_LEN, R8
L2:
	push 	@R7
	decda	R7
	dec		R8
	jnz		L2
	mov.w	SP,programmer			  ;save flash programmer module start address

	;copy the power cut handler module to RAM, this is ISR routine
;	mova	#POWER_HANDLER_ADDR_END, R7
;	mov.w	#POWER_HANDLER_LEN, R8
;L3:
;	push 	@R7
;	decda	R7
;	dec		R8
;	jnz		L3
;	mov.w	#UNMI_VECTOR_ADDRESS, R7   ;points to the the UNMI vector space in the RAM
;	mov.w	SP, 0(R7)                  ;copy power handler module start address to the vector address space


	;load firmware loader module into RAM
	mova	#LOADER_ADDR_END, R7
	mov.w	#LOADER_LEN, R8
L4:
	push 	@R7	; copy function into RAM
	decda	R7
	dec		R8
	jnz		L4
	mov.w	SP, loader
	calla 	loader				 ; call firmware loader
	;add		#2*MEM_READ_LENGTH,SP    ;Stack housekeeping
	;add		#2*FLASH_WRITE_LENGTH,SP
	;add		#2*LOADER_LENGTH,SP
	reta

;*********************************************************************************************
;				   FIRMWARE LOADER MODULE
;*********************************************************************************************
	;.text
firmware_loader:
	;allocate memory on a stack. this includes memory space for function return addresses,
	;memory space to hold total number of sections in the firmware image
	;memory space to hold the address of the next firmware section metadata to fetch
	;memory space to hold the address of the next machine code to be fetched from backup memory(flash)
	;memory space to hold the address of the next machine code byte to be programmed in program memory(msp flash)
	;allocate stack buffer, currently the loader needs about 128 bytes of buffer
	;used to transfer data between the flash memory and program memory(msp430 flash)
	;this size is choosen optimally considering the programming voltage applied to
	;128 block segments
	;first thing the loader does is get the total firmware image metadata from external memory
	;after erasing the entire program main memory

	mov.w 	SP, TOS
	sub.w	#10, TOS                  ;reserve space for function return addresses
	sub.w	#16, TOS    			  ;reserve space for addresses, offset0-metadata,offset1-code-addr,offset2-prog-addr
	mov.w	TOS, ADDR_PTR    	;R7(address ptr) holds the start address of allocated space
	sub.w   #BUFFERSIZE, TOS  	  ;allocate 128 bytes of stack buffer for holding machine codes....
	mov.w	R12, IMAGE_INFO_ADDR_OFFSET_L(ADDR_PTR);copy low word of metadata address in R12 into the reserved space
	mov.w	R13, IMAGE_INFO_ADDR_OFFSET_H(ADDR_PTR)                ;copy high word of image metadata to stack
	mov.w 	R14, INST_START_ADDR_OFFSET_L(ADDR_PTR)                ;copy low word of code address in flash memory
	mov.w	R15, INST_START_ADDR_OFFSET_H(ADDR_PTR)				  ;copy high word of code address to stack
;	bis.w	#SYSRIVECT, &SYSCTL       ; copy interrupt vectors to RAM,
									  ;this will help us detect power loss during firmware upgrade
									  ;this assumes that the power switching as well as,
									  ;auxiliary switch non maskable interrupt is enabled beforehand

	;---------------------------------------------------------------------------------------------------------------
	;							ERASE MAIN MEMORY
	;---------------------------------------------------------------------------------------------------------------

BUSY_1:
	bit.w	#BUSY,&FCTL3
	jnz		BUSY_1
	mov.w	#FWPW,&FCTL3    			; Clear LOCK
    mov.w	#FWPW+ERASE+MERAS, &FCTL1   ; Enable mass erase
	clr.w	&0xc000    					; Dummy write
BUSY_2:
	bit.w	#BUSY,&FCTL3				;wait until flash erasing is finished
	jnz		BUSY_2
	mov.w	#FWPW+LOCK,&FCTL3			;lock memory
    ;--------------------------------------------------------------------------------------------------------------------
START_UPDATE:

	;get total number of sections in the firmware image
	mov.w	TOS, BUF_PTR
	mov.w 	IMAGE_INFO_ADDR_OFFSET_L(ADDR_PTR), R13  ;lower word of image metadata address in flash
	mov.w	IMAGE_INFO_ADDR_OFFSET_H(ADDR_PTR), R14  ;high word of image metadata address in flash
	mov.w	#4, R15                            ;number of sections is 4 bytes in length
	call	eeprom_reader 						   ;this fetches the firmware metadata from external memory

	add.w	#4, IMAGE_INFO_ADDR_OFFSET_L(ADDR_PTR)    			;update section pointer to point to the next firmware section if applicable
	adc.w	IMAGE_INFO_ADDR_OFFSET_H(ADDR_PTR)
	;save number of image sections in to the section count register
	mov.w	@TOS, SEC_COUNT_REG

READ_NEXT_SECTION: 								;start reading firmware image section
	;setup arguments for eeprom_reader routine
	mov.w	TOS, BUF_PTR
	mov.w	IMAGE_INFO_ADDR_OFFSET_L(ADDR_PTR), R13
	mov.w	IMAGE_INFO_ADDR_OFFSET_H(ADDR_PTR), R14
	mov.w	#IMAGE_SECTION_SIZE,  R15
	call	eeprom_reader
	;update image metadata address in stack so that it will point
	;to the next section for later call
	add.w	#IMAGE_SECTION_SIZE, IMAGE_INFO_ADDR_OFFSET_L(ADDR_PTR)
	adc.w	IMAGE_INFO_ADDR_OFFSET_H(ADDR_PTR)
	;process image information we just read from external flash memory
	;this includes, the first 4 bytes are instruction program address
	;in the controller's program memory. so we copy this into the reserved
	;stack address for later processing. the 2nd 4 bytes are machine address offset
	;information for this section, this number will be added to the code address
	;that is passed to this loader to calculate the effective address of instruction
	;in the memory storage.
	;the last 4bytes contain number of machine instructions in this section
	;now copy the machine code address in a program memory

	;mov instruction start address in stack for offset addition
	mov.w	INST_START_ADDR_OFFSET_L(ADDR_PTR), R13
	mov.w	INST_START_ADDR_OFFSET_H(ADDR_PTR), R14
	mov.w   TOS, BUF_PTR
	mov.w	@BUF_PTR +, PROG_ADDR_OFFSET_L(ADDR_PTR) ;buffer section start address in program memory
	mov.w	@BUF_PTR +, PROG_ADDR_OFFSET_H(ADDR_PTR)
	add.w   @BUF_PTR +, R13					;buffer machine instruction start address in a backup memory
	addc.w	@BUF_PTR +, R14
	mova	@BUF_PTR, 	CODE_COUNT_REG					;initialize code size tracker of this section
	;move effective address of instrucion in a backup memory to the stack
	mov.w	R13, INST_BACKUP_ADDR_OFFSET_L(ADDR_PTR)
	mov.w	R14, INST_BACKUP_ADDR_OFFSET_H(ADDR_PTR)

PROGRAM_SECTION:								;start programming the firmware to flash memory
	;setup argument parameters to read machine instructions
	;from exteral memory
	mov.w	TOS, BUF_PTR
	mov.w	INST_BACKUP_ADDR_OFFSET_L(ADDR_PTR), R13	;setup registers for memory read call
	mov.w	INST_BACKUP_ADDR_OFFSET_H(ADDR_PTR), R14
	mova	CODE_COUNT_REG,	R15						;normally instructions are read in a multiple of 128 bytes
	cmpa	#BUFFERSIZE, R15
	jl		L_1
	mov.w	#BUFFERSIZE, R15
L_1:
	mov.w	R15, R11              ;save bytes to be programmed
	call	eeprom_reader 			  ;initiate memory read
	mov.w	TOS, BUF_PTR		  ;setup routine calling convention for program_flash
	mova	PROG_ADDR_OFFSET_L(ADDR_PTR), R13
	mov.w	R11, R14			  ; copy the size of instruction bytes to be programmed
	cmp.w	#128, R11             ;determine type of flash programming to be initiated. if data size is 128 bytes, block
	jnz		WORD_WRITE		      ;programming is initiated to improve performance, otherwise word programming
	clr.w	R15
	jmp		PROGRAM_FLASH
WORD_WRITE:
	mov.w	#1, R15
PROGRAM_FLASH:
	call	programmer		  	  ;initiate programming flash
	;update the next backup memory read address from which next firmware image byte is read
	add.w	R11, INST_BACKUP_ADDR_OFFSET_L(ADDR_PTR)
	adc.w	INST_BACKUP_ADDR_OFFSET_H(ADDR_PTR)
	;update next program memory write address where the next instruction byte will be programmed
	add.w	R11, PROG_ADDR_OFFSET_L(ADDR_PTR)
	adc.w	PROG_ADDR_OFFSET_H(ADDR_PTR)
	;update number of instruction bytes in the current section already programmed to the flash
	suba	R11,CODE_COUNT_REG
	cmpa	#0, CODE_COUNT_REG
	jnz		PROGRAM_SECTION		  ;go fetch the next instruction bytes in the current section to be programmed
	dec.w	SEC_COUNT_REG
	jnz		READ_NEXT_SECTION	  ;get the next firmware section to be programmed
								  ;the firmware update is successful, map the Interrupt vector spaces back to FLASH,
								  ;and reboot the system.
	;bic.w	#SYSRIVECT, &SYSCTL   ;restore interrupt vectors back to FLASH
	mov.b	#0xA5, &PMMCTL0_H
	bis.w	#PMMSWBOR, &PMMCTL0
LOADER_ADDR_END:
	reta

    .end
