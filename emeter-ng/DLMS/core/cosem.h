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
//  File: cosem.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: cosem.h,v 1.3 2010/05/13 07:47:36 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//

#if !defined(_COSEM_H_)
#define _COSEM_H_

#define DLMS_PROTOCOL_VERSION       6

/* Standardised DLMS PDUs used in COSEM - IEC62056-53 8.1 */
enum
{
    /* DLMS PDUs (no encryption selected) */
    PDU_INITIATE_REQUEST = 1,
    PDU_READ_REQUEST = 5,
    PDU_WRITE_REQUEST = 6,
    PDU_INITIATE_RESPONSE = 8,
    PDU_READ_RESPONSE = 12,
    PDU_WRITE_RESPONSE = 13,
    PDU_CONFIRMED_SERVICE_ERROR = 14,
    PDU_UNCONFIRMED_WRITE_REQUEST = 22,
    PDU_INFORMATION_REPORT_REQUEST = 24,
    /* The ACSE APDUs */
    PDU_AARQ = 96,
    PDU_AARE = 97,
    PDU_RLRQ = 98,
    PDU_RLRE = 99,
    /* APDUs used for data commmunication services, using LN referencing */
    PDU_GET_REQUEST = 192,
    PDU_SET_REQUEST = 193,
    PDU_EVENT_NOTIFICATION_REQUEST = 194,
    PDU_ACTION_REQUEST = 195,
    PDU_GET_RESPONSE = 196,
    PDU_SET_RESPONSE = 197,
    PDU_ACTION_RESPONSE = 199,
    /* Global ciphered PDUs */
    PDU_GLO_GET_REQUEST = 200,
    PDU_GLO_SET_REQUEST = 201,
    PDU_GLO_EVENT_NOTIFICATION_REQUEST = 202,
    PDU_GLO_ACTION_REQUEST = 203,
    PDU_GLO_GET_RESPONSE = 204,
    PDU_GLO_SET_RESPONSE = 205,
    PDU_GLO_ACTION_RESPONSE = 207,
    /* Dedicated ciphered PDUs */
    PDU_DED_GET_REQUEST = 208,
    PDU_DED_SET_REQUEST = 209,
    PDU_DED_EVENT_NOTIFICATION_REQUEST = 210,
    PDU_DED_ACTION_REQUEST = 211,
    PDU_DED_GET_RESPONSE = 212,
    PDU_DED_SET_RESPONSE = 213,
    PDU_DED_ACTION_RESPONSE = 215
};

#define IMPLICIT_BASE   0x80
#define EXPLICIT_BASE   0xA0

enum
{
    AARQ_APDU_TAG_PROTOCOL_VERSION = (IMPLICIT_BASE + 0),
    AARQ_APDU_TAG_APPLICATION_CONTEXT_NAME = (EXPLICIT_BASE + 1),
    AARQ_APDU_TAG_CALLED_AP_TITLE = (EXPLICIT_BASE + 2),
    AARQ_APDU_TAG_CALLED_AE_QUALIFIER = (EXPLICIT_BASE + 3),
    AARQ_APDU_TAG_CALLED_AP_INVOCATION_ID = (EXPLICIT_BASE + 4),
    AARQ_APDU_TAG_CALLED_AE_INVOCATION_ID = (EXPLICIT_BASE + 5),
    AARQ_APDU_TAG_CALLING_AP_TITLE = (EXPLICIT_BASE + 6),
    AARQ_APDU_TAG_CALLING_AE_QUALIFIER = (EXPLICIT_BASE + 7),
    AARQ_APDU_TAG_CALLING_AP_INVOCATION_ID = (EXPLICIT_BASE + 8),
    AARQ_APDU_TAG_CALLING_AE_INVOCATION_ID = (EXPLICIT_BASE + 9),
    AARQ_APDU_TAG_SENDER_ACSE_REQUIREMENTS = (IMPLICIT_BASE + 10),
    AARQ_APDU_TAG_MECHANISM_NAME = (IMPLICIT_BASE + 11),
    AARQ_APDU_TAG_CALLING_AUTHENTICATION_VALUE = (EXPLICIT_BASE + 12),
    AARQ_APDU_TAG_IMPLEMENTATION_INFORMATION = (EXPLICIT_BASE + 29),
    AARQ_APDU_TAG_USER_INFORMATION = (EXPLICIT_BASE + 30)
};

enum
{
    AARE_APDU_TAG_PROTOCOL_VERSION = (IMPLICIT_BASE + 0),
    AARE_APDU_TAG_APPLICATION_CONTEXT_NAME = (EXPLICIT_BASE + 1),
    AARE_APDU_TAG_RESULT = (EXPLICIT_BASE + 2),
    AARE_APDU_TAG_RESULT_SOURCE_DIAGNOSTIC = (EXPLICIT_BASE + 3),
    AARE_APDU_TAG_RESPONDING_AP_TITLE = (EXPLICIT_BASE + 4),
    AARE_APDU_TAG_RESPONDING_AP_QUALIFIER = (EXPLICIT_BASE + 5),
    AARE_APDU_TAG_RESPONDING_AP_INVOCATION_ID = (EXPLICIT_BASE + 6),
    AARE_APDU_TAG_RESPONDING_AE_INVOCATION_ID = (EXPLICIT_BASE + 7),
    AARE_APDU_TAG_RESPONDER_ACSE_REQUIREMENTS = (IMPLICIT_BASE + 8),
    AARE_APDU_TAG_MECHANISM_NAME = (IMPLICIT_BASE + 9),
    AARE_APDU_TAG_RESPONDING_AUTHENTICATION_VALUE = (EXPLICIT_BASE + 10),
    AARE_APDU_TAG_IMPLEMENTATION_INFORMATION = (EXPLICIT_BASE + 29),
    AARE_APDU_TAG_USER_INFORMATION = (EXPLICIT_BASE + 30)
};

enum
{
    TAG_AUTHENTICATION_INFORMATION_COMPONENT = (0x80 + 0),
    TAG_RESPONDER_ACSE_REQUIREMENTS_FIELD_COMPONENT = (0x80 + 8),
    TAG_RESPONDER_MECHANISM_NAME_COMPONENT = (0x80 + 9),
    TAG_ACSE_REQUIREMENTS_FIELD_COMPONENT = (0x80 + 10),
    TAG_MECHANISM_NAME_COMPONENT = (0x80 + 11),
};

/*
    ASN.1 BER tags break down according to bits 7 and 6
    Universal               0x00 - 0x3F
    Application specific    0x40 - 0x7F
    Context specific        0x80 - 0xBF
    Private                 0xC0 - 0xFF
 */
enum
{
    /* P = primitive, C = constructed */
    TAG_UNIVERSAL_EOC = 0,                  /* P (End-of-Content) */
    TAG_UNIVERSAL_BOOLEAN = 1,              /* P */
    TAG_UNIVERSAL_INTEGER = 2,              /* P */
    TAG_UNIVERSAL_BIT_STRING = 3,           /* P/C */
    TAG_UNIVERSAL_OCTET_STRING = 4,         /* P/C */
    TAG_UNIVERSAL_NULL = 5,                 /* P */
    TAG_UNIVERSAL_OBJECT_IDENTIFIER = 6,    /* P */
    TAG_UNIVERSAL_OBJECT_DESCRIPTOR = 7,    /* P */
    TAG_UNIVERSAL_EXTERNAL = 8,             /* C */
    TAG_UNIVERSAL_REAL = 9,                 /* P */
    TAG_UNIVERSAL_ENUMERATED = 10,          /* P */
    TAG_UNIVERSAL_EMBEDDED_PDV = 11,        /* C */
    TAG_UNIVERSAL_UTF8STRING = 12,          /* P/C */
    TAG_UNIVERSAL_RELATIVE_OID = 13,        /* P */
    TAG_UNIVERSAL_SEQUENCE = 16,            /* C (also TAG_UNIVERSAL_SEQUENCE_OF) */
    TAG_UNIVERSAL_SET = 17,                 /* C (also TAG_UNIVERSAL_SET_OF) */
    TAG_UNIVERSAL_NUMERIC_STRING = 18,      /* P/C */
    TAG_UNIVERSAL_PRINTABLE_STRING = 19,    /* P/C */
    TAG_UNIVERSAL_T61_STRING = 20,          /* P/C */
    TAG_UNIVERSAL_VIDEOTEX_STRING = 21,     /* P/C */
    TAG_UNIVERSAL_IA5_STRING = 22,          /* P/C */
    TAG_UNIVERSAL_UTC_TIME = 23,            /* P/C */
    TAG_UNIVERSAL_GENERALIZED_TIME = 24,    /* P/C */
    TAG_UNIVERSAL_GRAPHIC_STRING = 25,      /* P/C */
    TAG_UNIVERSAL_VISIBLE_STRING = 26,      /* P/C */
    TAG_UNIVERSAL_GENERAL_STRING = 27,      /* P/C */
    TAG_UNIVERSAL_UNIVERSAL_STRING = 28,    /* P/C */
    TAG_UNIVERSAL_CHARACTER_STRING = 29,    /* P/C */
    TAG_UNIVERSAL_BMPSTRING = 30            /* P/C */
};

enum
{
    TAG_NULLDATA = 0,
    TAG_ARRAY = 1,
    TAG_STRUCTURE = 2,
    TAG_BOOLEAN = 3,
    TAG_BITSTRING = 4,
    TAG_INT32 = 5,
    TAG_UINT32 = 6,
    TAG_FLOATING_POINT = 7,
    TAG_OCTET_STRING = 9,
    TAG_VISIBLE_STRING = 10,
    TAG_GENERALIZED_TIME = 11,
    TAG_BCD = 13,
    TAG_INT8 = 15,
    TAG_INT16 = 16,
    TAG_UINT8 = 17,
    TAG_UINT16 = 18,
    TAG_COMPACT_ARRAY = 19,
    TAG_INT64 = 20,
    TAG_UINT64 = 21,
    TAG_ENUM = 22,
    TAG_FLOAT32 = 23,
    TAG_FLOAT64 = 24,
    TAG_DATETIME = 25,
    TAG_DATE = 26,
    TAG_TIME = 27,
    TAG_DONTCARE = 255
};

/* Conformance block (60256-53/8.5 and 62056-53/Annex C)
    Reserved0                    byte 9 bit 7
    Reserved1                    byte 9 bit 6
    Reserved2                    byte 9 bit 5
    Read                         byte 9 bit 4
    Write                        byte 9 bit 3
    UnconfirmedWrite             byte 9 bit 2
    Reserved3                    byte 9 bit 1
    Reserved4                    byte 9 bit 0

    Attribute0SupportedWithSet   byte 10 bit 7
    PriorityMgmtSupported        byte 10 bit 6
    Attribute0SupportedWithGet   byte 10 bit 5
    BlockTransferWithGet         byte 10 bit 4
    BlockTransferWithSet         byte 10 bit 3
    BlockTransferWithAction      byte 10 bit 2
    MultipleReferences           byte 10 bit 1
    InformationReport            byte 10 bit 0

    Reserved5                    byte 11 bit 7
    Reserved6                    byte 11 bit 6
    ParameterizedAccess          byte 11 bit 5
    Get                          byte 11 bit 4
    Set                          byte 11 bit 3
    SelectiveAccess              byte 11 bit 2
    EventNotification            byte 11 bit 1
    Action                       byte 11 bit 0
*/

enum
{
    ACCESS_RESULT_SUCCESS = 0,
    ACCESS_RESULT_HARDWARE_FAULT = 1,
    ACCESS_RESULT_TEMPORARY_FAILURE = 2,
    ACCESS_RESULT_READ_WRITE_DENIED = 3,
    ACCESS_RESULT_OBJECT_UNDEFINED = 4,
    ACCESS_RESULT_OBJECT_CLASS_INCONSISTENT = 9,
    ACCESS_RESULT_OBJECT_UNAVAILABLE = 11,
    ACCESS_RESULT_TYPE_UNMATCHED = 12,
    ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED = 13,
    ACCESS_RESULT_DATA_BLOCK_UNAVAILABLE = 14,
    ACCESS_RESULT_LONG_GET_ABORTED = 15,
    ACCESS_RESULT_NO_LONG_GET_IN_PROGRESS = 16,
    ACCESS_RESULT_LONG_SET_ABORTED = 17,
    ACCESS_RESULT_NO_LONG_SET_IN_PROGRESS = 18,
    ACCESS_RESULT_OTHER_REASON = 250
};

enum
{
    ACTION_RESULT_SUCCESS = 0,
    ACTION_RESULT_HARDWARE_FAULT = 1,
    ACTION_RESULT_TEMPORARY_FAILURE = 2,
    ACTION_RESULT_READ_WRITE_DENIED = 3,
    ACTION_RESULT_OBJECT_UNDEFINED = 4,
    ACTION_RESULT_OBJECT_CLASS_INCONSISTENT = 9,
    ACTION_RESULT_OBJECT_UNAVAILABLE = 11,
    ACTION_RESULT_TYPE_UNMATCHED = 12,
    ACTION_RESULT_SCOPE_OF_ACCESS_VIOLATED = 13,
    ACTION_RESULT_DATA_BLOCK_UNAVAILABLE = 14,
    ACTION_RESULT_LONG_ACTION_ABORTED = 15,
    ACTION_RESULT_NO_LONG_ACTION_IN_PROGRESS = 16,
    ACTION_RESULT_OTHER_REASON = 250
};

enum
{
    CONFIRMED_SERVICE_ERROR_INITIATEERROR = 1,
    CONFIRMED_SERVICE_ERROR_GETSTATUS = 2,
    CONFIRMED_SERVICE_ERROR_GETNAMELIST = 3,
    CONFIRMED_SERVICE_ERROR_GETVARIABLEATTRIBUTE = 4,
    CONFIRMED_SERVICE_ERROR_READ = 5,
    CONFIRMED_SERVICE_ERROR_WRITE = 6,
    CONFIRMED_SERVICE_ERROR_GETDATASETATTRIBUTE = 7,
    CONFIRMED_SERVICE_ERROR_GETTIATTRIBUTE = 8,
    CONFIRMED_SERVICE_ERROR_CHANGESCOPE = 9,
    CONFIRMED_SERVICE_ERROR_START = 10,
    CONFIRMED_SERVICE_ERROR_STOP = 11,
    CONFIRMED_SERVICE_ERROR_RESUME = 12,
    CONFIRMED_SERVICE_ERROR_MAKEUSABLE = 13,
    CONFIRMED_SERVICE_ERROR_INITIATELOAD = 14,
    CONFIRMED_SERVICE_ERROR_LOADSEGMENT = 15,
    CONFIRMED_SERVICE_ERROR_TERMINATELOAD = 16,
    CONFIRMED_SERVICE_ERROR_INITIATEUPLOAD = 17,
    CONFIRMED_SERVICE_ERROR_UPLOADSEGMENT = 18,
    CONFIRMED_SERVICE_ERROR_TERMINATEUPLOAD = 19
};

enum
{
    SERVICE_ERROR_APPLICATION_REFERENCE = 0,
    SERVICE_ERROR_HARDWARE_RESOURCE = 1,
    SERVICE_ERROR_VDE_STATE_ERROR = 2,
    SERVICE_ERROR_SERVICE = 3,
    SERVICE_ERROR_DEFINITION = 4,
    SERVICE_ERROR_ACCESS = 5,
    SERVICE_ERROR_INITIATE = 6,
    SERVICE_ERROR_LOAD_DATA_SET = 7,
    SERVICE_ERROR_TASK = 9,
    SERVICE_ERROR_OTHER_ERROR = 10
};

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(__cplusplus)
}
#endif

#endif

/*- End of file ------------------------------------------------------------*/
