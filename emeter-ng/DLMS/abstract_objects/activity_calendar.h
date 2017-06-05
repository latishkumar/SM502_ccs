/*
 * activity _calendar.h
 *
 *  Created on: May 12, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_ACTIVITY_CALENDAR_H_
#define DLMS_ABSTRACT_OBJECTS_ACTIVITY_CALENDAR_H_
rtc_t activePassiveCalenderTime;

extern const uint8_t active_calendar_name[] ;  // calendar_name_active

extern const uint8_t passive_calendar_name[] ; // calendar_name_passive

extern const uint8_t activate_passive_calendar_time[] ;  //activate_passive_calendar_time

extern uint32_t active_demand_control_threshold ; //Active Demand Control Threshold

extern uint8_t current_active_tariff ; // Currently active tariff

extern const uint8_t season_profile_dummy[];

extern const uint8_t week_profile_table_dummy[];

void access_active_season_profile(void *data, int direction);
void access_active_week_profile_table(void *data, int direction);
void access_active_day_profile_table(void *data, int direction);
void access_passive_season_profile(void *data, int direction);
void access_passive_week_profile_table(void *data, int direction);
void access_passive_day_profile_table(void *data, int direction);
void set_get_passive_calendar_activation_time(void *data,int direction);
void activate_passive_calendar(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void get_special_days_list(void *data,int direction);
void insert_special_day(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);
void delete_special_day(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);




#endif /* DLMS_ABSTRACT_OBJECTS_ACTIVITY_CALENDAR_H_ */
