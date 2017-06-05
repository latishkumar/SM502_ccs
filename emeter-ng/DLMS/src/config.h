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
//  File: config.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: config.h,v 1.18 2011/11/18 08:20:50 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//
//#include <server/server_msgs.h>
/* Select which of the conformance block features are to be supported */
#ifndef _CONFIG_H
#define _CONFIG_H
#undef IEC62056_SUPPORT_RESERVED0_A0
#undef IEC62056_SUPPORT_RESERVED1_A0
#undef IEC62056_SUPPORT_RESERVED2_A0
#undef IEC62056_SUPPORT_READ_A0
#undef IEC62056_SUPPORT_WRITE_A0
#undef IEC62056_SUPPORT_UNCONFIRMEDWRITE_A0
#undef IEC62056_SUPPORT_RESERVED3_A0
#undef IEC62056_SUPPORT_RESERVED4_A0
/**/
#undef IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET_A0
#undef IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED_A0
#undef IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET_A0
#define IEC62056_SUPPORT_BLOCKTRANSFERWITHGET_A0
#undef IEC62056_SUPPORT_BLOCKTRANSFERWITHSET_A0
#undef IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION_A0
#undef IEC62056_SUPPORT_MULTIPLEREFERENCES_A0
#undef IEC62056_SUPPORT_INFORMATIONREPORT_A0
/**/
#undef IEC62056_SUPPORT_RESERVED5_A0
#undef IEC62056_SUPPORT_RESERVED6_A0
#undef IEC62056_SUPPORT_PARAMETERIZEDACCESS_A0
#define IEC62056_SUPPORT_GET_A0
#undef IEC62056_SUPPORT_SET_A0
#undef IEC62056_SUPPORT_SELECTIVEACCESS_A0
#undef IEC62056_SUPPORT_EVENTNOTIFICATION_A0
#undef IEC62056_SUPPORT_ACTION_A0

#undef IEC62056_SUPPORT_GLOBAL_CIPHER
#undef IEC62056_SUPPORT_DEDICATED_CIPHER

// Association 1 - Low Level Security

#undef IEC62056_SUPPORT_RESERVED0_A1
#undef IEC62056_SUPPORT_RESERVED1_A1
#undef IEC62056_SUPPORT_RESERVED2_A1
#undef IEC62056_SUPPORT_READ_A1
#undef IEC62056_SUPPORT_WRITE_A1
#undef IEC62056_SUPPORT_UNCONFIRMEDWRITE_A1
#undef IEC62056_SUPPORT_RESERVED3_A1
#undef IEC62056_SUPPORT_RESERVED4_A1
/**/
#undef IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET_A1
#undef IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED_A1
#undef IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET_A1
#define IEC62056_SUPPORT_BLOCKTRANSFERWITHGET_A1
#undef IEC62056_SUPPORT_BLOCKTRANSFERWITHSET_A1
#undef IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION_A1
#undef IEC62056_SUPPORT_MULTIPLEREFERENCES_A1
#undef IEC62056_SUPPORT_INFORMATIONREPORT_A1
/**/
#undef IEC62056_SUPPORT_RESERVED5_A1
#undef IEC62056_SUPPORT_RESERVED6_A1
#undef IEC62056_SUPPORT_PARAMETERIZEDACCESS_A1
#define IEC62056_SUPPORT_GET_A1
//#undef IEC62056_SUPPORT_SET_A1
#define IEC62056_SUPPORT_SET_A1
#define IEC62056_SUPPORT_SELECTIVEACCESS_A1
#undef IEC62056_SUPPORT_EVENTNOTIFICATION_A1
#define IEC62056_SUPPORT_ACTION_A1
//#undef IEC62056_SUPPORT_ACTION_A1

#undef IEC62056_SUPPORT_GLOBAL_CIPHER_A1
#undef IEC62056_SUPPORT_DEDICATED_CIPHER_A1

//Association 2 - High Level Security

#undef IEC62056_SUPPORT_RESERVED0_A2
#undef IEC62056_SUPPORT_RESERVED1_A2
#undef IEC62056_SUPPORT_RESERVED2_A2
#undef IEC62056_SUPPORT_READ_A2
#undef IEC62056_SUPPORT_WRITE_A2
#undef IEC62056_SUPPORT_UNCONFIRMEDWRITE_A2
#undef IEC62056_SUPPORT_RESERVED3_A2
#undef IEC62056_SUPPORT_RESERVED4_A2
/**/
#undef IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET_A2
#undef IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED_A2
#undef IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET_A2
#define IEC62056_SUPPORT_BLOCKTRANSFERWITHGET_A2
#define IEC62056_SUPPORT_BLOCKTRANSFERWITHSET_A2
#undef IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION_A2
#undef IEC62056_SUPPORT_MULTIPLEREFERENCES_A2
#undef IEC62056_SUPPORT_INFORMATIONREPORT_A2
/**/
#undef IEC62056_SUPPORT_RESERVED5_A2
#undef IEC62056_SUPPORT_RESERVED6_A2
#undef IEC62056_SUPPORT_PARAMETERIZEDACCESS_A2
#define IEC62056_SUPPORT_GET_A2
#define IEC62056_SUPPORT_SET_A2
#define IEC62056_SUPPORT_SELECTIVEACCESS_A2
#undef IEC62056_SUPPORT_EVENTNOTIFICATION_A2
#define IEC62056_SUPPORT_ACTION_A2

#undef IEC62056_SUPPORT_GLOBAL_CIPHER_A2
#undef IEC62056_SUPPORT_DEDICATED_CIPHER_A2

// Association 1 - Low Level Security

#undef IEC62056_SUPPORT_RESERVED0_A3
#undef IEC62056_SUPPORT_RESERVED1_A3
#undef IEC62056_SUPPORT_RESERVED2_A3
#undef IEC62056_SUPPORT_READ_A3
#undef IEC62056_SUPPORT_WRITE_A3
#undef IEC62056_SUPPORT_UNCONFIRMEDWRITE_A3
#undef IEC62056_SUPPORT_RESERVED3_A3
#undef IEC62056_SUPPORT_RESERVED4_A3
/**/
#undef IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHSET_A3
#undef IEC62056_SUPPORT_PRIORITYMGMTSUPPORTED_A3
#undef IEC62056_SUPPORT_ATTRIBUTE0SUPPORTEDWITHGET_A3
#define IEC62056_SUPPORT_BLOCKTRANSFERWITHGET_A3
#undef IEC62056_SUPPORT_BLOCKTRANSFERWITHSET_A3
#undef IEC62056_SUPPORT_BLOCKTRANSFERWITHACTION_A3
#undef IEC62056_SUPPORT_MULTIPLEREFERENCES_A3
#undef IEC62056_SUPPORT_INFORMATIONREPORT_A3
/**/
#undef IEC62056_SUPPORT_RESERVED5_A3
#undef IEC62056_SUPPORT_RESERVED6_A3
#undef IEC62056_SUPPORT_PARAMETERIZEDACCESS_A3
#define IEC62056_SUPPORT_GET_A3
#define IEC62056_SUPPORT_SET_A3
#define IEC62056_SUPPORT_SELECTIVEACCESS_A3
#undef IEC62056_SUPPORT_EVENTNOTIFICATION_A3
#define IEC62056_SUPPORT_ACTION_A3

#undef IEC62056_SUPPORT_GLOBAL_CIPHER_A3
#undef IEC62056_SUPPORT_DEDICATED_CIPHER_A3

enum
{
    MONDAY = 1,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY
};

enum
{
    JANUARY = 1,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
};

enum
{
   // Instantaneous Profile Item Tags
    ITEM_TAG_CURRENT_DATETIME = 1,
    ITEM_TAG_IR,
    ITEM_TAG_IY,
    ITEM_TAG_IB, //not used
    ITEM_TAG_VR,
    ITEM_TAG_RI_PLUS,  
    ITEM_TAG_RC_PLUS, 
    ITEM_TAG_RI_MINUS, 
    ITEM_TAG_RC_MINUS,  
    ITEM_TAG_PFR, 
    ITEM_TAG_ACTIVE_ENERGY_PLUS, 
    ITEM_TAG_ACTIVE_ENERGY_MINUS,  
    ITEM_TAG_PF_TOTAL,
    ITEM_TAG_FREQUENCY,
    
    ITEM_TAG_ACTIVE_POWER_IMPORT,
    ITEM_TAG_ACTIVE_POWER_EXPORT,
    ITEM_TAG_REACTIVE_POWER_IMPORT,
    ITEM_TAG_REACTIVE_POWER_EXPORT,
    
    ITEM_TAG_KVA_TOTAL,
    ITEM_TAG_KW_TOTAL,    
    ITEM_TAG_KVAR_TOTAL,
    
    ITEM_TAG_CUM_KWH_TOTAL,
    ITEM_TAG_CUM_KVAR_LAGH_TOTAL,
    ITEM_TAG_CUM_KVAR_LEADH_TOTAL,
    ITEM_TAG_CUM_KVAH_TOTAL,
    ITEM_TAG_NUM_POWER_OFFS,
    ITEM_TAG_CUM_POWER_OFF_DURATION,
    ITEM_TAG_CUM_TAMPER_COUNT,
    ITEM_TAG_CUM_MD_RESET_COUNT,
    ITEM_TAG_CUM_PROGRAMMING_COUNT,
    ITEM_TAG_CUM_LAST_MD_EVENT_DATETIME,
    ITEM_TAG_KW_MAX_DEMAND,
    ITEM_TAG_KVA_MAX_DEMAND,
    ITEM_TAG_CURRENT_TIME,
    
    
    ITEM_TAG_EVENT_CODE_SE,
    ITEM_TAG_DATETIME_SE,
    ITEM_TAG_EVENT_GROUP_SE,
    ITEM_TAG_EVENT_DATA, // the data associated with the event  
    
    // Load Profile item Tags
    ITEM_TAG_DATETIME_LP, 
    ITEM_TAG_VR_LP, 
    ITEM_TAG_ACTIVE_POWER_LP,
    ITEM_TAG_REACTIVE_POWER_LP,                                           
    ITEM_TAG_CUM_KWH_TOTAL_LP,                        
    
    // Billing Profile item TAGS
    ITEM_TAG_BILLING_DATETIME_START_BI,
    ITEM_TAG_BILLING_DATETIME_END_BI,
    ITEM_TAG_AE_QI_BI, 
    ITEM_TAG_AE_QII_BI, 
    
    ITEM_TAG_AE_QIII_BI,
    ITEM_TAG_AE_QIV_BI,
    ITEM_TAG_RE_QI_BI,
    ITEM_TAG_RE_QII_BI,
    ITEM_TAG_RE_QIII_BI,
    ITEM_TAG_RE_QIV_BI,
    ITEM_TAG_BALANCE_BI,
    
    ITEM_TAG_AE_IM_BI,
    ITEM_TAG_AE_EX_BI,
    ITEM_TAG_MAX_DEMAND_BI,
    ITEM_TAG_BILLING_DATETIME_MAX_BI,
    ITEM_TAG_BILLING_DATETIME_MAX_CURR,
    //Activity calender
    ITEM_TAG_DAY_ID,
    ITEM_TAG_DAY_PROFILE_START_TIME_1,
    ITEM_TAG_DAY_PROFILE_START_TIME_2,
    ITEM_TAG_DAY_PROFILE_START_TIME_3,
    ITEM_TAG_DAY_PROFILE_START_TIME_4,

    ITEM_TAG_DAY_PROFILE_SCRIPT_ID1,
    ITEM_TAG_DAY_PROFILE_SCRIPT_ID2,
    ITEM_TAG_DAY_PROFILE_SCRIPT_ID3,
    ITEM_TAG_DAY_PROFILE_SCRIPT_ID4,
    
    ITEM_TAG_DAY_ID_PASSIVE,
    ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_1,
    ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_2,
    ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_3,
    ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_4,

    ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID1,
    ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID2,
    ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID3,
    ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID4,
    
    ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME,
    ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY,

   //special days tag defination 
    ITEM_TAG_SPECIAL_DAY_INDEX,
    ITEM_TAG_SPECIAL_DAY_DATE,
    ITEM_TAG_SPECIAL_DAY_DAY_ID,
    
    //Disconnecct control script selector 
    ITEM_TAG_DISCONNECT_CONTROL_SCRIPT_SELECTOR,
    ITEM_TAG_DISCONNECTCONTORL_SCRIPT_EXE_TIME,
    ITEM_TAG_DISCONNECTCONTROL_SCRIPT_EXE_DAY,
    
    
    //calander switching time 
    ITEM_TAG_NEW_CALENDER_ACTIVATION_TIME_CONTRACT1,
    ITEM_TAG_NEW_CALENDER_ACTIVATION_TIME_CONTRACT2,
    ITEM_TAG_NEW_CALENDER_ACTIVATION_TIME_CONTRACT3,

    //Firware upgrade
    ITEM_TAG_IMAGE_SIZE,
    ITEM_TAG_IMAGE_IDENTIFICATION,
    ITEM_TAG_IMAGE_SIGNATURE,
    
	//SO4 Daily snapshot
	ITEM_TAG_DATETIME_LP_2,
	ITEM_TAG_CUM_KWH_TOTAL_LP_2,
	ITEM_TAG_REACTIVE_ENERGY_QI_LP_2,
	ITEM_TAG_REACTIVE_ENERGY_QIV_LP_2
    
    
};
#define STUFF_DATA 0x80

#define byte_swap(x)    ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0xFF))
#define INJECT8(x)      ((uint16_t) (x) & 0xFF)
#define INJECT16(x)     (((uint16_t) (x) >> 8) & 0xFF), ((uint16_t) (x) & 0xFF)
#define INJECT32(x)     (((uint32_t) (x) >> 24) & 0xFF), (((uint32_t) (x) >> 16) & 0xFF), (((uint32_t) (x) >> 8) & 0xFF), ((uint32_t) (x) & 0xFF)
#define INJECT64(x)     (((uint64_t) (x) >> 56) & 0xff), (((uint64_t) (x) >> 48) & 0xff),(((uint64_t) (x) >> 40) & 0xff),(((uint64_t) (x) >> 32) & 0xff),(((uint64_t) (x) >> 24) & 0xff),(((uint64_t) (x) >> 16) & 0xff),(((uint64_t) (x) >> 8) & 0xff),(((uint64_t) (x) & 0xff))
#define INJECT32X(x)     ((uint32_t) (x) & 0xFF),(((uint32_t) (x) >> 8) & 0xFF), (((uint32_t) (x) >> 16) & 0xFF), (((uint32_t) (x) >> 24) & 0xFF)
#define INJECT16X(x)     ((uint16_t) (x) & 0xFF),(((uint16_t) (x) >> 8) & 0xFF) 
//#define CHUNK_SIZE  100

#define CLOCK_START               0
#define NO_OF_CLOCK_OBJ           4
#define SAP_ASSN_START           CLOCK_START + NO_OF_CLOCK_OBJ
#define NO_OF_SAP_OBJ             7
#define IDENT_NUM_START          SAP_ASSN_START + NO_OF_SAP_OBJ
#define NO_OF_IDENT_OBJ           8
#define ACTIVITY_CAL_START       IDENT_NUM_START + NO_OF_IDENT_OBJ
#define NO_OF_ACTIVITY_OBJ        4
#define BILLING_PROF_START       ACTIVITY_CAL_START + NO_OF_ACTIVITY_OBJ
#define NO_OF_BILLING_OBJ         2
#define LONG_POWER_FAIL_START    BILLING_PROF_START + NO_OF_BILLING_OBJ
#define NO_OF_LONG_POWER_FAIL_OBJ 4
#define ERROR_START              LONG_POWER_FAIL_START + NO_OF_LONG_POWER_FAIL_OBJ
#define NO_OF_ERROR_OBJ           1
#define EVENT_START              ERROR_START + NO_OF_ERROR_OBJ
#define NO_OF_EVENT_OBJ           2
#define DISCONNECT_START         EVENT_START + NO_OF_EVENT_OBJ
#define NO_OF_DISCONNECT_OBJ      8
#define FIRMWARE_START           DISCONNECT_START + NO_OF_DISCONNECT_OBJ
#define NO_OF_FIRMWARE_OBJ        1
#define OTHER_ABSTRACT_START     FIRMWARE_START + NO_OF_FIRMWARE_OBJ
#define NO_OF_OTHER_ABS_OBJ       5
#define ENERGY_REG_START         OTHER_ABSTRACT_START + NO_OF_OTHER_ABS_OBJ
#define NO_OF_ENERGY_REG_OBJ     13
#define DEMAND_REG_START          ENERGY_REG_START + NO_OF_ENERGY_REG_OBJ
#define NO_OF_DEMAND_REG_OBJ      0
#define LOAD_PROF_START          DEMAND_REG_START + NO_OF_DEMAND_REG_OBJ
#define LOAD_PROF_OBJ             4
#define INST_VALUES_START        LOAD_PROF_START + LOAD_PROF_OBJ
#define NO_OF_INST_VALUE_OBJ     12
#define POWER_QUALITY_START      INST_VALUES_START + NO_OF_INST_VALUE_OBJ
#define NO_OF_POWER_QLTY_OBJ      5
#define OTHER_ELEC_OBJS_START    POWER_QUALITY_START + NO_OF_POWER_QLTY_OBJ
#define NO_OF_OTHER_ELEC_OBJ      6
#define PRIME_PLC_START          OTHER_ELEC_OBJS_START + NO_OF_OTHER_ELEC_OBJ
#define NO_OF_PRIME_PLC_OBJ       2
#define REFERENCE_OBIS_START     PRIME_PLC_START + NO_OF_PRIME_PLC_OBJ

extern unsigned long known_far_msap_list[];
extern const uint8_t *authentications[];

extern const int reject_ln_context;
extern const int reject_sn_context;

extern const struct object_desc_s object_list[];

extern int current_get_block_no;
extern int current_get_block_chunk_len;
extern int current_get_block_chunk_last;

void exchange_partner(void *data, int direction);
void exchange_date_time(void *data, int direction);
void exchange_authentication_mechanism(void *data, int direction);

void interpret_template(uint8_t *data, int direction, const uint8_t *msg_template, int msg_len);
int interpret_and_fill(uint8_t *to, const uint8_t *from, int len);

typedef struct 
{
  uint8_t Bill_Date[12];
  unsigned long ActiveImport;
  unsigned long ActiveExport;
  unsigned long ReactiveQI;
  unsigned long ReactiveQII;
  unsigned long ReqctiveQIII;
  unsigned long ReqctiveQIV;
  unsigned long MAX_Demand;
  unsigned char MAX_DE_DATE[12];  
  
} sBilling_Profile;

extern  sBilling_Profile  Billing_Data[];

typedef struct 
{
  const uint8_t *template;
  uint16_t sz_template;
  uint16_t start_entry;
  uint16_t num_entries;
  uint16_t entries_remaining;
  const uint16_t *column_szs;
} Msg_Info;  

extern Msg_Info msg_info;

extern int8_t access_selector;
extern uint32_t SA_From_Entry, SA_To_Entry;
typedef struct
{
   uint16_t Year;
   uint8_t Month;
   uint8_t Date;
   uint8_t Day;
   uint8_t Hr;
   uint8_t Min;  
} sSA_Range;

extern sSA_Range SA_Range[2];

extern const unsigned char aes_key[16];

#endif
