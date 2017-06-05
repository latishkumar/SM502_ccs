/*
 * load_profiles.h
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */

#ifndef DLMS_ELECTRICITY_RELATED_OBJECTS_HOURLY_LOAD_PROFILE_H_
#define DLMS_ELECTRICITY_RELATED_OBJECTS_HOURLY_LOAD_PROFILE_H_

rtc_t required_last_entry_time;

Search_t search_item;

extern uint8_t amr_profile_status;

extern uint32_t load_profile_entries_in_use;

extern uint32_t load_profile_entries;

extern uint32_t load_profile_capture_period;

extern uint8_t load_profile_sort_method;

extern const uint8_t load_profile_capture_objects[];

void change_hourly_load_profile_capture_period(void *data,int data_direction);

void capture_load_profile_data(void *data, int direction);

void obj_load_profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void obj_load_profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

extern uint8_t EnergyLoggingTime;
extern int8_t compare_energy(void *energy0,void *energy1);
extern int8_t compare_event(void *event0,void *event1);

#endif /* DLMS_ELECTRICITY_RELATED_OBJECTS_HOURLY_LOAD_PROFILE_H_ */
