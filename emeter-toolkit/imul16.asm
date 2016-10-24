//--------------------------------------------------------------------------
//
//  Software for MSP430 based e-meters.
//
//  THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
//  REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY,
//  INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
//  FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//  COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE.
//  TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET
//  POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY
//  INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR
//  YOUR USE OF THE PROGRAM.
//
//  IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
//  CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY
//  THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT
//  OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM.
//  EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF
//  REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS
//  OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF
//  USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S
//  AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF
//  YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS
//  (U.S.$500).
//
//  Unless otherwise stated, the Program written and copyrighted
//  by Texas Instruments is distributed as "freeware".  You may,
//  only under TI's copyright in the Program, use and modify the
//  Program without any charge or restriction.  You may
//  distribute to third parties, provided that you transfer a
//  copy of this license to the third party and the third party
//  agrees to these terms by its first use of the Program. You
//  must reproduce the copyright notice and any other legend of
//  ownership on each copy or partial copy, of the Program.
//
//  You acknowledge and agree that the Program contains
//  copyrighted material, trade secrets and other TI proprietary
//  information and is protected by copyright laws,
//  international copyright treaties, and trade secret laws, as
//  well as other intellectual property laws.  To protect TI's
//  rights in the Program, you agree not to decompile, reverse
//  engineer, disassemble or otherwise translate any object code
//  versions of the Program to a human-readable form.  You agree
//  that in no event will you alter, remove or destroy any
//  copyright notice included in the Program.  TI reserves all
//  rights not specifically granted under this license. Except
//  as specifically provided herein, nothing in this agreement
//  shall be construed as conferring by implication, estoppel,
//  or otherwise, upon you, any license or other right under any
//  TI patents, copyrights or trade secrets.
//
//  You may not use the Program in non-TI devices.
//
//  File: imul16.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: imul16.s43,v 1.3 2009/03/12 03:56:37 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include "io.h"

//This routine implements a 16x16->32 2's complement multiplier. If a hardware
//multiplier is available it is used. If no hardware multiplier is available,
//Booth's algorithm is used to directly implement a signed multiplier in software.
//C does not normally express a 16x16->32 operation, and the speedup of achieving
//a 32-bit answer by a full 32x32 multiply is substantial.

#if !defined(__IAR_SYSTEMS_ASM__)  ||  !(((__TID__ >> 8) & 0x7f) == 43)
#error This file is compatible with the IAR MSP430 assembler.
#endif

#if __VER__ >= 400
#define x               R12
#define y               R13
#define ret_hi          R13
#define ret_lo          R12
#else
#define x               R12
#define y               R14
#define ret_hi          R13
#define ret_lo          R12
#endif

;int32_t imul16(register int16_t x, register int16_t y)
    public imul16

    RSEG CODE
imul16
#if defined(__MSP430_HAS_MPY__)  ||  defined(__MSP430_HAS_MPY32__)
    push.w  SR
    dint
    mov     x,&MPYS_
    mov     y,&OP2_
    mov     &RESHI_,ret_hi
    mov     &RESLO_,ret_lo
    pop.w   SR
#if defined(__MSP430_HAS_MSP430X_CPU__) || defined(__MSP430_HAS_MSP430XV2_CPU__)
    reta
#else
    ret
#endif
#else
    push.w  R10
    push.w  R15
    mov     x,R15
    clr     R13
    mov     R13,x

    mov     R13,R10
    tst     R15
    jge     booth_2
    mov     #-1,R10
    jmp     booth_2

booth_6
    add     R15,x
    addc    R10,R13
booth_1
    rla     R15
    rlc     R10
booth_2
    rra     y
    jc      booth_5
    jne     booth_1
    jmp     booth_4

booth_5
    sub     R15,x
    subc    R10,R13
booth_3
    rla     R15
    rlc     R10
    rra     y
    jnc     booth_6
    cmp     #0xFFFF,y
    jne     booth_3

booth_4
    pop.w   R15
    pop.w   R10
#if defined(__MSP430_HAS_MSP430X_CPU__) || defined(__MSP430_HAS_MSP430XV2_CPU__)
    reta
#else
    ret
#endif
#endif
    end
