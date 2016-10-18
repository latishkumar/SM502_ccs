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
//  File: packing.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: packing.c,v 1.3 2010/05/13 07:47:37 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//

//#include <inttypes.h>
#include <stdint.h>

union uint16_u
{
    uint16_t value;
    uint8_t buf[2];
};

union uint32_u
{
    uint32_t value;
    uint8_t buf[4];
};

union uint64_u
{
    uint64_t value;
    uint8_t buf[8];
};

uint16_t pack16(const uint8_t msg[])
{
    union uint16_u v;

    v.buf[0] = msg[1];
    v.buf[1] = msg[0];
    return v.value;
}
/*- End of function --------------------------------------------------------*/

uint32_t pack32(const uint8_t msg[])
{
    int i;
    union uint32_u v;
    
    for (i = 0;  i < 4;  i++)
        v.buf[i] = msg[3 - i];
    return v.value;
}
/*- End of function --------------------------------------------------------*/

uint64_t pack64(const uint8_t msg[])
{
    int i;
    union uint64_u v;
    
    for (i = 0;  i < 8;  i++)
        v.buf[i] = msg[7 - i];
    return v.value;
}
/*- End of function --------------------------------------------------------*/

void unpack16(uint8_t msg[], uint16_t value)
{
    union uint16_u v;

    v.value = value;
    msg[1] = v.buf[0];
    msg[0] = v.buf[1];
}
/*- End of function --------------------------------------------------------*/

void unpack32(uint8_t msg[], uint32_t value)
{
    int i;
    union uint32_u v;
    
    v.value = value;
    for (i = 0;  i < 4;  i++)
        msg[3 - i] = v.buf[i];
}
/*- End of function --------------------------------------------------------*/

void unpack64(uint8_t msg[], uint64_t value)
{
    int i;
    union uint64_u v;
    
    v.value = value;
    for (i = 0;  i < 8;  i++)
        msg[7 - i] = v.buf[i];
}
/*- End of function --------------------------------------------------------*/
/*- End of file ------------------------------------------------------------*/
