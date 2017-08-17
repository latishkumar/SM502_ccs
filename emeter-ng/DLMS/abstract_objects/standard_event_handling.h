/*
 * event_handling.h
 *
 *  Created on: May 15, 2017
 *      Author: Eliab Erango
 */

#ifndef DLMS_ABSTRACT_OBJECTS_STANDARD_EVENT_HANDLING_H_
#define DLMS_ABSTRACT_OBJECTS_STANDARD_EVENT_HANDLING_H_

extern uint8_t standard_event_number;

extern uint32_t events_capture_period;

extern const uint8_t events_sort_method;

extern uint32_t events_entries_in_use;

extern uint32_t standard_event_profile_entries;

extern const uint8_t standard_event_log_objects[];

void capture_standard_event_log(void *data, int direction);

void obj_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void obj_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);


#endif /* DLMS_ABSTRACT_OBJECTS_STANDARD_EVENT_HANDLING_H_ */
