





	.cdecls C, LIST, "msp430f6779.h"

	;uint64_t mul_32_32(register uint32_t mul1, register mul2 x=mul2)
    .global mul_32_32

    .text

mul_32_32:

	PUSH    SR
	DINT			; Disable interrupts
   	NOP				; Account for latency
    MOV.W	R12, &MPY32L		; Load operand 1 into multiplier
    MOV.W	R13, &MPY32H
    MOV.W	R14, &OP2L
    MOV.W   R15, &OP2H
    NOP
    MOV.W   &RES0, R12
    MOV.W   &RES1, R13
    MOV.W   &RES2, R14
    MOV.W   &RES3, R15
	POP.W  SR
	NOP                             ; CPU19 Compatibility
    RETA
 	.end
