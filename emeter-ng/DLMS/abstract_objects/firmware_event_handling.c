/*
 * firmware_event_handling.c
 *
 *  Created on: Jun 9, 2017
 *      Author: Eliab Erango
 */

#include "headers.h"
#include "hourly_load_profile.h"
#include "firmware_event_handling.h"
/*
 * Event Object - firmware Event Log
 * event number (0 to 255)
 */
uint8_t firmware_event_number;

/*
 * firmware event profile entries
 * Default: >100
 */
uint32_t firmware_event_profile_entries = 1000;
const uint16_t firmware_event_log_column_szs[] = {16,18,25,32};
/*
 * Template for firmware event log profile
 */
const uint8_t firmware_event_log_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 4,
   STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_FRME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Event Time stump
   STUFF_DATA | TAG_UINT8, INJECT8(ITEM_TAG_EVENT_CODE_FRME),                                   // Event code
   STUFF_DATA | TAG_OCTET_STRING, 5,ITEM_TAG_ACTIVE_FIRMWARE, 0, 0, 0, 0,						// Firmware version
   STUFF_DATA | TAG_OCTET_STRING, 5,ITEM_TAG_FORMER_FIRMWARE, 0, 0, 0, 0	   	   	            // Former firmware version
};

/*
 * Capture objects for firmware event log profile
 */
const uint8_t firmware_event_log_objects[] =
{
    INJECT16(0x8000 | (4*18+1)),
         4,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),  // event code
                TAG_OCTET_STRING, 6, 0, 0, 96, 11, 4, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA), // active firmware
                TAG_OCTET_STRING, 6, 1, 0, 0, 2, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA), // former firmware
                TAG_OCTET_STRING, 6, 0, 0, 96, 1, 6, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)
};

/*
 * Call back function for firmware event log
 */
void capture_firmware_event_log(void *data, int direction)
{
    log_search_params.start_log_address  = FIRMWARE_LOG_ADDRESS_START;
    log_search_params.end_log_address    = FIRMWARE_LOG_ADDRESS_END;
    log_search_params.last_log_address   = last_firmware_event_log_address;
    log_search_params.log_size           = FIRMWARE_EVENT_LOG_TYPE_SIZE;
    log_search_params.maximum_event_logs = MAX_FIRMWARE_EVENT_LOGS;
    log_search_params.offset             = 6;
    log_search_params.overlap_status     = status.firmware_event_log_overlapped;
    log_search_params.template           = firmware_event_log_template;
    log_search_params.sz_template        = sizeof(firmware_event_log_template);
    log_search_params.log_column_size    = firmware_event_log_column_szs;

    get_captured_logs(&log_search_params);

}

/*
 * Reset method for firmware event log
 */
void obj_firmware_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 = FIRMWARE_LOG_ADDRESS_START;
      uint8_t temp8 = 7;
      uint8_t tmp1 = 7;
      last_firmware_event_log_address = tmp32;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);
      temp8 = 0;
      write_to_eeprom(&temp8,&tmp1,setEventOverlapFlag);
      *response_len= 0;

}

/*
 * Capture function for firmware event log
 */
extern uint8_t active_firmware_version[];
void obj_firmware_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
	uint8_t tmp = 7;
	firmware_event_log l;
	l.event_code = 255;
	memcpy((uint8_t *)l.active_firmware,(uint8_t *)&active_firmware_version[1],5);
	l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
	write_to_eeprom(&l,&tmp,log_events);
	*response_len= 0;
}



