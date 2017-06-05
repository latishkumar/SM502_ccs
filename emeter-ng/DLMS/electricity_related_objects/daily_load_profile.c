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

uint8_t find_num_daily_energy_log_entries_between(const sSA_Range *startRange,const sSA_Range *endRange,
                                                   uint16_t *startEntryNumber,uint16_t *numOfEntries)
{
     rtc_t temp_sRange ={startRange->Year,startRange->Month,startRange->Date,startRange->Hr,startRange->Min,0,0};
     rtc_t temp_eRange ={endRange->Year,endRange->Month,endRange->Date,endRange->Hr,endRange->Min,0,0};
     int8_t temp1 = compare_time(&temp_eRange,&temp_sRange); // if the end time specified comes before the first time requested, then abort
     if(temp1 < 0)
     {
       *startEntryNumber = 0;
       *numOfEntries = 0;
       return 1;
     }

     uint16_t MAX_Entries = 0;
     uint32_t add_start = DAILY_SNAPSHOT_LOG_ADDRESS_START;

    //confirm this with the number of entries we have
     if(last_daily_snapshot_log_address > DAILY_SNAPSHOT_LOG_ADDRESS_START)//if we have entries
     {

         if(status.energy_log_overlaped == 1) // if the cirular buffer is full
         {
           MAX_Entries = DAILY_SNAPSHOT_MAX_LOGS;
         }
         else
         {
            MAX_Entries = (last_daily_snapshot_log_address - add_start)/DAILY_SNAPSHOT_LOG_SIZE;
         }

         EnergyLog firstEntery,lastEntry;
         uint8_t z = get_daily_snapshot_energy_profile(&firstEntery,1); //get our first entry
         if(z == 0)
         {
           //error abort
         }

          rtc_t time_first = getTime(&firstEntery.timeStump);
          temp1 = compare_time(&temp_eRange,&time_first);//if the first entry we have is is after the last entry requested then we don't have the data
          if(temp1 < 0)
          {
             *startEntryNumber = 0;
             *numOfEntries = 0;
             return 1;
          }

          int8_t com2 = compare_time(&time_first,&temp_sRange);
          if(com2 < 0)//if time_first comes before temp_sRange
          {
            //search for the entry number for temp_sRange;

                //Search_t search_item;
                search_item.start_entry = 0;//startEntryNumber;
                search_item.end_entry = MAX_Entries;//search_item.start_entry +  *numOfEntries;
                //search_item.SingleItemSize = EnergyLogSize;
                EnergyLog l;
                EnergyLog l2;
                l2.timeStump = getTimeStamp(temp_sRange.year,temp_sRange.month,temp_sRange.day,temp_sRange.hour,temp_sRange.minute,temp_sRange.second);
      //          rtc_t time;
      //          search_item.current_c_data = &time;
                search_item.current_data = &l;
               // search_item.StartAddress = EnergyLogAddress_Start;
                search_item.PeakItem = &get_daily_snapshot_energy_profile;
                search_item.Compare = &compare_energy;
      //          search_item.getComapareItem = &getEnergyComapareItem;
                search_item.search_data = &l2;//&temp_sRange;

                uint32_t sa =  search_nearest_log(&search_item);
      //          find the actual last entry
      //          check if this entry is with in the start and end range

                if(sa ==0)
                {
                *startEntryNumber = search_item.last_entry_no;
                }
                else
                {
                  *startEntryNumber = sa;
                }
          }
          else
          {
             *startEntryNumber = 1;
          }


          z = get_daily_snapshot_energy_profile(&lastEntry,MAX_Entries);//get the last entry
          if(z == 0)
          {
                //error abort
          }
          rtc_t time_last = getTime(&lastEntry.timeStump);
          int8_t com3 = compare_time(&time_last,&temp_eRange);



          if(com3 > 0)//temp_eRange comes before  time_last
          {
            //search for temp_eRange

            void *t1 = &required_last_entry_time;
            void *t2 = &temp_eRange;
            memcpy(t1,t2,sizeof(required_last_entry_time));
            if(com2<0)
              *numOfEntries = (uint32_t)(getTimeDifferenceInMinutes(&temp_eRange,&temp_sRange)/daily_load_profile_capture_period);
            else
              *numOfEntries = (uint32_t)(getTimeDifferenceInMinutes(&temp_eRange,&time_first)/daily_load_profile_capture_period);
          }
          else
          {
            //last entry is the last number of entry

            void *t1 = &required_last_entry_time;
            void *t2 = &time_last;
            memcpy(t1,t2,sizeof(time_last));
            if(com2 < 0)
              *numOfEntries = (uint32_t)(getTimeDifferenceInMinutes(&time_last,&temp_sRange)/daily_load_profile_capture_period);
            else
              *numOfEntries = (uint32_t)(getTimeDifferenceInMinutes(&time_last,&time_first)/daily_load_profile_capture_period);
          }


          if((*startEntryNumber * DAILY_SNAPSHOT_MAX_LOGS) + DAILY_SNAPSHOT_LOG_ADDRESS_START  > last_daily_snapshot_log_address)
             *numOfEntries = 0;
          else
          {
             *numOfEntries = *numOfEntries>MAX_Entries?MAX_Entries:*numOfEntries;
          }
     }
     else
     {
       *startEntryNumber =0;
       *numOfEntries = 0;
     }
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

   if(status.event_log_overlaped == 1)
   {
     *num_entries = DAILY_SNAPSHOT_MAX_LOGS; // the buffer is full, so return all Logs
   }
   else{

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
 * Hourly Load profile
 * buffer-callback function
 */
void capture_daily_load_profile_data(void *data, int direction)
{
    /* Load Template for Load Profile 2*/
   msg_info.template=daily_load_profile_buffer_template;
   msg_info.sz_template=sizeof(daily_load_profile_buffer_template);

   if(access_selector == 1) //range_descriptor specifically by date range
   {
     if(SA_Range[1].Year>4095 || SA_Range[0].Year>4095 )
         msg_info.num_entries=0x0;
     else
         find_num_daily_energy_log_entries_between(&SA_Range[0],&SA_Range[1],&msg_info.start_entry,&msg_info.num_entries);/*msg_info.num_entries=5*/
   }
   else if(access_selector == 2) //entry descriptor
   {
//       SA_From_Entry;
//       SA_To_Entry;
       find_num_total_daily_energy_log_entries(&msg_info.num_entries,&msg_info.start_entry);
       if(SA_From_Entry <= SA_To_Entry)
       {
          msg_info.start_entry = msg_info.start_entry > SA_From_Entry? msg_info.start_entry :SA_From_Entry;
          SA_To_Entry -=  SA_From_Entry;
          msg_info.num_entries = msg_info.num_entries > SA_To_Entry? (SA_To_Entry):msg_info.num_entries;
       }
       else
       {

       }
   }
   else//   if(access_selector == 0)
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
 * Callback function for houlry load profile capture period
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
      uint8_t temp8=3;
      last_daily_snapshot_log_address = DAILY_SNAPSHOT_LOG_ADDRESS_START;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);
      write_to_eeprom(&temp8,(uint8_t *)0,setEnergyOverlapFlag);

}

/*
 * Hourly Load Profile
 * Capture method
 */
void obj_daily_load_profile_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}



