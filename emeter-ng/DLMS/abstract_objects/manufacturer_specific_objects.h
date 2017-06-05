/*
 * manufacturer_specific_objects.h
 *
 *  Created on: May 26, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_MANUFACTURER_SPECIFIC_OBJECTS_H_
#define DLMS_ABSTRACT_OBJECTS_MANUFACTURER_SPECIFIC_OBJECTS_H_

void reset_alarms(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);
void calibrate_power_scaler(void *data, int data_direction);

void calibrate_current_scaler(void *data, int data_direction);

void calibrate_voltage_scaler(void *data, int data_direction);

void calibrate_power(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

void calibrate_current(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

void calibrate_voltage(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

void increment_balance(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void decrement_balance(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);











#endif /* DLMS_ABSTRACT_OBJECTS_MANUFACTURER_SPECIFIC_OBJECTS_H_ */
