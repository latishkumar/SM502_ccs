/*
 * instantaneous_values.c
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */
#include "headers.h"
#include "instantaneous_values.h"

uint32_t instantaneous_values_entries_in_use = 1;
uint32_t instantaneous_values_entries = 1;          // 1:Profile entries is limited to 1
uint32_t instantaneous_values_capture_period = 0;   // 0:Asynchronously
uint8_t instantaneous_values_sort_method = 1;       // 1:unsorted (FIFO)
uint32_t instant_l2_l3_v_c=0;							//instantaneous current and voltage for l2 and l3
const uint16_t instantanious_profile_column_szs[] = {16,21,26,31,36,41,46,51,56,61,66,71,76};
const uint8_t instantanious_profile_buffer_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 13,
	STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Time stump
	STUFF_DATA |  TAG_UINT32, INJECT32(ITEM_TAG_VR),                          // VL1
	STUFF_DATA |  TAG_UINT32, INJECT32(ITEM_TAG_IR),                          // IL1
	STUFF_DATA |  TAG_UINT32, INJECT32(0),                                    // VL2
	STUFF_DATA |  TAG_UINT32, INJECT32(0),                                    // I2
	STUFF_DATA |  TAG_UINT32, INJECT32(0),                                    // VL3
	STUFF_DATA |  TAG_UINT32, INJECT32(0),                                    // IL3
	STUFF_DATA |  TAG_UINT32, INJECT32(ITEM_TAG_IR),                          // Total Current
	STUFF_DATA |  TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_POWER_IMPORT),         // Active Power import p1
	STUFF_DATA |  TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_POWER_EXPORT),         // Active Power export p1
	STUFF_DATA |  TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_POWER_IMPORT),       // reactive Power import p1
	STUFF_DATA |  TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_POWER_EXPORT),       // reactive Power export p1
	STUFF_DATA |  TAG_UINT32, INJECT32(ITEM_TAG_PF_TOTAL)                     // power factor
};

/*
 * Instantaneous Values
 * buffer-callback function
 */
void capture_instantaneous_profile_data(void *data,int direction)
{
   /* Load Template for Load Profile */
   msg_info.template = instantanious_profile_buffer_template;
   msg_info.sz_template = sizeof(instantanious_profile_buffer_template);
   msg_info.num_entries=1;
   msg_info.start_entry=1;
   msg_info.column_szs  = instantanious_profile_column_szs;
}

/*
 * Instantaneous Values
 * capture objects
 */
const uint8_t instantaneous_values_capture_objects[] =
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
