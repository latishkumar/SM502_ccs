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
//  File: config.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: config.c,v 1.22 2013/01/09 04:28:18 a0393626 Exp $
//
//--------------------------------------------------------------------------
//
//#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core/cosem.h" /*PATH CORRECTION*/
#include "core/obis.h"/*PATH CORRECTION*/
#include "core/iec62056_46_link.h"/*PATH CORRECTION*/
#include "core/msgs.h"/*PATH CORRECTION*/
#include "config.h"
#include "../SM502/Logger.h"
#include "../emeter-rtc.h"
#include "../SM502/utilities.h"
#include "../SM502/Status.h"
#include "../SM502/self_diagnosis.h"

#define FALSE 0
#define TRUE (!FALSE)

#define STUFF_DATA 0x80



#define DATA_OBJECTS_START 18
#define REGISTER_OBJECTS_START DATA_OBJECTS_START+32
#define PROFILE_GENERIC_OBJECTS_START REGISTER_OBJECTS_START + 36


/* Define the access patterns we will use for the various levels of access. */
/* Read only by anyone - e.g. for things like model name */

/* Basically , each field is 3 bits, but we want to handle 3 levels of association. This can be
   fitted into a single byte, because the public access association cannot perform anything but
   basic read operations. 2 bits are enough for this association, and 3 bits are available for
   each of the protected associations. */

#define ACCESS_PC___MR___US___MS__   (                                                  (0              ))
#define ACCESS_PC___MR___USR__MS__   (                                                  (ATTR_READ_ONLY << 3))
#define ACCESS_PC___MRR__USR__MS__   (                         (ATTR_READ_ONLY  << 6) | (ATTR_READ_ONLY << 3))
#define ACCESS_PCR__MRR__USR__MS__   ((ATTR_READ_ONLY  << 9) | (ATTR_READ_ONLY  << 6) | (ATTR_READ_ONLY << 3))
#define ACCESS_PC___MRR__USRW_MS__   (                         (ATTR_READ_ONLY  << 6) | (ATTR_READ_WRITE << 3))
#define ACCESS_PCR__MRR__USRW_MS__   ((ATTR_READ_ONLY  << 9) | (ATTR_READ_ONLY  << 6) | (ATTR_READ_WRITE << 3))
#define ACCESS_PCR__MRRW_USRW_MS__   ((ATTR_READ_ONLY  << 9) | (ATTR_READ_WRITE  << 6) | (ATTR_READ_WRITE<< 3))
#define ACCESS_PC___MRRW_USRW_MS__   (                         (ATTR_READ_WRITE  << 6) | (ATTR_READ_WRITE<< 3))
#define ACCESS_PCR__MR___US___MS__   ((ATTR_READ_ONLY  << 9)                                             )
#define ACCESS_PC___MRR__US___MS__   (                         (ATTR_READ_ONLY  << 6)                    )
#define ACCESS_PC___MR___USRW_MS__   (                                                (ATTR_READ_WRITE << 3))

#define ACCESS_PC___MR___US___MSR_   (                                                                        (ATTR_READ_ONLY ))          
#define ACCESS_PC___MR___US___MSRW   (                                                                        (ATTR_READ_WRITE ))          
#define ACCESS_PC___MR___USR__MSR_   (                                                  (ATTR_READ_ONLY << 3) | (ATTR_READ_ONLY ) )
#define ACCESS_PCR__MRR__USR__MSR_   ((ATTR_READ_ONLY << 9) |(ATTR_READ_ONLY << 6) |    (ATTR_READ_ONLY << 3) | (ATTR_READ_ONLY ) )
#define ACCESS_PC___MR___USRW_MSRW    (                                                 (ATTR_READ_WRITE << 3) | (ATTR_READ_WRITE ) )
#define ACCESS_PCR__MRR__USRW_MSRW    ((ATTR_READ_ONLY << 9)|(ATTR_READ_ONLY << 6) |    (ATTR_READ_WRITE << 3) | (ATTR_READ_WRITE ) )
        
#define ACCESS_PC___MR___US__   ACCESS_PC___MR___US___MS__
#define ACCESS_PC___MR___USR_   ACCESS_PC___MR___USR__MS__
#define ACCESS_PC___MRR__USR_   ACCESS_PC___MRR__USR__MS__
#define ACCESS_PCR__MRR__USR_   ACCESS_PCR__MRR__USR__MS__
#define ACCESS_PC___MRR__USRW   ACCESS_PC___MRR__USRW_MS__
#define ACCESS_PCR__MRR__USRW   ACCESS_PCR__MRR__USRW_MS__
#define ACCESS_PCR__MRRW_USRW   ACCESS_PCR__MRRW_USRW_MS__
#define ACCESS_PC___MRRW_USRW   ACCESS_PC___MRRW_USRW_MS__
#define ACCESS_PCR__MR___US__   ACCESS_PCR__MR___US___MS__
#define ACCESS_PC___MRR__US__   ACCESS_PC___MRR__US___MS__
#define ACCESS_PC___MR___USRW   ACCESS_PC___MRR__USRW_MS__


#define ASSOC_PC 0x01 
#define ASSOC_MR 0x02
#define ASSOC_US 0x04
#define ASSOC_MS 0x08

#define ASSOC_PC_MR_US_MS     ASSOC_PC+ASSOC_MR+ASSOC_US+ASSOC_MS
#define ASSOC_PC_MR_US        ASSOC_PC+ASSOC_MR+ASSOC_US
#define ASSOC_MR_US           ASSOC_MR+ASSOC_US
#define ASSOC_US_MS           ASSOC_US+ASSOC_MS

struct scalar_unit_s
{
    int16_t exp;
    uint8_t unit;
};

const int reject_ln_context = FALSE;
const int reject_sn_context = TRUE;

uint8_t time_string[] =
{
//     12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  12,INJECT16(2010), MARCH, 20, TUESDAY, 15, 30, 45, 0xFF, INJECT16(120), 0x00,
};

const uint8_t acceptable_conformance[4][3] =
{
#if defined(IEC62056_SUPPORT_RESERVED0_A0)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED1_A0)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED2_A0)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_READ_A0)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_WRITE_A0)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_UNCONFIRMEDWRITE_A0)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED3_A0)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED4_A0)
    0x01 |
#endif
    0, 
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET_A0)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED_A0)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET_A0)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHGET_A0)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHSET_A0)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION_A0)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_MULTIPLEREFERENCES_A0)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_INFORMATIONREPORT_A0)
    0x01 |
#endif
    0,
#if defined(IEC62056_SUPPORT_RESERVED5_A0)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED6_A0)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_PARAMETERIZEDACCESS_A0)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_GET_A0)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_SET_A0)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_SELECTIVEACCESS_A0)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_EVENTNOTIFICATION_A0)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_ACTION_A0)
    0x01 |
#endif
    0,
#if defined(IEC62056_SUPPORT_RESERVED0_A1)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED1_A1)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED2_A1)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_READ_A1)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_WRITE_A1)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_UNCONFIRMEDWRITE_A1)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED3_A1)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED4_A1)
    0x01 |
#endif
    0, 
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET_A1)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED_A1)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET_A1)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHGET_A1)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHSET_A1)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION_A1)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_MULTIPLEREFERENCES_A1)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_INFORMATIONREPORT_A1)
    0x01 |
#endif
    0,
#if defined(IEC62056_SUPPORT_RESERVED5_A1)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED6_A1)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_PARAMETERIZEDACCESS_A1)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_GET_A1)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_SET_A1)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_SELECTIVEACCESS_A1)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_EVENTNOTIFICATION_A1)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_ACTION_A1)
    0x01 |
#endif
    0,
#if defined(IEC62056_SUPPORT_RESERVED0_A2)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED1_A2)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED2_A2)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_READ_A2)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_WRITE_A2)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_UNCONFIRMEDWRITE_A2)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED3_A2)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED4_A2)
    0x01 |
#endif
    0, 
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET_A2)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED_A2)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET_A2)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHGET_A2)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHSET_A2)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION_A2)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_MULTIPLEREFERENCES_A2)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_INFORMATIONREPORT_A2)
    0x01 |
#endif
    0,
#if defined(IEC62056_SUPPORT_RESERVED5_A2)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED6_A2)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_PARAMETERIZEDACCESS_A2)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_GET_A2)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_SET_A2)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_SELECTIVEACCESS_A2)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_EVENTNOTIFICATION_A2)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_ACTION_A2)
    0x01 |
#endif
    0,
#if defined(IEC62056_SUPPORT_RESERVED0_A3)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED1_A3)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED2_A3)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_READ_A3)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_WRITE_A3)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_UNCONFIRMEDWRITE_A3)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED3_A3)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED4_A3)
    0x01 |
#endif
    0, 
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET_A3)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED_A3)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET_A3)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHGET_A3)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHSET_A3)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION_A3)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_MULTIPLEREFERENCES_A3)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_INFORMATIONREPORT_A3)
    0x01 |
#endif
    0,
#if defined(IEC62056_SUPPORT_RESERVED5_A3)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED6_A3)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_PARAMETERIZEDACCESS_A3)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_GET_A3)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_SET_A3)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_SELECTIVEACCESS_A3)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_EVENTNOTIFICATION_A3)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_ACTION_A3)
    0x01 |
#endif
    0
};

extern HardwareStatus hardware_status;
//extern uint8_t EnergyLoggingTime; //Minute 
extern unsigned long LastEnergyLogAddress ;
//extern void adjust_rtc(rtc_t *time);

uint32_t known_far_msap_list[5] = 
{
    0x40000L,
    0x80000L,
    0xC0000L,
    0x100000L,
    0
};

extern Password_128 Passwords[4];
uint8_t auth0[] = {0, 0};
uint8_t auth1[] = {1, 5, 'H', 'e', 'l', 'l', 'o'};
uint8_t auth2[] = {2, 16, 'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'};
uint8_t auth3[] = {2, 16, 'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b'};



const uint8_t *authentications[5] = 
{
  
    Passwords[0].password,
    Passwords[1].password,
    Passwords[2].password,
    Passwords[3].password,
    NULL
//    auth0,
//    auth0,
//    auth2,
//    auth3,
//    NULL
};

static const uint8_t sap_list[] =
{
    20 + 19 + 23 + 22 + 1,
        4,
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(16),                                                           /* SAP */
                TAG_OCTET_STRING, 13, 'P', 'U', 'B', 'L', 'I', 'C', ' ', 'C', 'L', 'I','E','N','T',                          /* Logical device name public client */
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(2),                                                            /* SAP */
                TAG_OCTET_STRING, 12, 'M', 'E', 'T', 'E', 'R', ' ', 'R', 'E', 'A', 'D','E','R',                              /* Logical device name reading client */
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1),                                                            /* SAP */
                TAG_OCTET_STRING, 16, 'U', 'T', 'I', 'L', 'I', 'I', 'T', 'Y', ' ', 'S','E','T','T','I','N','G',              /* Logical device name management client*/
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(3),                                                            /* SAP */
                TAG_OCTET_STRING, 15, 'F', 'I', 'R', 'M', 'W', 'A', 'R', 'E', ' ', 'U','P','D','A','T','E',                  /* Logical device name firmware client*/

                
};


//extern uint8_t Temp_EEPROM_data_Buffer[640];
/*
  the  firist three octet-string uniquely identifies the MANUFACTURER 
  can be a maximum of 16 octets 
*/
//static const uint8_t logical_device_name[] =
//{
//    13, 'd', 'V', 's', '1', '2', '3', '4', '5', '6', '7','8','9','0'
//};

static const int16_t clock_time_zone = 0x1234;
static const uint8_t clock_status = 0;
static const int8_t clock_dst_deviation = 0;
static const int8_t clock_dst_enabled = 0; // no daylight saving 
static const uint8_t clock_base = 1;//internal crystal 


static const uint16_t uint8val = 0x00;
extern uint8_t Phase_Presence;
//static const uint16_t uint16val = 0x5555;
extern unsigned long FirmwareVersion; //major version 0 and sub version 1
extern CurrentBalance Current_balance;

static const uint8_t application_context_name_logical_names[] =
{
    7, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01
};

#if 0
/* This is an alternative way to express the application context name, using a structure, instead of an octet string */
static const uint8_t application_context_name_logical_names[] =
{
    15 + 1,
        7,
            TAG_UINT8, 2,                   /* Joint ISO CCITT */
            TAG_UINT8, 16,                  /* Country */
            TAG_UINT16, INJECT16(0x02F4),   /* Country name */
            TAG_UINT8, 5,                   /* Identified organisation */
            TAG_UINT8, 8,                   /* DLMS UA */
            TAG_UINT8, 1,                   /* Application context don was 2*/
            TAG_UINT8, 1                    /* Context ID */
};
#endif
#if 0
static const uint8_t application_context_name_short_names[] =
{
    7, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x02
};
static const uint8_t application_context_name_logical_names_ciphered[] =
{
    7, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x03
};
static const uint8_t application_context_name_short_names_ciphered[] =
{
    7, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x04
};
#endif



//static const uint8_t security_setup_reference[] =
//{
//    3, '?', '?', '?'
//};

static const uint8_t context_info[] =
{
    19,
        6,
            TAG_BITSTRING, 24,
/* Conformance */
#if defined(IEC62056_SUPPORT_RESERVED0)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED1)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED2)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_READ)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_WRITE)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_UNCONFIRMEDWRITE)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED3)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED4)
    0x01 |
#endif
    0, 
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHGET)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHSET)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_MULTIPLEREFERENCES)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_INFORMATIONREPORT)
    0x01 |
#endif
    0,
#if defined(IEC62056_SUPPORT_RESERVED5)
    0x80 |
#endif
#if defined(IEC62056_SUPPORT_RESERVED6)
    0x40 |
#endif
#if defined(IEC62056_SUPPORT_PARAMETERIZEDACCESS)
    0x20 |
#endif
#if defined(IEC62056_SUPPORT_GET)
    0x10 |
#endif
#if defined(IEC62056_SUPPORT_SET)
    0x08 |
#endif
#if defined(IEC62056_SUPPORT_SELECTIVEACCESS)
    0x04 |
#endif
#if defined(IEC62056_SUPPORT_EVENTNOTIFICATION)
    0x02 |
#endif
#if defined(IEC62056_SUPPORT_ACTION)
    0x01 |
#endif
    0,
            TAG_UINT16, INJECT16(0x80),             /* Max Rx PDU */
            TAG_UINT16, INJECT16(0x80),             /* Max Tx PDU */
            TAG_UINT8, DLMS_PROTOCOL_VERSION,       /* DLMS version number */
            TAG_INT8, 0,                            /* Quality of service */
            TAG_OCTET_STRING, 0x00                  /* Cyphering info */
};


rtc_t activePassiveCalenderTime;

const uint8_t partner_HS[] =
{
    6,
        2,
            TAG_INT8, 0x30,                         /* Client SAP */
            TAG_UINT16, INJECT16(1)                 /* Server SAP */
};

const uint8_t partner_LS[] =
{
    6,
        2,
            TAG_INT8, 0x20,                         /* Client SAP */
            TAG_UINT16, INJECT16(1)                 /* Server SAP */
};

const uint8_t partner_NS[] =
{
    6,
        2,
            TAG_INT8, 0x10,                         /* Client SAP */
            TAG_UINT16, INJECT16(1)                 /* Server SAP */
};

__no_init__(Data_Buffer) uint8_t Data_Buffer[CHUNK_SIZE+64]; /* General purpose buffer */
 //uint8_t Data_Buffer1[128];

/* VARIABLES AND FUNCTIONS FOR COMMUNICATIONS SETUP OBJECTS */

typedef enum
{
    BAUD_300 = 0,
    BAUD_600 = 1,
    BAUD_1200 = 2,
    BAUD_2400 = 3,
    BAUD_4800 = 4,
    BAUD_9600 = 5,
    BAUD_19200 = 6,
    BAUD_38400 = 7,
    BAUD_57600 = 8,
    BAUD_115200 = 9
} Baud_Rates;

const uint8_t Comm_Speed = BAUD_19200;
const uint8_t Window_Size_Transmit = 1;
const uint8_t Window_Size_Receive = 1;
const uint16_t Max_Info_Field_Len_TX = 128;
const uint16_t Max_Info_Field_Len_RX = 128;
const uint16_t Inter_Octet_Timeout = 3000;
const uint16_t Inactivity_Time_Out = 1000;
const uint16_t Device_Address = 0x10;

/* CONSTANTS, VARIABLES & FUNCTIONS FOR NAME PLATE DETAILS */

extern  uint8_t Meter_Sr_No[];

extern uint8_t Manufacturer_Name[];


extern uint16_t TransformerRatio_VoltageNum ;
extern uint16_t TransformerRatio_VoltageDen ;

extern uint16_t TransformerRatio_CurrentNum ;
extern uint16_t TransformerRatio_CurrentDen ;

//extern uint16_t TimeThresholdVoltageSag ;
//extern uint16_t TimeThresholdVoltageSwell;

extern uint16_t UnderVoltageTripDuration;
extern uint16_t OverVoltageTripDuration;

extern uint16_t TimeThresholdPowerDemandExceed ;
extern uint16_t TimeThresholdLongPowerFaile;

extern uint16_t ThrusholdVoltageSagPer ;
extern uint16_t ThrusholdVoltageSwellPer ;
extern uint16_t ThrusholdPowerDemandCloseToContractPower ;
extern uint16_t VoltageThresholdLongPowerFailePer;
//extern uint16_t VoltageReferance;
extern uint16_t Nominal_Voltage;

extern uint8_t ActiveQuadrant;
extern uint8_t MaxAllowedTimeShift;
extern uint16_t ClockTimeShiftInvalidLim;

extern uint32_t Active_Demand_Control_Threshold;
extern uint8_t *SpecialDayEntries;
extern uint8_t CurrentActiveTariff;


extern uint16_t Output_State;
extern uint16_t previous_output_state;
extern uint8_t control_state;
extern uint8_t control_mode;

extern uint16_t NumberOfLongPowerFails;

extern uint16_t DurationOfAllLongPowerFailes;
extern uint32_t ErrorValue;
extern uint16_t ImportActivePowerOverThrStatus;
extern uint16_t ExportActivePowerOverThrStatus;
extern uint32_t ImportActivePowerThreshold;
extern uint32_t ExportActivePowerThreshold;

extern uint8_t Device_ID_2[] ;
extern uint8_t Device_ID_3[] ;
extern uint8_t Device_ID_4[] ;
extern uint8_t Device_ID_5[] ;
extern uint8_t Device_ID_6[] ;
extern uint8_t Device_ID_7[] ;
extern uint8_t Device_ID_8[] ;
extern uint8_t DateTimeFormat[];

extern uint8_t PLC_FW_Version[];
extern uint16_t PLC_Vendor_ID;
extern uint8_t PLC_Product_ID;
//extern uint8_t MAC_Address[];

extern uint32_t MAXActivePowerTripPoint ; //in w watt steps 
extern uint32_t MAXActivePowerExportTripPoint ;

extern uint8_t EnergyLoggingTime;

/* The management logical device. This is a lowest level security device, used to find out
   what the physical device can do. See IEC62056-62 4.6.5. */
uint8_t FirmwareVersionStr[]={5,'V','0','0','0','1'};
void changeFirmwareVersion(void *data,int direction)
{
  if(direction == ATTR_READ)
  {
    uint8_t *datap = (uint8_t*)data;
    *datap = 5;
     datap++;
     *datap ='V';
     datap++;

    int outlen=0;
    uint8_t buff[8];
    longToString_c(FirmwareVersion,buff,8,&outlen);
    
    uint8_t i=0;
    for(i=outlen;i<4;i++)
    buff[i]= '0';
    
    *datap = buff[3] + 48;
     datap++;
    *datap = buff[2] + 48;
     datap++;    
     *datap = buff[1] + 48;
     datap++;
     *datap = buff[0] + 48;
     datap++;
    //memcpy(datap,buff,4);
    
  }
  else
  {
  }
}


const uint8_t FW_Version_No[] =
{
    6, 'A','A','1','.','0','0'
};
const uint8_t FW_Version_No_Comm[] =
{
    6, 'B','B','1','.','0','0'
};
const uint16_t Internal_CT_Ratio = 1;
const uint16_t Internal_PT_Ratio = 1;
const uint16_t Year_of_Manufacture = 2010;

/* CONSTANTS, VARIABLES & FUNCTIONS FOR PROGRAMMABLE PARAMETERS */

uint32_t Demand_Integration_Period = 1;
uint8_t OperatingMode = 1;


const uint8_t Season_Profile_Dummy[] =
{
    2*32 + 1,
        1,
            TAG_STRUCTURE, 3,
                TAG_OCTET_STRING, 8, 'A', 'L', 'L', ' ', 'Y', 'E','A','R',
                TAG_OCTET_STRING, 12, INJECT16(2002), DECEMBER, 4, WEDNESDAY, 10, 6, 11, 0xFF, INJECT16(120), 0x00,
                TAG_OCTET_STRING, 8, 'A', 'L', 'L', ' ', 'Y', 'E','A','R',/*week profile name*/

};                        
  
const uint8_t Week_Profile_Table_Dummy[] =
{
    2*24 + 1,
        1,
            TAG_STRUCTURE, 8,
                TAG_OCTET_STRING, 8, 'A', 'L', 'L', ' ', 'Y', 'E','A','R',
                TAG_UINT8, 1,   /* Monday */
                TAG_UINT8, 1,   /* Tuesday */
                TAG_UINT8, 1,   /* Wednesday */
                TAG_UINT8, 1,   /* Thursday */
                TAG_UINT8, 1,   /* Friday */
                TAG_UINT8, 2,   /* Saturday */
                TAG_UINT8, 2,   /* Sunday */
 };                       

const uint16_t Day_Profile_Table_Column_Szs[] = {16,19,24,29,34,39,44,49,51,56,61};
const uint8_t Day_Profile_Table_Template[] =
{              
            STUFF_DATA | TAG_STRUCTURE, 2,                       
                STUFF_DATA|TAG_UINT8, ITEM_TAG_DAY_ID,                 /* Day-ID -1 */
                TAG_ARRAY, 4,
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_START_TIME_1, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_SCRIPT_ID1),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_START_TIME_2, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_SCRIPT_ID2),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_START_TIME_3, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_SCRIPT_ID3),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_START_TIME_4, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_SCRIPT_ID4),        
               
};

const uint8_t Day_Profile_Table_Passive_Template[] =
{              
            STUFF_DATA | TAG_STRUCTURE, 2,                       
                STUFF_DATA|TAG_UINT8, ITEM_TAG_DAY_ID_PASSIVE,                 /* Day-ID -1 */
                TAG_ARRAY, 4,
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_1, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID1),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_2, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID2),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_3, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID3),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_4, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID4),        
               
};
                  

uint8_t DeviceIdentifier[]={13,'D','V','E','0','1','1','0','0','0','0','0','3','4'};
uint8_t MACAddress[]={0,0xB,0,0,0,0,0x22};



extern uint8_t EUI[];
extern uint8_t device_identifier[];

//extern int get_weekday(rtc_t *datetime);
void get_date_time_string(void *data, int direction)
{
  if(direction == ATTR_READ)
  {  
//     uint8_t dw = 2;
     uint8_t dw = get_weekday(&rtcc);
     uint8_t temp[] = {12,INJECT16(rtcc.year), rtcc.month, rtcc.day, dw , rtcc.hour, rtcc.minute, rtcc.second, 0xFF, INJECT16(120), 0x00};    
     memcpy(data,temp,13);               
  }
  else{
    
   //TODO. Test this 
    
    
      
      
     rtc_t temp;
     uint8_t *datap = data;
     uint8_t utemp;
     utemp = *datap++;//length
     utemp = *datap++;
     temp.year = *datap++;
     temp.year |= (utemp<<8);
     temp.month = *datap++;
     temp.day = *datap++;
     *datap++; // daw of the week
     temp.hour = *datap++;
     temp.minute = *datap++;
     temp.second = *datap++;
     
     hardware_status.RTCResetToDefaultStatus = 0;// && rtc is not reset to default
     hardware_status.RTC_Status = 1; // rtc is now ok 
     
      adjust_rtc(&temp,1);
      
      EventLog evl;
      evl.EventCode = CLOCK_PROGRAMMED;
      evl.timeStump = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
      evl.Checksum  =(getCheckSum(&(evl.timeStump.TimestampLow),4) + evl.timeStump.TimestampUp + evl.EventCode)&0xff;
      evl.value = 0;
//      logEvent(&evl);
      write_to_eeprom(&evl,(uint8_t *)0,logEvent);
      
  }
  
}
extern uint32_t scroll_display_time;
extern uint32_t AutoDisplayTime;
void changeDisplayScrollTime(void *data,int direction)/*TODO. Test this*/
{
   if(direction == ATTR_READ)
   {
    
   }
   else
   {
      uint32_t *ptr = data;
      uint32_t temp = (uint32_t)(*ptr);
      scroll_display_time = temp;
      AutoDisplayTime = scroll_display_time*1000;
      setAutoDisplayTime(&AutoDisplayTime,(uint8_t * )0);
   }
}



void getSetDeviceIdentifier(void *data,int data_direction)
{
     uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      datap++;
      setLogicDeviceName(datap,(uint8_t * )0);
      //datap++;
      memcpy(device_identifier,datap,13);
    }
    else if(data_direction == ATTR_READ)
    {      
       uint8_t  *datap2= (DeviceIdentifier+1);
       memcpy(datap2,device_identifier,13);
       memcpy(datap,DeviceIdentifier,14);
    } 
}

extern void Reset_System();
void getSetOperatingMode(void *data,int data_direction)
{
   uint8_t *datap=data;
    if(data_direction == ATTR_WRITE)
    {
         OperatingMode = *datap;
         setOperatingMode(datap,(uint8_t * )0);                        
    }
    else if(data_direction == ATTR_READ)
    {      
       *datap = OperatingMode;
    } 
}

extern CurrentBalance Current_balance;
void getSetCurrentBalance(void *data,int data_direction)
{
  
    if(data_direction == ATTR_WRITE)
    {
      uint32_t nv_2=0;
      uint8_t *ptr2 = data;
//#pragma message("Compiling 5")
      nv_2 = *(ptr2+3);
      nv_2 |= ((uint32_t)(*(ptr2+2))<<8); 
      nv_2 |= ((uint32_t)(*(ptr2+1))<<16);  
      nv_2 |= ((uint32_t)(*(ptr2))<<24);
           
      
      CurrentBalance b;
      b.balance = (int32_t)nv_2;
      setCurrentBalance(&b,(uint8_t * )0);
      Current_balance.balance = b.balance;
      
//      //datap++;
//      setMacAddress(datap);
//      memcpy(EUI,datap,6);
    }
    else if(data_direction == ATTR_READ)
    {      

    } 
}


//TODO. activePassiveCalenderTime is not initialized
void setGetActivePassiveCalendarTime(void *data,int direction)
{
  uint8_t *datap = data;

  if(direction == ATTR_READ)
  {
    activePassiveCalenderTime.year = 2015;
    activePassiveCalenderTime.month = 5;
    activePassiveCalenderTime.hour = 5;
    activePassiveCalenderTime.minute = 5;
    activePassiveCalenderTime.second = 5;
    
    uint8_t data2[] = {12,INJECT16(activePassiveCalenderTime.year),activePassiveCalenderTime.month,
                          activePassiveCalenderTime.day,get_weekday(&activePassiveCalenderTime),
                          activePassiveCalenderTime.hour,activePassiveCalenderTime.minute,activePassiveCalenderTime.second,0xff,INJECT16(120), 0x00};
    
    memcpy(data,data2,13);
  }
  else if(direction == ATTR_WRITE)
  {
    
      datap++; //length
      activePassiveCalenderTime.year = *datap++;
      activePassiveCalenderTime.year |= (*datap++<<8);
      
      activePassiveCalenderTime.month =  *datap++;
      activePassiveCalenderTime.day = *datap++;
      datap++;//day of the week
      activePassiveCalenderTime.hour = *datap++;
      activePassiveCalenderTime.minute = *datap++;
      activePassiveCalenderTime.second = *datap++;
      
  }
}
void getSetMacAddress(void *data,int data_direction)
{
     uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      datap++;
      setMacAddress(datap,(uint8_t *)0);
      //datap++;
      memcpy(EUI,datap,6);
    }
    else if(data_direction == ATTR_READ)
    {      
       uint8_t  *datap2= (MACAddress+1);
       memcpy(datap2,EUI,6);
       memcpy(datap,MACAddress,7);
    } 
}
void getSetFwVersion(void *data,int data_direction)
{

    if(data_direction == ATTR_WRITE)
    {
       int x=0;
       x++;
    }
    else if(data_direction == ATTR_READ)
    {      
       int y=0;
       y++;
    } 
}


void Access_Season_Profile(void *data, int direction)
{
    /********* This is a dummy function ******************/ 
    if (direction != ATTR_READ)
        return;
    memcpy(data, Season_Profile_Dummy, sizeof(Season_Profile_Dummy));
    //interpret_template(data, direction, Season_Profile_Dummy, sizeof(Season_Profile_Dummy));
}

void Access_Week_Profile_Table(void *data, int direction)
{
    /********** This is a dummy function *****************/
    if (direction != ATTR_READ)
        return;
    memcpy(data, Week_Profile_Table_Dummy, sizeof(Week_Profile_Table_Dummy));
    //interpret_template(data, direction, Week_Profile_Table_Dummy, sizeof(Week_Profile_Table_Dummy));
}

void Access_Day_Profile_Table_Passive(void *data, int direction)
{
 //   interpret_template(data, direction, Day_Profile_Table_Dummy, sizeof(Day_Profile_Table_Dummy));
  if (direction == ATTR_READ || direction ==ATTR_WRITE){
    msg_info.template=Day_Profile_Table_Passive_Template;
    msg_info.sz_template=sizeof(Day_Profile_Table_Passive_Template);
    msg_info.num_entries=3; //we only have three day profiles 
    msg_info.start_entry=1;
    msg_info.column_szs=NULL;
  }
  else
  {
   __asm__ __volatile__ (" NOP");
  }  

}

void Access_Day_Profile_Table(void *data, int direction)
{
 //   interpret_template(data, direction, Day_Profile_Table_Dummy, sizeof(Day_Profile_Table_Dummy));
  if (direction == ATTR_READ || direction ==ATTR_WRITE){
    msg_info.template=Day_Profile_Table_Template;
    msg_info.sz_template=sizeof(Day_Profile_Table_Template);
    msg_info.num_entries=3; //we only have three day profiles 
    msg_info.start_entry=1;
    msg_info.column_szs=NULL;
  }
  else
  {
   __asm__ __volatile__ (" NOP");
  }  

}

uint8_t Billing_Script_Execution_Time_Template[] =
{
            TAG_STRUCTURE, 2,
                TAG_OCTET_STRING, 4, ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME, 0, 0, 0,              // Time-Midnight        //8,9
                TAG_OCTET_STRING, 5, ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY, 0, 0, 0, 0,    // Last day of Month   //17
};          





const uint16_t Special_Days_Column_Size[] = {5,12,14};
uint8_t Special_Days_List_Template[] =
{
            TAG_STRUCTURE, 3,
                STUFF_DATA|TAG_UINT16, INJECT16(ITEM_TAG_SPECIAL_DAY_INDEX),
                STUFF_DATA|TAG_OCTET_STRING,5,ITEM_TAG_SPECIAL_DAY_DATE,0,0,0,0,
                STUFF_DATA|TAG_UINT8,  ITEM_TAG_SPECIAL_DAY_DAY_ID,            
}; 



/*TODO finish this */
void get_Special_Days_List(void *data,int direction)
{
   //memcpy(data, Tariff_Scripts_Dummy, sizeof(Tariff_Scripts_Dummy));
    //   interpret_template(data, direction, Day_Profile_Table_Dummy, sizeof(Day_Profile_Table_Dummy));
    if (direction == ATTR_READ || direction == ATTR_WRITE){
        msg_info.template=Special_Days_List_Template;
        msg_info.sz_template=sizeof(Special_Days_List_Template);
        msg_info.num_entries = 24; // we only have 24 holidays 
        msg_info.start_entry = 0;
        msg_info.column_szs = Special_Days_Column_Size;
    }
    else
    {
      __asm__ __volatile__ (" NOP");
    }   
}


uint8_t DiconnectControl_Script_Execution_Time[]=
{
            TAG_STRUCTURE, 2,
                STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DISCONNECTCONTORL_SCRIPT_EXE_TIME, 0, 0, 0,              // Time-Midnight        //8,9
                STUFF_DATA|TAG_OCTET_STRING, 5, ITEM_TAG_DISCONNECTCONTROL_SCRIPT_EXE_DAY, 0, 0, 0, 0,    // Last day of Month   //17
};

void Access_Billing_Script_Execution_Time(void *data, int direction)
{
 //   interpret_template(data, direction, Day_Profile_Table_Dummy, sizeof(Day_Profile_Table_Dummy));
  if (direction == ATTR_READ || direction ==ATTR_WRITE){
    msg_info.template=Billing_Script_Execution_Time_Template;
    msg_info.sz_template=sizeof(Billing_Script_Execution_Time_Template);
    msg_info.num_entries=12;
    msg_info.start_entry=1;
    msg_info.column_szs=Day_Profile_Table_Column_Szs;
  }
  else
  {
   __asm__ __volatile__ (" NOP");
  }  

}

uint8_t Calendar_Name_Active[] =
{
    15,'A','C','T','I','V','E',' ', 'C', 'A', 'L', 'E', 'N', 'D', 'A', 'R'
};

uint8_t Calendar_Name_Passive[] =
{
    16,'P','A','S','S','I','V','E',' ', 'C', 'A', 'L', 'E', 'N', 'D', 'A', 'R'
};


uint8_t Activate_Passive_Calendar_Time[] =
{
    12, INJECT16(2002), DECEMBER, 4, WEDNESDAY, 10, 6, 11, 0xFF, INJECT16(120), 0x00
};



const uint8_t Disconnect_Scripts_Table[] =
{  
    2*26 + 1,
        /*TAG_ARRAY,*/ 2, 
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1), /* Script ID - 1 disconnect script*/
                TAG_ARRAY, 1,
                TAG_STRUCTURE, 5,
                        TAG_ENUM, 2,    /* execute method */
                        TAG_UINT16, INJECT16(70), /* class id*/
                        TAG_OCTET_STRING, 6, 0, 0, 96, 3, 10, 255, /* disconnect control*/
                        TAG_INT8, 1,
                        TAG_INT8, 0,
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(2), /* Script ID - 2 connect script*/
                TAG_ARRAY, 1,
                TAG_STRUCTURE, 5,
                        TAG_ENUM, 2,    /* execute method */
                        TAG_UINT16, INJECT16(70), /* class id*/
                        TAG_OCTET_STRING, 6, 0, 0, 96, 3, 10, 255,  /* disconnect control */
                        TAG_INT8, 2,
                        TAG_INT8, 0
};
const uint8_t GlobalReset_Scripts_Table[] =
{
    1*26 + 1,
        /*TAG_ARRAY,*/ 1, 
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1), /* Script ID - 1 Reset Meter*/
                TAG_ARRAY, 1,
                TAG_STRUCTURE, 5,
                        TAG_ENUM, 2,    /* execute method */
                        TAG_UINT16, INJECT16(70), /* class id*/
                        TAG_OCTET_STRING, 0, 0, 0, 96, 3, 10, 255, /* meter rest object*/
                        TAG_INT8, 1, //execute method one of the global reset object
                        TAG_INT8, 0,

};

void Access_Disconnect_Scripts(void *data, int direction)
{
    if (direction != ATTR_READ)
        return;
    memcpy(data, Disconnect_Scripts_Table, sizeof(Disconnect_Scripts_Table));
}

void Access_GlobalReset_Scripts(void *data, int direction)
{
    if (direction != ATTR_READ)
        return;
    memcpy(data, GlobalReset_Scripts_Table, sizeof(GlobalReset_Scripts_Table));
}

const uint8_t Tariff_Scripts_Dummy[] =
{
    2*26 + 1,
        2, 
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1), /* Script ID - 1 */
                TAG_ARRAY, 1,
                    TAG_STRUCTURE, 5,
                        TAG_ENUM, 1,    /* Write attribute */
                        TAG_UINT16, INJECT16(1),
                        TAG_OCTET_STRING, 6, 255, 255, 255, 255, 255, 255,
                        TAG_INT8, 1,
                        TAG_INT8, 1,
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(2), /* Script ID - 2 */
                TAG_ARRAY, 1,
                    TAG_STRUCTURE, 5,
                        TAG_ENUM, 1,    /* Write attribute */
                        TAG_UINT16, INJECT16(1),
                        TAG_OCTET_STRING, 6, 255, 255, 255, 255, 255, 255,
                        TAG_INT8, 1,
                        TAG_INT8, 1
};

void Access_Tariff_Scripts(void *data, int direction)
{
    if (direction != ATTR_READ)
        return;
    memcpy(data, Tariff_Scripts_Dummy, sizeof(Tariff_Scripts_Dummy));
    //interpret_template(data, direction, Tariff_Scripts_Dummy, sizeof(Tariff_Scripts_Dummy));
}

const uint8_t Executed_Billing_Script[] =
{
    11 + 1,
        2,
            TAG_OCTET_STRING, 6, 0, 0, 10, 0, 1, 255,
            TAG_UINT16, INJECT16(1),
};   
const uint8_t Executed_DiconnectControl_Script[] =
{
    11 + 1,
        2,
            TAG_OCTET_STRING, 6, 0, 0, 10, 0, 106, 255,       //disconnect Script
            TAG_UINT16, INJECT16(1),
}; 
  
const uint8_t Billing_Script_Type = 1;
const uint8_t DiconnectControl_Type = 1;

const uint8_t Billing_Script[] =
{
    1*26 + 1,
        1, 
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1), //Script ID - 1
                TAG_ARRAY, 1,
                    TAG_STRUCTURE, 5,   // Invoke Capture Method of Billing Period Profile 
                        TAG_ENUM, 2,    // Execute specific method
                        TAG_UINT16, INJECT16(CLASS_ID_PROFILE_GENERIC),
                        TAG_OCTET_STRING, 6, 1, 0, 98, 1, 0, 255,
                        TAG_INT8, 1,
                        TAG_INT8, 0
};

/* CONSTANTS, VARIABLES & FUNCTIONS FOR METER PARAMETERS */

#define SCALER_CURRENT              -3
#define SCALER_VOLTAGE              -2
#define SCALER_POWER_FACTOR         -3
#define SCALER_FREQUENCY            -2
#define SCALER_APPARENT_POWER        0
#define SCALER_ACTIVE_POWER          0
#define SCALER_REACTIVE_POWER        0
#define SCALER_ACTIVE_ENERGY        -1
#define SCALER_REACTIVE_ENERGY      -1
#define SCALER_APPARENT_ENERGY      -1


const uint8_t scalar_unit_Time[]=
{
   5,
      2,
         TAG_INT8,INJECT8(0),
         TAG_ENUM, OBIS_UNIT_TIME_SECOND
};
const uint8_t scalar_unit_dem_colseToContractPower[]=
{
   5,
      2,
         TAG_INT8,INJECT8(-2),
         TAG_ENUM, OBIS_UNIT_PERCENT
};

const uint8_t scalar_unit_Time_Shift_EvtLim[]=
{
   5,
      2,
         TAG_INT8,INJECT8(0),
         TAG_ENUM, OBIS_UNIT_TIME_SECOND
};
const uint8_t scalar_unit_I[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-3),
            TAG_ENUM, OBIS_UNIT_CURRENT_AMPERE
};
const uint8_t scalar_unit_V[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT
};
const uint8_t scalar_unit_referance_voltage[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT
};
const uint8_t scalar_unit_ThresholdVoltageSag_Swell[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_PERCENT
};

const uint8_t scalar_unit_PF[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-4),
            TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT
};

const uint8_t scalar_unit_power_current_voltage_calib[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT
};

const uint8_t scalar_unit_Frequency[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_FREQUENCY_HERTZ
};

const uint8_t scalar_unit_KVA[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_APPARENT_POWER_VA
};

const uint8_t scalar_unit_KW[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_ACTIVE_POWER_WATT
};
const uint8_t scalar_unit_active_power[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_ACTIVE_POWER_WATT
};
const uint8_t scalar_unit_total_active_energy_import[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_ACTIVE_ENERGY_WATT_HOUR
};
const uint8_t scalar_unit_reactive_energy[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_REACTIVE_ENERGY_VAR_HOUR
};
const uint8_t scalar_unit_KVAr[] =
{
    5,
        2,
            TAG_INT8, 0,
            TAG_ENUM, OBIS_UNIT_REACTIVE_POWER_VAR
};

uint8_t TimeShiftEvtLimVal = 30;
uint16_t TimeShiftEvtInvalidLimVal = 600;

uint16_t Vrms_R = 23000;
uint16_t Vrms_Y = 23000;
uint16_t Vrms_B = 23000;
uint32_t Irms_R = 120000;
uint32_t Irms_Y = 100000;
uint32_t Irms_B = 100000;
uint16_t Mains_Freq = 5000;
int16_t sPF_R = 999;
int16_t sPF_Y = 999;
int16_t sPF_B = 999;
int16_t sPF_Avg = 999;
int32_t P_Active = 40;
int32_t P_Reactive = 0;

extern uint32_t P_Reactive_positive ;
extern uint32_t P_Reactive_negative ;
extern uint32_t P_Active_Plus;
extern uint32_t P_Active_Negative;

uint32_t P_Apparent = 23000;
uint32_t Cum_Power_Off_Count = 0;
uint32_t Cum_Tamper_Count = 0;
uint32_t Cum_MD_Reset_Count = 0;
uint32_t Cum_Prog_Count = 0;
uint32_t Temp_Uint32 = 0;

uint32_t Total_Active_Energy_Import = 470;
uint32_t Total_Active_Energy_Export = 470;
uint32_t Reactive_Energy_QI = 340;
uint32_t Reactive_Energy_QII = 340;
uint32_t Reactive_Energy_QIII = 340;
uint32_t Reactive_Energy_QIV = 340;


uint8_t Last_MD_Rst_DT[] =
{
    12, INJECT16(2002), DECEMBER, 4, WEDNESDAY, 10, 6, 11, 0xFF, INJECT16(120), 0x00
};







const uint16_t Instantanious_Profile_Column_Szs2[] = {16,19,24,29,34,39,44,49,54,59,64,69,74};
const uint8_t Instantanious_Profile_Buffer_Template2[] =
{
   STUFF_DATA | TAG_STRUCTURE, 13,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*Time stump*/
        STUFF_DATA | TAG_UINT16, INJECT16(ITEM_TAG_VR),                       /* VL1 */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_IR),            /* IL1 */
                      TAG_UINT32, INJECT32(0),           /* VL2 delete from this */  
                      TAG_UINT32, INJECT32(0),             /* IL2*/
                      TAG_UINT32, INJECT32(0),             /* VL3*/
                      TAG_UINT32, INJECT32(0),             /* IL3 delete to this*/
                      TAG_UINT32, INJECT32(0),             /* Total Current*/
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_POWER_IMPORT),             /* Active Power import*/
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_POWER_EXPORT),             /* Active Power export*/
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_POWER_IMPORT),             /* reactive Power import*/
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_POWER_EXPORT),             /* reactive Power export */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_PF_TOTAL)             /* power factor */
};

const uint16_t Instantanious_EnergyProfile_Column_Szs2[] = {16,21,26,31,36,41,46,51};
const uint8_t Instantanious_EnergyProfile_Buffer_Template2[] =
{
   STUFF_DATA | TAG_STRUCTURE, 8,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*Time stump*/
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_ENERGY_PLUS),                       /* import Energy */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_ENERGY_MINUS),             /* export Energy */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RI_PLUS),             /* RE QI   */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RC_PLUS),             /* RE QII  */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RI_MINUS),            /* RE QIII */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RC_MINUS),            /* RE QIV  */
                     TAG_UINT32, INJECT32(0),            /* RE QIV  *///???????????? NO IDEA WHY,IT JUST SOLVES THE PROBLEM
};



const uint16_t Instantanious_Profile_Column_Szs[] = {16,19,24,29,34,39,44,49,51,56,61};
const uint8_t Instantanious_Profile_Buffer_Template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 11,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_LP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*Time stump*/
        STUFF_DATA | TAG_UINT16, INJECT16(ITEM_TAG_VR_LP),                       /* Vr */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_CUM_KWH_TOTAL_LP),            /* Active Energy */
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_POWER_LP),           /* Reactive power*/
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_POWER_LP),             /* Active Power*/
                     TAG_UINT32, INJECT32(0),             /* Active Power*/
                     TAG_UINT32, INJECT32(0),             /* Active Power*/
                     TAG_UINT32, INJECT32(0),             /* Active Power*/
                     TAG_UINT32, INJECT32(0),             /* Active Power*/
                     TAG_UINT32, INJECT32(0),             /* Active Power*/
                     TAG_UINT32, INJECT32(0)              /* Active Power*/
};

const uint16_t Load_Profile_Column_Szs[] = {16,18,23,28,33,38,43,48};//
/*const uint8_t Load_Profile_Buffer_Template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 8,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_LP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Time stump
                     TAG_UINT8, INJECT8(1),                                    AMR profile status
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_CUM_KWH_TOTAL_LP),             Active Energy A+
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_POWER_LP),              Active Power
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_POWER_LP),            Reactive power QI
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_VR_LP),                        Voltage
                     TAG_UINT32, INJECT32(0),              QIII
                     TAG_UINT32, INJECT32(0),              QIV

};*/
const uint8_t Load_Profile_Buffer_Template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 8,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_LP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*Time stump*/
                     TAG_UINT8, INJECT8(1),                                      /* AMR profile status*/
		STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_CUM_KWH_TOTAL_LP),            /* Active Energy A+     :Ai */
		 	 	 	 TAG_UINT32, INJECT32(0), 									 /* Active Energy A-     :Ae */
	    STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_POWER_LP),           /* Reactive Energy QI   :R1 */
					 TAG_UINT32, INJECT32(0),            						 /* Reactive Energy QII  :R2 */
					 TAG_UINT32, INJECT32(0),            						 /* Reactive Energy QIII :R3 */
		STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_VR_LP),          			 /* Reactive Energy QIV  :R4 *///originally this was for voltage

};
const uint16_t Standard_Event_Log_Column_Szs[] = {16,18};//,20,25};
const uint8_t Standard_Event_Log_Template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 2,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_SE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*Event Time stump*/
        STUFF_DATA | TAG_UINT8, ITEM_TAG_EVENT_CODE_SE//,  //Event code 
//                     TAG_UINT8, 0,//ITEM_TAG_EVENT_GROUP_SE,
//        STUFF_DATA | TAG_UINT32,INJECT32(ITEM_TAG_EVENT_DATA)//ITEM_TAG_EVENT_DATA, // the data associated with the event  
};


      
const uint16_t Billing_Profile_Column_Szs[]={16,21,26,31,36,41,46,51,65};
const uint8_t Billing_Profile_Buffer_Template[]=
{
  STUFF_DATA | TAG_STRUCTURE, 9,
       STUFF_DATA | TAG_OCTET_STRING, 12,  ITEM_TAG_BILLING_DATETIME_MAX_CURR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //Billing Period Start
       STUFF_DATA | TAG_INT32,  INJECT32(ITEM_TAG_AE_IM_BI), //AE IM 
       STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_AE_EX_BI), // AE EP 
       STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RE_QI_BI), // RE QI
       STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RE_QII_BI), // RE QII
       STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RE_QIII_BI), // RE QIII
       STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RE_QIV_BI), // RE QIV
       STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_MAX_DEMAND_BI), // MAX Demand Value
       STUFF_DATA | TAG_OCTET_STRING, 12,  ITEM_TAG_BILLING_DATETIME_MAX_BI, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //Billing Period Start
}; 
   

const uint8_t Instant_Profile_Sort_Object[] =
{
    16 + 1,
        4,
            TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
            TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
            TAG_INT8, 3,
            TAG_UINT16, INJECT16(0)
};

const uint8_t Instant_Scaler_Profile_Buffer[] =
{
    (20*6 + 2+1),
        /*TAG_ARRAY,*/ 1,
          TAG_STRUCTURE, 20,                    
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(0),
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of Date and Time
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_CURRENT),
                TAG_ENUM, OBIS_UNIT_CURRENT_AMPERE,     // Unit of Ir
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_CURRENT),
                TAG_ENUM, OBIS_UNIT_CURRENT_AMPERE,     // Unit of Iy
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_CURRENT),
                TAG_ENUM, OBIS_UNIT_CURRENT_AMPERE,     // Unit of Ib
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_VOLTAGE),
                TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT,       // Unit of Vr
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_VOLTAGE),
                TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT,       // Unit of Vy
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_VOLTAGE),
                TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT,       // Unit of Vb
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_POWER_FACTOR),
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of PF_R
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_POWER_FACTOR),
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of PF_Y
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_POWER_FACTOR),
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of PF_B
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_POWER_FACTOR),
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of PF_Total
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_FREQUENCY),
                TAG_ENUM, OBIS_UNIT_FREQUENCY_HERTZ,    // Unit of PF_Total
            TAG_STRUCTURE, 2,
                TAG_INT8, SCALER_APPARENT_POWER,
                TAG_ENUM, OBIS_UNIT_APPARENT_POWER_VA,  // Unit of KVA
            TAG_STRUCTURE, 2,
                TAG_INT8, SCALER_ACTIVE_POWER,
                TAG_ENUM, OBIS_UNIT_ACTIVE_POWER_WATT,  // Unit of KW
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_REACTIVE_POWER),
                TAG_ENUM, OBIS_UNIT_REACTIVE_POWER_VAR, // Unit of KVAr
            TAG_STRUCTURE, 2,
                TAG_INT8, 0,
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of Cumulative Power off Duration
            TAG_STRUCTURE, 2,
                TAG_INT8, 0,
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of Cumulative Tamper Count
            TAG_STRUCTURE, 2,
                TAG_INT8, 0,
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of Cumulative MD Reset Count
            TAG_STRUCTURE, 2,
                TAG_INT8, 0,
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,     // Unit of Cumulative Programming Count
            TAG_STRUCTURE, 2,
                TAG_INT8, 0,
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT      // Unit of Last MD Reset Date and Time
};

const uint8_t Load_Scaler_Profile_Buffer[] =
{
    11*6 + 2 +1,
        1,
            TAG_STRUCTURE, 11,
            TAG_STRUCTURE, 2,
                TAG_INT8, 0,
                TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT,             // Unit of Date and Time
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_CURRENT),
                TAG_ENUM, OBIS_UNIT_CURRENT_AMPERE,             // Unit of Ir
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_CURRENT),
                TAG_ENUM, OBIS_UNIT_CURRENT_AMPERE,             // Unit of Iy
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_CURRENT),
                TAG_ENUM, OBIS_UNIT_CURRENT_AMPERE,             // Unit of Ib
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_VOLTAGE),
                TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT,               // Unit of Vr
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_VOLTAGE),
                TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT,               // Unit of Vy
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_VOLTAGE),
                TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT,               // Unit of Vb
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_ACTIVE_ENERGY),
                TAG_ENUM, OBIS_UNIT_ACTIVE_ENERGY_WATT_HOUR,    // Unit of Active Energy
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_REACTIVE_ENERGY),
                TAG_ENUM, OBIS_UNIT_REACTIVE_ENERGY_VAR_HOUR,   // Unit of Reactive Energy Lag
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_REACTIVE_ENERGY),
                TAG_ENUM, OBIS_UNIT_REACTIVE_ENERGY_VAR_HOUR,   // Unit of Reactive Energy Lead
            TAG_STRUCTURE, 2,
                TAG_INT8, INJECT8(SCALER_APPARENT_ENERGY),
                TAG_ENUM, OBIS_UNIT_APPARENT_ENERGY_VA_HOUR,    // Unit of Apparent Energy
};

const uint8_t Instant_Scaler_Profile_Capture_Objects[] =
{
    INJECT16(0x8000 | (20*18 + 1)),
        /*TAG_ARRAY,*/ 20,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255,    // Date & Time
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L1_CURRENT, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L2_CURRENT, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L3_CURRENT, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L1_VOLTAGE, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L2_VOLTAGE, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L3_VOLTAGE, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L1_AVERAGE_POWER_FACTOR, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L2_AVERAGE_POWER_FACTOR, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_L3_AVERAGE_POWER_FACTOR, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_AVERAGE_POWER_FACTOR, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_SUPPLY_FREQUENCY, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_APPARENT_POWER_PLUS, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_ACTIVE_POWER_PLUS, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ELECTRICITY_OBJECTS, 0, OBIS_ELECTRICITY_GROUP_C_REACTIVE_POWER_PLUS, OBIS_ELECTRICITY_GROUP_D_INSTANTANEOUS_VALUE, 0, 255,
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 96, 7, 15, 255,  // Cumulative Power Off Duration  
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 94, 91, 0, 255,  // Cumulative Tamper Count  
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 0, 1, 0, 255,    // Cumulative MD Reset Count  
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 96, 2, 0, 255,   // Cumulative Programming Count  
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 0, 1, 2, 255,    // Date and Time of Last MD Reset  
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0)
};

const uint8_t Instant_Scaler_Profile_Sort_Object[] =
{
    16 + 1,
        4,
            TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
            TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
            TAG_INT8, 3,
            TAG_UINT16, INJECT16(0)
};

/*TODO. Redo this */

const uint8_t Instantaneous_Values_Capture_Objects[] =
{  
    INJECT16(0x8000 | (13*18 + 1)),
         13,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, 0, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 32, 7, 0, 255, // Voltage L1
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 31, 7, 0, 255, //Current L1
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 52, 7, 0, 255, // Voltage L2
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 51, 7, 0, 255, // Currrent L2
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 72, 7, 0, 255, // Voltage L3
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 71, 7, 0, 255, // Current L3
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 90, 7, 0, 255, // Il1+Il2+Il3
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 1, 7, 0, 255, // Active Power +
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 2, 7, 0, 255, // Active Power -
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 3, 7, 0, 255, // Q+
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 4, 7, 0, 255, // Q-
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 13, 7, 0, 255, // Pf
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)               
               
};
/*TODO. Redo this */
const uint8_t Instantaneous_Values_Sort_Object[] =
{
    16 + 1,
        4,
            TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
            TAG_OCTET_STRING, 6, 0, 0, 1, 0, 0, 255, // Date & Time
            TAG_INT8, 3,
            TAG_UINT16, INJECT16(0)
};


const uint8_t Load_Profile_Capture_Objects[] =
{
    INJECT16(0x8000 | (8*18 + 1)),
         8,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //Ae Import
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, 1, 0, 1, 29, 0, 255,              //[E.E] was 0, 0, 96, 10, 7, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //Ae Export
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 2, 29, 0, 255,      		   //[E.E]was 1, 0, 1, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //ReQ1
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 5, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //ReQ2
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 6, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                           //ReQ3
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 7, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                           //not used   //ReQ4
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 8, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //not used //Voltage
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 32, 7, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
};

/*TODO. Redo this*/
const uint8_t Instantaneous_Energy_Capture_Objects2[] =
{
    INJECT16(0x8000 | (8*18 + 1)),
         8,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, 1, 0, 1, 8, 0, 255,  
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 2, 8, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 5, 8, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 6, 8, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 7, 8, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 8, 8, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),//,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 8, 8, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)  
     
};



/*TODO. Redo this */
const uint8_t Instantaneous_Energy_Sort_Object[] =
{
    16 + 1,
        4,
            TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
            TAG_OCTET_STRING, 6, 0, 0, 1, 0, 0, 255, // Date & Time
            TAG_INT8, 3,
            TAG_UINT16, INJECT16(0)
};

const uint8_t Standard_Event_Log_Objects[] =
{
    INJECT16(0x8000 | (2*18+1)),
         2,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, 0, 0, 96, 11, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)
          
};



const uint8_t Load_Profile_Sort_Object[] =
{
    16 + 1,
        4,
            TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
            TAG_OCTET_STRING, 6, 0, 0, 1, 0, 0, 255, // Date & Time
            TAG_INT8, 3,
            TAG_UINT16, INJECT16(0)
};

const uint8_t Billing_Profile_Sort_Object[] =
{
    16 + 1,
        4,
            TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
            TAG_OCTET_STRING, 6, 0, 0, 1, 0, 0, 255, // Date & Time
            TAG_INT8, 3,
            TAG_UINT16, INJECT16(0)
};

const uint8_t Load_Scaler_Profile_Capture_Objects[] =
{
    INJECT16(0x8000 | (11*18 + 1)),
        /*TAG_ARRAY,*/ 11,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, 0, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 31, 27, 0, 255, // Current-Ir
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 51, 27, 0, 255, // Current-Iy
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 71, 27, 0, 255, // Current-Ib
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 32, 27, 0, 255, // Voltage-Vr
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 52, 27, 0, 255, // Voltage-Vy
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 72, 27, 0, 255, // Voltage-Vb
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 1, 29, 0, 255, // Block Energy KWh
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 5, 29, 0, 255, // Block Energy KVArh - Lag
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 8, 29, 0, 255, // Block Energy KVArh - Lead
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 9, 29, 0, 255, // Block Energy KVAh 
                TAG_INT8, 3,
                TAG_UINT16, INJECT16(0)
};

const uint8_t Billing_Profile_Capture_Objects[] =
{
    INJECT16(0x8000 | (9*18 + 1)),
        9,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),    // Current Time
                TAG_OCTET_STRING, 6, 0, 0, 1, 0, 0, 255, 
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),               
            TAG_STRUCTURE, 4,                           // Active Energy Import
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 1, 8, 10, 255, 
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                           //Active Energy Export
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 2, 8, 10, 255, 
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                           //Reqctive Energy QI
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 5, 8, 10, 255, 
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                           //Reactive Energy QII
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 6, 8, 10, 255, 
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                           //Reqctive Energy QIII
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 7, 8, 10, 255, 
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                           //Reqctive Energy QIV
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 8, 8, 10, 255, 
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0), 
            TAG_STRUCTURE, 4,                           //Max Demand Value
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 1, 6, 10, 255, 
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                           //Max Demand Date Time
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, 1, 0, 1, 6, 10, 255, 
                TAG_INT8, 5,
                TAG_UINT16, INJECT16(0),                
};


const uint8_t Load_Scaler_Profile_Sort_Object[] =
{
    16 + 1,
        4,
            TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
            TAG_OCTET_STRING, 6, 0, 0, 1, 0, 0, 255, // Date & Time
            TAG_INT8, 3,
            TAG_UINT16, INJECT16(0)
};

const uint32_t Instant_Profile_Entries_In_Use = 1;
const uint32_t Instant_Profile_Entries = 1;
const uint32_t Instant_Profile_Capture_Period = 1;
const uint8_t Instant_Profile_Sort_Method = 1;

const uint32_t Instant_Scaler_Profile_Entries_In_Use = 1;
const uint32_t Instant_Scaler_Profile_Entries = 1;
const uint32_t Instant_Scaler_Profile_Capture_Period = 1;
const uint8_t Instant_Scaler_Profile_Sort_Method = 1;

uint32_t Load_Profile_Entries_In_Use = 5;
uint32_t Load_Profile_Entries = 5;
uint32_t Load_Profile_Capture_Period = 1;
uint8_t Load_Profile_Sort_Method = 1;


uint32_t Instantaneous_Energy_Entries_In_Use = 1;
uint32_t Instantaneous_Energy_Entries = 1;
uint32_t Instantaneous_Energy_Capture_Period = 1;
uint8_t Instantaneous_Energy_Sort_Method = 1;

uint32_t Instantaneous_Values_Entries_In_Use = 1;
uint32_t Instantaneous_Values_Entries = 1;
uint32_t Instantaneous_Values_Capture_Period = 1;
uint8_t Instantaneous_Values_Sort_Method = 1;


const uint32_t Load_Scaler_Profile_Entries_In_Use = 1;
const uint32_t Load_Scaler_Profile_Entries = 1;
const uint32_t Load_Scaler_Profile_Capture_Period = 1;
const uint8_t Load_Scaler_Profile_Sort_Method = 1;

const uint32_t Billing_Profile_Entries_In_Use = 6;
const uint32_t Billing_Profile_Entries = 6;
const uint32_t Billing_Profile_Capture_Period = 1;
const uint8_t Billing_Profile_Sort_Method = 1;

/* OBJECT DEFINITIONS OF MANDATORY OBJECTS */

/* Security object for meter reader association */
void security_activate(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void global_key_transfer(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

/* Security object for utility settings association */

/* Clock object */
void adjust_to_quarter(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void adjust_to_measuring_period(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void adjust_to_minute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void adjust_to_preset_time(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void preset_adjusting_time(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void shift_time(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
/* Association object */
void reply_to_hls_auth(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void reply_to_hls_auth_MS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void change_hls_secret(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void change_hls_secret_NS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void change_hls_secret_LS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void change_hls_secret_HS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void add_object(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void remove_object(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void tariff_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void Disconnect_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void GlobalReset_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void insert_special_day(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void delete_special_day(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void remote_reconnect(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void remote_disconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);
void pre_remote_disconnect(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void pre_remote_reconnect(uint8_t data,uint16_t data_len,uint8_t response,uint16_t response_len);

void ChangeReferanceVoltage(void *data,int data_direction);
void changeTimeThresholdLongPowerFaile(void *data,int data_direction);
void changeTimeThresholdVoltageSwell(void *data,int direction);
void changeTimeThresholdVoltageSag(void *data,int direction);
void changeThresholdVoltageSwell(void *data,int data_direction);
void changeThresholdVoltageSag(void *dcata,int data_direction);
void changeThresholdLongPowerFaile(void *data,int data_direction);
void changeExportActivePowerThreshold(void *data,int data_direction);
void changeImportActivePowerThreshold(void *data,int data_direction);
void changeNumberOfLongPowerFailes(void *data,int data_direction);
void changeCumulativeTamperCount(void *data,int data_direction);
void changeLoadProfileCapturePeriod(void *data,int data_direction);

void changeMeterSerialNumber(void *data,int data_direction);
void changeDeviceId2(void *data,int data_direction);
void changeDeviceId3(void *data,int data_direction);
void changeDeviceId4(void *data,int data_direction);
void changeDeviceId5(void *data,int data_direction);
void changeDeviceId6(void *data,int data_direction);
void changeDeviceId7(void *data,int data_direction);
void changeDeviceId8(void *data,int data_direction);
extern int8_t compare_energy(void *energy0,void *energy1);
extern int8_t compare_event(void *event0,void *event1);
rtc_t required_last_entry_time;
Search_t search_item;


uint8_t find_num_energy_log__entries_between(const sSA_Range *startRange,const sSA_Range *endRange,
                                                   uint16_t *startEntryNumber,uint16_t *numOfEntries)
{
     rtc_t temp_sRange ={startRange->Year,startRange->Month,startRange->Date,startRange->Hr,startRange->Min,0,0};
     rtc_t temp_eRange ={endRange->Year,endRange->Month,endRange->Date,endRange->Hr,endRange->Min,0,0};   
     int8_t temp1 = compare_time(&temp_eRange,&temp_sRange); // if the end time specified comes before the first time requested, then abort
     if(temp1 < 0)
     {
       *startEntryNumber = 0;
       *numOfEntries = 0;
       return 1;
     }
     
     uint16_t MAX_Entries = 0;
     uint32_t add_start = EnergyLogAddress_Start;
          
    //confirm this with the number of entries we have 
     if(LastEnergyLogAddress>EnergyLogAddress_Start)//if we have entries 
     {
         
         if(status.energy_log_overlaped == 1) // if the cirular buffer is full
         {
           MAX_Entries = EnergyLog_SIZE;
         }
         else{
            MAX_Entries = (LastEnergyLogAddress - add_start)/EnergyLogSize; 
         }
         
         
         EnergyLog firstEntery,lastEntry;  
          uint8_t z = getEnergy2(&firstEntery,1); //get our first entry     
          if(z == 0)
          {
            //error abort 
          }
          
          rtc_t time_first = getTime(&firstEntery.timeStump); 
          temp1 = compare_time(&temp_eRange,&time_first);//if the first entry we have is is after the last entry requested then we don't have the data
          if(temp1 < 0)
          {
             *startEntryNumber = 0;
             *numOfEntries = 0;
             return 1;
          }
                             
          int8_t com2 = compare_time(&time_first,&temp_sRange);
          if(com2 < 0)//if time_first comes before temp_sRange 
          {
            //search for the entry number for temp_sRange;
            
                //Search_t search_item;
                search_item.start_entry = 0;//startEntryNumber;
                search_item.end_entry = MAX_Entries;//search_item.start_entry +  *numOfEntries;
                //search_item.SingleItemSize = EnergyLogSize;
                EnergyLog l;
                EnergyLog l2;
                l2.timeStump = getTimeStamp(temp_sRange.year,temp_sRange.month,temp_sRange.day,temp_sRange.hour,temp_sRange.minute,temp_sRange.second);
      //          rtc_t time;
      //          search_item.current_c_data = &time;
                search_item.current_data = &l;
               // search_item.StartAddress = EnergyLogAddress_Start;
                search_item.PeakItem = &getEnergy2;
                search_item.Compare = &compare_energy;
      //          search_item.getComapareItem = &getEnergyComapareItem;
                search_item.search_data = &l2;//&temp_sRange;
                
                uint32_t sa =  search_nearest_log(&search_item);
      //          find the actual last entry 
      //          check if this entry is with in the start and end range 
                
                if(sa ==0)
                {
                *startEntryNumber = search_item.last_entry_no;
                }
                else 
                {
                  *startEntryNumber = sa;
                }
          }
          else
          {
             *startEntryNumber = 1;
          }
          
          
          z = getEnergy2(&lastEntry,MAX_Entries);//get the last entry  
          if(z == 0)
          {
                //error abort 
          }              
          rtc_t time_last = getTime(&lastEntry.timeStump);   
          int8_t com3 = compare_time(&time_last,&temp_eRange);
          
          
          
          if(com3 >= 0)//temp_eRange comes before  time_last
          {
            //search for temp_eRange
            
            void *t1 = &required_last_entry_time;
            void *t2 = &temp_eRange;
            memcpy(t1,t2,sizeof(required_last_entry_time));
            if(com2<0)
              *numOfEntries = (uint32_t)(getTimeDifferenceInMinutes(&temp_eRange,&temp_sRange)/EnergyLoggingTime);
            else
              *numOfEntries = (uint32_t)(getTimeDifferenceInMinutes(&temp_eRange,&time_first)/EnergyLoggingTime);
          }
          else
          {
            //last entry is the last number of entry 
            
            void *t1 = &required_last_entry_time;
            void *t2 = &time_last;
            memcpy(t1,t2,sizeof(time_last));
            if(com2 < 0)
              *numOfEntries = (uint32_t)(getTimeDifferenceInMinutes(&time_last,&temp_sRange)/EnergyLoggingTime);
            else
              *numOfEntries = (uint32_t)(getTimeDifferenceInMinutes(&time_last,&time_first)/EnergyLoggingTime);
          }
          
          
          if((*startEntryNumber*EnergyLogSize) + EnergyLogAddress_Start  > LastEnergyLogAddress)
             *numOfEntries = 0;
          else{ 
             *numOfEntries = *numOfEntries>MAX_Entries?MAX_Entries:*numOfEntries;
          }             
     }
     else
     {
       *startEntryNumber =0;
       *numOfEntries = 0;
     }
      return 1;//success
}

//uint32_t MAX_Entries = 0;
uint8_t find_num_event_log__entries_between(const sSA_Range *startRange,const sSA_Range *endRange,
                                                   uint16_t *startEntryNumber,uint16_t *numOfEntries)
{
     rtc_t temp_sRange ={startRange->Year,startRange->Month,startRange->Date,startRange->Hr,startRange->Min,0,0};
     rtc_t temp_eRange ={endRange->Year,endRange->Month,endRange->Date,endRange->Hr,endRange->Min,0,0};   
     int8_t temp1 = compare_time(&temp_eRange,&temp_sRange); // if the end time specified comes before the first time requested, then abort
     if(temp1 < 0)
     {
       *startEntryNumber = 0;
       *numOfEntries = 0;
       return 1;
     }
     
     uint16_t MAX_Entries = 0;
     uint32_t add_start = EventLogAddress_Start;
          
    //confirm this with the number of entries we have 
     if(LastEventLogAddress>EventLogAddress_Start)//if we have entries 
     {
         
         if(status.event_log_overlaped == 1) // if the cirular buffer is full
         {
           MAX_Entries = EventLog_SIZE;
         }
         else{
            MAX_Entries = (LastEventLogAddress - add_start)/EventLogSize; 
         }
         
         
         EventLog firstEntery,lastEntry;  
          uint8_t z = getEvent2(&firstEntery,1); //get our first entry     
          if(z == 0)
          {
            //error abort 
          }
          
          rtc_t time_first = getTime(&firstEntery.timeStump); 
          temp1 = compare_time(&temp_eRange,&time_first);//if the first entry we have is is after the last entry requested then we don't have the data
          if(temp1 < 0)
          {
             *startEntryNumber = 0;
             *numOfEntries = 0;
             return 1;
          }
                             
          int8_t com2 = compare_time(&time_first,&temp_sRange);
          if(com2 < 0)//if time_first comes before temp_sRange 
          {
            //search for the entry number for temp_sRange;
            
                //Search_t search_item;
                search_item.start_entry = 0;//startEntryNumber;
                search_item.end_entry = MAX_Entries;//search_item.start_entry +  *numOfEntries;
                //search_item.SingleItemSize = EnergyLogSize;
                EventLog l;
                EventLog l2;
                l2.timeStump = getTimeStamp(temp_sRange.year,temp_sRange.month,temp_sRange.day,temp_sRange.hour,temp_sRange.minute,temp_sRange.second);
      //          rtc_t time;
      //          search_item.current_c_data = &time;
                search_item.current_data = &l;
               // search_item.StartAddress = EnergyLogAddress_Start;
                search_item.PeakItem = &getEvent2;
                search_item.Compare = &compare_event;
      //          search_item.getComapareItem = &getEnergyComapareItem;
                search_item.search_data = &l2;//&temp_sRange;
                
                uint32_t sa =  search_nearest_log(&search_item);
      //          find the actual last entry 
      //          check if this entry is with in the start and end range 
                
                if(sa ==0)
                {
                *startEntryNumber = search_item.last_entry_no;
                }
                else 
                {
                  *startEntryNumber = sa;
                }
          }
          else
          {
             *startEntryNumber = 1;
          }
          
          
          z = getEvent2(&lastEntry,MAX_Entries);//get the last entry  
          if(z == 0)
          {
                //error abort 
          }              
          rtc_t time_last = getTime(&lastEntry.timeStump);   
          int8_t com3 = compare_time(&time_last,&temp_eRange);
          
          
          //Correct for this forward ??????????????????????????????????
          if(com3 >= 0)//temp_eRange comes before  time_last
          {
            //search for temp_eRange
            void *t1 = &required_last_entry_time;
            void *t2 = &temp_eRange;
            memcpy(t1,t2,sizeof(required_last_entry_time));
             
          }
          else
          {
            //last entry is the last number of entry 
            void *t1 = &required_last_entry_time;
            void *t2 = &time_last;
            memcpy(t1,t2,sizeof(time_last));
          }
           *numOfEntries = MAX_Entries;
     }
     else
     {
       *startEntryNumber =0;
       *numOfEntries = 0;
     }
      return 1;//success
}



uint8_t find_num_total_Standard_Event_Log_entries(uint16_t *num_entries,uint16_t *start_entry)
{
   //count the total number of entries we have 
       /*get the last energy log address 
         divide it by the size of energy log struct 
          return this number  
       */
  
      *start_entry = 1;
      *num_entries = 0;
      
      if(status.event_log_overlaped == 1)
      {
        *num_entries = EventLog_SIZE;
      }
      else
      {
        int32_t x=0;
        uint32_t Add_Start = EventLogAddress_Start;
        if(LastEventLogAddress > Add_Start)
          x = LastEventLogAddress - Add_Start;
        
        while(x > 0)
        {
            x -= EventLogSize;
            *num_entries= *num_entries + 1;
        }
      }
      
      if(*num_entries>0)
        *start_entry = 1;

   
  return 1;//success
}

uint8_t find_num_total_energy_log_entries(uint16_t *num_entries,uint16_t *start_entry)
{
   //count the total number of entries we have 
       /*get the last energy log address 
         divide it by the size of energy log struct 
          return this number  
       */
  
   *start_entry = 0;
   *num_entries = 0;
   int32_t x=0;
   
   if(status.event_log_overlaped == 1)
   {
     *num_entries = EnergyLog_SIZE; // the buffer is full, so return all Logs
   }
   else{
   
     uint32_t add_start = EnergyLogAddress_Start;
     if(LastEnergyLogAddress > add_start)
       x = LastEnergyLogAddress - add_start;
     
     while(x > 0)
     {
            x -= EnergyLogSize;
            *num_entries= *num_entries + 1;
     }
   }
   if(*num_entries > 0)
     *start_entry = 1;
//   *num_entries = (LastEnergyLogAddress - EnergyLogAddress_Start) / EnergyLogSize;
   
  return 1;//success
}

void Capture_Instantaneous_Profile_Data(void *data,int direction)
{
   /* Load Template for Load Profile */ 
   msg_info.template = Instantanious_Profile_Buffer_Template2;
   msg_info.sz_template = sizeof(Instantanious_Profile_Buffer_Template2);
   msg_info.num_entries=1;
   msg_info.start_entry=1;
   msg_info.column_szs  = Instantanious_Profile_Column_Szs2;
}

void Capture_Instantaneous_EnergyProfile_Data(void *data,int direction)
{
   /* Load Template for Load Profile */   
   msg_info.template = Instantanious_EnergyProfile_Buffer_Template2;
   msg_info.sz_template = sizeof(Instantanious_EnergyProfile_Buffer_Template2);
   msg_info.num_entries=1;
   msg_info.start_entry=1;
   msg_info.column_szs  = Instantanious_EnergyProfile_Column_Szs2;
}



void Capture_Standard_Event_Log(void *data, int direction)
{
    /* Load Template for Load Profile */
   // interpret_template(data, direction, Load_Profile_Buffer_Template, sizeof(Load_Profile_Buffer_Template));
   msg_info.template=Standard_Event_Log_Template;
   msg_info.sz_template=sizeof(Standard_Event_Log_Template);
    if(access_selector==1)
    {  
        if(SA_Range[1].Year>4095 || SA_Range[0].Year>4095 )
           msg_info.num_entries=0x0;
        else
           find_num_event_log__entries_between(&SA_Range[0],&SA_Range[1],&msg_info.start_entry,&msg_info.num_entries);/*msg_info.num_entries=5*/ 
    }
    else if(access_selector==2)
    {
        find_num_total_Standard_Event_Log_entries(&msg_info.num_entries,&msg_info.start_entry); 
       if(SA_From_Entry <= SA_To_Entry)
       {
          msg_info.start_entry = msg_info.start_entry > SA_From_Entry? msg_info.start_entry :SA_From_Entry;
          SA_To_Entry -=  SA_From_Entry;
          msg_info.num_entries = msg_info.num_entries > SA_To_Entry? (SA_To_Entry):msg_info.num_entries;
       }
       else
       {

       }
    }
    else
    {
        find_num_total_Standard_Event_Log_entries(&msg_info.num_entries,&msg_info.start_entry); 
    }    
      msg_info.column_szs=Standard_Event_Log_Column_Szs;  
}
extern EnergyLog LastTxEnergyCopy;
void Capture_Load_Profile_Data(void *data, int direction)
{
    /* Load Template for Load Profile */ 
   msg_info.template=Load_Profile_Buffer_Template;
   msg_info.sz_template=sizeof(Load_Profile_Buffer_Template);
    
   
   
   if(access_selector == 1) //range_descriptor specifically by date range
   {     
     if(SA_Range[1].Year>4095 || SA_Range[0].Year>4095 )
         msg_info.num_entries=0x0;
     else
        find_num_energy_log__entries_between(&SA_Range[0],&SA_Range[1],&msg_info.start_entry,&msg_info.num_entries);/*msg_info.num_entries=5*/ 
   }
   else if(access_selector == 2) //entry descriptor
   {     
//       SA_From_Entry;
//       SA_To_Entry;
       find_num_total_energy_log_entries(&msg_info.num_entries,&msg_info.start_entry); 
       if(SA_From_Entry <= SA_To_Entry)
       {
          msg_info.start_entry = msg_info.start_entry > SA_From_Entry? msg_info.start_entry :SA_From_Entry;
          SA_To_Entry -=  SA_From_Entry;
          msg_info.num_entries = msg_info.num_entries > SA_To_Entry? (SA_To_Entry):msg_info.num_entries;
       }
       else
       {

       }
   }
   else//   if(access_selector == 0)
   {
      find_num_total_energy_log_entries(&msg_info.num_entries,&msg_info.start_entry); 
   }
   
   getEnergy2(&LastTxEnergyCopy,msg_info.start_entry-1);
   //find_num_total_energy_log_entries(&msg_info.num_entries,&msg_info.start_entry);
   msg_info.column_szs=Load_Profile_Column_Szs;  
}



uint8_t pass_temp[17];

//TODO. Test this 
void autentication_secreate_LS(void *data, int direction)
{
   uint8_t *datap = data;
   if(direction == ATTR_READ)
   {
       //according to the current grid's specification this is 
       //an eight bit lenght string 
       memcpy(datap,(Passwords[1].password+1),Passwords[1].password[1]);
   }
   else // write 
   {
     //assume that the format is according to the octat string format i.e. len, data 
     uint8_t len = (*datap++);
     
     if(len > 16) //error 
     {
       //
     }
     else 
     {
       uint8_t i =1;
       Passwords[i].password[i] = len;
       memcpy((Passwords[i].password+2),datap,len);
       
       //update the EEPROM 
       //TODO. ENCRIPT before writting. There is already a method that encripts and writes to EEPROM in logger module 
       //refactor and use it.
       write_to_eeprom(&Passwords[i],&i,setPassword);
//       setPassword(&Passwords[1],1);//TODO encript before setting the password
     }
   }
}





void Capture_Billing_Profile_Data(void *data, int direction)
{
    msg_info.template=Billing_Profile_Buffer_Template;
    msg_info.sz_template=sizeof(Billing_Profile_Buffer_Template);
    
    msg_info.num_entries=1;
    msg_info.start_entry=1;
    
    msg_info.column_szs=Billing_Profile_Column_Szs;    
}


static const struct attribute_desc_s clock_attrs[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[0].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *)time_string, get_date_time_string},//,exchange_date_time
    {3, ACCESS_PCR__MRR__USRW, TAG_INT16,           (void *) &clock_time_zone, NULL},
    {4, ACCESS_PCR__MRR__USR_, TAG_UINT8,           (void *) &clock_status, NULL},
    {5, ACCESS_PCR__MRR__USRW, TAG_OCTET_STRING,    (void *) time_string, NULL},
    {6, ACCESS_PCR__MRR__USRW, TAG_OCTET_STRING,    (void *) time_string, NULL},
    {7, ACCESS_PCR__MRR__USRW, TAG_INT8,            (void *) &clock_dst_deviation, NULL},
    {8, ACCESS_PCR__MRR__USRW, TAG_BOOLEAN,         (void *) &clock_dst_enabled, NULL},
    {9, ACCESS_PCR__MRR__USR_, TAG_ENUM,            (void *) &clock_base, NULL},
};


static const struct method_desc_s ln_40_methods_NS[] =
{
    {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
    {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_NS},
    {3, ACCESS_PC___MR___USR_, add_object},
    {4, ACCESS_PC___MR___USR_, remove_object}
};
static const struct method_desc_s ln_40_methods_LS[] =
{
    {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
    {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_LS},
    {3, ACCESS_PC___MR___USR_, add_object},
    {4, ACCESS_PC___MR___USR_, remove_object}
};
static const struct method_desc_s ln_40_methods_HS[] =
{
    {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
    {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_HS},
    {3, ACCESS_PC___MR___USR_, add_object},
    {4, ACCESS_PC___MR___USR_, remove_object}
};
static const struct method_desc_s ln_40_methods_HS_FC[] =
{
    {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
    {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_HS},
    {3, ACCESS_PC___MR___USR_, add_object},
    {4, ACCESS_PC___MR___USR_, remove_object}
};


const uint8_t auth_mech_name_NS[]={0x07,0x60,0x85,0x74,0x05,0x08,0x02,0x00};
const uint8_t auth_mech_name_LS[]={0x07,0x60,0x85,0x74,0x05,0x08,0x02,0x01};
const uint8_t auth_mech_name_HS[]={0x07,0x60,0x85,0x74,0x05,0x08,0x02,0x07};


/*
  public client association 
*/
static const struct attribute_desc_s ln_40_attrs_NS[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[2].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
    {3, ACCESS_PCR__MRR__USR_, TAG_STRUCTURE,       (void *) partner_NS, NULL},
    {4, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
    {5, ACCESS_PCR__MRR__USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
    {6, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_NS, NULL},
    {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[0].password +1), NULL},
    {8, ACCESS_PCR__MRR__USR_, TAG_ENUM,            (void *) &uint8val, NULL}
//    {9, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) security_setup_reference, NULL}
};

/*
  Read client association
*/
static const struct attribute_desc_s ln_40_attrs_LS[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[3].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) partner_LS, NULL},
    {4, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
    {5, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
    {6, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_LS, NULL},
    {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[1].password+1), autentication_secreate_LS},
    {8, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &uint8val, NULL}
//    {9, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) security_setup_reference, NULL}
};

/*
 utility setting (management client association 
*/
static const struct attribute_desc_s ln_40_attrs_HS[] =
{
    {1, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) object_list[4].instance_id, NULL},
    {2, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
    {3, ACCESS_PC___MR___USR_, TAG_STRUCTURE,       (void *) partner_HS, NULL},
    {4, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
    {5, ACCESS_PC___MR___USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
    {6, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_HS, NULL},
    {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[2].password+1), NULL},
    {8, ACCESS_PC___MR___USR_, TAG_ENUM,            (void *) &uint8val, NULL}
//    {9, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) security_setup_reference, NULL}
};

/*
  Firmware client association 
*/
static const struct attribute_desc_s ln_40_attrs_HS_FC[] =
{
    {1, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) object_list[5].instance_id, NULL},
    {2, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
    {3, ACCESS_PC___MR___USR_, TAG_STRUCTURE,       (void *) partner_HS, NULL},
    {4, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
    {5, ACCESS_PC___MR___USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
    {6, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_HS, NULL},
    {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[3].password+1), NULL},
    {8, ACCESS_PC___MR___USR_, TAG_ENUM,            (void *) &uint8val, NULL}
//    {9, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) security_setup_reference, NULL}
};




/* Provides the list of known SAPs */
static const struct attribute_desc_s sap_41_attrs[] =
{
    {1, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) object_list[6].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR__MSR_, TAG_ARRAY,           (void *) sap_list, NULL},
};




/* OBJECT DEFINITIONS OF COMMUNICATION OBJECTS */

static const struct attribute_desc_s Obj_HDLC_Setup[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[7].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &Comm_Speed, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_UINT8,           (void *) &Window_Size_Transmit, NULL},
    {4, ACCESS_PC___MRR__USR_, TAG_UINT8,           (void *) &Window_Size_Receive, NULL},
    {5, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &Max_Info_Field_Len_TX, NULL},
    {6, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &Max_Info_Field_Len_RX, NULL},
    {7, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &Inter_Octet_Timeout, NULL},
    {8, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &Inactivity_Time_Out, NULL},
    {9, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &Device_Address, NULL},
};


static const struct attribute_desc_s Obj_DeviceIdentifier[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START + 1].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_OCTET_STRING,    (void *) &DeviceIdentifier, getSetDeviceIdentifier}
}; 
static const struct attribute_desc_s Obj_Meter_Sr_No[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+2].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Meter_Sr_No, changeMeterSerialNumber},//changeMeterSerialNumber
};
static const struct attribute_desc_s Obj_Device_ID_2[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+3].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_2, changeDeviceId2},
};

static const struct attribute_desc_s Obj_Device_ID_3[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+4].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_3, changeDeviceId3},
};

static const struct attribute_desc_s Obj_Device_ID_4[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+5].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_4, changeDeviceId4},
};
static const struct attribute_desc_s Obj_Device_ID_5[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+6].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_5, changeDeviceId5},
};
static const struct attribute_desc_s Obj_Device_ID_6[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+7].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_6, changeDeviceId6},
};
static const struct attribute_desc_s Obj_Device_ID_7[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+8].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_7, changeDeviceId7},
};
static const struct attribute_desc_s Obj_Device_ID_8[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+9].instance_id, NULL},
    {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_8, changeDeviceId8},
};
static const struct attribute_desc_s Obj_Currently_Active_Tariff[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START + 10].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &CurrentActiveTariff, NULL}
};
static const struct  attribute_desc_s Obj_TransformerRatio_VoltageNum[]=
{
  {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START + 11].instance_id, NULL},
  {2, ACCESS_PCR__MRR__USRW, TAG_UINT16,    (void *)&TransformerRatio_VoltageNum, NULL},
};
static const struct  attribute_desc_s Obj_TransformerRatio_VoltageDen[]=
{
  {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START + 12].instance_id, NULL},
  {2, ACCESS_PCR__MRR__USRW, TAG_UINT16,    (void *) &TransformerRatio_VoltageDen, NULL},
};
static const struct  attribute_desc_s Obj_TransformerRatio_CurrentNum[]=
{
  {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+13].instance_id, NULL},
  {2, ACCESS_PCR__MRR__USRW, TAG_UINT16,    (void *) &TransformerRatio_CurrentNum, NULL},
};
static const struct  attribute_desc_s Obj_TransformerRatio_CurrentDen[]=
{
  {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+14].instance_id, NULL},
  {2, ACCESS_PCR__MRR__USRW, TAG_UINT16,    (void *) &TransformerRatio_CurrentDen, NULL},
};
static const struct  attribute_desc_s Obj_Clock_Time_Shift_Event_Limit[]=
{
  {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+15].instance_id, NULL},
  {2, ACCESS_PCR__MRR__USRW, TAG_UINT8,    (void *) &MaxAllowedTimeShift, NULL},
};
static const struct  attribute_desc_s Obj_ExportActivePowerOverThrStatus[] =
{
  {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+16].instance_id, NULL},
  {2, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,    (void *) &ExportActivePowerOverThrStatus, NULL},  
};
static const struct  attribute_desc_s Obj_ImportActivePowerOverThrStatus[] =
{
  {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+17].instance_id, NULL},
  {2, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,    (void *) &ImportActivePowerOverThrStatus, NULL},  
};
static const struct  attribute_desc_s Obj_DateTime_Format[]=
{
  {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+18].instance_id, NULL},
  {2, ACCESS_PC___MRR__USRW, TAG_OCTET_STRING,    (void *) DateTimeFormat, NULL},
};
static const struct attribute_desc_s Obj_Demand_Integration_Period[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+19].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT32,          (void *) &Demand_Integration_Period, NULL}
};
static const struct attribute_desc_s Obj_Profile_Capture_Period[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+20].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &EnergyLoggingTime, changeLoadProfileCapturePeriod}
};
/* Mandatory object - logical device name */
static const struct attribute_desc_s Obj_Phase_Presence[] =
{
    {1, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+21].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR__MSR_, TAG_UINT8,    (void *)&Phase_Presence, NULL},
};
static const struct attribute_desc_s Obj_FirmwareVersion[] =
{
    {1, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+22].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *)&FirmwareVersionStr,NULL },//changeFirmwareVersion
};
static const struct attribute_desc_s Obj_ErrorObject[] =
{
    {1, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+23].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR__MSR_, TAG_UINT32,    (void *)&ErrorValue,NULL }
};
static const struct attribute_desc_s Obj_Cum_Tamper_Count[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+24].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Cum_Tamper_Count, changeCumulativeTamperCount}
};
static const struct attribute_desc_s Obj_Cum_MD_Reset_Count[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+25].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Cum_MD_Reset_Count, NULL}
};
static const struct attribute_desc_s Obj_Cum_Prog_Count[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+26].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Cum_Prog_Count, NULL}
};
static const struct attribute_desc_s Obj_Last_MD_Rst_DT[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+27].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) Last_MD_Rst_DT, NULL}
};
static const struct attribute_desc_s Obj_NumberOfLongPowerFails[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+28].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT16,          (void *) &NumberOfLongPowerFails, changeNumberOfLongPowerFailes}
};
static const struct attribute_desc_s Obj_StandardEventItem[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+29].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &NumberOfLongPowerFails, NULL}
};
static const struct attribute_desc_s Obj_OpearatingMode[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+30].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_INT8,    (void *) &OperatingMode, getSetOperatingMode}
};
static const struct  attribute_desc_s Obj_ActiveQuadrant[] =
{
  {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+31].instance_id, NULL},
  {2, ACCESS_PCR__MRR__USRW, TAG_UINT8,    (void *) &ActiveQuadrant, NULL},  /*Active Quadrant from 0 to 4 */
};
//static const struct attribute_desc_s Obj_CurrentBalance[] =
//{
//    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+32].instance_id, NULL},
//    {2, ACCESS_PC___MRRW_USRW, TAG_INT32,    (void *) &Current_balance.balance, getSetCurrentBalance}
//};

static const struct attribute_desc_s Obj_MacAddress[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DATA_OBJECTS_START+32].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_OCTET_STRING,    (void *) &MACAddress, getSetMacAddress}
}; 


void activate_passive_calendar(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

static const struct method_desc_s Obj_Activity_Calendar_Methods[] =
{
    {1, ACCESS_PC___MRR__USR_, activate_passive_calendar}
};

static const struct attribute_desc_s Obj_Activity_Calendar[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[12].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_OCTET_STRING,    (void *) Calendar_Name_Active, NULL},
    {3, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_Season_Profile},
    {4, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_Week_Profile_Table},
    {5, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_Day_Profile_Table},
    {6, ACCESS_PC___MRRW_USRW, TAG_OCTET_STRING,    (void *) Calendar_Name_Passive, NULL},
    {7, ACCESS_PC___MRRW_USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_Season_Profile},
    {8, ACCESS_PC___MRRW_USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_Week_Profile_Table},
    {9, ACCESS_PC___MRRW_USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_Day_Profile_Table_Passive},
    {10,ACCESS_PC___MRRW_USRW, TAG_OCTET_STRING,    (void *) Activate_Passive_Calendar_Time, setGetActivePassiveCalendarTime}
};





static const struct method_desc_s Bbj_Special_days_Method[] =
{
    {1, ACCESS_PC___MRR__USR_, insert_special_day},
    {2, ACCESS_PC___MRR__USR_, delete_special_day}
};

static const struct method_desc_s Obj_GlobalReset_Script_Methods[] =
{
    {1, ACCESS_PC___MRR__USR_, GlobalReset_script_execute}
};

static const struct method_desc_s Obj_Disconnect_Script_Methods[] =
{
    {1, ACCESS_PC___MRRW_USRW, Disconnect_script_execute}
};

static const struct method_desc_s Obj_Tariff_Script_Methods[] =
{
    {1, ACCESS_PC___MRR__USR_, tariff_script_execute}
};
static const struct method_desc_s Obj_Connect_Methods[] =
{
    {1, ACCESS_PC___MRRW_USRW,  remote_disconnect},
    {2, ACCESS_PC___MRRW_USRW,  remote_reconnect}
};


static const struct attribute_desc_s Obj_Disconnet_Script_Table[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[15].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_Disconnect_Scripts}
};
static const struct attribute_desc_s Obj_GlobalReset_Script_Table[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[16].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_GlobalReset_Scripts}
};

static const struct attribute_desc_s Obj_Tariff_Script_Table[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[47].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, Access_Tariff_Scripts}
};    

static const struct attribute_desc_s Obj_Billing_Schedule[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[17].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_STRUCTURE,       (void *) Executed_Billing_Script, NULL},
    {3, ACCESS_PC___MRR__USRW, TAG_ENUM,            (void *) &Billing_Script_Type, NULL},
    {4, ACCESS_PCR__MRRW_USRW, TAG_ARRAY,           (void *) Data_Buffer,Access_Billing_Script_Execution_Time}
}; 

static const struct attribute_desc_s Obj_DiconnectControl_Schedule[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[18].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_STRUCTURE,       (void *) Executed_DiconnectControl_Script, NULL},
    {3, ACCESS_PC___MRR__USRW, TAG_ENUM,            (void *) &DiconnectControl_Type, NULL},
    {4, ACCESS_PCR__MRRW_USRW, TAG_ARRAY,           (void *) DiconnectControl_Script_Execution_Time,NULL}
};

void billing_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

static const struct method_desc_s Obj_Billing_Script_Methods[] =
{
    {1, ACCESS_PC___MRR__USR_, billing_script_execute}
};

static const struct attribute_desc_s Obj_Billing_Script_Table[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[14].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Billing_Script, NULL}
};

//extern void LogTamperStatus();
void reset_alarms(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  uint8_t type = *data++;//type is 18
  uint16_t x=0;
  x= *data << 8; 
  x |= *(++data);//x is the tamper type from 0 --- 3
  status.write_tamper_status = 1;
  *response_len = 0;
  switch(x)
  {
    case 1:
        status.UpperCoverRemovedTamperStatus = 0;
      return;
    case 2:
        status.LowerCoverRemovedTamperStatus = 0;
      return;
    case 3:
        status.MangneticTamperStatus = 0;
      return;
    case 4:
        status.NeutralTamperStatus = 0;
      return;
  }
  status.write_tamper_status = 0;
}
extern void updateCalibrationFactor(int16_t ErrorPercent,uint8_t type);
void calibratePMeter(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  uint8_t type = *data++;//type is 18
  int16_t x=0;
  x= *data << 8; 
  x |= *(++data);//x is the tamper type from 0 --- 3
  
  updateCalibrationFactor(x,0);

}
void calibrateIMeter(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  uint8_t type = *data++;//type is 18
  int16_t x=0;
  x= *data << 8; 
  x |= *(++data);//x is the tamper type from 0 --- 3

  updateCalibrationFactor(x,1);
  

}
void calibrateVMeter(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  uint8_t type = *data++;//type is 18
  int16_t x=0;
  x= *data << 8; 
  x |= *(++data);//x is the tamper type from 0 --- 3
//  x -=  1;
//  x= ~x;
  updateCalibrationFactor(x,2);
 
}
static const struct method_desc_s Obj_Reset_Methods[] =
{
    {1, ACCESS_PC___MRRW_USRW, reset_alarms}
};
static const struct method_desc_s Obj_CalibratePMeter_Methods[] =
{
    {1, ACCESS_PC___MRRW_USRW, calibratePMeter}
   // {2, ACCESS_PC___MRRW_USRW, calibrateIMeter},
   // {3, ACCESS_PC___MRRW_USRW, calibrateVMeter}
};

static const struct method_desc_s Obj_CalibrateCMeter_Methods[] =
{
    {1, ACCESS_PC___MRRW_USRW, calibrateIMeter}
 };

static const struct method_desc_s Obj_CalibrateVMeter_Methods[] =
{
   {1, ACCESS_PC___MRRW_USRW, calibrateVMeter}
};
/* OBJECT DEFINITIONS OF INSTANTANEOUS PARAMETERS */
static const struct attribute_desc_s ObjClock_Time_Shift_Event_Limit[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START+1].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT8,           (void *) &TimeShiftEvtLimVal, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time_Shift_EvtLim, NULL}
};
static const struct attribute_desc_s Obj_Clock_Time_Shift_Invalid_Event[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START+2].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT16,           (void *) &ClockTimeShiftInvalidLim, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time_Shift_EvtLim, NULL}
};
static const struct attribute_desc_s Obj_Active_Demand_Control_Threshold[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 3].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT32,           (void *) &Active_Demand_Control_Threshold, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KW, NULL}
};
static const struct attribute_desc_s Obj_TimeThresholdVoltageSag[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 4].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,           (void *) &UnderVoltageTripDuration, changeTimeThresholdVoltageSag},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time, NULL}
};
static const struct attribute_desc_s Obj_TimeThresholdVoltageSwell[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 5].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT16,           (void *) &OverVoltageTripDuration, changeTimeThresholdVoltageSwell},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time, NULL}
};
static const struct attribute_desc_s Obj_Demand_close_to_contract_power[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 6].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT16,           (void *) &ThrusholdPowerDemandCloseToContractPower, NULL},//TODO. impiment set getmethod for this object
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_dem_colseToContractPower, NULL}
};
static const struct attribute_desc_s Obj_TimeThresholdLongPowerFaile[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 7].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,           (void *) &TimeThresholdLongPowerFaile, changeTimeThresholdLongPowerFaile},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time_Shift_EvtLim, NULL}
};
static const struct attribute_desc_s Obj_ThresholdVoltageSage[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 8].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,           (void *) &ThrusholdVoltageSagPer, changeThresholdVoltageSag},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_ThresholdVoltageSag_Swell, NULL}
};
static const struct attribute_desc_s Obj_ThresholdVoltageSwell[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 9].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,           (void *) &ThrusholdVoltageSwellPer, changeThresholdVoltageSwell},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_ThresholdVoltageSag_Swell, NULL}
};
static const struct attribute_desc_s Obj_ThresholdLongPowerFaile[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 10].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,           (void *) &VoltageThresholdLongPowerFailePer, changeThresholdLongPowerFaile},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_ThresholdVoltageSag_Swell, NULL}
};
static const struct attribute_desc_s Obj_Instantanious_Current_L1[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 11].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.I_rms, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_I, NULL}
};  
static const struct attribute_desc_s Obj_Total_Active_Energy_Export[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 12].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->active_energy_export, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_total_active_energy_import, NULL}
};
static const struct attribute_desc_s Obj_Instantanious_Voltage_L1[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 13].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.V_rms , NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_V, NULL}
};
static const struct attribute_desc_s Obj_Voltage_Referance[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 14].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,           (void *) &Nominal_Voltage, ChangeReferanceVoltage},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_referance_voltage, NULL}
};
static const struct attribute_desc_s Obj_PF_R[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 15].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_INT16,           (void *) &phase->readings.power_factor, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_PF, NULL}
};
static const struct attribute_desc_s Obj_Scroll_Display_Time[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 16].instance_id, NULL},
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT32,           (void *) &scroll_display_time, changeDisplayScrollTime},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time, NULL}
};
static const struct attribute_desc_s Obj_NeutralCurrent[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 17].instance_id, NULL},
//#ifdef NEUTRAL_MONITOR_SUPPORT    
//    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &neutral.readings.I_rms, NULL},
//#else
    {2, ACCESS_PC___MRRW_USRW, TAG_UINT32,           (void *) &scroll_display_time, changeDisplayScrollTime},
//#endif 
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_I, NULL}
};
static const struct attribute_desc_s Obj_Frequency[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 18].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &phase->readings.frequency, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Frequency, NULL}
};
static const struct attribute_desc_s Obj_Active_Power_Positive[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 19].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &P_Active_Plus, NULL}, //check this 
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_active_power, NULL}
};
static const struct attribute_desc_s Obj_Total_Active_Energy_Import[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 20].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->active_energy_import, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_total_active_energy_import, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Energy_QI[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 21].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->readings.reactive_energy_Q1, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Energy_QII[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 22].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->readings.reactive_energy_Q2, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Energy_QIII[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 23].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->readings.reactive_energy_Q3, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Energy_QIV[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 24].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->readings.reactive_energy_Q4, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}
};
static const struct attribute_desc_s Obj_Active_Power_Negative[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 25].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &P_Active_Negative, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_active_power, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Power_Positive[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 26].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *)&P_Reactive_positive, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_active_power, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Power_Negative[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 27].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &P_Reactive_negative, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_active_power, NULL}
};
static const struct attribute_desc_s Obj_KVA_Total[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 28].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &P_Apparent, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KVA, NULL}
};
static const struct attribute_desc_s Obj_DurationOfAllLongPowerFailes[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 29].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT16,           (void *) &DurationOfAllLongPowerFailes, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KW, NULL}
};

static const struct attribute_desc_s Obj_ImportActivePowerThreshold[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 30].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT32,           (void *) &MAXActivePowerTripPoint, changeImportActivePowerThreshold},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KW, NULL}
};
static const struct attribute_desc_s Obj_ExportActivePowerThreshold[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 31].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_UINT32,           (void *) &MAXActivePowerExportTripPoint, changeExportActivePowerThreshold},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KW, NULL}
};
static const struct attribute_desc_s Obj_Instantanious_Power_Factor_Avarage[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 32].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_INT16,           (void *) &phase->readings.power_factor, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_PF, NULL}
};
static const struct attribute_desc_s Obj_ResetAlarm[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 33].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_INT16,           (void *) &phase->readings.power_factor, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_PF, NULL}
};
static const struct attribute_desc_s Obj_CalibratePMeter[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 34].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_INT16,           (void *) &nv_parms.seg_a.s.chan1.current.P_scale_factor[0], NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_power_current_voltage_calib, NULL} 
    //{3, ACCESS_PC___MRR__USR_, TAG_INT16,       (void *) &nv_parms.seg_a.s.chan1.current.I_rms_scale_factor, NULL},
   // {4, ACCESS_PC___MRR__USR_, TAG_INT16,       (void *) &nv_parms.seg_a.s.chan1.V_rms_scale_factor, NULL}
};

static const struct attribute_desc_s Obj_CalibrateCMeter[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 35].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_INT16,           (void *) &nv_parms.seg_a.s.chan1.current.I_rms_scale_factor, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_power_current_voltage_calib, NULL} 
};

static const struct attribute_desc_s Obj_CalibrateVMeter[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REGISTER_OBJECTS_START + 36].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_INT16,           (void *) &nv_parms.seg_a.s.chan1.V_rms_scale_factor, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_power_current_voltage_calib, NULL} 
};


static const struct attribute_desc_s Obj_Prime_PLC_FW_Version[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[8].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) &PLC_FW_Version, getSetFwVersion},
    {3, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &PLC_Vendor_ID, NULL},
    {4, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &PLC_Product_ID, NULL}
};
static const struct attribute_desc_s Obj_Prime_Device_Setup[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[9].instance_id, NULL},
    {2, ACCESS_PC___MRR__USRW, TAG_OCTET_STRING,    (void *) &MACAddress, getSetMacAddress}, /* of length 6*/

};







static const struct attribute_desc_s Obj_Special_Days[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[10].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_ARRAY,           (void *) &Data_Buffer, get_Special_Days_List},
};

//TODO: integrate the Output_state, control_state and control_mode with our meter 
static const struct attribute_desc_s Obj_DisconnectControl[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[11].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,           (void *) &Output_State, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_ENUM,       (void *) &control_state, NULL},
    {4, ACCESS_PC___MRR__USRW, TAG_ENUM,       (void *) &control_mode, NULL}

};

/**********************************************Added for SO2 support****************************************************/
static const struct attribute_desc_s Obj_Total_Active_Energy_Import_SO2[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 8].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->active_energy_import, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_total_active_energy_import, NULL}
};
static const struct attribute_desc_s Obj_Total_Active_Energy_Export_SO2[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 9].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->active_energy_export, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_total_active_energy_import, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Energy_QI_SO2[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 10].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->readings.reactive_energy_Q1, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Energy_QII_SO2[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 11].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->readings.reactive_energy_Q2, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Energy_QIII_SO2[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 12].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->readings.reactive_energy_Q3, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}
};
static const struct attribute_desc_s Obj_Reactive_Energy_QIV_SO2[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 13].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->readings.reactive_energy_Q4, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}
};
/************************************************************************************************************************/
/**********************************************Added for SO1 support****************************************************/
static const struct attribute_desc_s Obj_Previous_switch_State[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[11].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,         (void *) &previous_output_state, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_ENUM,       		(void *) &control_state, NULL},
    {4, ACCESS_PC___MRR__USRW, TAG_ENUM,       		(void *) &control_mode, NULL}

};
/************************************************************************************************************************/




/* OBJECT DEFINITIONS OF INSTANTANEOUS PARAMETERS PROFILE */

extern void Obj_Instant_Profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
extern void Obj_Instant_Profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
  



 

/* OBJECT DEFINITION OF BLOCK LOAD PARAMETERS PROFILE */

void Obj_Load_Profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void Obj_Load_Profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void Obj_Event_Log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void Obj_Event_Log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

extern uint32_t power_brakes[];
extern uint32_t ConsumptionSinceLastBilling;
extern int CheckTariff(const uint32_t *power_brakes,unsigned long *ConsumptionSinceLastBilling);
void Obj_IncrementBalance(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    uint8_t type = *data++;//type is 6
    uint32_t x=0;
    uint32_t temp=0;
    temp = *data;
    x  = temp << 24; 
    temp = (*(++data));
    x |= temp<<16;//x is the tamper type from 0 --- 3
    temp = (*(++data));
    x |= temp<<8;
    x |= *(++data);
    type++;  
    Current_balance.balance += x;
    write_to_eeprom(&Current_balance,(uint8_t *)0,setCurrentBalance);
    CheckTariff(power_brakes,&ConsumptionSinceLastBilling);
    *response_len = 0;
}
void Obj_DecrementBalance(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    uint8_t type = *data++;//type is 6
    uint32_t x=0;
    uint32_t temp=0;
    temp = *data;
    x  = temp << 24; 
    temp = (*(++data));
    x |= temp<<16;//x is the tamper type from 0 --- 3
    temp = (*(++data));
    x |= temp<<8;
    x |= *(++data);
    type++;
    Current_balance.balance -= x;
    write_to_eeprom(&Current_balance,(uint8_t *)0,setCurrentBalance);
    CheckTariff(power_brakes,&ConsumptionSinceLastBilling);
    *response_len = 0;
}


static const struct method_desc_s Obj_Load_Profile_Methods[] =
{
    {1, ACCESS_PC___MR___USR_, Obj_Load_Profile_reset},
    {2, ACCESS_PC___MR___USR_, Obj_Load_Profile_capture}
};



static const struct method_desc_s Obj_Event_Log_Methods[] =
{
    {1, ACCESS_PC___MR___USR_, Obj_Event_Log_reset},
    {2, ACCESS_PC___MR___USR_, Obj_Event_Log_capture}
};
static const struct method_desc_s Obj_Balance_Methods[] =
{
    {1, ACCESS_PC___MRRW_USRW, Obj_IncrementBalance},
    {2, ACCESS_PC___MRRW_USRW, Obj_DecrementBalance}
};

void Obj_Load_Scaler_Profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void Obj_Load_Scaler_Profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);



static const struct attribute_desc_s Obj_Billing_Profile[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 5].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, Capture_Billing_Profile_Data},
    {3, ACCESS_PC___MRR__USR_, TAG_ARRAY,           (void *) Billing_Profile_Capture_Objects,NULL},
    {4, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Billing_Profile_Capture_Period, NULL},
    {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &Billing_Profile_Sort_Method, NULL},
    {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) Billing_Profile_Sort_Object, NULL},
    {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Billing_Profile_Entries_In_Use, NULL},
    {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Billing_Profile_Entries, NULL},
};
static const struct attribute_desc_s Obj_StandardEventLog[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 2].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, Capture_Standard_Event_Log},
    {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Standard_Event_Log_Objects, NULL},
    {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &Load_Profile_Capture_Period, NULL},
    {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &Load_Profile_Sort_Method, NULL},
    {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) Load_Profile_Sort_Object, NULL},
    {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Load_Profile_Entries_In_Use, NULL},
    {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Load_Profile_Entries, NULL},
};
static const struct attribute_desc_s Obj_Instantaneous_Energy_Values[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 3].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, Capture_Instantaneous_EnergyProfile_Data},
    {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Instantaneous_Energy_Capture_Objects2, NULL},
    {4, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Instantaneous_Energy_Capture_Period, NULL},
    {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &Instantaneous_Energy_Sort_Method, NULL},
    {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) Instantaneous_Energy_Sort_Object, NULL},
    {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Instantaneous_Energy_Entries_In_Use, NULL},
    {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Instantaneous_Energy_Entries, NULL},
};
static const struct attribute_desc_s Obj_Instantaneous_Values_buff[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 4].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, Capture_Instantaneous_Profile_Data},
    {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Instantaneous_Values_Capture_Objects, NULL},
    {4, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Instantaneous_Values_Capture_Period, NULL},
    {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &Instantaneous_Values_Sort_Method, NULL},
    {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) Instantaneous_Values_Sort_Object, NULL},
    {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Instantaneous_Values_Entries_In_Use, NULL},
    {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Instantaneous_Values_Entries, NULL},
};
static const struct attribute_desc_s Obj_Load_Profile[] =
{
    {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 1].instance_id, NULL},
    {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, Capture_Load_Profile_Data},
    {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Load_Profile_Capture_Objects, NULL},
    {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &Load_Profile_Capture_Period, NULL},
    {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &Load_Profile_Sort_Method, NULL},
    {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) Load_Profile_Sort_Object, NULL},
    {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Load_Profile_Entries_In_Use, NULL},
    {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Load_Profile_Entries, NULL},
};

static const struct attribute_desc_s Obj_CurrentBalance2[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 6].instance_id, NULL},
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Current_balance.balance, NULL},
    {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_PF, NULL}
};


uint32_t image_first_not_transferred_block_number=124;
uint8_t image_transfer_enabled = 1;
/**
* NOT-TRANSFERED = 0;
* TRANSFERED = 1;
*/
uint8_t image_transferred_blocks_status;


enum image_transfer_status_t{
  
   TRANSFER_INITIATED=1,
   VERIFICATION_INITIATED,
   VERIFICATION_SUCCESSFUL,
   VERIFICATION_FAILED,
   ACTIVATION_INITIATED,
   ACTIVATION_SUCCESSFUL,
   ACTIVATION_FAILED,   
};
uint8_t image_transfer_status;

uint32_t image_block_size = 64;//let's assume a single image block size is 64 bytes.
uint8_t image_identification[]={6,1,2,3,4,5,6};
uint8_t image_signature []={6,1,2,3,4,5,6};

const uint8_t image_to_activate_info[] =
{
    INJECT16(0x8000 | (1*21 + 1)),
         1,
            TAG_STRUCTURE, 3,
                TAG_UINT32, INJECT32(ITEM_TAG_IMAGE_SIZE),
                STUFF_DATA|TAG_OCTET_STRING, 6, ITEM_TAG_IMAGE_IDENTIFICATION,2,3,4,5,6, 
                STUFF_DATA|TAG_OCTET_STRING, 6, ITEM_TAG_IMAGE_SIGNATURE,2,3,4,5,6               
};


/**
* the format of the image identifier is 
* dv_M_mm where M=>Major release number 
*              mm=>Minor release number
* this should be copied to the active firmware version after the new firmware is activated 
* and the active firmware version has a lenght of 5 
* so when coping ignore the '_' characters i.e 
*                                          the active firmware will be just dvMmm
* 
* according to the current DCU the image identifier will be transmitted as hex value 
* ascii which has 2x the size of the firmware version + null byte 
* so the size of the firmware version for our case will be 2*7 + 2 = 16
* Note: the file name of the image withou extension will be used as image identifier so the image file names should be 
* named as dv_1_00.hex   for firmware major version 1 and release 0
*/
uint8_t image_ident[16];
uint8_t image_ident_size=16;//let the max be 10
uint32_t image_size=0;
#define max_image_size 122880ul
void image_transfer_initiate(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    uint8_t type = *data++;//type is 2 struct 
    uint8_t size = *data++;//size 2
    image_ident_size = *data++; // octat string
    int i;
    for(i=0;i<image_ident_size;i++) //read image identifier
    {
      image_ident[i] = *data++;
    }
    writeImageIdentifier(image_ident,type);
    uint8_t size_type = *data++;//uint32_t 
    //read image size 
    uint32_t temp=0;
    temp = *data++;
    image_size  = temp << 24; 
    temp = (*data++);
    image_size |= temp<<16;
    temp = (*data++);
    image_size |= temp<<8;
    image_size |= *data++;
    
    //TODO. do something with the image size and the image identifier 
    //write the image identifier to the EEPROM and prepare the EEPROM for image transfer
    //set image transfer started indicator flag 
    //if the image size is above our max limit cancel the transfer
    //finally update the "image_transferred_status" variable with "image_transferred_status_t" initiated status 
    //if everything is fine so far
    if(image_size>max_image_size)
    {
       *response_len = 0;
       return;
    }
    writeImageSize(image_size,temp);   
    EEPROM_reformatImageTransferStatusBytes();
    image_transfer_status = TRANSFER_INITIATED;        
    *response_len = 0;
}
uint32_t previous_block_number;
uint32_t current_block_number=0;
uint8_t block_data[64];
/**
* checks for missed block numbers and updates the status to EEPROM
*/
void updateBlocksStatus()
{
  if(current_block_number > previous_block_number+1)
  {
    while(previous_block_number == current_block_number-1)
    {
       ++previous_block_number;
       updatateFirwmareBlockStatus(previous_block_number,0);//set this block as not recived
    }
  }
}


void image_block_transfer(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    //TODO.if image_transferred_blocks_status is initiated, accept blocks 
    //otherwise ignore them 
    if(image_transfer_status == TRANSFER_INITIATED)
    {
      uint8_t type = *data++;//type is 2 struct 
      uint8_t size = *data++;//size 2
      
      uint8_t blockNumberType = *data++;//uint32_t
      
      //read image size 
      uint32_t temp=0;
      temp = *data++;
      current_block_number  = temp << 24; 
      temp = (*data++);
      current_block_number |= temp<<16;
      temp = (*data++);
      current_block_number |= temp<<8;
      current_block_number |= *data++;
      
      //the data
      uint8_t block_data_type = *data++;//octate string
      uint8_t _block_size = *data++; // octate string should be 64, image_block_size
      
      //TODO write them directly to eeprom
      int i;
      for( i=0;i<_block_size;i++) //read block
      {
        block_data[i] = *data++;
      }
      /*
      * Write the data to EEPROM on the appropriate block
      * use the "current_block_number" var as the block number to write
      * figure out a mechanisim to collect failed block numbers for the final blocks retransmision
      */
      
      writeFirmwareBlock(block_data,current_block_number);
      updateBlocksStatus();
      previous_block_number=current_block_number;
      writePreviousImageNumber(previous_block_number,temp);
      updatateFirwmareBlockStatus(previous_block_number,1);//set this block as recived
    }
    *response_len = 0;
}
void image_verify(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    
    
    //verify the iamge and update, may be calculate for image checksum 
    //image_transfer_status
    
    
    *response_len = 0;
}
void image_activate(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    
    //if image is not verified 
      //verify the image and update 
    //activate the image. 
    //restart and start boot firmware upgrade proccess  
    
    
    *response_len = 0;
}
void getimage_transferred_blocks_status(void *data,int data_direction) // read only method
{
    //uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {

      
    }
    else if(data_direction == ATTR_READ)
    {      
      //TODO. read the transfered images bit string from EEPROM and send it
      readAllFirwmareBlockStatus(data,0);
    }
}
void getimage_first_not_transferred_block_number(void *data,int data_direction)
{
    //uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {

      
    }
    else if(data_direction == ATTR_READ)
    {
              
      //TODO. read the first block  not transfered from EEPROM and send it
      //write some algorithm to find the first block that is not transfered 
      //read a block and mask it with 0xff
      uint8_t data=0,i=0,j=0,z=0;
      uint32_t blocks_count =256;//TODO. get the blocks count
      for(;i<blocks_count;i++)
      {
        readFirwmareBlockStatus(&data,i);
        
        if(data & 0xff)
        {
          
        }
        else{
          uint8_t x=1;
          for(;j<8;j++)
          {
            if( data&(x<<j) != 1 )
            {
              data = (i*8) + j;
              z=1;
              break;
            }            
          }
          if(z == 1)
          {
            break;
          }
        }
      }
      
      if(z==1)
      {
        //return > size
      }
      
    }
}
static const struct method_desc_s Obj_ImageTransfer_Methods[] =
{
    {1, ACCESS_PC___MRRW_USRW, image_transfer_initiate},
    {2, ACCESS_PC___MRRW_USRW, image_block_transfer},
    {3, ACCESS_PC___MRRW_USRW, image_verify},
    {4, ACCESS_PC___MRRW_USRW, image_activate}
};
//uint8_t image_to_activate_info;
static const struct attribute_desc_s Obj_ImageTransfer[] =
{
    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PROFILE_GENERIC_OBJECTS_START + 7].instance_id, NULL},//Done
    {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &image_block_size, NULL}, //Done
    {3, ACCESS_PC___MRR__USR_, TAG_BITSTRING,       (void *) &image_transferred_blocks_status, getimage_transferred_blocks_status},
    {4, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &image_first_not_transferred_block_number, getimage_first_not_transferred_block_number},
    {5, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,         (void *) &image_transfer_enabled, NULL},//done
    {6, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &image_transfer_status, NULL},//done
    {7, ACCESS_PC___MRR__USR_, TAG_ARRAY,           (void *) &image_to_activate_info, NULL}//done
};

const struct object_desc_s object_list[] =
{
    /* Mandatory objects */
    {ASSOC_PC_MR_US_MS, CLASS_ID_CLOCK,                    0, {  0,   0,   1,   0,   0, 255}, 9,  clock_attrs, 0, NULL },//0 clock_methods
    
    /*If the current association object is requested by the client one of the following for associations are returned based on the current established association */
    {ASSOC_PC_MR_US_MS, CLASS_ID_ASSOCIATION_LN,           0, {  0,   0,  40,   0,   0, 255}, 8,  ln_40_attrs_LS, 4, ln_40_methods_LS},//1   /*Current association object  mandatory for public client*/ TODO. check

    {ASSOC_PC_MR_US,    CLASS_ID_ASSOCIATION_LN,           0, {  0,   0,  40,   0,   1, 255}, 8,  ln_40_attrs_NS, 4, ln_40_methods_NS},//2  
    {ASSOC_MR_US,       CLASS_ID_ASSOCIATION_LN,           0, {  0,   0,  40,   0,   2, 255}, 8,  ln_40_attrs_LS, 4, ln_40_methods_LS},//3  
    {ASSOC_US_MS,       CLASS_ID_ASSOCIATION_LN,           0, {  0,   0,  40,   0,   3, 255}, 8,  ln_40_attrs_HS, 4, ln_40_methods_HS},//4  
    {ASSOC_US_MS,       CLASS_ID_ASSOCIATION_LN,           0, {  0,   0,  40,   0,   4, 255}, 8,  ln_40_attrs_HS_FC, 4, ln_40_methods_HS_FC},//5 /*Unused association for firmware upgrade client */ TODO. check 

    {ASSOC_PC_MR_US,    CLASS_ID_SAP_ASSIGNMENT,           0, {  0,   0,  41,   0,   0, 255}, 2,  sap_41_attrs, 0, NULL},//6 done 
    
    /* Communication setup object */
    {ASSOC_MR_US,       CLASS_ID_IEC_HDLC_SETUP,           1, {  0,   0,  22,   0,  0, 255}, 9, Obj_HDLC_Setup, 0, NULL},//7 TODO. check 
    {ASSOC_MR_US,       CLASS_ID_PRIME_PLC_App_ID,         0, {  0,   0,  28,  07, 00, 255}, 4, Obj_Prime_PLC_FW_Version, 0, NULL},  //8    
    {ASSOC_MR_US,       CLASS_ID_ETHERNET_SETUP,           0, {  0,   0,  28,  06, 00, 255}, 4, Obj_Prime_Device_Setup, 0, NULL},  //9     
    {ASSOC_MR_US,       CLASS_ID_SPECIAL_DAYS,             0, {  0,   0,   11, 0,   0, 255}, 3, Obj_Special_Days, 0, Bbj_Special_days_Method},//10    
    {ASSOC_MR_US,       CLASS_ID_DISCONNECT_CONTROL,       0, {  0,   0,  96,  3,  10, 255}, 4, Obj_DisconnectControl, 2, Obj_Connect_Methods},//11/*  previous disconnect control */ //done 



    {ASSOC_MR_US,       CLASS_ID_ACTIVITY_CALENDAR,        0, {  0,   0,  13,  0,   0, 255},10,  Obj_Activity_Calendar, 1, Obj_Activity_Calendar_Methods},//12 done 
    
    {ASSOC_MR_US,       CLASS_ID_SCRIPT_TABLE,             0, {  0,   0,  10,  0, 100, 255}, 2,  Obj_Tariff_Script_Table, 1, Obj_Tariff_Script_Methods},//13
    {ASSOC_MR_US,       CLASS_ID_SCRIPT_TABLE,             0, {  0,   0,  10,  0,   1, 255}, 2,  Obj_Billing_Script_Table, 1, Obj_Billing_Script_Methods},//14
    {ASSOC_MR_US,       CLASS_ID_SCRIPT_TABLE,             0, {  0,   0,  10,  0, 106, 255}, 2,  Obj_Disconnet_Script_Table,   1, Obj_Disconnect_Script_Methods},//15 done 
    {ASSOC_MR_US,       CLASS_ID_SCRIPT_TABLE,             0, {  0,   0,  10,  0,   0, 255}, 2,  Obj_GlobalReset_Script_Table, 1, Obj_GlobalReset_Script_Methods},//16 done 

    
    {ASSOC_MR_US,       CLASS_ID_SINGLE_ACTION_SCHEDULE,   0, {  0,   0,  15,  1,  01, 255}, 4,  Obj_Billing_Schedule, 0, NULL},//17
    {ASSOC_MR_US,       CLASS_ID_SINGLE_ACTION_SCHEDULE,   0, {  0,   0,  15, 01,  02, 255}, 4,  Obj_DiconnectControl_Schedule, 0, NULL},//18

    
    /* Abstract parameters - name plate details */ 
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  42,   0,   0, 255}, 2,  Obj_DeviceIdentifier, 0, NULL},// 1 /*COSEM Logical device name object  mandatory*/ done    
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  96,   1,   0, 255}, 2,  Obj_Meter_Sr_No, 0, NULL},//2  done
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  96,   1,   1, 255}, 2,  Obj_Device_ID_2, 0, NULL},//3 done 
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  96,   1,   2, 255}, 2,  Obj_Device_ID_3, 0, NULL},//4 done 
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  96,   1,   3, 255}, 2,  Obj_Device_ID_4, 0, NULL},//5 done 
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  96,   1,   4, 255}, 2,  Obj_Device_ID_5, 0, NULL},//6 done
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  96,   1,   5, 255}, 2,  Obj_Device_ID_6, 0, NULL},//7 done
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  96,   1,   6, 255}, 2,  Obj_Device_ID_7, 0, NULL},//8 done
    {ASSOC_PC_MR_US_MS,       CLASS_ID_DATA,               0, {  0,   0,  96,   1,   7, 255}, 2,  Obj_Device_ID_8, 0, NULL},//9 done 
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  96, 14,   1, 255}, 2,   Obj_Currently_Active_Tariff, 0, NULL},//10 /* has version from 1 to 3  */    
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  1,   0,   0,  4,    3, 255}, 2,  Obj_TransformerRatio_VoltageNum, 0, NULL},//11 /* Transformer ratio */ //done 
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  1,   0,   0,  4,    6, 255}, 2,  Obj_TransformerRatio_VoltageDen, 0, NULL},//12 /* Transformer ratio */ //done
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  1,   0,   0,  4,    2, 255}, 2,  Obj_TransformerRatio_CurrentNum, 0, NULL},//13 /* Transformer ratio */ //done
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  1,   0,   0,  4,    5, 255}, 2,  Obj_TransformerRatio_CurrentDen, 0, NULL},//14 /* Transformer ratio */ //done
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  1,   0,   0,  9,    11, 255}, 2, Obj_Clock_Time_Shift_Event_Limit, 0, NULL},//15 /* Maximum allowed time shift without raising time shift event */
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   1,  94, 34,   32, 255}, 2,  Obj_ExportActivePowerOverThrStatus, 0, NULL},//16  //done  
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   1,  94, 34,   30, 255}, 2,  Obj_ImportActivePowerOverThrStatus, 0, NULL},//17  //done  
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  96,   2,   12, 255}, 2, Obj_DateTime_Format, 0, NULL},//18 /* Clock Synchronization */ done
    {ASSOC_US,                CLASS_ID_DATA,               0, {  1,   0,   0,  8,   0, 255}, 2,   Obj_Demand_Integration_Period, 0, NULL},//19
    {ASSOC_US,                CLASS_ID_DATA,               0, {  1,   0,   0,  8,   4, 255}, 2,   Obj_Profile_Capture_Period, 0, NULL},//20 done, intergrated    
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  1,   1,  94, 34, 104, 255}, 2,   Obj_Phase_Presence, 0, NULL},//21 /* Phase Presence */ done
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  1,   0,   0,  2,   0, 255}, 2,   Obj_FirmwareVersion, 0, NULL},//22 /* Phase Presence */ done
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  97, 97,   0, 255}, 2,   Obj_ErrorObject, 0, NULL},//23  
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  94, 91,   0, 255}, 2,   Obj_Cum_Tamper_Count, 0, NULL},//24 done, integrated 
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,   0,  1,   0, 255}, 2,   Obj_Cum_MD_Reset_Count, 0, NULL},//25
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  96,  2,   0, 255}, 2,   Obj_Cum_Prog_Count, 0, NULL},//26
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,   0,  1,   2, 255}, 2,   Obj_Last_MD_Rst_DT, 0, NULL},//27
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  96,  7,  5, 255},   2,  Obj_NumberOfLongPowerFails, 0, NULL},//28   
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  96, 11, 0,   255},  2,  Obj_StandardEventItem, 0, NULL},//29
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  00, 00, 5,   255},  2,  Obj_OpearatingMode, 0, NULL},//30
    {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  1,   1,  94, 34,  100, 255}, 2,  Obj_ActiveQuadrant, 0, NULL},//31 /* Transformer ratio */
//    {ASSOC_MR_US,             CLASS_ID_DATA,                     0, {  0,   0,  00, 00, 3,   255},  2,  Obj_CurrentBalance, 0, NULL},//32    
    {ASSOC_MR_US,             CLASS_ID_DATA,                     0, {  0,   0,  00, 00, 2,   255},  2,  Obj_MacAddress, 0, NULL},//32 //reset this
//  {ASSOC_MR_US,       CLASS_ID_DATA,                     0, {  1,   0,  94,  34, 130, 255},  2,  Obj_NewCalenderActivationTime, 0, NULL},//87    
    
    
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   0,   9, 11, 255}, 3,  ObjClock_Time_Shift_Event_Limit, 0, NULL},  //1 
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   1,  94, 34,   1, 255}, 3,  Obj_Clock_Time_Shift_Invalid_Event, 0, NULL},  //2   
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   1,  94, 34,   1, 255}, 3,  Obj_Active_Demand_Control_Threshold, 0, NULL},//3 /*  */ //done 
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  12, 43,   0, 255}, 3,  Obj_TimeThresholdVoltageSag, 0, NULL}, //4  //done  fin    
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  12, 44,   0, 255}, 3,  Obj_TimeThresholdVoltageSwell, 0, NULL},//5  //done fin   
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0,  94, 34,  70, 255}, 3,  Obj_Demand_close_to_contract_power, 0, NULL},//6  //95% of contract power //done 
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0,  96,  7,  20, 255}, 3,   Obj_TimeThresholdLongPowerFaile, 0, NULL},//7  //More than 3 minutes  fin
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  12, 31,   0, 255}, 3,   Obj_ThresholdVoltageSage, 0, NULL}, //8 //7% Vref //done fin
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  12, 35,   0, 255}, 3,   Obj_ThresholdVoltageSwell, 0, NULL}, //9 //7% Vref //done fin
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0,  94, 34,  60, 255}, 3,   Obj_ThresholdLongPowerFaile, 0, NULL},//10  // fin
    /* Instantaneous parameters - direct access */
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  31,  7,   0, 255}, 3,  Obj_Instantanious_Current_L1, 0, NULL},//11 done 
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   2,  8,   0, 255}, 3,  Obj_Total_Active_Energy_Export, 0, NULL},  //12 done , integrated  //[E.E] was 1,0,2,8,0,255
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  32,  7,   0, 255}, 3,  Obj_Instantanious_Voltage_L1, 0, NULL},//13 done , integrated , IDC-3 read tested 
//    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  72,  7,   0, 255}, 3,  Obj_V_BN, 0, NULL},//14
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   0,  6,   4, 255}, 3,  Obj_Voltage_Referance, 0, NULL},//14  //Referance Voltage for power quality measurement  fin    
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  33,  7,   0, 255}, 3,  Obj_PF_R, 0, NULL},//15 done , integrated????????/


    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0,  94, 34, 110, 255}, 3,  Obj_Scroll_Display_Time, 0, NULL},//16
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0,  00, 00, 4,   255}, 3,  Obj_NeutralCurrent, 0, NULL},//17     
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  14,  7,   0, 255}, 3,  Obj_Frequency, 0, NULL},//18 ?????????????????????????????????????/// , integrated
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   1,  7,   0, 255}, 3,  Obj_Active_Power_Positive, 0, NULL},//19 done , integrated fin,import
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   1,  8,   0, 255}, 3,  Obj_Total_Active_Energy_Import, 0, NULL},  //20 done , integrated //[E.E] was 1,0,1,8,0,255
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   5,  8,   0, 255}, 3,  Obj_Reactive_Energy_QI, 0, NULL},//21      //done , integrated 	//[E.E] was 1,0,5,8,0,255
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   6,  8,   0, 255}, 3,  Obj_Reactive_Energy_QII, 0, NULL},//22     //done , integrated 	//[E.E] was 1,0,6,8,0,255
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   7,  8,   0, 255}, 3,  Obj_Reactive_Energy_QIII, 0, NULL},//23    //done , integrated 	//[E.E] was 1,0,7,8,0,255
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   8,  8,   0, 255}, 3,  Obj_Reactive_Energy_QIV, 0, NULL},//24     //done , integrated 	//[E.E] was 1,0,8,8,0,255
    
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   2,  7,   0, 255}, 3,  Obj_Active_Power_Negative, 0, NULL},//25   //done fin
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   3,  7,   0, 255}, 3,  Obj_Reactive_Power_Positive, 0, NULL},//26 //done fin
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   4,  7,   0, 255}, 3,  Obj_Reactive_Power_Negative, 0, NULL},//27 //done fin   
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   9,  7,   0, 255}, 3,  Obj_KVA_Total, 0, NULL},//28
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0,  96,  7, 15, 255},  3,  Obj_DurationOfAllLongPowerFailes, 0, NULL}, //29  
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   1,  94, 34, 31, 255},  3,  Obj_ImportActivePowerThreshold, 0, NULL},//30 //done 
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   1,  94, 34, 33, 255},  3,  Obj_ExportActivePowerThreshold, 0, NULL},//31 //done 
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,  13,  7,   0, 255}, 3,  Obj_Instantanious_Power_Factor_Avarage, 0, NULL},//32 done, integrated 
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0, 140,  0,   0, 255}, 3,  Obj_ResetAlarm, 1, Obj_Reset_Methods},  //33 
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0, 141,  0,   0, 255}, 3,  Obj_CalibratePMeter, 1, Obj_CalibratePMeter_Methods},  //34 was CLASS_ID_CALIBRATION
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0, 142,  0,   0, 255}, 3,  Obj_CalibrateCMeter, 1, Obj_CalibrateCMeter_Methods},  //35
    {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0, 143,  0,   0, 255}, 3,  Obj_CalibrateVMeter, 1, Obj_CalibrateVMeter_Methods},  //36
    
    /* Instantaneous parameters - profile access */
    {ASSOC_MR_US,       CLASS_ID_PROFILE_GENERIC,          1, {  1,   0,  99,  1,   0, 255}, 8,  Obj_Load_Profile, 2, Obj_Load_Profile_Methods},//5 /* Hourly Load Profile one entry per 1Hr present*/    
    {ASSOC_MR_US,       CLASS_ID_PROFILE_GENERIC,          1, {  0,   0,  99, 98,   0, 255}, 8,  Obj_StandardEventLog, 2, Obj_Event_Log_Methods},//2 //TODO. Object Deleted, 
    {ASSOC_MR_US,       CLASS_ID_PROFILE_GENERIC,          1, {  0,   0,  21,  0,  06, 255}, 8,  Obj_Instantaneous_Energy_Values, 0, NULL},//3 done
    {ASSOC_MR_US,       CLASS_ID_PROFILE_GENERIC,          1, {  0,   0,  21,  0,  05, 255}, 8,  Obj_Instantaneous_Values_buff, 0, NULL},//4 done 
    {ASSOC_MR_US,       CLASS_ID_PROFILE_GENERIC,          1, {  0,   0,  21,  0,  11, 255}, 8,  Obj_Billing_Profile, 0, NULL},//1    Obj_Load_Billing_Profile_Methods

    {ASSOC_MR_US,       CLASS_ID_BALANCE,                  0, {  0,   0, 131,  0,   0, 255}, 3,   Obj_CurrentBalance2, 2, Obj_Balance_Methods},  //6 
    {ASSOC_MR_US,       CLASS_ID_IMAGE_TRANSFER,           0, {  0,   0, 44,  0,   0, 255},  7,   Obj_ImageTransfer, 4, Obj_ImageTransfer_Methods},  //7
	/*SO2 */
	{ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   1,  29,   0, 255}, 3,  Obj_Total_Active_Energy_Import_SO2, 0, NULL},  //Ai
	{ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   2,  29,   0, 255}, 3,  Obj_Total_Active_Energy_Export_SO2, 0, NULL},  //Ae
	{ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   5,  29,   0, 255}, 3,  Obj_Reactive_Energy_QI_SO2, 0, NULL},          //R1
	{ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   6,  29,   0, 255}, 3,  Obj_Reactive_Energy_QII_SO2, 0, NULL},         //R2
	{ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   7,  29,   0, 255}, 3,  Obj_Reactive_Energy_QIII_SO2, 0, NULL},        //R3
	{ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  1,   0,   8,  29,   0, 255}, 3,  Obj_Reactive_Energy_QIV_SO2, 0, NULL},         //R4
	{ASSOC_MR_US,       CLASS_ID_DISCONNECT_CONTROL,       0, {  0,   1,  94,  34,  20, 255}, 4,  Obj_Previous_switch_State, 2, Obj_Connect_Methods},//11/*  previous disconnect control not integrated
    {0,0, 0, {0, 0, 0, 0, 0, 0}, 0,  NULL}
};
//Tariff Scheam
//LanguageSetting_SIZE
//Low battery back up time
//Monthly billing log 
//current phase //on with phase the meter is connected
//Energy log time 
