/*
 * manufacturer_specific_objects.h
 *
 *  Created on: May 26, 2017
 *      Author: Eliab Erango
 */

#ifndef DLMS_ABSTRACT_OBJECTS_MANUFACTURER_SPECIFIC_OBJECTS_H_
#define DLMS_ABSTRACT_OBJECTS_MANUFACTURER_SPECIFIC_OBJECTS_H_

extern int16_t alarm_status;

extern uint8_t permitted_power_imbalance_fraction;

extern uint8_t usb_comm_speed;

void reset_persistent_events(void *data, int data_direction);

void reset_alarms(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

void calibrate_power_scaler(void *data, int data_direction);

void calibrate_current_scaler(void *data, int data_direction);

void calibrate_voltage_scaler(void *data, int data_direction);

void calibrate_power(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

void calibrate_current(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

void calibrate_voltage(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

void increment_balance(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void decrement_balance(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void bor_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void configure_usb_baudrate(void *data, int data_direction);

void calibrate_neutral_power_scaler(void *data, int data_direction);

void calibrate_neutral_current_scaler(void *data, int data_direction);

void configure_permitted_power_imbalance_fraction(void *data, int data_direction);

void reset_meter(void *data, int data_direction);

void reset_balance();

#endif /* DLMS_ABSTRACT_OBJECTS_MANUFACTURER_SPECIFIC_OBJECTS_H_ */
