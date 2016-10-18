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
//  File: msgs.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: msgs.h,v 1.7 2011/05/26 09:50:41 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//

enum
{
    ATTR_NO_ACCESS = 0,
    ATTR_READ_ONLY = 1,
    ATTR_WRITE_ONLY = 2,
    ATTR_READ_WRITE = 3,
    ATTR_AUTHENTICATED_READ_ONLY = 4,
    ATTR_AUTHENTICATED_WRITE_ONLY = 5,
    ATTR_AUTHENTICATED_READ_WRITE = 6,
};

enum
{
    ATTR_READ = 0x01,
    ATTR_WRITE = 0x02
};

typedef void (*exchange_data_t)(void *data, int direction);
typedef void (*method_t)(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

struct attribute_desc_s
{
    uint8_t id;
    uint16_t access_mode;
    uint8_t type;
    uint8_t *addr;
    exchange_data_t callback;
};

struct method_desc_s
{
    uint8_t id;
    uint16_t access_mode;
    method_t callback;
};

struct object_desc_s
{
    uint8_t assoc;
    uint8_t class_id;
    uint8_t version;
    uint8_t instance_id[6];
    uint8_t attr_elements;
    const struct attribute_desc_s *attrs;
    uint8_t method_elements;
    const struct method_desc_s *methods;
};

struct datetime_s
{
    uint16_t year;
    uint8_t month;
    uint8_t day_of_the_month;
    uint8_t day_of_the_week;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t centi_seconds;
    uint16_t deviation;
    uint8_t status;
};
