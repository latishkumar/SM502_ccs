/*
 * synchronization_event_handling.c
 *
 *  Created on: Jun 9, 2017
 *      Author: Eliab Erango
 */
#include "headers.h"
#include "standard_event_handling.h"
#include "hourly_load_profile.h"
#include "synchronization_event_handling.h"
/*
 * Event Object - synchronization Event Log
 * event number (0 to 255)
 */
uint8_t synchronization_event_number;

/*
 * synchronization event profile entries
 * Default: >100
 */
const uint32_t synchronization_event_profile_entries = 1000;
const uint16_t synchronization_event_log_column_szs[] = {16,18,32};
/*
 * Template for synchronization event log profile
 */
const uint8_t synchronization_event_log_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 3,
   STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_SYNE2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Event Time stump
   STUFF_DATA | TAG_UINT8, INJECT8(ITEM_TAG_EVENT_CODE_SYNE),                                    // Event code
   STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_SYNE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Former clock Time stump
};

/*
 * Capture objects for synchronization event log profile
 */
const uint8_t synchronization_event_log_objects[] =
{
    INJECT16(0x8000 | (3*18+1)),
         3,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),				// Event code
                TAG_OCTET_STRING, 6, 0, 0, 96, 11, 8, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
			TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),				// Former clock
                TAG_OCTET_STRING, 6, 0, 0, 96, 2, 12, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)
};

/*
 * Call back function for synchronization event lof
 */
void capture_synchronization_event_log(void *data, int direction)
{
    log_search_params.start_log_address  = SYNCHRONIZATION_LOG_ADDRESS_START;
    log_search_params.end_log_address    = SYNCHRONIZATION_LOG_ADDRESS_END;
    log_search_params.last_log_address   = last_synchronization_event_log_address;
    log_search_params.log_size           = TIME_BOUND_EVENT_LOG_TYPE_SIZE;
    log_search_params.maximum_event_logs = MAX_SYNCHRONIZATION_EVENT_LOGS;
    log_search_params.offset             = 1;
    log_search_params.overlap_status     = status.synchronization_event_log_overlapped;
    log_search_params.template           = synchronization_event_log_template;
    log_search_params.sz_template        = sizeof(synchronization_event_log_template);
    log_search_params.log_column_size    = synchronization_event_log_column_szs;

    get_captured_logs(&log_search_params);
}

/*
 * Reset method for synchronization event log
 */
void obj_synchronization_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 = SYNCHRONIZATION_LOG_ADDRESS_START;
      uint8_t temp8 = 8;
      uint8_t tmp1 = 8;
      last_synchronization_event_log_address = tmp32;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);
      temp8 = 0;
      write_to_eeprom(&temp8,&tmp1,setEventOverlapFlag);
      *response_len = 0;
}

/*
 * Capture function for synchronization event log
 */
void obj_synchronization_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
	uint8_t tmp = 8;
	time_bound_event_log l;
	l.event_code = 0;
	l.begin_time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute-1, rtcc.second);
	l.end_time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	l.checksum = (int) (l.event_code + l.end_time_stamp.TimestampLow + l.end_time_stamp.TimestampUp);
	write_to_eeprom(&l,&tmp,log_events);
	*response_len = 0;
}



