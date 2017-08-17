/*
 * prime_plc_setup.h
 *
 *  Created on: May 17, 2017
 *      Author: Eliab Erango
 */

#ifndef DLMS_ABSTRACT_OBJECTS_PRIME_PLC_SETUP_H_
#define DLMS_ABSTRACT_OBJECTS_PRIME_PLC_SETUP_H_

extern uint8_t plc_firmware_Version[];

extern uint16_t plc_vendor_id;

extern uint8_t plc_product_id;

extern uint8_t mac_address[];

extern uint8_t EUI[];

void get_set_firmware_version(void *data,int data_direction);

void get_set_mac_address(void *data,int data_direction);





#endif /* DLMS_ABSTRACT_OBJECTS_PRIME_PLC_SETUP_H_ */
