/*
 * event_handling.c
 *
 *  Created on: May 15, 2017
 *      Author: abi
 */

#include "headers.h"
#include "event_handling.h"
#include "hourly_load_profile.h"
/*
 * Event Object - Standard Event Log
 * event number (0 to 255)
 */
uint8_t standard_event_number;

/*
 * Capture period for events
 * value: 0 asynchronously
 */
const uint32_t events_capture_period = 0;

/*
 * Event sort method
 * value:1 /unsorted (FIFO)
 */
const uint8_t events_sort_method = 1;

/*
 * Entries in use for events log
 */
uint32_t events_entries_in_use;

/*
 * Standard event profile entries
 * Default: >100
 */
const uint32_t standard_event_profile_entries = 5701;
const uint16_t standard_event_log_column_szs[] = {16,18};
/*
 * Template for standard event log profile
 */
const uint8_t standard_event_log_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 2,
        STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_SE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Event Time stump
        STUFF_DATA | TAG_UINT8, INJECT8(ITEM_TAG_EVENT_CODE_SE)                                  // Event code
};

/*
 * Capture objects for standard event log profile
 */
const uint8_t standard_event_log_objects[] =
{
    INJECT16(0x8000 | (2*18+1)),
         2,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),
                TAG_OCTET_STRING, 6, 0, 0, 96, 11, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)

};
uint8_t find_num_standard_event_log__entries_between(const sSA_Range *startRange,const sSA_Range *endRange,
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
     const uint32_t add_start = EventLogAddress_Start;

    //confirm this with the number of entries we have
     if(LastEventLogAddress > add_start)//if we have entries
     {

         if(status.standard_event_log_overlapped == 1) // if the cirular buffer is full
         {
           MAX_Entries = MAX_STANDARD_EVENT_LOGS;
         }
         else{
            MAX_Entries = (LastEventLogAddress - add_start)/EventLogSize;
         }

         EventLog firstEntery,lastEntry;
         uint8_t z = getEvent2(&firstEntery,1); //get our first entry
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
                EventLog l;
                EventLog l2;
                l2.timeStump = getTimeStamp(temp_sRange.year,temp_sRange.month,temp_sRange.day,temp_sRange.hour,temp_sRange.minute,temp_sRange.second);
      //          rtc_t time;
      //          search_item.current_c_data = &time;
                search_item.current_data = &l;
               // search_item.StartAddress = EnergyLogAddress_Start;
                search_item.PeakItem = &getEvent2;
                search_item.Compare = &compare_event;
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


          z = getEvent2(&lastEntry,MAX_Entries);//get the last entry
          if(z == 0)
          {
                //error abort
          }
          rtc_t time_last = getTime(&lastEntry.timeStump);
          int8_t com3 = compare_time(&time_last,&temp_eRange);


          //Correct for this forward ??????????????????????????????????
          if(com3 >= 0)//temp_eRange comes before  time_last
          {
            //search for temp_eRange
            void *t1 = &required_last_entry_time;
            void *t2 = &temp_eRange;
            memcpy(t1,t2,sizeof(required_last_entry_time));

          }
          else
          {
            //last entry is the last number of entry
            void *t1 = &required_last_entry_time;
            void *t2 = &time_last;
            memcpy(t1,t2,sizeof(time_last));
          }
           *numOfEntries = MAX_Entries;
     }
     else
     {
       *startEntryNumber =0;
       *numOfEntries = 0;
     }
      return 1;//success
}

uint8_t find_num_total_Standard_Event_Log_entries(uint16_t *num_entries,uint16_t *start_entry)
{
   //count the total number of entries we have
       /*get the last energy log address
         divide it by the size of energy log struct
          return this number
       */

      *start_entry = 1;
      *num_entries = 0;

      if(status.standard_event_log_overlapped == 1)
      {
        *num_entries = MAX_STANDARD_EVENT_LOGS;
      }
      else
      {
        int32_t x=0;
        const uint32_t Add_Start = EventLogAddress_Start;
        if(LastEventLogAddress > Add_Start)
          x = LastEventLogAddress - Add_Start;

        while(x > 0)
        {
            x -= EventLogSize;
            *num_entries= *num_entries + 1;
        }
      }

      if(*num_entries>0)
        *start_entry = 1;


  return 1;//success
}
/*
 * Call back function for standard event lof
 */
void capture_standard_event_log(void *data, int direction)
{
    /* Load Template for Load Profile */
   // interpret_template(data, direction, Load_Profile_Buffer_Template, sizeof(Load_Profile_Buffer_Template));
   msg_info.template=standard_event_log_template;
   msg_info.sz_template=sizeof(standard_event_log_template);
    if(access_selector==1)
    {
        if(SA_Range[1].Year>4095 || SA_Range[0].Year>4095 )
           msg_info.num_entries=0x0;
        else
           find_num_standard_event_log__entries_between(&SA_Range[0],&SA_Range[1],&msg_info.start_entry,&msg_info.num_entries);/*msg_info.num_entries=5*/
    }
    else if(access_selector==2)
    {
        find_num_total_Standard_Event_Log_entries(&msg_info.num_entries,&msg_info.start_entry);
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
    else
    {
        find_num_total_Standard_Event_Log_entries(&msg_info.num_entries,&msg_info.start_entry);
    }
      msg_info.column_szs=standard_event_log_column_szs;
}

/*
 * Reset method for standard event log
 */
void obj_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 = EventLogAddress_Start;
      uint8_t temp8 = 3;
      LastEventLogAddress = tmp32;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);
      temp8 = 0;
      write_to_eeprom(&temp8,(uint8_t *)3,setEventOverlapFlag);
}

/*
 * Capture function for standard event log
 */
void obj_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/**********************************************************************/

