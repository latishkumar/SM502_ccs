/*
 * identification_numbers.h
 *
 *  Created on: May 12, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_IDENTIFICATION_NUMBERS_H_
#define DLMS_ABSTRACT_OBJECTS_IDENTIFICATION_NUMBERS_H_
extern uint8_t Meter_Sr_No[];

extern uint8_t Device_ID_2[];

extern uint8_t Device_ID_3[];

extern uint8_t Device_ID_4[];

extern uint8_t Device_ID_5[];

extern uint8_t Device_ID_6[];

extern uint8_t Device_ID_7[];

extern uint8_t Device_ID_8[];

extern uint8_t Manufacturer_Name[];

void change_meter_serial_number(void *data,int data_direction);
void change_device_id2(void *data,int data_direction);
void change_device_id3(void *data,int data_direction);
void change_device_id4(void *data,int data_direction);
void change_device_id5(void *data,int data_direction);
void change_device_id6(void *data,int data_direction);
void change_device_id7(void *data,int data_direction);
void change_device_id8(void *data,int data_direction);
#endif /* DLMS_ABSTRACT_OBJECTS_IDENTIFICATION_NUMBERS_H_ */
