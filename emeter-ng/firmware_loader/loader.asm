
;----------------------------------------------------------------------------
;	File: loader.asm
;
;  Getaneh Likasa
;
;--------------------------------------------------------------------------
	.cdecls C, LIST, "msp430f6779.h"

	.define 128, BUFFERSIZE
	.define 0x0000,FIRMADDRLOW
	.define 0x0004, FIRMADDRHIGH
	.define 8, METADATASIZE
	.define 14, SECTIONSIZE
	.define 12, WORDINDEX              ;is used to point to the code size in the current section on the stack
	.define 16, DOUBLEINDEX            ;is used to point to the address of the next instruction to be programmed on the flash
	.define	R4, eeprom_reader	           ;stores the entry address of memory reader routine on the stack
	.define R5,	programmer             ;stores the entry address of flash programmer routine on the stack
	.define R6,	loader             	   ;stores the entry address of loader routine on the stack
	.define R7, TOS                    ;points to top of stack
	.define R8, CODESIZE;  code_len
	.define R9, PROGMPTR ;next_prog_mem_ptr
	.define R10,BACKMPTR ;next_backup_mem_ptr
	.define R11, stack_addr_ptr
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
	;void init_loader()
	.global init_loader
	.text
;this routine is used to load all important modules to RAM, before the firmware loader starts
;4 modules are currently loaded to the RAM
;the external memory driver module is used for reading firmware images from external flash
;the flash programmer module is used to program msp430 flash
;power handler is used to handle the main power cut while firmware upgrading is under progress-this is
;essentially an interrupt service routine vectored from RAM interrupt vector space
;the loader module which is responsible for the overall firmware upgrade process
;finally the init_loader passes control the loader module in the RAM and firmware upgrade starts

init_loader:

	dint							;disable interrupts during firmware updating
	nop
	mov.w	#5A80h,&WDTCTL			;disable watchdog
	;-------------------------------------------------------------------------------------
	;copy to RAM important modules used to carry out firmware upgrade
	;-------------------------------------------------------------------------------------
	mova	#EEPROM_DRIVER_ADDR_END, R11      ;copy the last memory address(ret) of memory driver module
	mov.w	#EEPROM_DRIVER_LEN, R15 		   ;number of instruction words to be copy into RAM
L1:
	push 	@R11						   ; copy instruction word into RAM
	decda	R11
	dec		R15
	jnz		L1
	mov		SP, eeprom_reader			 	   ;save memory driver start address to R8

	;copy flash programmer module to RAM
	mova	#FLASH_PROGRAMMER_ADDR_END, R11
	mov.w	#FLASH_PROGRAMMER_LEN, R15
L2:
	push 	@R11
	decda	R11
	dec		R15
	jnz		L2
	mov.w	SP,programmer			  ;save flash programmer module start address

	;copy the power cut handler module to RAM, this is ISR routine
;	mova	#POWER_HANDLER_ADDR_END, R11
;	mov.w	#POWER_HANDLER_LEN, R15
;L3:
;	push 	@R11
;	decda	R11
;	dec		R15
;	jnz		L3
;	mov.w	#UNMI_VECTOR_ADDRESS, R11   ;points to the the UNMI vector space in the RAM
;	mov.w	SP, 0(R11)                  ;copy power handler module start address to the vector address space


	;load firmware loader module into RAM
	mova	#LOADER_ADDR_END, R11
	mov.w	#LOADER_LEN, R15
L4:
	push 	@R11	; copy function into RAM
	decda	R11
	dec		R15
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
	sub.w	#6, TOS             	  ;reserve space for holding next section fetch address, and total no of sections in the image
	mov.w	TOS, R11    			  ;copy the reserved space into general purpose register
	sub.w	#4,	TOS				  	  ; reserve space for holding next program address in msp flash memory
	mov.w	TOS, PROGMPTR			  ; next_prog_mem_ptr ; copy the start address in a stack
	sub.w   #4, TOS                   ;reserve memory for holding next program address in backup memory
	mov.w	TOS, BACKMPTR             ;next_backup_mem_ptr ;stack_buff_ptr
	sub.w   #BUFFERSIZE, TOS  		  ;allocate 128 bytes of stack buffer for holding machine codes....

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
	;cmp.w	#0,	&start_loader
	;jeq	START_UPDATE
	;mov.b	#0x00, &LCDM30
	;mov.b	#0xFF, &LCDM30
	;copy the firmware metadata address to the stack memory
	mov.w	#FIRMADDRLOW,  0(R11)
	mov.w	#FIRMADDRHIGH, 2(R11)
	;copy firmware image metadata from external memory
	mov.w	TOS, R12                    	   ; copy start of stack buffer to R12 which is used by read_mem routine
	mov.w 	#FIRMADDRLOW, R13  				   ;lower word of firmware start address in flash
	mov.w	#FIRMADDRHIGH, R14 				   ;higher word of firmware start address in flash
	mov.w	#METADATASIZE, R15
	call	eeprom_reader 						   ;this fetches the firmware metadata from external memory

	add.w	#METADATASIZE, 0(R11)    			;update section pointer to point to the next firmware section if applicable
	adc.w	2(R11)
	add.w	#4 , R11							;update stack_addr_ptr to point to number of sections address
	mov.w 	@TOS, 0(R11)						;copy the number of sections to its address,this assumes that
												;the first word in the metadata contains total number of sections in the image

READ_NEXT_SECTION: 								;start reading firmware image section
	mov.w	SP,R11
	sub.w	#DOUBLEINDEX  , R11    				;get next section address from the stack
	mov.w	TOS, R12
	mov.w	0(R11), R13
	mov.w	2(R11), R14
	mov.w	#SECTIONSIZE,  R15
	call	eeprom_reader                     		;initiate memory read to get the next section info
	add.w	#SECTIONSIZE, 0(R11)				;update section pointer to point to the next section, if applicable
	adc.w	2(R11)
	;now copy the machine code address in a program memory
	mov.w   TOS, R11
	mov.w	@R11 +, 0(PROGMPTR)					;buffer section start address in program memory
	mov.w	@R11 +, 2(PROGMPTR)
	mov.w   @R11 +, 0(BACKMPTR)					;buffer machine instruction start address in a backup memory
	mov.w	@R11 +, 2(BACKMPTR)
	mova	@R11, 	CODESIZE					;initialize code size tracker of this section

PROGRAM_SECTION:								;start programming the firmware to flash memory
	mov.w	TOS, R12
	mov.w	0(BACKMPTR), R13					;setup registers for memory read call
	mov.w	2(BACKMPTR), R14
	mova	CODESIZE,	R15						;normally instructions are read in a multiple of 128 bytes
	cmpa	#BUFFERSIZE, R15
	jl		L_1
	mov.w	#BUFFERSIZE, R15
L_1:
	mov.w	R15, R11              ;save bytes to be programmed
	call	eeprom_reader 			  ;initiate memory read
	mov.w	TOS, R12			  ;setup routine calling convention for program_flash
	mova	0(PROGMPTR), R13
	mov.w	R11, R14			  ; copy the size of instruction bytes to be programmed
	cmp.w	#128, R11             ;determine type of flash programming to be initiated. if data size is 128 bytes, block
	jnz		WORD_WRITE		      ;programming is initiated to improve performance, otherwise word programming
	clr.w	R15
	jmp		PROGRAM_FLASH
WORD_WRITE:
	mov.w	#1, R15
PROGRAM_FLASH:
	call	programmer		  	  ;initiate programming flash
	add.w	R11, 0(BACKMPTR) 	  ;update the next backup memory read address from which next firmware image byte is read
	adc.w	2(BACKMPTR)
	add.w	R11, 0(PROGMPTR)	  ;update next program memory write address where the next instruction byte will be programmed
	adc.w	2(PROGMPTR)
	suba	R11,CODESIZE		  ;update number of instruction bytes in the current section already programmed to the flash
	cmpa	#0, CODESIZE
	jnz		PROGRAM_SECTION		  ;go fetch the next instruction bytes in the current section to be programmed
	mov.w   SP, R11
	sub.w	#WORDINDEX, R11		  ;get number of remaining sections in the firmware image
	dec.w	0(R11)
	jnz		READ_NEXT_SECTION	  ;get the next firmware section to be programmed
								  ;the firmware update is successful, map the Interrupt vector spaces back to FLASH,
								  ;and reboot the system.
;	bic.w	#SYSRIVECT, &SYSCTL   ;restore interrupt vectors back to FLASH
	mov.b	#0xA5, &PMMCTL0_H
	bis.w	#PMMSWBOR, &PMMCTL0  ;PMMSWPOR
LOADER_ADDR_END:
	reta

    .end
