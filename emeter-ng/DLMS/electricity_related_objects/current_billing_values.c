/*
 * current_billing_values.c
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */
#include "headers.h"
#include "current_billing_values.h"

const uint32_t billing_profile_entries_in_use = 6;
const uint32_t billing_profile_entries = 1;
const uint32_t billing_profile_capture_period = 0;
const uint8_t billing_profile_sort_method = 1; // 1:unsorted (FIFO)

const uint16_t billing_profile_column_szs[] = {16,21,26,31,36,41,46,51,65};
const uint8_t billing_profile_puffer_template[]=
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

/* Current billing values profile
 * buffer-callback function
 */
void capture_billing_profile_data(void *data, int direction)
{
    msg_info.template=billing_profile_puffer_template;
    msg_info.sz_template=sizeof(billing_profile_puffer_template);

    msg_info.num_entries=1;
    msg_info.start_entry=1;

    msg_info.column_szs=billing_profile_column_szs;
}

/* Current billing values profile
 * capture_objects
 */
const uint8_t billing_profile_capture_objects[] =
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


