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
//  File: div_sh48.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: div_sh48.c,v 1.6 2007/08/07 05:53:42 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit.h"

int32_t div_sh48(register int16_t x[3], int sh, register int16_t y)
{
#if !(defined(__GNUC__)  &&  defined(__MSP430__))  &&  !defined(__AQCOMPILER__)
    int64_t tmp;
#endif

    /* Preshift a 48 bit integer upwards by a specified amount. Then divide
       a 16 bit integer into the shifted 48 bit one. Expect the answer to be
       no greater than 32 bits, so return the answer as a 32 bit integer.
       A somewhat domain specific divide operation, but pretty useful when
       handling dot products. */
    int32_t x1;
    int32_t z;
    register int16_t xx0;
    register int16_t xx1;
    register int16_t xx2;
    
    /* Avoid any divide by zero trouble */
    if (y == 0)
        return 0;
    xx0 = x[0];
    xx1 = x[1];
    xx2 = x[2];

    if (sh > 0)
    {
        while (sh--)
        {
#if defined(__GNUC__)  &&  defined(__MSP430__)
            __asm__ __volatile__ (
                " rla %[xx0] \n"
                " rlc %[xx1] \n"
                " rlc %[xx2] \n"
                : [xx0] "+r"(xx0), [xx1] "+r"(xx1), [xx2] "+r"(xx2)
                : );
#elif defined(__AQCOMPILER__)
            /$
                rla @xx0
                rlc @xx1
                rlc @xx2
            $/
#else
            tmp = (uint16_t) xx2;
            tmp <<= 16;
            tmp |= (uint16_t) xx1;
            tmp <<= 16;
            tmp |= (uint16_t) xx0;
            tmp <<= 1;
            xx2 = (tmp >> 32) & 0xFFFF;
            xx1 = (tmp >> 16) & 0xFFFF;
            xx0 = tmp & 0xFFFF;
#endif
        }
    }
    else
    {
        while (sh++)
        {
#if defined(__GNUC__)  &&  defined(__MSP430__)
            __asm__ __volatile__ (
                " rra %[xx2] \n"
                " rrc %[xx1] \n"
                " rrc %[xx0] \n"
                : [xx0] "+r"(xx0), [xx1] "+r"(xx1), [xx2] "+r"(xx2)
                : );
#elif defined(__AQCOMPILER__)
            /$
                rra @xx2
                rrc @xx1
                rrc @xx0
            $/
#else
            tmp = (uint16_t) xx2;
            tmp <<= 16;
            tmp |= (uint16_t) xx1;
            tmp <<= 16;
            tmp |= (uint16_t) xx0;
            /* Make sure sign extension works */
            tmp <<= 16;
            tmp >>= 17;
            xx2 = (tmp >> 32) & 0xFFFF;
            xx1 = (tmp >> 16) & 0xFFFF;
            xx0 = tmp & 0xFFFF;
#endif
        }
    }
    x1 = xx2%y;
    x1 <<= 16;
    x1 |= (uint16_t) xx1;
    z = x1/y;
    x1 = x1%y;
    x1 <<= 16;
    x1 |= (uint16_t) xx0;
    z = (z << 16) + x1/y;
    return z;
}
