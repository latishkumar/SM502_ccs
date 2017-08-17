/*
 * event_handling.c
 *
 *  Created on: May 15, 2017
 *      Author: Eliab Erango
 */
#include "headers.h"
#include "standard_event_handling.h"
#include "hourly_load_profile.h"
/*
 * Event Object - Standard Event Log
 * event number (0 to 255)
 */
uint8_t standard_event_number;

/*
 * Capture period for events
 * value: 0 asynchronously
 */
uint32_t events_capture_period = 0;

/*
 * Event sort method
 * value:1 /unsorted (FIFO)
 */
const uint8_t events_sort_method = 1;

/*
 * Entries in use for events log
 */
uint32_t events_entries_in_use;

/*
 * Standard event profile entries
 * Default: >100
 */
uint32_t standard_event_profile_entries = 5701;
const uint16_t standard_event_log_column_szs[] = {16,18};
/*
 * Template for standard event log profile
 */
const uint8_t standard_event_log_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 2,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_SE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Event Time stump
        STUFF_DATA | TAG_UINT8, INJECT8(ITEM_TAG_EVENT_CODE_SE)                                  // Event code
};

/*
 * Capture objects for standard event log profile
 */
const uint8_t standard_event_log_objects[] =
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

/*
 * Call back function for standard event log
 */
void capture_standard_event_log(void *data, int direction)
{
    log_search_params.start_log_address  = STANDARD_EVENT_LOG_ADDRESS_START;
    log_search_params.end_log_address    = STANDARD_EVENT_LOG_ADDRESS_END;
    log_search_params.last_log_address   = last_standard_event_log_address;
    log_search_params.log_size           = EVENT_LOG_TYPE_SIZE;//EventLogSize;
    log_search_params.maximum_event_logs = MAX_STANDARD_EVENT_LOGS;
    log_search_params.offset             = 1;
    log_search_params.overlap_status     = status.standard_event_log_overlapped;
    log_search_params.template           = standard_event_log_template;
    log_search_params.sz_template        = sizeof(standard_event_log_template);
    log_search_params.log_column_size    = standard_event_log_column_szs;

    get_captured_logs(&log_search_params);
}

/*
 * Reset method for standard event log
 */
void obj_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 = STANDARD_EVENT_LOG_ADDRESS_START;
      uint8_t temp8 = 3;
      uint8_t tmp1 = 3;
      last_standard_event_log_address = tmp32;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);
      temp8 = 0;
      write_to_eeprom(&temp8,&tmp1,setEventOverlapFlag);
      *response_len = 0;
}

/*
 * Capture function for standard event log
 */
void obj_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
	uint8_t tmp = 3 ;
	event_log l;
	l.event_code = 0;
	l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
	write_to_eeprom(&l,&tmp,log_events);
	*response_len = 0;
}






