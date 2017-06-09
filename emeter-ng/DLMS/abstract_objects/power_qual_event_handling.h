/*
 * power_qual_event_handling.h
 *
 *  Created on: Jun 9, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_POWER_QUAL_EVENT_HANDLING_H_
#define DLMS_ABSTRACT_OBJECTS_POWER_QUAL_EVENT_HANDLING_H_

extern uint8_t power_qual_event_number;

extern uint32_t power_qual_event_profile_entries;

extern const uint8_t power_qual_event_log_objects[];

void capture_power_qual_event_log(void *data, int direction);

void obj_power_qual_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void obj_power_qual_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);





#endif /* DLMS_ABSTRACT_OBJECTS_POWER_QUAL_EVENT_HANDLING_H_ */