;**************************************************************************************
; 			file: power_handler.asm
; 		    Created on: Nov 11, 2016
;   		Author: Getaneh
;***************************************************************************************

;***************************************************************************************
;						POWER MANAGEMENT MODULE
;***************************************************************************************
;routine for detecting and handling power outage during firmware upgrade
;this is essentially auxiliary switch interrupt service routine which is
;served from user non maskable interrupt vector in RAM

	.cdecls C, LIST, "msp430f6779.h"

	.define 465, BATTERY_OK_THRESHOLD

POWER_HANDLER_LEN	 .equ   (POWER_HANDLER_ADDR_END - power_handler + 2)/2

	.global	POWER_HANDLER_ADDR_END
	.global POWER_HANDLER_LEN



	.text

power_handler:
	;mov.b	#0x13, &LCDM30
	bit.w	#AUXSWNMIFG, &AUXIFG
	jz		POWER_HANDLER_ADDR_END
	bic.w	#AUXSWNMIFG, &AUXIFG        ;clear non maskable interrupt flag
	bit.w	#AUX1SWIFG, &AUXIFG
	jz		POWER_HANDLER_ADDR_END
	bic.w	#AUX1SWIFG, &AUXIFG         ;clear auxiliary switched to battery flag
	;if the logic gets here, this indicates that the power autage is detected,
	;and it has switched to auxiliary battery supply.
	;the routine proceeds by first monitoring the voltage level on the battery,
	;and if the voltage is high enough to drive the external flash memory,
	;firmware upgrade routine will be restarted.
	bic.b	#BIT2, &P3OUT 					;power up external flash from battery
	mov.w	#20000, R12						;wait for about 10msecs until flash resets
L1:
	dec	R12
	jnz	L1
	;now monitor battery level
	mov.w	#REFON+REFVSEL0+REFTCOFF+REFMSTR, &REFCTL0  ;enable reference generator
	bis.w	#AUXADCSEL_1+AUXADC, &AUXADCCTL 			;enable AUXVCC1 channel for sampling
	bic.w	#ADC10ENC, &ADC10CTL0 						;disable adc module for configuration
	bic.w	#ADC10IFG0, &ADC10IFG 						; clear flag incase it is pending
	mov.w	#ADC10SHT_6+ADC10ON, &ADC10CTL0         	;sample and hold time 43usec(128ADC clock)*/
	mov.w	#ADC10SSEL_2+ADC10SHP+ADC10DIV_7, &ADC10CTL1;ADC10CONSEQ_2 ;divide by 8
	bis.w	#ADC10SREF_1+ADC10INCH_12, &ADC10MCTL0  	;select analog channel 12 for battery
	bis.w	#ADC10ENC + ADC10SC, &ADC10CTL0 			;start conversion
L2:
	bit.w	#ADC10IFG0, &ADC10IFG
	jz		L2
	bic.w	#AUXADC, &AUXADCCTL 					;disconnect AUXVCC1 channel from adc
	cmp.w   #BATTERY_OK_THRESHOLD, &ADC10MEM0
	jl		BATTERY_FAILED					;battery level is low and firmware upgrade cannot progress
	;if it gets here, it means that the battery level is okay and the upgrade can restart
	;update the return address on the stack to point to the start of loader disable
	bic.w	#AUXSWNMIE, &AUXIE  			;disable auxiliary switched non maskable interrupt to avoid frequent switching
	mov.w	R6, 2(SP)  						;update return address to the start of the loader,this is preemptive behaviour
	jmp 	POWER_HANDLER_ADDR_END
BATTERY_FAILED:
	bic.w	#SYSRIVECT, &SYSCTL          			;restore interrupt vectors back to FLASH
	bis.w	#GIE+CPUOFF+OSCOFF+SCG1+SCG0, 0(SP)		;enter LPM4 after exit
POWER_HANDLER_ADDR_END:
	reti

	.end
