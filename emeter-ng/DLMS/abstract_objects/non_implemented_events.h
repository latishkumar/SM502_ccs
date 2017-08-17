/*
 * non_implemented_events.h
 *
 *  Created on: Jun 11, 2017
 *      Author: Eliab Erango
 */

#ifndef DLMS_ABSTRACT_OBJECTS_NON_IMPLEMENTED_EVENTS_H_
#define DLMS_ABSTRACT_OBJECTS_NON_IMPLEMENTED_EVENTS_H_
extern uint8_t event_number;

extern uint32_t event_profile_entries;


extern const uint8_t demand_event_log_objects[];

extern const uint8_t finished_qual_event_log_objects[];

extern const uint8_t import_event_log_objects[];

void capture_demand_event_log(void *data, int direction);
void capture_finished_qual_event_log(void *data, int direction);
void capture_import_event_log(void *data, int direction);
void obj_non_impl_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void obj_non_impl_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void capture_demand_event_log(void *data, int direction);



#endif /* DLMS_ABSTRACT_OBJECTS_NON_IMPLEMENTED_EVENTS_H_ */
