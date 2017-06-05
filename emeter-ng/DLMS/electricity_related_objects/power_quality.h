/*
 * power_quality.h
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */

#ifndef DLMS_ELECTRICITY_RELATED_OBJECTS_POWER_QUALITY_H_
#define DLMS_ELECTRICITY_RELATED_OBJECTS_POWER_QUALITY_H_

extern uint16_t nominal_voltage;

extern uint16_t max_vrms;

extern uint8_t min_vrms;

extern uint8_t  max_irms;

extern uint16_t under_voltage_trip_duration;

extern uint16_t over_voltage_trip_duration;

extern uint16_t voltage_sag_threshold;

extern uint16_t voltage_swell_threshold;

extern uint16_t ThresholdLongPowerFaile; //TODO
void change_reference_voltage(void *data,int data_direction);

void change_time_threshold_voltage_sag(void *data,int data_direction);

void change_threshold_voltage_sag(void *data,int data_direction);

void change_time_threshold_voltage_swell(void *data,int data_direction);

void change_threshold_voltage_swell(void *data,int data_direction);









#endif /* DLMS_ELECTRICITY_RELATED_OBJECTS_POWER_QUALITY_H_ */
