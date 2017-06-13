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
#include "cosem.h"
#include "obis.h"
#include "iec62056_46_link.h"
#include "msgs.h"
#include "config.h"
#include "Logger.h"
#include "emeter-rtc.h"
#include "utilities.h"
#include "Status.h"
#include "self_diagnosis.h"
#include "all_objects_header.h"
#define FALSE 0
#define TRUE (!FALSE)

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

const int reject_ln_context = FALSE;
const int reject_sn_context = TRUE;

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

uint32_t known_far_msap_list[5] = 
{
    0x40000L,
    0x80000L,
    0xC0000L,
    0x100000L,
    0
};

//extern Password_128 Passwords[4];
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
};

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
uint8_t OperatingMode = 1; //TODO check it

__no_init__(Data_Buffer) uint8_t Data_Buffer[CHUNK_SIZE+64]; /* General purpose buffer */

const struct object_desc_s object_list[] =
{
/********************************************************ABSTRACT OBJECTS*************************************************************/
  /* clock */

    /* Clock
     * Class id-8
     * 0-0:1.0.0.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_CLOCK,        0,{0,0,1,0,0,255}
                                     ,9
                                     ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[0].instance_id, NULL},
                                            {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Data_Buffer, get_date_time_string},//,exchange_date_time
                                            {3, ACCESS_PCR__MRR__USRW, TAG_INT16,           (void *) &clock_time_zone, NULL},
                                            {4, ACCESS_PCR__MRR__USR_, TAG_UINT8,           (void *) &clock_status, NULL},
                                            {5, ACCESS_PCR__MRR__USRW, TAG_OCTET_STRING,    (void *) daylights_savings_begin, NULL},
                                            {6, ACCESS_PCR__MRR__USRW, TAG_OCTET_STRING,    (void *) daylights_savings_end, NULL},
                                            {7, ACCESS_PCR__MRR__USRW, TAG_INT8,            (void *) &clock_dst_deviation, NULL},
                                            {8, ACCESS_PCR__MRR__USRW, TAG_BOOLEAN,         (void *) &clock_dst_enabled, NULL},
                                            {9, ACCESS_PCR__MRR__USR_, TAG_ENUM,            (void *) &clock_base, NULL},

                                     }
                                     ,0
                                     ,NULL
     },

     /* Clock Synchronization
      * Class id-1
      * 0-0:96.2.12.255
      */
     {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,2,12,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[1].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USRW, TAG_OCTET_STRING,    (void *) last_clock_synchronized_date_time, NULL}
                                     }
                                     ,0
                                     ,NULL
      },

     /* Clock Time Shift Event Limit
      * Class id-3
      * 1-0:0.9.11.255
      */
     {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,0,9,11,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[2].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USRW, TAG_UINT8,           (void *) &max_allowed_time_shift, NULL},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time_Shift_EvtLim, NULL}
                                     }
                                     ,0
                                     ,NULL
      },

      /* Clock Time Shift Invalid Limit
       * Class id-3
       * 1-1:94.34.1.255
       */
      {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,1,94,34,1,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[3].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT16,           (void *) &clock_time_shift_invalid_lim, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time_Shift_EvtLim, NULL}
                                     }
                                     ,0
                                     ,NULL
      },

    /*****************************SAP assignment, Association LN, COSEM logical device name***************************/

    /* SAP Assignment
     * Class id-17,
     * 0-0:41.0.0.255
     */
    {ASSOC_PC_MR_US, CLASS_ID_SAP_ASSIGNMENT,        0,{0,0,41,0,0,255}
                                    ,2
                                    ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) object_list[SAP_ASSN_START].instance_id, NULL},
                                            {2, ACCESS_PCR__MRR__USR__MSR_, TAG_ARRAY,           (void *) sap_list, NULL},
                                    }
                                    ,0
                                    ,NULL
    },

    /* Association LN-Current Association
     * Class id-15,
     * 0-0:40.0.0.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_ASSOCIATION_LN,        0,{0,0,40,0,0,255}
                                    ,8
                                    ,(const struct attribute_desc_s[]) {
    
                                            {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[SAP_ASSN_START + 1].instance_id, NULL},
                                            {2, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
                                            {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) partner_LS, NULL},
                                            {4, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
                                            {5, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
                                            {6, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_LS, NULL},
                                            {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[1].password+1), authentication_secret_LS},
                                            {8, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &uint8val, NULL}

                                    }
                                    ,4
                                    ,(const struct method_desc_s[]) {

                                            {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
                                            {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_LS},
                                            {3, ACCESS_PC___MR___USR_, add_object},
                                            {4, ACCESS_PC___MR___USR_, remove_object}
                                    }
    },

    /* Association LN-Public Client Association
     * Class id-15,
     * 0-0:40.0.1.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_ASSOCIATION_LN,        0,{0,0,40,0,1,255}
                                    ,8
                                    ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[SAP_ASSN_START + 2].instance_id, NULL},
                                            {2, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
                                            {3, ACCESS_PCR__MRR__USR_, TAG_STRUCTURE,       (void *) partner_NS, NULL},
                                            {4, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
                                            {5, ACCESS_PCR__MRR__USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
                                            {6, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_NS, NULL},
                                            {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[0].password +1), NULL},
                                            {8, ACCESS_PCR__MRR__USR_, TAG_ENUM,            (void *) &uint8val, NULL}

                                    }
                                    ,4
                                    ,(const struct method_desc_s[]) {

                                            {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
                                            {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_NS},
                                            {3, ACCESS_PC___MR___USR_, add_object},
                                            {4, ACCESS_PC___MR___USR_, remove_object}
                                    }
    },

    /* Association LN-Reading Client Association
    * Class id-15,
    * 0-0:40.0.2.255
    */
    {ASSOC_MR_US, CLASS_ID_ASSOCIATION_LN,        0,{0,0,40,0,2,255}
                                   ,8
                                   ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[SAP_ASSN_START + 3].instance_id, NULL},
                                            {2, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
                                            {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) partner_LS, NULL},
                                            {4, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
                                            {5, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
                                            {6, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_LS, NULL},
                                            {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[1].password+1), authentication_secret_LS},
                                            {8, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &uint8val, NULL}

                                   }
                                   ,4
                                   ,(const struct method_desc_s[]) {

                                           {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
                                           {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_LS},
                                           {3, ACCESS_PC___MR___USR_, add_object},
                                           {4, ACCESS_PC___MR___USR_, remove_object}
                                   }
    },

    /* Association LN-Management Client Association
    * Class id-15,
    * 0-0:40.0.3.255
    */
    {ASSOC_US_MS, CLASS_ID_ASSOCIATION_LN,        0,{0,0,40,0,3,255}
                                  ,8
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) object_list[SAP_ASSN_START + 4].instance_id, NULL},
                                          {2, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
                                          {3, ACCESS_PC___MR___USR_, TAG_STRUCTURE,       (void *) partner_HS, NULL},
                                          {4, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
                                          {5, ACCESS_PC___MR___USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
                                          {6, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_HS, NULL},
                                          {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[2].password+1), NULL},
                                          {8, ACCESS_PC___MR___USR_, TAG_ENUM,            (void *) &uint8val, NULL}

                                  }
                                  ,4
                                  ,(const struct method_desc_s[]) {

                                          {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
                                          {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_HS},
                                          {3, ACCESS_PC___MR___USR_, add_object},
                                          {4, ACCESS_PC___MR___USR_, remove_object}
                                  }
    },

    /* Association LN-Firmware Upgrade
    * Class id-15,
    * 0-0:40.0.4.255
    */
    {ASSOC_US_MS, CLASS_ID_ASSOCIATION_LN,        0,{0,0,40,0,4,255}
                                 ,8
                                 ,(const struct attribute_desc_s[]) {

                                         {1, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) object_list[SAP_ASSN_START + 5].instance_id, NULL},
                                         {2, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) Data_Buffer, NULL},
                                         {3, ACCESS_PC___MR___USR_, TAG_STRUCTURE,       (void *) partner_HS, NULL},
                                         {4, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) application_context_name_logical_names, NULL},
                                         {5, ACCESS_PC___MR___USR_, TAG_STRUCTURE,       (void *) &context_info, NULL},
                                         {6, ACCESS_PC___MR___USR_, TAG_OCTET_STRING,    (void *) auth_mech_name_HS, NULL},
                                         {7, ACCESS_PC___MR___USRW, TAG_OCTET_STRING,    (void *) (Passwords[3].password+1), NULL},
                                         {8, ACCESS_PC___MR___USR_, TAG_ENUM,            (void *) &uint8val, NULL}

                                 }
                                 ,4
                                 ,(const struct method_desc_s[]) {

                                         {1, ACCESS_PC___MR___USRW, reply_to_hls_auth},
                                         {2, ACCESS_PC___MR___USRW_MSRW, change_hls_secret_HS},
                                         {3, ACCESS_PC___MR___USR_, add_object},
                                         {4, ACCESS_PC___MR___USR_, remove_object}
                                 }
    },

    /* COSEM logical device name
    * Class id-1,
    * 0-0:42.0.0.255
    */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,42,0,0,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[SAP_ASSN_START + 6].instance_id, NULL},
                                          {2, ACCESS_PC___MRRW_USRW, TAG_OCTET_STRING,    (void *) &DeviceIdentifier, get_set_device_identifier}
                                  }
                                  ,0
                                  ,NULL
    },

    /**********************************************Identification Numbers*********************************************/

    /* Device ID 1-E-meter Serial Number
     * Class id-1,
     * 0-0:96.1.0.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,96,1,0,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[IDENT_NUM_START].instance_id, NULL},
                                          {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Meter_Sr_No, change_meter_serial_number}
                                  }
                                  ,0
                                  ,NULL
    },

    /* Device ID 2-E-meter equipment identifier, part  of UNESA barcode
     * manufacturer code + model code + manufacturing's year.
     * Class id-1,
     * 0-0:96.1.1.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,96,1,1,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[IDENT_NUM_START + 1].instance_id, NULL},
                                          {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_2, change_device_id2}
                                  }
                                  ,0
                                  ,NULL
    },

    /* Device ID 3-Equipment type identifier
     * equipment type="contador" + protocol identifier (“DLMS”+protocol version).
     * Class id-1,
     * 0-0:96.1.2.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,96,1,2,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[IDENT_NUM_START + 2].instance_id, NULL},
                                          {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_3, change_device_id3}
                                  }
                                  ,0
                                  ,NULL
    },

    /* Device ID 4-Equipment type identifier
     * Tariff Contract Profile Number [10]+ calendar schedules identifier[10]
     * Class id-1,
     * 0-0:96.1.3.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,96,1,3,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[IDENT_NUM_START + 3].instance_id, NULL},
                                          {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_4, change_device_id4}
                                  }
                                  ,0
                                  ,NULL
    },

    /* Device ID 5-General communications unique identifier.
     * FLAG_ID (meter manufacturer)[3] + E-meter serial number[10]
     * Class id-1,
     * 0-0:96.1.4.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,96,1,4,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[IDENT_NUM_START + 4].instance_id, NULL},
                                          {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_5, change_device_id5}
                                  }
                                  ,0
                                  ,NULL
    },

    /* Device ID 6-multicast communications identifier
     * Class id-1,
     * 0-0:96.1.5.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,96,1,5,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[IDENT_NUM_START + 5].instance_id, NULL},
                                          {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_6, change_device_id6}
                                  }
                                  ,0
                                  ,NULL
    },

    /* Device ID 7-Former firmware version
     * Class id-1,
     * 0-0:96.1.6.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,96,1,6,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[IDENT_NUM_START + 6].instance_id, NULL},
                                          {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_7, change_device_id7}
                                  }
                                  ,0
                                  ,NULL
    },

    /* Device ID 8-Scroll Display Mode
     * Class id-1,
     * 0-0:96.1.7.255
     */
    {ASSOC_PC_MR_US_MS, CLASS_ID_DATA,        0,{0,0,96,1,7,255}
                                  ,2
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[IDENT_NUM_START + 7].instance_id, NULL},
                                          {2, ACCESS_PCR__MRRW_USRW, TAG_OCTET_STRING,    (void *) Device_ID_8, change_device_id8}
                                  }
                                  ,0
                                  ,NULL
    },
    
    /**********************Activity Calendars and Special Days Tables********************************************/

    /* Activity Calendar-for tariff contract number 1
    * Class id-20,
    * 0-0:13.0.1.255
    */
    {ASSOC_MR_US, CLASS_ID_ACTIVITY_CALENDAR,        0,{0,0,13,0,1,255}
                                 ,10
                                 ,(const struct attribute_desc_s[]) {

                                         {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ACTIVITY_CAL_START].instance_id, NULL},
                                         {2, ACCESS_PC___MRR__USRW, TAG_OCTET_STRING,    (void *) active_calendar_name, NULL},
                                         {3, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, access_active_season_profile},
                                         {4, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, access_active_week_profile_table},
                                         {5, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, access_active_day_profile_table},
                                         {6, ACCESS_PC___MRRW_USRW, TAG_OCTET_STRING,    (void *) passive_calendar_name, NULL},
                                         {7, ACCESS_PC___MRRW_USRW, TAG_ARRAY,           (void *) Data_Buffer, access_passive_season_profile},
                                         {8, ACCESS_PC___MRRW_USRW, TAG_ARRAY,           (void *) Data_Buffer, access_passive_week_profile_table},
                                         {9, ACCESS_PC___MRRW_USRW, TAG_ARRAY,           (void *) Data_Buffer, access_passive_day_profile_table},
                                         {10,ACCESS_PC___MRRW_USRW, TAG_OCTET_STRING,    (void *) activate_passive_calendar_time, set_get_passive_calendar_activation_time}

                                 }
                                 ,1
                                 ,(const struct method_desc_s[]) {

                                         {1, ACCESS_PC___MRR__USR_, activate_passive_calendar}
                                 }
    },

    /* Activity Calendar-for tariff contract number 2
    * Class id-20,
    * 0-0:13.0.2.255
    */
    //TODO

    /* Activity Calendar-for tariff contract number 3
     * Class id-20,
     * 0-0:13.0.3.255
     */
     //TODO

    /* Special Days Table-for active tariff contract 1
     * Class id-11,
     * 0-0:11.0.1.255
     */
     {ASSOC_MR_US, CLASS_ID_SPECIAL_DAYS,        0,{0,0,11,0,1,255}
                                  ,3
                                  ,(const struct attribute_desc_s[]) {

                                          {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ACTIVITY_CAL_START + 1].instance_id, NULL},
                                          {2, ACCESS_PC___MRR__USR_, TAG_ARRAY,           (void *) &Data_Buffer, get_special_days_list},

                                  }
                                  ,2
                                  ,(const struct method_desc_s[]) {

                                          {1, ACCESS_PC___MRR__USR_, insert_special_day},
                                          {2, ACCESS_PC___MRR__USR_, delete_special_day}
                                  }
     },

     /* Special Days Table-for active tariff contract 2
      * Class id-11,
      * 0-0:11.0.2.255
      */
     //TODO

     /* Special Days Table-for active tariff contract 3
      * Class id-11,
      * 0-0:11.0.3.255
      */
     //TODO

     /* Active Demand Control Threshold
      * Class id-3
      * 0-1:94.34.1.255
      */
     {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,1,94,34,1,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ACTIVITY_CAL_START + 2].instance_id, NULL},
                                             {2, ACCESS_PC___MRRW_USRW, TAG_UINT32,           (void *) &active_demand_control_threshold, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KW, NULL}
                                     }
                                     ,0
                                     ,NULL
      },

      /* Active Demand Control Threshold
       * Class id-3
       * 0-1:94.34.x.255 x=2..6 and x=21..26
       */
      //TODO

      /* Passive Demand Control Threshold
       * Class id-3
       * 0-1:94.34.x.255  x=11..16. x = 41..46
       */
      //TODO

      /* Currently active tariff
       * Class id-1,
       * 0-0:96.14.1.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,14,1,255}
                                    ,2
                                    ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ACTIVITY_CAL_START + 3].instance_id, NULL},
                                            {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &current_active_tariff, NULL}
                                    }
                                    ,0
                                    ,NULL
      },

      /* Currently active tariff
       * Class id-1,
       * 0-0:96.14.x.255 x=2,3
       */
      //TODO

      /*******************************Billing Periods and Profiles*******************************************/

      /* Predefined Scripts - MDI reset/ end of billing period
       * Class id-9,
       * 0-0:10.0.1.255
       */
       {ASSOC_MR_US, CLASS_ID_SCRIPT_TABLE,        0,{0,0,10,0,1,255}
                                    ,2
                                    ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[BILLING_PROF_START].instance_id, NULL},
                                            {2, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) billing_script, NULL}

                                    }
                                    ,1
                                    ,(const struct method_desc_s[]) {

                                            {1, ACCESS_PC___MRR__USR_, billing_script_execute}
                                    }
       },

      /* Active end of billing period 1
       * Class id-22,
       * 0-0:15.1.1.255
       */
       {ASSOC_MR_US, CLASS_ID_SINGLE_ACTION_SCHEDULE,        0,{0,0,15,1,1,255}
                                    ,4
                                    ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[BILLING_PROF_START + 1].instance_id, NULL},
                                            {2, ACCESS_PC___MRR__USRW, TAG_STRUCTURE,       (void *) executed_billing_script, NULL},
                                            {3, ACCESS_PC___MRR__USRW, TAG_ENUM,            (void *) &billing_script_type, NULL},
                                            {4, ACCESS_PCR__MRRW_USRW, TAG_ARRAY,           (void *) Data_Buffer,access_billing_script_execution_time}

                                    }
                                    ,0
                                    ,NULL
       },

      /* Active end of billing period 1
       * Class id-22,
       * 0-0:15.1.x.255 x=2,3 tariff contract number
       */
       //TODO

      /* Passive end of billing period 1
       * Class id-1,
       * 0-0:94.34.x.255 x = 41..43, Contract 1..3
       */
      //TODO

      /* Data of billing period 1
       * Stored Billing Values Profile
       * Class id-7,
       * 0-0:98.1.c.255 c=1..3, tariff contract number
       */
      //TODO

      /* Time stamp of billing period 1 last reset
       * Date/time of the most recent billing period
       * Class id-1,
       * 0-0:94.34.x.255 x = 11..13, tariff contract number 1, 2, 3 respectively
       */
      //TODO

      /* End of billing period 2
       * Class id-22,
       * 0-0:15.2.x.255 c=1..3, tariff contract number
       */
      //TODO

      /* Data of billing period 2
       * Stored Billing Values Profile /daily billing values
       * Class id-7,
       * 0-0:98.2.c.255 c=1..3, tariff contract number
       */
      //TODO

      /******************************Long Power Failures (Counters, Threshold)****************************/

      /* Time threshold for long power failure
       * Class id-3,
       * 0-0:96.7.20.255
       */
      {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,96,7,20,255}
                                    ,3
                                    ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[LONG_POWER_FAIL_START].instance_id, NULL},
                                             {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,          (void *) &time_threshold_for_long_power_failure, change_time_threshold_for_long_power_failure},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time_Shift_EvtLim, NULL}
                                    }
                                    ,0
                                    ,NULL
      },

     /* Threshold for long power failure
      * Class id-3,
      * 0-0:94.34.60.255
      */
      {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,94,34,60,255}
                                    ,3
                                    ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[LONG_POWER_FAIL_START + 1].instance_id, NULL},
                                            {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,          (void *) &voltage_threshold_for_long_power_failure, change_threshold_for_long_power_failure},
                                            {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_ThresholdVoltageSag_Swell, NULL}
                                    }
                                    ,0
                                    ,NULL
      },

     /* Duration of all long power failures in all phases
      * Class id-3,
      * 0-0:96.7.15.255
      */
      {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,96,7,15,255}
                                    ,3
                                    ,(const struct attribute_desc_s[]) {

                                            {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[LONG_POWER_FAIL_START + 2].instance_id, NULL},
                                            {2, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &duration_of_all_long_power_failure, NULL},
                                            {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KW, NULL}

                                    }
                                    ,0
                                    ,NULL
      },

     /* Number of long power failures in all phases
      * Class id-1,
      * 0-0:96.7.5.255
      */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,7,5,255}
                                    ,2
                                    ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[LONG_POWER_FAIL_START + 3].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT16,          (void *) &number_of_long_power_failures, change_number_of_long_power_failures}

                                    }
                                    ,0
                                    ,NULL
      },

     /***********************************Error and Alarm Handling (error register, alarm register)********************/

     /* Error Object
      * Class id-1,
      * 0-0:97.97.0.255
      */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,97,97,0,255}
                                    ,2
                                    ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) object_list[ERROR_START].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR__MSR_, TAG_UINT32,    (void *)&error_value,NULL }

                                    }
                                    ,0
                                    ,NULL
      },

     /* Alarm Object
      * Class id-3,
      * 0-0:97.98.0.255
      */
      //TODO

     /* Alarm Filter
      * Class id-1,
      * 0-0:97.98.10.255
      */
      //TODO

      /**************************************Event Handling (events and logs)********************************/

      /* Event Object - Standard Event Log
       * Class id-1,
       * 0-0:96.11.0.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,0,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &standard_event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Standard Event Log Profile
       * Class id-7,
       * 0-0:99.98.0.255
       */
       {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,0,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 1].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_standard_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) standard_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &standard_event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_event_log_capture}
                                      }
       },

      /* Event Object - Fraud Detection Log
       * Class id-1,
       * 0-0:96.11.1.255
       */
	  {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,1,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 2].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &fraud_event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Fraud Detection Log ProfileSFraud Detection
       * Class id-7,
       * 0-0:99.98.1.255
       */
	  {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,1,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 3].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_fraud_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) fraud_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &fraud_event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_fraud_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_fraud_event_log_capture}
                                      }
       },

      /* Event Object - Power Quality Log
       * Class id-1,
       * 0-0:96.11.5.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,5,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 4].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &power_qual_event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Power Quality Log Profile
       * Class id-7,
       * 0-0:99.98.5.255
       */
      {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,5,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 5].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_power_qual_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) power_qual_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &power_qual_event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_power_qual_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_power_qual_event_log_capture}
                                      }
       },

      /* Time trigger for voltage sags,
       * for voltage swell or long
       * power failure
       * Class id-1,
       * 0-0:94.34.80.255
       */
	  {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,94,34,80,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 6].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_OCTET_STRING,   (void *) last_clock_synchronized_date_time, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Event Object - Finished Power Quality Log
       * Class id-1,
       * 0-0:96.11.9.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,9,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 7].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Finished Power Quality Event Log Profile
       * Class id-7,
       * 0-0:99.98.9.255
       */
       {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,9,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 8].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_finished_qual_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) finished_qual_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_non_impl_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_non_impl_event_log_capture}
                                      }
       },

      /* Event Object - Demand Management Log
       * Class id-1,
       * 0-0:96.11.6.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,6,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 9].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Demand Management Event Log Profile
       * Class id-7,
       * 0-0:99.98.6.255
       */
      {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,6,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 10].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_demand_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) demand_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_non_impl_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_non_impl_event_log_capture}
                                      }
       },

      /* Event Object - Common Event Log
       * Class id-1,
       * 0-0:96.11.7.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,7,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 11].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &common_event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Common Event Log Profile
       * Class id-7,
       * 0-0:99.98.7.255
       */
      {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,7,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 12].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_common_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) common_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &common_event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_common_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_common_event_log_capture}
                                      }
       },

      /* Event Object - Import Power Contract Event Log
       * Class id-1,
       * 0-0:96.11.3.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,3,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 13].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },


      /* Import Power Contract Event Log Profile
       * Class id-7,
       * 0-0:99.98.3.255
       */
     {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,3,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 14].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_import_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) import_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_non_impl_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_non_impl_event_log_capture}
                                      }
       },



      /* Event Object - Firmware Event Log
       * Class id-1,
       * 0-0:96.11.4.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,4,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 15].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &firmware_event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Firmware Event Log Profile
       * Class id-7,
       * 0-0:99.98.4.255
       */
      {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,4,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 16].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_firmware_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) firmware_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &firmware_event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_firmware_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_firmware_event_log_capture}
                                      }
       },

      /* Event Object - Synchronization Event Log
       * Class id-1,
       * 0-0:96.11.8.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,8,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 17].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &synchronization_event_number, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Synchronization Event Log Profile
       * Class id-7,
       * 0-0:99.98.8.255
       */
      {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,8,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 18].instance_id, NULL},
                                             {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_synchronization_event_log},
                                             {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) synchronization_event_log_objects, NULL},
                                             {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &synchronization_event_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_synchronization_event_log_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_synchronization_event_log_capture}
                                      }
       },


      /* Standard Event Log Filter
       * Class id-1,
       * 0-1:94.34.105.255
       */
      //TODO

      /* Fraud Detection Event Log Filter
       * Class id-1,
       * 0-1:94.34.106.255
       */
      //TODO

      /* Power Quality Non-finished Event Log Filter
       * Class id-1,
       * 0-1:94.34.107.255
       */
      //TODO

      /* Power Quality finished Event Log Filter
       * Class id-1,
       * 0-1:94.34.108.255
       */
      //TODO

      /* Demand Management Event Log Filter
       * Class id-1,
       * 0-1:94.34.109.255
       */
      //TODO

      /* Common Event Log Filter
       * Class id-1,
       * 0-1:94.34.110.255
       */
      //TODO

      /* Power Contract Event Log Filter
       * Class id-1,
       * 0-1:94.34.111.255
       */
      //TODO

      /* Firmware Event Log Filter
       * Class id-1,
       * 0-1:94.34.112.255
       */
      //TODO

      /* Synchronization Event Log Filter
       * Class id-1,
       * 0-1:94.34.113.255
       */
      //TODO

      /* Disconnect Control Event Log Filter
       * Class id-1,
       * 0-1:94.34.114.255
       */
      //TODO

      /* Power Contract Event Log Filter
       * Class id-1,
       * 0-1:94.34.115.255
       */
      //TODO

      /************************************Disconnect Control and Log******************************/

      /* Disconnect control Scheduler
       * Class id-22,
       * 0-0:15.0.1.255
       */
       {ASSOC_MR_US, CLASS_ID_SINGLE_ACTION_SCHEDULE,        0,{0,0,15,0,1,255}
                                     ,4
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DISCONNECT_START].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_STRUCTURE,       (void *) executed_diconnect_control_script, NULL},
                                             {3, ACCESS_PC___MRR__USRW, TAG_ENUM,            (void *) &diconnect_control_type, NULL},
                                             {4, ACCESS_PCR__MRRW_USRW, TAG_ARRAY,           (void *) diconnect_control_script_execution_time,NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Disconnect Script Table
       * Class id-9,
       * 0-0:10.0.106.255
       */
       {ASSOC_MR_US, CLASS_ID_SCRIPT_TABLE,        0,{0,0,10,0,106,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DISCONNECT_START + 1].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, access_disconnect_scripts}

                                     }
                                     ,1
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, disconnect_script_execute}
                                     }
       },

      /* Disconnect Control
       * Controls the connection and disconnection of the premises of the consumer
       * Class id-70,
       * 0-0:96.3.10.255
       */
       {ASSOC_MR_US, CLASS_ID_DISCONNECT_CONTROL,        0,{0,0,96,3,10,255}
                                     ,4
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,      (void *) object_list[DISCONNECT_START + 2].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,           (void *) &output_state, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_ENUM,              (void *) &control_state, NULL},
                                              {4, ACCESS_PC___MRR__USRW, TAG_ENUM,              (void *) &control_mode, NULL}

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW,  remote_disconnect},
                                             {2, ACCESS_PC___MRRW_USRW,  remote_reconnect}
                                     }
       },

      /* Previous Disconnect Control
       * Controls the connection and disconnection of the premises of
       * the consumer previous status
       * Class id-70,
       * 0-1:94.34.20.255
       *///TODO integrate
       {ASSOC_MR_US, CLASS_ID_DISCONNECT_CONTROL,        0,{0,1,94,34,20,255}
                                     ,4
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,      (void *) object_list[DISCONNECT_START + 3].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,           (void *) &output_state, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_ENUM,              (void *) &control_state, NULL},
                                              {4, ACCESS_PC___MRR__USRW, TAG_ENUM,              (void *) &control_mode, NULL}

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW,  remote_disconnect},
                                             {2, ACCESS_PC___MRRW_USRW,  remote_reconnect}
                                     }
       },

      /* Event Object - Disconnect Control Event Log
       * Class id-1,
       * 0-0:96.11.2.255
       */
	   {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,11,2,255}
	                                        ,2
	                                        ,(const struct attribute_desc_s[]) {

	                                                {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,   (void *) object_list[EVENT_START + 4].instance_id, NULL},
	                                                {2, ACCESS_PC___MRR__USRW, TAG_UINT8,          (void *) &disconnect_event_number, NULL}

	                                        }
	                                        ,0
	                                        ,NULL
		},

      /* Disconnect Control Log Profile
       * Class id-7,
       * 0-0:99.98.2.255
       */
      {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,99,98,2,255}
										,8
										,(const struct attribute_desc_s[]) {

												{1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[EVENT_START + 5].instance_id, NULL},
												{2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_disconnect_event_log},
												{3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) disconnect_event_log_objects, NULL},
												{4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &events_capture_period, NULL},
												{5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &events_sort_method, NULL},
												{6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
												{7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &events_entries_in_use, NULL},
												{8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &disconnect_event_profile_entries, NULL},

										}
										,2
										,(const struct method_desc_s[]) {

												{1, ACCESS_PC___MRRW_USRW, obj_disconnect_event_log_reset},
												{2, ACCESS_PC___MRRW_USRW, obj_disconnect_event_log_capture}
										 }
		  },

      /* Import Active Power over threshold status
       * Class id-1,
       * 0-1:94.34.30.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,1,94,34,30,255}
                                            ,2
                                            ,(const struct attribute_desc_s[]) {

                                                    {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DISCONNECT_START + 6].instance_id, NULL},
                                                    {2, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,         (void *) &import_active_power_over_thr_status, NULL},

                                            }
                                            ,0
                                            ,NULL
        },

      /* Currently Import Active Power Threshold
       * Class id-3,
       * 0-1:94.34.31.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,1,94,34,31,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DISCONNECT_START + 7].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT32,          (void *) &current_import_active_power_threshold, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KW, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Export Active Power over threshold status
       * Class id-1,
       * 0-1:94.34.32.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,1,94,34,32,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DISCONNECT_START + 8].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,         (void *) &export_active_power_over_thr_status, NULL},

                                     }
                                     ,0
                                     ,NULL
       },

      /* Currently Export Active Power Threshold
       * Class id-3,
       * 0-1:94.34.33.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,1,94,34,31,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[DISCONNECT_START + 9].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT32,          (void *) &current_export_active_power_threshold, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KW, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /****************************Firmware Update objects******************************/

      /* Image Transfer
       * Class id-18,
       * 0-0:44.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_IMAGE_TRANSFER,        0,{0,0,44,0,0,255}
                                     ,7
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[FIRMWARE_START].instance_id, NULL},//Done
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &image_block_size, NULL}, //Done
                                              {3, ACCESS_PC___MRR__USR_, TAG_BITSTRING,       (void *) &image_transferred_blocks_status, get_image_transferred_blocks_status},
                                              {4, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &image_first_not_transferred_block_number, get_image_first_not_transferred_block_number},
                                              {5, ACCESS_PC___MRR__USR_, TAG_BOOLEAN,         (void *) &image_transfer_enabled, NULL},//done
                                              {6, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &image_transfer_status, NULL},//done
                                              {7, ACCESS_PC___MRR__USR_, TAG_ARRAY,           (void *) &image_to_activate_info, NULL}//done

                                     }
                                     ,4
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, image_transfer_initiate},
                                             {2, ACCESS_PC___MRRW_USRW, image_block_transfer},
                                             {3, ACCESS_PC___MRRW_USRW, image_verify},
                                             {4, ACCESS_PC___MRRW_USRW, image_activate}

                                      }
       },

      /* Image Activation Scheduler
       * Class id-22,
       * 0-0:15.0.2.255
       */
       //TODO

      /* Predefined Scripts – Image Activation
       * Class id-9,
       * 0-0:10.0.107.255
       */
       //TODO

      /****************************************Other abstract objects**********************************/

      /* Billing reset lockout time
       * This object defined the block period for reset after any reset.
       * Class id-3,
       * 1-0:0.9.12.255
       */
       //TODO

      /* End of billing by push bottom activation
       * Class id-1,
       * 1-0:94.34.120.255
       */
       //TODO Our meter doesn't have push button

      /* Metering point ID
       * Class id-1,
       * 0-0:96.1.10.255
       */
       //TODO Spanish standard Metering Point ID. C.U.P.S.

      /* Active Firmware identifier
       * Class id-1,
       * 1-0:0.2.0.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{1,0,0,2,0,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ABSTRACT_START].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) &active_firmware_version,NULL }

                                     }
                                     ,0
                                     ,NULL
       },

      /* Threshold for Demand close to contract power
       * Class id-3,
       * 0-0:94.34.70.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,94,34,70,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ABSTRACT_START + 1].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT16,          (void *) &power_demand_close_to_contract_power_thr, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_dem_colseToContractPower, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* IEC HDLC setup – Optical port
       * Class id-23,
       * 0-0:22.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_IEC_HDLC_SETUP,        1,{0,0,22,0,0,255}
                                     ,9
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ABSTRACT_START + 2].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &comm_Speed, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_UINT8,           (void *) &window_size_transmit, NULL},
                                             {4, ACCESS_PC___MRR__USR_, TAG_UINT8,           (void *) &window_size_receive, NULL},
                                             {5, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &max_info_field_len_tx, NULL},
                                             {6, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &max_info_field_len_rx, NULL},
                                             {7, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &inter_octet_timeout, NULL},
                                             {8, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &inactivity_time_out, NULL},
                                             {9, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &device_address, NULL},

                                     }
                                     ,0
                                     ,NULL
       },

      /* IEC HDLC setup – Serial port
       * Class id-23,
       * 0-0:22.0.0.255
       */
      //TODO

      /* Time for Scroll Display
       * Class id-3,
       * 0-0:94.34.110.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,94,34,110,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ABSTRACT_START + 3].instance_id, NULL},
                                             {2, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &scroll_display_time, change_display_scroll_time},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Timeout open session for reading
       * client through PLC channel
       * Class id-3,
       * 0-0:94.34.51.255
       */
      //TODO

      /* Timeout open session for Management
       * client through PLC channel
       * Class id-3,
       * 0-0:94.34.52.255
       */
      //TODO

      /* Timeout open session for Firmware update
       * client through PLC channel
       * Class id-3,
       * 0-0:94.34.53.255
       */
      //TODO

      /* Global Meter Reset
       * Parameters and registers reset
       * Class id-9,
       * 0-0:10.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_SCRIPT_TABLE,        0,{0,0,10,0,0,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ABSTRACT_START + 4].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USRW, TAG_ARRAY,           (void *) Data_Buffer, access_global_reset_scripts}

                                     }
                                     ,1
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, global_reset_script_execute}

                                      }
       },

      /* Time stamp for new calendar activation.
       * Parameters and registers reset
       * Class id-1,
       * 1-0:94.34.130.255
       */
       //TODO
/************************************************ELECTRICITY RELATED OBJECTS************************************************************/

      /*************************Energy Registers****************************************/

      /* Active energy import (+A)-Total
       * Absolute value
       * Class id-3,
       * 1-0:1.8.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,1,8,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->active_energy_import, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_total_active_energy_import, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Active energy import (+A)
       * Absolute value
       * Class id-3,
       * 1-0:1.8.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Active energy import (+A) incremental
       * Incremental Value
       * Class id-3,
       * 1-0:1.9.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Active energy import (+A) incremental for LP1
       * Incremental Value for Load Profile 1
       * Class id-3,
       * 1-0:1.29.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,1,29,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 1].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->active_energy_import, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_total_active_energy_import, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Active energy export (−A)-Total
       * Absolute value
       * Class id-3,
       * 1-0:2.8.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,2,8,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 2].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->active_energy_export, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_total_active_energy_import, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Active energy export (−A)
       * Absolute value
       * Class id-3,
       * 1-0:2.8.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Active energy export (−A) incremental
       * Incremental Value
       * Class id-3,
       * 1-0:2.9.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Active energy export (−A) incremental for LP1
       * Incremental Value for Load Profile 1
       * Class id-3,
       * 1-0:2.29.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,2,29,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 3].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &phase->active_energy_export, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_total_active_energy_import, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Reactive energy QI (+Ri)-Total
       * Absolute value
       * Class id-3,
       * 1-0:5.8.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,5,8,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 4].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.reactive_energy_Q1, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Reactive energy QI (+Ri)
       * Absolute value
       * Class id-3,
       * 1-0:5.8.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Reactive energy QI (+Ri) incremental
       * Incremental Value
       * Class id-3,
       * 1-0:5.9.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Reactive energy QI (+Ri) incremental for LP1
       * Incremental Value for Load Profile 1
       * Class id-3,
       * 1-0:5.29.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,5,29,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 5].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.reactive_energy_Q1, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Reactive energy QII (+Rc)-Total
       * Absolute value
       * Class id-3,
       * 1-0:6.8.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,6,8,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 6].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.reactive_energy_Q2, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Reactive energy QII (+Rc)
       * Absolute value
       * Class id-3,
       * 1-0:6.8.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Reactive energy QII (+Rc) incremental
       * Incremental Value
       * Class id-3,
       * 1-0:6.9.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Reactive energy QII (+Rc) incremental for LP1
       * Incremental Value for Load Profile 1
       * Class id-3,
       * 1-0:6.29.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,6,29,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 7].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.reactive_energy_Q2, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Reactive energy QIII (-Ri)-Total
       * Absolute value
       * Class id-3,
       * 1-0:7.8.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,7,8,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 8].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.reactive_energy_Q3, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Reactive energy QIII (-Ri)
       * Absolute value
       * Class id-3,
       * 1-0:7.8.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Reactive energy QIII (-Ri) incremental
       * Incremental Value
       * Class id-3,
       * 1-0:7.9.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Reactive energy QIII (-Ri) incremental for LP1
       * Incremental Value for Load Profile 1
       * Class id-3,
       * 1-0:7.29.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,7,29,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 9].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.reactive_energy_Q3, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Reactive energy QIV (-Rc)-Total
       * Absolute value
       * Class id-3,
       * 1-0:8.8.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,8,8,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 10].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.reactive_energy_Q4, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Reactive energy QIV (-Rc)
       * Absolute value
       * Class id-3,
       * 1-0:8.8.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Reactive energy QIV (-Rc) incremental
       * Incremental Value
       * Class id-3,
       * 1-0:8.9.x.255
       *    x=10. Total rate contract 1
       *    x=11..16. rates contract 1
       *    x=20. Total rate contract 2
       *    x=21..26 rates contract 2
       *    x=30. Total rate contract 3
       *    x=31..36 rates contract 3
       *    255 = current billing period
       */
      //TODO

      /* Reactive energy QIV (-Rc) incremental for LP1
       * Incremental Value for Load Profile 1
       * Class id-3,
       * 1-0:8.29.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,8,29,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 11].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.reactive_energy_Q4, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_reactive_energy, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous Energy Values-Profile generic
       * Class id-7,
       * 0-0:21.0.6.255
       */
       {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,21,0,6,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[ENERGY_REG_START + 12].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_instantaneous_energy_profile_data},
                                              {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) instantaneous_energy_capture_objects, NULL},
                                              {4, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instantaneous_energy_capture_period, NULL},
                                              {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &instantaneous_energy_sort_method, NULL},
                                              {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                              {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instantaneous_energy_entries_in_use, NULL},
                                              {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instantaneous_energy_entries, NULL}

                                     }
                                     ,0
                                     ,NULL
       },



      /* Current billing values-Profile generic
       * Class id-7,
       * 0-0:21.0.z.255
       * z=12,13
       */
      //TODO

      /*************************************Demand Registers*********************************/

      /* Demand Register import (+A)
       * Average demand
       * Class id-5,
       * 1-0:1.4.0.255
       */
      //TODO

      /* Demand Register export (-A)
       * Average demand
       * Class id-5,
       * 1-0:2.4.0.255
       */
      //TODO

      /* Maximum Demand Register import (+A)
       * Class id-4,
       * 1-0:1.6.0.255
       */
      //TODO

      /* Maximum Demand Register export (-A)
       * Class id-4,
       * 1-0:2.6.0.255
       */
      //TODO

      /*************************************Load Profiles********************************/

      /* AMR profile status for Load profile with period 1
       * Class id-1,
       * 0-0:96.10.7.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,10,7,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[LOAD_PROF_START].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,           (void *) &amr_profile_status, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Load profile with period 1
       * Hourly Load Profile
       * Class id-7,
       * 1-0:99.1.0.255
       */
       {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{1,0,99,1,0,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[LOAD_PROF_START + 1].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_load_profile_data},
                                              {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) load_profile_capture_objects, NULL},
                                              {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &load_profile_capture_period, change_hourly_load_profile_capture_period},
                                              {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &load_profile_sort_method, NULL},
                                              {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                              {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &load_profile_entries_in_use, NULL},
                                              {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &load_profile_entries, NULL}

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MR___USR_, obj_load_profile_reset},
                                             {2, ACCESS_PC___MR___USR_, obj_load_profile_capture}

                                     }
       },

      /* AMR profile status for Load profile with period 2
       * Class id-1,
       * 0-0:96.10.8.255
       */
      {ASSOC_MR_US, CLASS_ID_DATA,        0,{0,0,96,10,8,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[LOAD_PROF_START + 2].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &daily_amr_profile_status, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Load profile with period 2
       * Daily Load Profile
       * Class id-7,
       * 1-0:99.2.0.255
       */
      {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{1,0,99,2,0,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[LOAD_PROF_START + 3].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_daily_load_profile_data},
                                              {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) daily_load_profile_capture_objects, NULL},
                                              {4, ACCESS_PC___MRRW_USRW, TAG_UINT32,          (void *) &daily_load_profile_capture_period, change_daily_load_profile_capture_period},
                                              {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &daily_load_profile_sort_method, NULL},
                                              {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                              {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &daily_load_profile_entries_in_use, NULL},
                                              {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &daily_load_profile_entries, NULL},

                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, obj_daily_load_profile_reset},
                                             {2, ACCESS_PC___MRRW_USRW, obj_daily_load_profile_capture}

                                     }
       },

      /**********************************Instantaneous Values***********************/

      /* Instantaneous voltage L1
       * Class id-3,
       * 1-0:32.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,32,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.V_rms , NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_V, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous current L1
       * Class id-3,
       * 1-0:31.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,31,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 1].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.I_rms, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_I, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous voltage L2
       * Class id-3,
       * 1-0:52.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,52,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 2].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instant_l2_l3_v_c, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_V, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous current L2
       * Class id-3,
       * 1-0:51.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,51,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 3].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instant_l2_l3_v_c, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_I, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

       /* Instantaneous voltage L3
       * Class id-3,
       * 1-0:72.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,72,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 4].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instant_l2_l3_v_c, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_V, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous current L3
       * Class id-3,
       * 1-0:71.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,71,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 5].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instant_l2_l3_v_c, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_I, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

	  /* IInstantaneous current (sum over all phases)
       * Class id-3,
       * 1-0:90.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,90,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 6].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.I_rms, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_I, NULL}

                                     }
                                     ,0
                                     ,NULL
       },
      /* Instantaneous active power +(P)
       * Class id-3,
       * 1-0:1.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,1,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 7].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &P_Active_Plus, NULL}, //check this
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_active_power, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous active power -(P)
       * Class id-3,
       * 1-0:2.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,2,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 8].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &P_Active_Negative, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_active_power, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous reactive power + (Q)
       * Class id-3,
       * 1-0:3.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,3,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 9].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &P_Reactive_positive, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_active_power, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous reactive power - (Q)
       * Class id-3,
       * 1-0:4.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,4,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 10].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &P_Reactive_negative, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_active_power, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous power factor (PF)
       * Class id-3,
       * 1-0:13.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,13,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 11].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_INT16,           (void *) &phase->readings.power_factor, NULL},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_PF, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Instantaneous Values
       * Class id-7,
       * 0-0:21.0.5.255
       */
       {ASSOC_MR_US, CLASS_ID_PROFILE_GENERIC,        1,{0,0,21,0,5,255}
                                     ,8
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[INST_VALUES_START + 12].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) Data_Buffer, capture_instantaneous_profile_data},
                                              {3, ACCESS_PCR__MRR__USR_, TAG_ARRAY,           (void *) instantaneous_values_capture_objects, NULL},
                                              {4, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instantaneous_values_capture_period, NULL},
                                              {5, ACCESS_PC___MRR__USR_, TAG_ENUM,            (void *) &instantaneous_values_sort_method, NULL},
                                              {6, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) NULL, NULL},
                                              {7, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instantaneous_values_entries_in_use, NULL},
                                              {8, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &instantaneous_values_entries, NULL},

                                     }
                                     ,0
                                     ,NULL
       },

      /************************Power Quality (Voltage sags and swells)*******************************/

      /* Reference Voltage for power quality measurement
       * Class id-3,
       * 1-0:0.6.4.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,0,6,4,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[POWER_QUALITY_START].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,          (void *) &nominal_voltage, change_reference_voltage},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_referance_voltage, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Time Threshold for voltage sag
       * Class id-3,
       * 1-0:12.43.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,12,43,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[POWER_QUALITY_START + 1].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,          (void *) &under_voltage_trip_duration, change_time_threshold_voltage_sag},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Threshold for voltage sag
       * Class id-3,
       * 1-0:12.31.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,12,31,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[POWER_QUALITY_START + 2].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,          (void *) &voltage_sag_threshold, change_threshold_voltage_sag},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_ThresholdVoltageSag_Swell, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Number of voltage sags in any phase
       * Class id-1,
       * 1-0:12.32.0.255
       */
      //TODO

      /* Duration of voltage sags in any phase
       * Class id-3,
       * 1-0:12.33.0.255
       */
      //TODO

      /* Duration of current voltage sags in any phase
       * Class id-3,
       * 1-0:12.33.1.255
       */
      //TODO

      /* Number of voltage sags in phase L1
       * Class id-1,
       * 1-0:32.32.0.255
       */
      //TODO

      /* Duration of voltage sags in phase L1
       * Class id-1,
       * 1-0:32.33.0.255
       */
      //TODO

      /* Time Threshold for voltage swell
       * Class id-3,
       * 1-0:12.44.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,12,44,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[POWER_QUALITY_START + 3].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USRW, TAG_UINT16,          (void *) &over_voltage_trip_duration, change_time_threshold_voltage_swell},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Time, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Threshold for voltage swell
       * Class id-3,
       * 1-0:12.35.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,12,35,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[POWER_QUALITY_START + 4].instance_id, NULL},
                                             {2, ACCESS_PC___MRRW_USRW, TAG_UINT16,          (void *) &voltage_swell_threshold, change_threshold_voltage_swell},
                                             {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_ThresholdVoltageSag_Swell, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Number of voltage swells in any phase
       * Class id-1,
       * 1-0:12.36.0.255
       */
      //TODO

      /* Duration of voltage swells in any phase
       * Class id-3,
       * 1-0:12.37.0.255
       */
      //TODO

      /* Duration of current voltage swells in any phase
       * Class id-3,
       * 1-0:12.37.1.255
       */
      //TODO

      /* Number of voltage swells in phase L1
       * Class id-1,
       * 1-0:32.36.0.255
       */
      //TODO

      /* Duration of voltage sags in phase L1
       * Class id-1,
       * 1-0:32.37.0.255
       */
      //TODO

      /*****************************Other electricity related objects***********************************/

      /* Active Quadrant
       * Class id-1,
       * 1-1:94.34.100.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{1,1,94,34,100,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ELEC_OBJS_START].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USRW, TAG_UINT8,           (void *) &active_quadrant, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Phase presence
       * Class id-1,
       * 1-1:94.34.104.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{1,1,94,34,104,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR__MSR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ELEC_OBJS_START + 1].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USR__MSR_, TAG_UINT8,           (void *) &phase_presence, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Transformer ratio – current (numerator)
       * Class id-1,
       * 1-0:0.4.2.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{1,0,0,4,2,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ELEC_OBJS_START + 2].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USRW, TAG_UINT16,          (void *) &transformer_ratio_current_num, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Transformer ratio – voltage (numerator)
       * Class id-1,
       * 1-0:0.4.3.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{1,0,0,4,3,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ELEC_OBJS_START + 3].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USRW, TAG_UINT16,          (void *) &transformer_ratio_voltage_num, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Transformer ratio – current (denominator)
       * Class id-1,
       * 1-0:0.4.5.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{1,0,0,4,5,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ELEC_OBJS_START + 4].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USRW, TAG_UINT16,          (void *) &transformer_ratio_current_den, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Transformer ratio – voltage (denominator)
       * Class id-1,
       * 1-0:0.4.6.255
       */
       {ASSOC_MR_US, CLASS_ID_DATA,        0,{1,0,0,4,6,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[OTHER_ELEC_OBJS_START + 5].instance_id, NULL},
                                              {2, ACCESS_PCR__MRR__USRW, TAG_UINT16,          (void *) &transformer_ratio_voltage_den, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /********************Objects to set up data exchange using PRIME PLC***********************/

      /* PRIME PLC Application identification
       * Class id-86,
       * 0-0:28.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_PRIME_PLC_App_ID,        0,{0,0,28,7,0,255}
                                     ,4
                                     ,(const struct attribute_desc_s[]) {

                                             {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PRIME_PLC_START].instance_id, NULL},
                                             {2, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) &plc_firmware_Version, get_set_firmware_version},
                                             {3, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &plc_vendor_id, NULL},
                                             {4, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &plc_product_id, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* PRIME device setup
       * Class id-86,
       * 0-0:28.6.0.255
       */
       {ASSOC_MR_US, CLASS_ID_ETHERNET_SETUP,        0,{0,0,28,6,0,255}
                                     ,2
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[PRIME_PLC_START + 1].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_OCTET_STRING,    (void *) &mac_address, get_set_mac_address}

                                     }
                                     ,0
                                     ,NULL
       },

      /***************************************************REFERENCE IMPLEMENTATION********************************************************************/

      /* Supply Frequency, Inst. value
       * Class id-3,
       * 1-0:14.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,14,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT16,          (void *) &phase->readings.frequency, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_Frequency, NULL}

                                     }
                                     ,0
                                     ,NULL
       },

      /* Total Apparent power, Inst. value
       * Class id-3,
       * 1-0:9.7.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{1,0,9,7,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PCR__MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START + 1].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &phase->readings.apparent_power, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_KVA, NULL}
                                     }
                                     ,0
                                     ,NULL
       },

      /* Alarm reset-persistent error reset
       * Class id-1,
       * 0-0:140.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,140,0,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START + 2].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_INT16,           (void *) &alarm_status, reset_persistent_events},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_power_current_voltage_calib, NULL}
       	   	   	   	   	   	   	   	 }
                                     ,1
                                     ,(const struct method_desc_s[]) {

                                              {1, ACCESS_PC___MRRW_USRW, reset_alarms}

                                     }
       },

      /* Power scaler calibration
       * Class id-3,
       * 0-0:141.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,141,0,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START + 3].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_INT16,           (void *) Data_Buffer, calibrate_power_scaler},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_power_current_voltage_calib, NULL}
                                     }
                                     ,1
                                     ,(const struct method_desc_s[]) {

                                              {1, ACCESS_PC___MRRW_USRW, calibrate_power}

                                     }
       },

      /* Current scaler calibration
       * Class id-3,
       * 0-0:142.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,142,0,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START + 4].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_INT16,           (void *) Data_Buffer, calibrate_current_scaler},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_power_current_voltage_calib, NULL}
                                     }
                                     ,1
                                     ,(const struct method_desc_s[]) {

                                              {1, ACCESS_PC___MRRW_USRW, calibrate_current}

                                     }
       },

      /* Voltage scaler calibration
       * Class id-3,
       * 0-0:143.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,143,0,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START + 5].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_INT16,           (void *) Data_Buffer, calibrate_voltage_scaler},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_power_current_voltage_calib, NULL}
                                     }
                                     ,1
                                     ,(const struct method_desc_s[]) {

                                              {1, ACCESS_PC___MRRW_USRW, calibrate_voltage}

                                     }
       },

      /* Balance
       * Class id-73,
       * 0-0:131.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_BALANCE,        0,{0,0,131,0,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START + 6].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) &Current_balance.balance, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_PF, NULL}
                                     }
                                     ,2
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, increment_balance},
                                             {2, ACCESS_PC___MRRW_USRW, decrement_balance}

                                     }
       },

	  /* BOR reset
       * Class id-3,
       * 0-0:144.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,144,0,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START + 7].instance_id, NULL},
                                              {2, ACCESS_PC___MRR__USR_, TAG_UINT32,          (void *) Data_Buffer, NULL},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_PF, NULL}

                                     }
                                     ,1
                                     ,(const struct method_desc_s[]) {

                                             {1, ACCESS_PC___MRRW_USRW, bor_reset},
                                     }
       },

	  /* USB communication baudrate config
       * Class id-3,
       * 0-0:145.0.0.255
       */
       {ASSOC_MR_US, CLASS_ID_REGISTER,        0,{0,0,145,0,0,255}
                                     ,3
                                     ,(const struct attribute_desc_s[]) {

                                              {1, ACCESS_PC___MRR__USR_, TAG_OCTET_STRING,    (void *) object_list[REFERENCE_OBIS_START + 8].instance_id, NULL},
                                              {2, ACCESS_PC___MRRW_USRW, TAG_UINT8,           (void *) &usb_comm_speed, configure_usb_baudrate},
                                              {3, ACCESS_PC___MRR__USR_, TAG_STRUCTURE,       (void *) &scalar_unit_referance_voltage, NULL}

                                     }
                                     ,0
                                     ,NULL
       },


       {0,0, 0, {0, 0, 0, 0, 0, 0}, 0,  NULL}

  //     {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   1,  94, 34, 33, 255},  3,  Obj_ExportActivePowerThreshold, 0, NULL},//31 //done
  //     {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   1,  94, 34, 31, 255},  3,  Obj_ImportActivePowerThreshold, 0, NULL},//30 //done

    // {ASSOC_MR_US,       CLASS_ID_SCRIPT_TABLE,             0, {  0,   0,  10,  0, 100, 255}, 2,  Obj_Tariff_Script_Table, 1, Obj_Tariff_Script_Methods},//13
    // {ASSOC_US,                CLASS_ID_DATA,               0, {  1,   0,   0,  8,   0, 255}, 2,   Obj_Demand_Integration_Period, 0, NULL},//19
    // {ASSOC_US,                CLASS_ID_DATA,               0, {  1,   0,   0,  8,   4, 255}, 2,   Obj_Profile_Capture_Period, 0, NULL},//20 done, intergrated
    // {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  94, 91,   0, 255}, 2,   Obj_Cum_Tamper_Count, 0, NULL},//24 done, integrated
    // {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,   0,  1,   0, 255}, 2,   Obj_Cum_MD_Reset_Count, 0, NULL},//25
    // {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  96,  2,   0, 255}, 2,   Obj_Cum_Prog_Count, 0, NULL},//26
    // {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,   0,  1,   2, 255}, 2,   Obj_Last_MD_Rst_DT, 0, NULL},//27
    // {ASSOC_MR_US,             CLASS_ID_DATA,               0, {  0,   0,  00, 00, 5,   255},  2,  Obj_OpearatingMode, 0, NULL},//30
    // {ASSOC_MR_US,       CLASS_ID_REGISTER,                 0, {  0,   0,  00, 00, 4,   255}, 3,  Obj_NeutralCurrent, 0, NULL},//17





};
//Tariff Scheam
//LanguageSetting_SIZE
//Low battery back up time
//Monthly billing log 
//current phase //on with phase the meter is connected
//Energy log time 
