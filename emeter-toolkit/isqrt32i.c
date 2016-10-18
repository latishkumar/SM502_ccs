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
//  File: isqrt32i.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: isqrt32i.c,v 1.3 2007/08/07 05:53:46 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

int16_t isqrt32i(register int32_t h)
{
    register int32_t x;
    register int32_t y;
    register int16_t i;

    //The body of this routine only calculates the square
    //root of numbers up to 30-bit. It blows up if x exceeds
    //0x3FFFFFFF, so we protect against this at the
    //beginning. It makes no real difference to the results,
    //since even the square root of 0x3FFF0001 hits the
    //maximum positive integer.
    if (h > 0x3FFF0001)
        return 0x7FFF;
    //The answer is calculated as a 32 bit value, where the last
    //16 bits are fractional. It is then half bit rounded to a
    //16 bit integer.
#if defined(__GNUC__)  &&  defined(__MSP430__)
    x = 0x80000000;
    y = 0;
    i = 32;
    __asm__ (
        "1: \n"
        " setc \n"
        " rlc   %A[x] \n"
        " rlc   %B[x] \n"
        " sub   %A[x],%A[y] \n"
        " subc  %B[x],%B[y] \n"
        " jhs   2f \n"
        " add   %A[x],%A[y] \n"
        " addc  %B[x],%B[y] \n"
        " sub   #2,%A[x] \n"
        "2: \n"
        " inc   %A[x] \n"
        " rla   %A[h] \n"
        " rlc   %B[h] \n"
        " rlc   %A[y] \n"
        " rlc   %B[y] \n"
        " rla   %A[h] \n"
        " rlc   %B[h] \n"
        " rlc   %A[x] \n"
        " rlc   %B[x] \n"
        " dec   %[i] \n"
        " jne   1b \n"
        : [x] "+r"(x), [y] "+r"(y)
        : [h] "r"(h), [i] "r"(i));
#elif defined(__AQCOMPILER__)
    x =
    y = 0;
    i = 32;
    /$
sqrt10i
        setc
        rlc     @x.1
        rlc     @x.0
        sub     @x.1,@y.1
        subc    @x.0,@y.0
        jhs     sqrt12i
        add     @x.1,@y.1
        addc    @x.0,@y.0
        sub     #2,@x.1
sqrt12i
        inc     @x.1
        rla     @h.1
        rlc     @h.0
        rlc     @y.1
        rlc     @y.0
        rla     @h.1
        rlc     @h.0
        rlc     @y.1
        rlc     @y.0
        dec     @i
        jne     sqrt10i
    $/
#else
    x =
    y = 0;
    for (i = 0;  i < 32;  i++)
    {
        x = (x << 1) | 1;
        if (y < x)
            x -= 2;
        else
            y -= x;
        x++;
        y <<= 1;
        if ((h & 0x80000000))
            y |= 1;
        h <<= 1;
        y <<= 1;
        if ((h & 0x80000000))
            y |= 1;
        h <<= 1;
    }
#endif
    //x now contains the answer, with a fractional point
    //between bits 15 and 16. Now we do half bit rouding,
    //and return an integer result.
    return  ((x + 0x8000L) >> 16);
}
