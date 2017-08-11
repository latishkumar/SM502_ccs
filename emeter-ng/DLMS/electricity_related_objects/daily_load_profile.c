/*
 * daily_load_profile.c
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */

/*
 * AMR profile status for Load
 * profile with period 1
 * {IV, CA, CY, VH , MP, INT,AL, RES}
 */
#include "headers.h"
#include "daily_load_profile.h"
#include "hourly_load_profile.h"

uint8_t daily_amr_profile_status;

uint32_t daily_load_profile_entries_in_use = 5;

uint32_t daily_load_profile_entries = 2160;           // >= 2160 (90 days, 3 months with capture_period=60 min

uint32_t daily_load_profile_capture_period = 86400;    // 86400 s (1 day)
uint16_t daily_load_profile_capture_period_min = 1440; // 1440 min (1day)
uint8_t daily_load_profile_sort_method = 1;           // 1:unsorted (FIFO)

const uint16_t daily_load_profile_column_szs[] = {16,18,23,28,33,38,43,48};//
const uint8_t daily_load_profile_buffer_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 8,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_LP_2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //Time stump
                     TAG_UINT8, INJECT8(1),                                   //    AMR profile status
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_CUM_KWH_TOTAL_LP_2),       //    Active Energy A+     :Ai
                     TAG_UINT32, INJECT32(0),                                 //    Active Energy A-     :Ae
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QI_LP_2),  //    Reactive Energy QI   :R1
                     TAG_UINT32, INJECT32(0),                                 //    Reactive Energy QII  :R2
                     TAG_UINT32, INJECT32(0),                                 //    Reactive Energy QIII :R3
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QIV_LP_2)  //    Reactive Energy QIV  :R4 //originally this was for voltage

};

/*
 * Get daily captured energy log by time range
 */
uint8_t get_daily_energy_captured_log_by_time_range(const sSA_Range *startRange,const sSA_Range *endRange,uint16_t *startEntryNumber,uint16_t *numOfEntries)
{
     rtc_t temp_sRange ={startRange->Year,startRange->Month,startRange->Date,startRange->Hr,startRange->Min,0,0};
     rtc_t temp_eRange ={endRange->Year,endRange->Month,endRange->Date,endRange->Hr,endRange->Min,0,0};
     int8_t temp1 = compare_time(&temp_eRange,&temp_sRange);
     int8_t temp2 = 0;
     if(temp1 < 0)     // if the end time specified comes before the first time requested, then abort
     {
       *startEntryNumber = 0;
       *numOfEntries = 0;
       return 1;
     }

     uint16_t MAX_Entries = 0;
     uint32_t add_start = DAILY_SNAPSHOT_LOG_ADDRESS_START;
     if(status.daily_snapshot_energy_overlapped == 1) // if the circular buffer is full
     {
         MAX_Entries = DAILY_SNAPSHOT_MAX_LOGS;
     }
     else
     {
        MAX_Entries = (last_daily_snapshot_log_address - add_start)/DAILY_SNAPSHOT_LOG_SIZE;
        if(MAX_Entries == 0)
        {
            *startEntryNumber =0;
            *numOfEntries = 0;
            return 1;
        }
     }

    rtc_t time_first;
    rtc_t time_last ;
    uint8_t z = get_daily_energy_log_time_stamp(&time_first,1); //get our first entry
    if(z == 0)
    {
        //error abort
    }
    z = get_daily_energy_log_time_stamp(&time_last,MAX_Entries);//get the last entry
    if(z == 0)
    {
        //error abort
    }

    temp1 = compare_time(&temp_eRange,&time_first);
    temp2 = compare_time(&time_last,&temp_sRange);
    if(temp1 < 0 || temp2 < 0) // if the first entry we have is after the last entry requested or
    {                          // last entry we have is before the first entry requested then we don't have the data.
        *startEntryNumber = 0;
        *numOfEntries = 0;
        return 1;
    }
    temp1 = compare_time(&temp_sRange,&time_first);
    temp2 = compare_time(&time_last,&temp_eRange);
    // if the first entry we have comes after the first entry requested and the the last entry we have comes before the last entry requested
    // then return all data.
    if(temp1 <= 0 && temp2 <= 0) // if the first entry we have is after the last entry requested or
    {                            // last entry we have is before the first entry requested then we don't have the data.
        *startEntryNumber = 1;
        *numOfEntries = MAX_Entries;
        return 1;
    }
    //search for start entry and number of entries
    uint16_t index = 1;
    uint16_t number_of_entries = 0;
    for(;;)
    {
        z = get_daily_energy_log_time_stamp(&time_first,index);//get the last entry
        if(z == 0)
        {
            //error abort
        }
        temp1 = compare_time(&temp_sRange,&time_first);
        temp2 = compare_time(&time_first,&temp_eRange);
        if(temp1 <= 0 && temp2 <= 0 && index <= MAX_Entries) // we get an entry within the range
        {
            index++;
            number_of_entries++;
        }
        else if(index > MAX_Entries || temp2 > 0)
        {
            break; // reached last entry
        }
        else
        {
            index++;
        }
    }
    *startEntryNumber = index - number_of_entries ;
    *numOfEntries = number_of_entries;

    return 1;//success
}

uint8_t find_num_total_daily_energy_log_entries(uint16_t *num_entries,uint16_t *start_entry)
{
   //count the total number of entries we have
       /*get the last energy log address
         divide it by the size of energy log struct
          return this number
       */
    *start_entry = 0;
    *num_entries = 0;
    int32_t x=0;
    if(status.daily_snapshot_energy_overlapped == 1)
    {
        *num_entries = DAILY_SNAPSHOT_MAX_LOGS; // the buffer is full, so return all Logs
    }
    else
    {
        uint32_t add_start = DAILY_SNAPSHOT_LOG_ADDRESS_START;
        if(last_daily_snapshot_log_address > add_start)
        x = last_daily_snapshot_log_address - add_start;

        while(x > 0)
        {
            x -= DAILY_SNAPSHOT_LOG_SIZE;
            *num_entries= *num_entries + 1;
        }
    }
    if(*num_entries > 0)
    *start_entry = 1;

    return 1;//success
}

/*
 * Daily Load profile
 * buffer-callback function
 */
void capture_daily_load_profile_data(void *data, int direction)
{
    /* Load Template for Load Profile 2*/
   msg_info.template = daily_load_profile_buffer_template;
   msg_info.sz_template = sizeof(daily_load_profile_buffer_template);

    if(access_selector == 1) //range_descriptor specifically by date range
    {
        if(SA_Range[1].Year>4095 || SA_Range[0].Year>4095 )
        msg_info.num_entries = 0x0;
        else
        get_daily_energy_captured_log_by_time_range(&SA_Range[0],&SA_Range[1],&msg_info.start_entry,&msg_info.num_entries);/*msg_info.num_entries=5*/
    }
    else if(access_selector == 2) //entry descriptor
    {
        find_num_total_daily_energy_log_entries(&msg_info.num_entries,&msg_info.start_entry);
        if(SA_From_Entry <= SA_To_Entry)
        {
            msg_info.start_entry = msg_info.start_entry > SA_From_Entry? msg_info.start_entry :SA_From_Entry;
            SA_To_Entry = SA_To_Entry > msg_info.num_entries? msg_info.num_entries : SA_To_Entry;
            SA_To_Entry -=  SA_From_Entry;
            msg_info.num_entries = SA_To_Entry + 1;
        }
        else
        {
            msg_info.num_entries = 0x0;
        }
    }
   else //access_selector == 0
   {
      find_num_total_daily_energy_log_entries(&msg_info.num_entries,&msg_info.start_entry);
   }
   msg_info.column_szs=daily_load_profile_column_szs;
}

/*
 * Hourly Load profile
 * Capture objects
 */
const uint8_t daily_load_profile_capture_objects[] =
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
void change_daily_load_profile_capture_period(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      daily_load_profile_capture_period = *datap;
      write_to_eeprom(&daily_load_profile_capture_period,(uint8_t *)0,setEnergyLogTime);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap= daily_load_profile_capture_period;
    }
}
/*
 * Hourly Load Profile
 * Reset method
 */
void obj_daily_load_profile_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 = DAILY_SNAPSHOT_LOG_ADDRESS_START;
      uint8_t temp8 = 1;
      last_daily_snapshot_log_address = DAILY_SNAPSHOT_LOG_ADDRESS_START;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);
      write_to_eeprom(&temp8,(uint8_t *)0,setEnergyOverlapFlag);

}
void capture_daily_snapshot()
{
	daily_energy_log_t daily_snapshot;
	daily_snapshot.active_import_energy = chan1.import_active_energy_QI_QIV;
	daily_snapshot.active_export_energy = chan1.export_active_energy_QII_QIII;
	daily_snapshot.reactive_energy_QI   = chan1.consumed_reactive_energy_QI;
	daily_snapshot.reactive_energy_QII  = chan1.consumed_reactive_energy_QII;
	daily_snapshot.reactive_energy_QIII = chan1.consumed_reactive_energy_QIII;
	daily_snapshot.reactive_energy_QIV  = chan1.consumed_reactive_energy_QIV;
	daily_snapshot.timestamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	daily_snapshot.crc = daily_snapshot.active_import_energy + daily_snapshot.reactive_energy_QI + daily_snapshot.timestamp.TimestampLow+ daily_snapshot.timestamp.TimestampUp;
	write_to_eeprom(&daily_snapshot,(uint8_t *)0,log_daily_energy_snapshot);
}
/*
 * Hourly Load Profile
 * Capture method
 */
void obj_daily_load_profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
	capture_daily_snapshot();
}


/*
void test_circular_buffer()
{
    uint32_t add_start = DAILY_SNAPSHOT_LOG_ADDRESS_START;
    uint16_t tmp2 = 0;
    uint8_t day = 6;
    uint8_t hour = 0;
    uint8_t minute = 0;
    daily_energy_log_t tmp;

    while(tmp2 < 89)//EnergyLog_SIZE-1)
    {

        tmp.active_import_energy = 1000 + adderr;
        tmp.active_export_energy = 2000 + adderr;
        tmp.reactive_energy_QI   = 3000 + adderr;
        tmp.reactive_energy_QII  = 4000 + adderr;
        tmp.reactive_energy_QIII = 5000 + adderr;
        tmp.reactive_energy_QIV  = 6000 + adderr;
        tmp.timestamp = getTimeStamp(rtcc.year, rtcc.month, day, hour, minute, 0);
        tmp.crc = tmp.active_export_energy + tmp.reactive_energy_QI + tmp.timestamp.TimestampLow+ tmp.timestamp.TimestampUp;
        write_to_eeprom(&tmp,(uint8_t *)0,log_daily_energy_snapshot);
        tmp2 = (last_daily_snapshot_log_address - add_start)/DAILY_SNAPSHOT_LOG_SIZE;
        minute+=30;
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
        adderr++;
    }

    tmp2 = (LastEnergyLogAddress - add_start)/INCREMENTAL_ENERGY_LOG_SIZE;
}

*/
