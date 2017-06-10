/*
 * firmware_event_handling.c
 *
 *  Created on: Jun 9, 2017
 *      Author: abi
 */
#include "headers.h"
#include "event_handling.h"
#include "hourly_load_profile.h"
#include "firmware_event_handling.h"
/*
 * Event Object - firmware Event Log
 * event number (0 to 255)
 */
uint8_t firmware_event_number;

/*
 * firmware event profile entries
 * Default: >100
 */
const uint32_t firmware_event_profile_entries = 1000;
const uint16_t firmware_event_log_column_szs[] = {16,18,25,32};
/*
 * Template for firmware event log profile
 */
const uint8_t firmware_event_log_template[] =
{
   STUFF_DATA | TAG_STRUCTURE, 4,
   STUFF_DATA | TAG_OCTET_STRING, 12,ITEM_TAG_DATETIME_FRME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Event Time stump
   STUFF_DATA | TAG_UINT8, INJECT8(ITEM_TAG_EVENT_CODE_FRME),                                   // Event code
   STUFF_DATA | TAG_OCTET_STRING, 5,ITEM_TAG_ACTIVE_FIRMWARE, 0, 0, 0, 0,						// Firmware version
   STUFF_DATA | TAG_OCTET_STRING, 5,ITEM_TAG_FORMER_FIRMWARE, 0, 0, 0, 0	   	   	            // Former firmware version
};

/*
 * Capture objects for firmware event log profile
 */
const uint8_t firmware_event_log_objects[] =
{
    INJECT16(0x8000 | (4*18+1)),
         4,
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_CLOCK),
                TAG_OCTET_STRING, 6, OBIS_GROUP_A_ABSTRACT_OBJECTS, 0, 1, 0, 0, 255, // Date & Time
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA),  // event code
                TAG_OCTET_STRING, 6, 0, 0, 96, 11, 4, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA), // active firmware
                TAG_OCTET_STRING, 6, 1, 0, 0, 2, 0, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0),
            TAG_STRUCTURE, 4,
                TAG_UINT16, INJECT16(CLASS_ID_DATA), // former firmware
                TAG_OCTET_STRING, 6, 0, 0, 96, 1, 6, 255,
                TAG_INT8, 2,
                TAG_UINT16, INJECT16(0)
};
uint8_t find_num_firmware_event_log__entries_between(const sSA_Range *startRange,const sSA_Range *endRange,
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
     const uint32_t add_start = FIRMWARE_LOG_ADDRESS_START;
     //confirm this with the number of entries we have
     if(last_firmware_event_log_address > add_start)//if we have entries
     {
         if(status.firmware_event_log_overlapped == 1) // if the circular buffer is full
         {
           MAX_Entries = MAX_FIRMWARE_EVENT_LOGS;
         }
         else{
            MAX_Entries = (last_firmware_event_log_address - add_start)/FIRMWARE_EVENT_LOG_TYPE_SIZE;
         }
         firmware_event_log firstEntery,lastEntry;
         uint8_t z = get_firmware_event(&firstEntery,1); //get our first entry
         if(z == 0)
         {
            //error abort
         }
         rtc_t time_first = getTime(&firstEntery.time_stamp);
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
                search_item.start_entry = 0;
                search_item.end_entry = MAX_Entries;
                firmware_event_log l;
                firmware_event_log l2;
                l2.time_stamp = getTimeStamp(temp_sRange.year,temp_sRange.month,temp_sRange.day,temp_sRange.hour,temp_sRange.minute,temp_sRange.second);
                search_item.current_data = &l;
                search_item.PeakItem = &get_firmware_event;
                search_item.Compare = &compare_firmware_event;
                search_item.search_data = &l2;//&temp_sRange;
                uint32_t sa =  search_nearest_log(&search_item);
                // find the actual last entry
                // check if this entry is with in the start and end range
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
          z = get_firmware_event(&lastEntry,MAX_Entries);//get the last entry
          if(z == 0)
          {
                //error abort
          }
          rtc_t time_last = getTime(&lastEntry.time_stamp);
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

uint8_t find_num_total_firmware_event_log_entries(uint16_t *num_entries,uint16_t *start_entry)
{
       //count the total number of entries we have
       /*get the last energy log address
         divide it by the size of energy log struct
          return this number
       */
      *start_entry = 1;
      *num_entries = 0;
      if(status.firmware_event_log_overlapped == 1)
      {
        *num_entries = MAX_FIRMWARE_EVENT_LOGS;
      }
      else
      {
        int32_t x=0;
        const uint32_t Add_Start = FIRMWARE_LOG_ADDRESS_START;
        if(last_firmware_event_log_address > Add_Start)
          x = last_firmware_event_log_address - Add_Start;
        while(x > 0)
        {
            x -= FIRMWARE_EVENT_LOG_TYPE_SIZE;
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
void capture_firmware_event_log(void *data, int direction)
{
    /* Load Template for Load Profile */
   // interpret_template(data, direction, Load_Profile_Buffer_Template, sizeof(Load_Profile_Buffer_Template));
   msg_info.template=firmware_event_log_template;
   msg_info.sz_template=sizeof(firmware_event_log_template);
    if(access_selector==1)
    {
        if(SA_Range[1].Year>4095 || SA_Range[0].Year>4095 )
           msg_info.num_entries=0x0;
        else
           find_num_firmware_event_log__entries_between(&SA_Range[0],&SA_Range[1],&msg_info.start_entry,&msg_info.num_entries);/*msg_info.num_entries=5*/
    }
    else if(access_selector==2)
    {
        find_num_total_firmware_event_log_entries(&msg_info.num_entries,&msg_info.start_entry);
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
        find_num_total_firmware_event_log_entries(&msg_info.num_entries,&msg_info.start_entry);
    }
      msg_info.column_szs = firmware_event_log_column_szs;
}

/*
 * Reset method for firmware event log
 */
void obj_firmware_event_log_reset(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
      uint32_t tmp32 = FIRMWARE_LOG_ADDRESS_START;
      uint8_t temp8 = 7;
      uint8_t tmp1 = 7;
      last_firmware_event_log_address = tmp32;
      write_to_eeprom(&tmp32,&temp8,setLastLogAddress);
      temp8 = 0;
      write_to_eeprom(&temp8,&tmp1,setEventOverlapFlag);
      *response_len= 0;

}

/*
 * Capture function for firmware event log
 */
extern uint8_t active_firmware_version[];
void obj_firmware_event_log_capture(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
	uint8_t tmp = 7;
	firmware_event_log l;
	l.event_code = 8;
	memcpy((uint8_t *)l.active_firmware,(uint8_t *)&active_firmware_version[1],5);
	l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
	write_to_eeprom(&l,&tmp,log_events);
	l.event_code = 9;
	memcpy((uint8_t *)l.active_firmware,(uint8_t *)&active_firmware_version[1],5);
	l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
	write_to_eeprom(&l,&tmp,log_events);
	l.event_code = 10;
	memcpy((uint8_t *)l.active_firmware,(uint8_t *)&active_firmware_version[1],5);
	l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
	l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
	write_to_eeprom(&l,&tmp,log_events);
	*response_len= 0;
}



