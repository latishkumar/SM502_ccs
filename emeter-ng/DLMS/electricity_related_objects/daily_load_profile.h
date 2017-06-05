/*
 * daily_load_profile.h
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */

#ifndef DLMS_ELECTRICITY_RELATED_OBJECTS_DAILY_LOAD_PROFILE_H_
#define DLMS_ELECTRICITY_RELATED_OBJECTS_DAILY_LOAD_PROFILE_H_

extern uint8_t daily_amr_profile_status;;

extern uint32_t daily_load_profile_entries_in_use;

extern uint32_t daily_load_profile_entries;

extern uint32_t daily_load_profile_capture_period;

extern uint8_t daily_load_profile_sort_method;

extern const uint8_t daily_load_profile_capture_objects[];

void capture_daily_load_profile_data(void *data, int direction);
void change_daily_load_profile_capture_period(void *data,int data_direction);
void obj_daily_load_profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void obj_daily_load_profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);




#endif /* DLMS_ELECTRICITY_RELATED_OBJECTS_DAILY_LOAD_PROFILE_H_ */
