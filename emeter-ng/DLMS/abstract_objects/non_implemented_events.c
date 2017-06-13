/*
 * non_implemented_events.c
 *
 *  Created on: Jun 11, 2017
 *      Author: MedStud
 */
#include "headers.h"
#include "event_handling.h"
#include "hourly_load_profile.h"
#include "non_implemented_events.h"
/*
 * event number (0 to 255)
 */
uint8_t event_number;
/*
 * events profile entries
 * Default: >100
 */
const uint32_t event_profile_entries = 1000;
const uint16_t demand_event_log_column_szs[] = {16,18};
/*
 * Template for demand event log profile
 */
const uint8_t demand_event_log_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 2,
   STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Event Time stump
               TAG_UINT8, INJECT8(0)                                  // Event code
};

/*
 * Capture objects for demand event log profile
 */
const uint8_t demand_event_log_objects[] =
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
                TAG_OCTET_STRING, 6, 0, 0, 96, 11, 7, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)
};

/*
 * Call back function for demand event log
 */
void capture_demand_event_log(void *data, int direction)
{
   /* Load Template for Load Profile */
   // interpret_template(data, direction, Load_Profile_Buffer_Template, sizeof(Load_Profile_Buffer_Template));
   msg_info.template = demand_event_log_template;
   msg_info.sz_template = sizeof(demand_event_log_template);
   msg_info.num_entries = 1;
   msg_info.start_entry=1;
   msg_info.column_szs = demand_event_log_column_szs;
}
const uint16_t finished_qual_event_log_column_szs[] = {16,18};
/*
 * Template for finished quality event log profile
 */
const uint8_t finished_qual_event_log_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 2,
   STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Event Time stump
                TAG_UINT8, INJECT8(0),                                  					     // Event code
		//		TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // Event Time stump
};

/*
 * Capture objects for finished quality event log profile
 */
const uint8_t finished_qual_event_log_objects[] =
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
                TAG_OCTET_STRING, 6, 0, 0, 96, 11, 7, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
//			TAG_STRUCTURE, 4,
//				TAG_UINT16, INJECT16(CLASS_ID_DATA),
//				TAG_OCTET_STRING, 6, 0, 0, 94, 34, 80, 255, // Date & Time
//				TAG_INT8, 2,
//				TAG_UINT16, INJECT16(0)
};

/*
 * Call back function for finished quality event log
 */
void capture_finished_qual_event_log(void *data, int direction)
{
   /* Load Template for Load Profile */
   // interpret_template(data, direction, Load_Profile_Buffer_Template, sizeof(Load_Profile_Buffer_Template));
   msg_info.template = finished_qual_event_log_template;
   msg_info.sz_template = sizeof(finished_qual_event_log_template);
   msg_info.num_entries = 1;
   msg_info.start_entry = 1;
   msg_info.column_szs = finished_qual_event_log_column_szs;
}
const uint16_t import_event_log_column_szs[] = {16,18};
/*
 * Template for import contract event log profile
 */
const uint8_t import_event_log_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 2,
   STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Event Time stump
                TAG_UINT8, INJECT8(0),                                  // Event code
  // STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_CURRENT_DATETIME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Event Time stump
};

/*
 * Capture objects for import contract event log profile
 */
const uint8_t import_event_log_objects[] =
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
                TAG_OCTET_STRING, 6, 0, 0, 96, 11, 3, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)
};

/*
 * Call back function for import contract event log
 */
void capture_import_event_log(void *data, int direction)
{
   /* Load Template for Load Profile */
   // interpret_template(data, direction, Load_Profile_Buffer_Template, sizeof(Load_Profile_Buffer_Template));
   msg_info.template = import_event_log_template;
   msg_info.sz_template = sizeof(import_event_log_template);
   msg_info.num_entries = 1;
   msg_info.start_entry=1;
   msg_info.column_szs = import_event_log_column_szs;
}
/*
 * Reset method for event log
 */
void obj_non_impl_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    *response_len= 0;
}

/*
 * Capture function for event log
 */
void obj_non_impl_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    *response_len= 0;
}









