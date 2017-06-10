/*
 * instantaneous_energy_values_profile.c
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */
#include "headers.h"
#include "instantaneous_energy_values_profile.h"

uint32_t instantaneous_energy_capture_period = 0;   // 0: Asynchronously
uint8_t instantaneous_energy_sort_method = 1;       // 1: unsorted (FIFO)
uint32_t instantaneous_energy_entries_in_use = 1;
uint32_t instantaneous_energy_entries = 1;          // 1: Profile entries is limited to 1

const uint16_t instantaneous_energy_profile_column_szs[] = {16,21,26,31,36,41,46};
const uint8_t instantaneous_energy_profile_buffer_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 7,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Time stump
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_ENERGY_PLUS),                               // Import Active Energy
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_ENERGY_MINUS),                              // Export Active Energy
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RI_PLUS),                                          // RE QI
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RC_PLUS),                                          // RE QII
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RI_MINUS),                                         // RE QIII
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_RC_MINUS),                                         // RE QIV
};

/* Instantaneous Energy Values profile
 * buffer
 */
void capture_instantaneous_energy_profile_data(void *data,int direction)
{
   /* Load Template for Load Profile */
   msg_info.template = instantaneous_energy_profile_buffer_template;
   msg_info.sz_template = sizeof(instantaneous_energy_profile_buffer_template);
   msg_info.num_entries=1;
   msg_info.start_entry=1;
   msg_info.column_szs  = instantaneous_energy_profile_column_szs;
}

/* Instantaneous Energy Values profile
 * capture_objects
 */
const uint8_t instantaneous_energy_capture_objects[] =
{
    INJECT16(0x8000 | (8*18 + 1)),
         8,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 1, 8, 0, 255,    // Instantaneous Energy A+
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 2, 8, 0, 255,    // Instantaneous Energy A-
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 5, 8, 0, 255,    // Instantaneous Energy Ri+
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 6, 8, 0, 255,   // Instantaneous Energy Rc+
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 7, 8, 0, 255,   // Instantaneous Energy Ri-
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 8, 8, 0, 255,    // Instantaneous Energy Rc-
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
			TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA), 	// added only to resolve emulator error when the number of objects are 7
                TAG_OCTET_STRING, 6, 0, 0, 0, 0, 0, 255,    //
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)
};
