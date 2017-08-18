/*
 * load_profiles.c
 *
 *  Created on: May 16, 2017
 *      Author: Eliab Erango
 */
#include "headers.h"
#include "hourly_load_profile.h"

/*
 * AMR profile status for Load
 * profile with period 1
 * {IV, CA, CY, VH , MP, INT,AL, RES}
 */
uint8_t amr_profile_status;

uint32_t load_profile_entries_in_use = 5;

uint32_t load_profile_entries = 2160;           // >= 2160 (90 days, 3 months with capture_period=60 min

uint32_t load_profile_capture_period = 3600;    // 3.600 s (1 hour)

uint8_t load_profile_sort_method = 1;           // 1:unsorted (FIFO)

const uint16_t load_profile_column_szs[] = {16,18,23,28,33,38,43,48};//
const uint8_t load_profile_buffer_template[] = //load_hourly_energy_to_ram
{
   STUFF_DATA | TAG_STRUCTURE, 8,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_LP_1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //Time stump
                     TAG_UINT8, INJECT8(1),                                      //    AMR profile status
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_ENERGY_IMPORT_LP_1),   //    Active Energy A+     :Ai
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_ENERGY_EXPORT_LP_1),   //    Active Energy A-     :Ae
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QI_LP_1),     //    Reactive Energy QI   :R1
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QII_LP_1),    //    Reactive Energy QII  :R2
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QIII_LP_1),   //    Reactive Energy QIII :R3
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QIV_LP_1),    //    Reactive Energy QIV  :R4 //originally this was for voltage

};

/*
 * Hourly Load profile
 * buffer-callback function
 */
void capture_load_profile_data(void *data, int direction)
{
    log_search_params.start_log_address  = HOURLY_ENERGY_LOG_ADDRESS_START;
    log_search_params.end_log_address    = HOURLY_ENERGY_LOG_ADDRESS_END;
    log_search_params.last_log_address   = last_hourly_energy_log_address;
    log_search_params.log_size           = INCREMENTAL_ENERGY_LOG_TYPE_SIZE;
    log_search_params.maximum_event_logs = HOURLY_ENERGY_MAX_LOGS;
    log_search_params.offset             = 12;
    log_search_params.overlap_status     = status.hourly_energy_log_overlapped;
    log_search_params.template           = load_profile_buffer_template;
    log_search_params.sz_template        = sizeof(load_profile_buffer_template);
    log_search_params.log_column_size    = load_profile_column_szs;

    get_captured_logs(&log_search_params);
}

/*
 * Hourly Load profile
 * Capture objects
 */
const uint8_t load_profile_capture_objects[] =
{
    INJECT16(0x8000 | (8*18 + 1)),
         8,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //AMR profile status
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, 0, 0, 96, 10, 7, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //Ae Import
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 1, 29, 0, 255,              //[E.E] was 0, 0, 96, 10, 7, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //Ae Export
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 2, 29, 0, 255,              //[E.E]was 1, 0, 1, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //ReQ1
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 5, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                          //ReQ2
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 6, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                           //ReQ3
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 7, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,                                           //ReQ4
                TAG_UINT16, INJECT16(CLASS_ID_REGISTER),
                TAG_OCTET_STRING, 6, 1, 0, 8, 29, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)
};

/*
 * Callback function for hourly load profile capture period
 */
void change_hourly_load_profile_capture_period(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      EnergyLoggingTime = *datap;
      write_to_eeprom(&EnergyLoggingTime,(uint8_t *)0,setEnergyLogTime);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap= EnergyLoggingTime;
    }
}
/*
 * Hourly Load Profile
 * Reset method
 */
void obj_load_profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 = HOURLY_ENERGY_LOG_ADDRESS_START;
      uint8_t temp1=0;
      last_hourly_energy_log_address = tmp32;
      write_to_eeprom(&tmp32,&temp1,setLastLogAddress);
      write_to_eeprom(&temp1,&temp1,setEnergyOverlapFlag);
      *response_len = 0;
}

/*
 * Hourly Load Profile
 * Capture method //
 */
__inline__ void calculate_avergae_v_pf_f();
void obj_load_profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    // first calculate average voltage, power factor and frequency before capturing
    calculate_avergae_v_pf_f();

    hourly_energy_log_t tmp;
    tmp.inc_active_import_energy = (uint16_t) chan1.inc_import_active_energy; // Active import Energy
    tmp.inc_active_export_energy = (uint16_t) chan1.inc_export_active_energy; // peak power
    tmp.inc_reactive_energy_QI   = (uint16_t) chan1.inc_reactive_energy_QI;   // Reactive Energy QIV
    tmp.inc_reactive_energy_QII  = (uint16_t) chan1.inc_reactive_energy_QII;  // average voltage
    tmp.inc_reactive_energy_QIII = (uint16_t) chan1.inc_reactive_energy_QIII; // average power factor
    tmp.inc_reactive_energy_QIV  = (uint16_t) chan1.inc_reactive_energy_QIV;  // average frequency
    tmp.timestamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
    tmp.crc = tmp.inc_active_import_energy + tmp.inc_reactive_energy_QIV + tmp.timestamp.TimestampLow+ tmp.timestamp.TimestampUp;
    write_to_eeprom(&tmp,(uint8_t *)0,log_hourly_energy_profile);

    //reset hourly energy registers
    chan1.inc_import_active_energy = 0;
    chan1.inc_export_active_energy = 0;
    chan1.inc_reactive_energy_QI   = 0;
    chan1.inc_reactive_energy_QII  = 0;
    chan1.inc_reactive_energy_QIII = 0;
    chan1.inc_reactive_energy_QIV  = 0;
    *response_len = 0;
}

/*
int adderr = 1000;
void test_circular_buffer()
{
    uint32_t add_start = EnergyLogAddress_Start;
    uint16_t tmp2 = 0;
    uint8_t day = 13;
    uint8_t hour = 0;
    uint8_t minute = 0;
    hourly_energy_log_t tmp;

    while(tmp2 < 3000)//EnergyLog_SIZE-1)
    {

        tmp.inc_active_import_energy = 1000 + adderr;
        tmp.inc_active_export_energy = 2000 + adderr;
        tmp.inc_reactive_energy_QI   = 3000 + adderr;
        tmp.inc_reactive_energy_QII  = 4000 + adderr;
        tmp.inc_reactive_energy_QIII = 5000 + adderr;
        tmp.inc_reactive_energy_QIV  = 6000 + adderr;
        tmp.time_stump = getTimeStamp(rtcc.year, rtcc.month, day, hour, minute, 0);
        tmp.crc = tmp.inc_active_import_energy + tmp.inc_reactive_energy_QIV + tmp.time_stump.TimestampLow+ tmp.time_stump.TimestampUp;
        write_to_eeprom(&tmp,(uint8_t *)0,log_hourly_energy_profile);
        tmp2 = (LastEnergyLogAddress - add_start)/INCREMENTAL_ENERGY_LOG_SIZE;
        minute++;
        if (minute == 60)
        {
            minute = 0;
            hour++;
            if(hour == 24)
            {
                hour = 0;
                day++;
            }
        }
        adderr+=10;
    }

    tmp2 = (LastEnergyLogAddress - add_start)/INCREMENTAL_ENERGY_LOG_SIZE;

}
*/
