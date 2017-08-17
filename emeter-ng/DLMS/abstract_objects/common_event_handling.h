/*
 * common_event_handling.h
 *
 *  Created on: Jun 9, 2017
 *      Author: Eliab Erango
 */

#ifndef DLMS_ABSTRACT_OBJECTS_COMMON_EVENT_HANDLING_H_
#define DLMS_ABSTRACT_OBJECTS_COMMON_EVENT_HANDLING_H_

extern uint8_t common_event_number;

extern uint32_t common_event_profile_entries;

extern const uint8_t common_event_log_objects[];

void capture_common_event_log(void *data, int direction);

void obj_common_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void obj_common_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);



#endif /* DLMS_ABSTRACT_OBJECTS_COMMON_EVENT_HANDLING_H_ */
