/*
 * other_abstract_objects.c
 *
 *  Created on: May 15, 2017
 *      Author: Eliab Erango
 */
#include "headers.h"
#include "other_abstract_objects.h"

/*Threshold for Demand close to contract power
 * Default: 95,00 % of power contract to
 * activate the demand close to contract power event.
 */
uint16_t power_demand_close_to_contract_power_thr = 9500;

uint8_t comm_Speed = BAUD_9600;             // comm_speed default: 9600
const uint8_t window_size_transmit = 1;     // default: 1
const uint8_t window_size_receive = 1;      // default: 1
const uint16_t max_info_field_len_tx = 128; // minimum 128
const uint16_t max_info_field_len_rx = 128; // minimum 128
const uint16_t inter_octet_timeout = 3000;  // 150 ms
const uint16_t inactivity_time_out = 1000;  // 180 s
uint16_t device_address = 0x10;

uint8_t active_firmware_version[]={5,'V','0','1','0','3'}; //Active Firmware identifier-V1.2

extern uint32_t scroll_display_time;
extern uint32_t AutoDisplayTime;

const uint8_t GlobalReset_Scripts_Table[] =
{
    1*26 + 1,
        /*TAG_ARRAY,*/ 1,
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1), /* Script ID - 1 Reset Meter*/
                TAG_ARRAY, 1,
                TAG_STRUCTURE, 5,
                        TAG_ENUM, 2,    /* execute method */
                        TAG_UINT16, INJECT16(70), /* class id*/
                        TAG_OCTET_STRING, 0, 0, 0, 96, 3, 10, 255, /* meter rest object*/
                        TAG_INT8, 1, //execute method one of the global reset object
                        TAG_INT8, 0,

};

/*
 * Call back function for Time for Scroll Display object
 */
void change_display_scroll_time(void *data,int direction)/*TODO. Test this*/
{
   if(direction == ATTR_READ)
   {

   }
   else
   {
      uint32_t *ptr = data;
      uint32_t temp = (uint32_t)(*ptr);
      scroll_display_time = temp;
      AutoDisplayTime = scroll_display_time*1000;
      setAutoDisplayTime(&AutoDisplayTime,(uint8_t * )0);
   }
}

/*
 * Callback function for Global meter reset object
 */
void access_global_reset_scripts(void *data, int direction)
{
    if (direction != ATTR_READ)
        return;
    memcpy(data, GlobalReset_Scripts_Table, sizeof(GlobalReset_Scripts_Table));
}

/*
 * Global meter reset
 * Reset method
 */
void global_reset_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    if(*data == 1) // parameter reset this is the script-ID in the corresponding script table
    {
        //TODO. reset all the configuration parameters to the default status,
        // change the EEPROM Formatted indicator
        // and restart
    }
    else if(*data == 2) //data reset
    {
      //TODO. reset the Meter logged data's event,load profile and billing profile.
    }
}

//void reset_global_meter()
//{
//    // Reset Energy registers and counters
//    phase->active_power_counter                  = 0;
//    phase->import_active_energy_counter_QI_QIV   = 0;
//    phase->export_active_energy_counter_QII_QIII = 0;
//    phase->import_active_energy_QI_QIV           = 0;
//    phase->export_active_energy_QII_QIII         = 0;
//    phase->consumed_reactive_energy_QI           = 0;
//    phase->consumed_reactive_energy_QII          = 0;
//    phase->consumed_reactive_energy_QIII         = 0;
//    phase->consumed_reactive_energy_QIV          = 0;
//    phase->reactive_energy_counter_QI            = 0;
//    phase->reactive_energy_counter_QII           = 0;
//    phase->reactive_energy_counter_QIII          = 0;
//    phase->reactive_energy_counter_QIV           = 0;
//    phase->inc_import_active_energy              = 0;
//    phase->inc_export_active_energy              = 0;
//    phase->inc_reactive_energy_QI                = 0;
//    phase->inc_reactive_energy_QII               = 0;
//    phase->inc_reactive_energy_QIII              = 0;
//    phase->inc_reactive_energy_QIV               = 0;
//
//    // Reset Daily load profile
//    obj_daily_load_profile_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset Hourly load profile
//    obj_load_profile_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset Communication log profile
//    obj_common_event_log_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset disconnect event log profile
//    obj_disconnect_event_log_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset firmware event log profile
//    obj_firmware_event_log_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset fraud event log profile
//    obj_fraud_event_log_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset power quality event log profile
//    obj_power_qual_event_log_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset standard event log profile
//    obj_event_log_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset synchronization event log profile
//    obj_synchronization_event_log_reset((uint8_t *) 0, 0,(uint8_t *)0,(uint16_t *)0);
//    // Reset balance
//    reset_balance();
//    //Reset error alarms
//    status.UpperCoverRemovedTamperStatus = 0;
//    status.LowerCoverRemovedTamperStatus = 0;
//    status.MangneticTamperStatus         = 0;
//    status.NeutralTamperStatus           = 0;
//    status.write_tamper_status           = 1;
//
//}
