/*
 * long_power_failures.h
 *
 *  Created on: May 15, 2017
 *      Author: Eliab Erango
 */

#ifndef DLMS_ABSTRACT_OBJECTS_LONG_POWER_FAILURES_H_
#define DLMS_ABSTRACT_OBJECTS_LONG_POWER_FAILURES_H_

extern uint16_t time_threshold_for_long_power_failure ;

extern uint16_t voltage_threshold_for_long_power_failure;

extern uint16_t duration_of_all_long_power_failure;

extern uint16_t number_of_long_power_failures;


void change_time_threshold_for_long_power_failure(void *data,int data_direction);

void change_threshold_for_long_power_failure(void *data,int data_direction);

void change_number_of_long_power_failures(void *data,int data_direction);

#endif /* DLMS_ABSTRACT_OBJECTS_LONG_POWER_FAILURES_H_ */
