/*
 * instantaneous_values.h
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */

#ifndef DLMS_ELECTRICITY_RELATED_OBJECTS_INSTANTANEOUS_VALUES_H_
#define DLMS_ELECTRICITY_RELATED_OBJECTS_INSTANTANEOUS_VALUES_H_

extern uint32_t instantaneous_values_entries_in_use;

extern uint32_t instantaneous_values_entries;

extern uint32_t instantaneous_values_capture_period;

extern uint8_t instantaneous_values_sort_method;

extern uint32_t instant_l2_l3_v_c;

extern const uint8_t instantaneous_values_capture_objects[];

void capture_instantaneous_profile_data(void *data,int direction);

#endif /* DLMS_ELECTRICITY_RELATED_OBJECTS_INSTANTANEOUS_VALUES_H_ */
