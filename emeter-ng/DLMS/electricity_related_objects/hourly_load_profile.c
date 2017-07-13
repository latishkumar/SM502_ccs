/*
 * load_profiles.c
 *
 *  Created on: May 16, 2017
 *      Author: abi
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
                     TAG_UINT8, INJECT8(1),                                   //    AMR profile status
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_ENERGY_IMPORT_LP_1),         //    Active Energy A+     :Ai
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_ACTIVE_ENERGY_EXPORT_LP_1),                                 //    Active Energy A-     :Ae
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QI_LP_1),        //    Reactive Energy QI   :R1
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QII_LP_1),                                 //    Reactive Energy QII  :R2
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QIII_LP_1),                                 //    Reactive Energy QIII :R3
        STUFF_DATA | TAG_UINT32, INJECT32(ITEM_TAG_REACTIVE_ENERGY_QIV_LP_1),                    //    Reactive Energy QIV  :R4 //originally this was for voltage

};

uint8_t find_num_total_energy_log_entries(uint16_t *num_entries,uint16_t *start_entry)
{
   //count the total number of entries we have
       /*get the last energy log address
         divide it by the size of energy log struct
          return this number
       */

   *start_entry = 0;
   *num_entries = 0;
   int32_t x=0;

   if(status.energy_log_overlapped == 1)
   {
     *num_entries = EnergyLog_SIZE; // the buffer is full, so return all Logs
   }
   else{

     uint32_t add_start = EnergyLogAddress_Start;
     if(LastEnergyLogAddress > add_start)
       x = LastEnergyLogAddress - add_start;

     while(x > 0)
     {
            x -= EnergyLogSize;
            *num_entries= *num_entries + 1;
     }
   }
   if(*num_entries > 0)
     *start_entry = 1;
//   *num_entries = (LastEnergyLogAddress - EnergyLogAddress_Start) / EnergyLogSize;

  return 1;//success
}
uint8_t get_captured_log_by_time_range(const sSA_Range *startRange,const sSA_Range *endRange,uint16_t *startEntryNumber,uint16_t *numOfEntries);
/*
 * Hourly Load profile
 * buffer-callback function
 */
void capture_load_profile_data(void *data, int direction)
{
    /* Load Template for Load Profile */
   msg_info.template=load_profile_buffer_template;
   msg_info.sz_template=sizeof(load_profile_buffer_template);

   if(access_selector == 1) //range_descriptor specifically by date range
   {
     if(SA_Range[1].Year>4095 || SA_Range[0].Year>4095 )
         msg_info.num_entries = 0x0;
     else
        get_captured_log_by_time_range(&SA_Range[0],&SA_Range[1],&msg_info.start_entry,&msg_info.num_entries);
   }
   else if(access_selector == 2) //entry descriptor
   {
//       SA_From_Entry;
//       SA_To_Entry;
       find_num_total_energy_log_entries(&msg_info.num_entries,&msg_info.start_entry);
       if(SA_From_Entry <= SA_To_Entry)
       {
          msg_info.start_entry = msg_info.start_entry > SA_From_Entry? msg_info.start_entry :SA_From_Entry;
          SA_To_Entry = SA_To_Entry > msg_info.num_entries? msg_info.num_entries : SA_To_Entry;
          SA_To_Entry -=  SA_From_Entry;
          msg_info.num_entries = SA_To_Entry + 1;
         // msg_info.num_entries = msg_info.num_entries > SA_To_Entry? (SA_To_Entry):msg_info.num_entries;
       }
       else
       {
           msg_info.num_entries = 0x0;
       }
   }
   else//   if(access_selector == 0)
   {
      find_num_total_energy_log_entries(&msg_info.num_entries,&msg_info.start_entry);
   }
   msg_info.column_szs=load_profile_column_szs;
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
 * Callback function for houlry load profile capture period
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
      uint32_t tmp32 = EnergyLogAddress_Start;
      uint8_t temp8=0;
      LastEnergyLogAddress = EnergyLogAddress_Start;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);
      write_to_eeprom(&temp8,(uint8_t *)0,setEnergyOverlapFlag);
      *response_len = 0;
}

/*
 * Hourly Load Profile
 * Capture method //
 */
void obj_load_profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    hourly_energy_log_t tmp;
    tmp.inc_active_import_energy = (uint16_t) chan1.inc_active_import_energy;
    tmp.inc_active_export_energy = (uint16_t) chan1.inc_active_export_energy;
    tmp.inc_reactive_energy_QI   = (uint16_t) chan1.inc_reactive_energy_QI;
    tmp.inc_reactive_energy_QII  = (uint16_t) chan1.inc_reactive_energy_QII;
    tmp.inc_reactive_energy_QIII = (uint16_t) chan1.inc_reactive_energy_QIII;
    tmp.inc_reactive_energy_QIV  = (uint16_t) chan1.inc_reactive_energy_QIV;
    tmp.time_stump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
    tmp.crc = tmp.inc_active_import_energy + tmp.inc_reactive_energy_QIV + tmp.time_stump.TimestampLow+ tmp.time_stump.TimestampUp;
    write_to_eeprom(&tmp,(uint8_t *)0,log_hourly_energy_profile);

    //reset hourly energy registers
    chan1.inc_active_import_energy = 0;
    chan1.inc_active_export_energy = 0;
    chan1.inc_reactive_energy_QI = 0;
    chan1.inc_reactive_energy_QII = 0;
    chan1.inc_reactive_energy_QIII = 0;
    chan1.inc_reactive_energy_QIV = 0;
    *response_len = 0;
}

uint8_t get_captured_log_by_time_range(const sSA_Range *startRange,const sSA_Range *endRange,uint16_t *startEntryNumber,uint16_t *numOfEntries)
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
     uint32_t add_start = EnergyLogAddress_Start;
     if(status.energy_log_overlapped == 1) // if the circular buffer is full
     {
         MAX_Entries = EnergyLog_SIZE;
     }
     else
     {
        MAX_Entries = (LastEnergyLogAddress - add_start)/INCREMENTAL_ENERGY_LOG_SIZE;
        if(MAX_Entries == 0)
        {
            *startEntryNumber =0;
            *numOfEntries = 0;
            return 1;
        }
     }

    rtc_t time_first;
    rtc_t time_last ;
    uint8_t z = get_hourly_energy_log_time_stamp(&time_first,1); //get our first entry
    if(z == 0)
    {
        //error abort
    }
    z = get_hourly_energy_log_time_stamp(&time_last,MAX_Entries);//get the last entry
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
        z = get_hourly_energy_log_time_stamp(&time_first,index);//get the last entry
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

int adderr = 5000;
void test_circular_buffer()
{
    uint32_t add_start = EnergyLogAddress_Start;
    uint16_t tmp2 = 0;
    uint8_t day = 13;
    uint8_t hour = 0;
    uint8_t minute = 0;
    hourly_energy_log_t tmp;

    while(tmp2 < EnergyLog_SIZE-1)
    {

        tmp.inc_active_import_energy = 65000;
        tmp.inc_active_export_energy = 62000;
        tmp.inc_reactive_energy_QI   = 60000;
        tmp.inc_reactive_energy_QII  = 58000;
        tmp.inc_reactive_energy_QIII = 56000;
        tmp.inc_reactive_energy_QIV  = 54000;
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
    }

    tmp2 = (LastEnergyLogAddress - add_start)/INCREMENTAL_ENERGY_LOG_SIZE;

}
