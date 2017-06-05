/*
 * instantaneous_energy_values_profile.h
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */

#ifndef DLMS_ELECTRICITY_RELATED_OBJECTS_INSTANTANEOUS_ENERGY_VALUES_PROFILE_H_
#define DLMS_ELECTRICITY_RELATED_OBJECTS_INSTANTANEOUS_ENERGY_VALUES_PROFILE_H_

extern uint32_t instantaneous_energy_capture_period;

extern uint8_t instantaneous_energy_sort_method ;

extern uint32_t instantaneous_energy_entries_in_use ;

extern uint32_t instantaneous_energy_entries ;

extern const uint16_t instantaneous_energy_profile_column_szs[];

extern const uint8_t instantaneous_energy_profile_buffer_template[];

extern const uint8_t instantaneous_energy_capture_objects[];

void capture_instantaneous_energy_profile_data(void *data,int direction);





#endif /* DLMS_ELECTRICITY_RELATED_OBJECTS_INSTANTANEOUS_ENERGY_VALUES_PROFILE_H_ */
