/*
 * other_abstract_objects.h
 *
 *  Created on: May 15, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_OTHER_ABSTRACT_OBJECTS_H_
#define DLMS_ABSTRACT_OBJECTS_OTHER_ABSTRACT_OBJECTS_H_

/*
 * VARIABLES AND FUNCTIONS FOR COMMUNICATIONS SETUP OBJECTS
 */
typedef enum
{
    BAUD_300 = 0,
    BAUD_600 = 1,
    BAUD_1200 = 2,
    BAUD_2400 = 3,
    BAUD_4800 = 4,
    BAUD_9600 = 5,
    BAUD_19200 = 6,
    BAUD_38400 = 7,
    BAUD_57600 = 8,
    BAUD_115200 = 9
} Baud_Rates;

extern uint16_t power_demand_close_to_contract_power_thr;

extern uint8_t comm_Speed;

extern const uint8_t window_size_transmit;

extern const uint8_t window_size_receive;

extern const uint16_t max_info_field_len_tx;

extern const uint16_t max_info_field_len_rx;

extern const uint16_t inter_octet_timeout;

extern const uint16_t inactivity_time_out;

extern uint16_t device_address;

extern uint8_t active_firmware_version[];

extern uint32_t scroll_display_time;

extern const uint8_t GlobalReset_Scripts_Table[];

void change_display_scroll_time(void *data,int direction);

void access_global_reset_scripts(void *data, int direction);

void global_reset_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);





#endif /* DLMS_ABSTRACT_OBJECTS_OTHER_ABSTRACT_OBJECTS_H_ */
