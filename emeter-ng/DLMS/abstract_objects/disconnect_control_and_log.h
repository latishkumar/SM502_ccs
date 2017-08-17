/*
 * disconnect_control_and_log.h
 *
 *  Created on: May 15, 2017
 *      Author: Eliab Erango
 */

#ifndef DLMS_ABSTRACT_OBJECTS_DISCONNECT_CONTROL_AND_LOG_H_
#define DLMS_ABSTRACT_OBJECTS_DISCONNECT_CONTROL_AND_LOG_H_
#include "Logger.h"
typedef struct{
    rtc_t execution_time;
    uint16_t control_script_type;
}DisconnectControlScript;
//TODO .write this to EEPROM
DisconnectControlScript disconnect_script;

/*
 * Disconnect control Scheduler
 * Type
 */
extern const uint8_t diconnect_control_type;

extern uint8_t output_state;

extern uint8_t control_state;

extern uint8_t control_mode;

extern uint32_t current_import_active_power_threshold;

extern uint8_t export_active_power_over_thr_status;

extern uint8_t import_active_power_over_thr_status;

extern uint32_t current_export_active_power_threshold;

extern uint8_t diconnect_control_script_execution_time[];

extern const uint8_t executed_diconnect_control_script[];

void access_disconnect_scripts(void *data, int direction);

void disconnect_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void remote_disconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);

void remote_reconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len);



#endif /* DLMS_ABSTRACT_OBJECTS_DISCONNECT_CONTROL_AND_LOG_H_ */
