/*
 * disconnect_control_and_log.c
 *
 *  Created on: May 15, 2017
 *      Author: abi
 */
#include "headers.h"
#include "disconnect_control_and_log.h"
/*
 * Disconnect control Scheduler
 * Type
 */
 const uint8_t diconnect_control_type;
/*
 * Disconnect Control
 * output_state
 * Type: boolean
 */
uint8_t output_state = 0;

/*
 * Disconnect Control
 * control_state
 *      (0): Disconnected
 *      (1): Connected
 *      (2):Ready for Re-connection (Manually)
 */
uint8_t control_state =0;

/*
 * Disconnect Control
 * control_mode
 * Default: 4
 */
uint8_t control_mode = 5;
/*
 * Currently Import Active Power Threshold
 *
 */
uint32_t current_import_active_power_threshold;

/*
 * Export Active Power over threshold status
 * Value: boolean
 *        /True: Export Active P is over threshold
 */
uint8_t export_active_power_over_thr_status = 0;

/*
 * Import Active Power over threshold status
 * value: boolean
 *       True: Import Active P is over threshold
 */
uint8_t import_active_power_over_thr_status = 0;


/*
 * Currently Export Active Power Threshold
 *
 */
uint32_t current_export_active_power_threshold;

/*
 * Disconnect control Scheduler
 * executed script
 */
const uint8_t executed_diconnect_control_script[] =
{
    11 + 1,
        2,
            TAG_OCTET_STRING, 6, 0, 0, 10, 0, 106, 255,       //disconnect Script
            TAG_UINT16, INJECT16(1),
};

/*
 * Disconnect script table
 */
const uint8_t Disconnect_Scripts_Table[] =
{
    2*26 + 1,
        /*TAG_ARRAY,*/ 2,
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1), /* Script ID - 1 disconnect script*/
                TAG_ARRAY, 1,
                TAG_STRUCTURE, 5,
                        TAG_ENUM, 2,    /* execute method */
                        TAG_UINT16, INJECT16(70), /* class id*/
                        TAG_OCTET_STRING, 6, 0, 0, 96, 3, 10, 255, /* disconnect control*/
                        TAG_INT8, 1,
                        TAG_INT8, 0,
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(2), /* Script ID - 2 connect script*/
                TAG_ARRAY, 1,
                TAG_STRUCTURE, 5,
                        TAG_ENUM, 2,    /* execute method */
                        TAG_UINT16, INJECT16(70), /* class id*/
                        TAG_OCTET_STRING, 6, 0, 0, 96, 3, 10, 255,  /* disconnect control */
                        TAG_INT8, 2,
                        TAG_INT8, 0
};
/*
 * Disconnect control Scheduler
 * execution time
 */
uint8_t diconnect_control_script_execution_time[] =
{
            TAG_STRUCTURE, 2,
                STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DISCONNECTCONTORL_SCRIPT_EXE_TIME, 0, 0, 0,              // Time-Midnight        //8,9
                STUFF_DATA|TAG_OCTET_STRING, 5, ITEM_TAG_DISCONNECTCONTROL_SCRIPT_EXE_DAY, 0, 0, 0, 0,    // Last day of Month   //17
};
/*
 * Call back function for Disconnect Script Table object
 */
void access_disconnect_scripts(void *data, int direction)
{
    if (direction != ATTR_READ)
        return;
    memcpy(data, Disconnect_Scripts_Table, sizeof(Disconnect_Scripts_Table));
}

/*
 * Execute methode for Disconnect Script Table object
 */
void disconnect_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
  //test this method and try to use the data_len argument
    uint16_t methodNumber = *data;
             methodNumber |= ((uint16_t)(*(data++)))<< 8;

     if(methodNumber == 1)
     {
        status.DisconnectCommandRecived = 1;
     }
     else if(methodNumber == 2)
     {
        status.ConnectCommandRecived = 1;
     }
}

/*
 * Disconnect Control
 * Remote disconnect method
 */
void remote_disconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
   status.DisconnectCommandRecived = 1;
   status.ConnectCommandRecived = 0;
}

/*
 * Disconnect control
 * Remote connect method
 */
void remote_reconnect(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    status.ConnectCommandRecived = 1;
    status.DisconnectCommandRecived = 0;
}
