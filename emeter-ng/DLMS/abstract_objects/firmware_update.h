/*
 * firmware_update.h
 *
 *  Created on: May 15, 2017
 *      Author: Eliab Erango
 */

#ifndef DLMS_ABSTRACT_OBJECTS_FIRMWARE_UPDATE_H_
#define DLMS_ABSTRACT_OBJECTS_FIRMWARE_UPDATE_H_

enum image_transfer_status_t{

   TRANSFER_INITIATED=1,
   VERIFICATION_INITIATED,
   VERIFICATION_SUCCESSFUL,
   VERIFICATION_FAILED,
   ACTIVATION_INITIATED,
   ACTIVATION_SUCCESSFUL,
   ACTIVATION_FAILED,
};

extern uint32_t image_block_size;

extern uint8_t image_transferred_blocks_status;

extern uint32_t image_first_not_transferred_block_number;

extern uint8_t image_transfer_enabled;

extern uint8_t image_transfer_status;

extern const uint8_t image_to_activate_info[];

extern uint8_t image_identification[];

extern uint8_t image_signature [];

void get_image_transferred_blocks_status(void *data,int data_direction);

void get_image_first_not_transferred_block_number(void *data,int data_direction);

void image_transfer_initiate(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void image_block_transfer(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void image_verify(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void image_activate(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);










#endif /* DLMS_ABSTRACT_OBJECTS_FIRMWARE_UPDATE_H_ */
