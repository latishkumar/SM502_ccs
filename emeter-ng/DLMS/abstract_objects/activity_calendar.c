/*
 * activity _calendar.c
 *
 *  Created on: May 12, 2017
 *      Author: abi
 */
//#include "headers.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "cosem.h"
#include "msgs.h"
#include "obis.h"
#include "Logger.h"
#include "activity_calendar.h"
const uint8_t active_calendar_name[] =  // calendar_name_active
{
    15,'A','C','T','I','V','E',' ', 'C', 'A', 'L', 'E', 'N', 'D', 'A', 'R'
};

const uint8_t passive_calendar_name[] = // calendar_name_passive
{
    16,'P','A','S','S','I','V','E',' ', 'C', 'A', 'L', 'E', 'N', 'D', 'A', 'R'
};

const uint8_t activate_passive_calendar_time[] =  //activate_passive_calendar_time
{
    12, INJECT16(2002), DECEMBER, 4, WEDNESDAY, 10, 6, 11, 0xFF, INJECT16(120), 0x00
};

uint32_t active_demand_control_threshold = 11000; //Active Demand Control Threshold

uint8_t current_active_tariff = 1; // Currently active tariff

const uint8_t season_profile_dummy[] =
{
    2*32 + 1,
        1,
            TAG_STRUCTURE, 3,
                TAG_OCTET_STRING, 8, 'A', 'L', 'L', ' ', 'Y', 'E','A','R',
                TAG_OCTET_STRING, 12, INJECT16(2002), DECEMBER, 4, WEDNESDAY, 10, 6, 11, 0xFF, INJECT16(120), 0x00,
                TAG_OCTET_STRING, 8, 'A', 'L', 'L', ' ', 'Y', 'E','A','R',/*week profile name*/

};

const uint8_t week_profile_table_dummy[] =
{
    2*24 + 1,
        1,
            TAG_STRUCTURE, 8,
                TAG_OCTET_STRING, 8, 'A', 'L', 'L', ' ', 'Y', 'E','A','R',
                TAG_UINT8, 1,   /* Monday */
                TAG_UINT8, 1,   /* Tuesday */
                TAG_UINT8, 1,   /* Wednesday */
                TAG_UINT8, 1,   /* Thursday */
                TAG_UINT8, 1,   /* Friday */
                TAG_UINT8, 2,   /* Saturday */
                TAG_UINT8, 2,   /* Sunday */
 };
const uint16_t day_profile_table_column_Szs[] = {16,19,24,29,34,39,44,49,51,56,61};
const uint8_t day_profile_table_template[] =
{
            STUFF_DATA | TAG_STRUCTURE, 2,
                STUFF_DATA|TAG_UINT8, ITEM_TAG_DAY_ID,                 /* Day-ID -1 */
                TAG_ARRAY, 4,
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_START_TIME_1, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_SCRIPT_ID1),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_START_TIME_2, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_SCRIPT_ID2),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_START_TIME_3, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_SCRIPT_ID3),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_START_TIME_4, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_SCRIPT_ID4),

};

const uint8_t passive_day_profile_table_template[] =
{
            STUFF_DATA | TAG_STRUCTURE, 2,
                STUFF_DATA|TAG_UINT8, ITEM_TAG_DAY_ID_PASSIVE,                 /* Day-ID -1 */
                TAG_ARRAY, 4,
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_1, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID1),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_2, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID2),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_3, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID3),
                      TAG_STRUCTURE, 3,
                              STUFF_DATA|TAG_OCTET_STRING, 4, ITEM_TAG_DAY_PROFILE_PASSIVE_START_TIME_4, 0, 0, 0,
                              TAG_OCTET_STRING, 6, 6, 0, 0, 10, 0, 100,
                              STUFF_DATA|TAG_UINT16,INJECT16(ITEM_TAG_DAY_PROFILE_PASSIVE_SCRIPT_ID4),

};
const uint16_t special_days_column_size[] = {5,12,14};
uint8_t special_days_list_template[] =
{
            TAG_STRUCTURE, 3,
                STUFF_DATA|TAG_UINT16, INJECT16(ITEM_TAG_SPECIAL_DAY_INDEX),
                STUFF_DATA|TAG_OCTET_STRING,5,ITEM_TAG_SPECIAL_DAY_DATE,0,0,0,0,
                STUFF_DATA|TAG_UINT8,  ITEM_TAG_SPECIAL_DAY_DAY_ID,
};


/*
 * season_profile_active
 */
void access_active_season_profile(void *data, int direction)
{
    /********* This is a dummy function ******************/
    if (direction != ATTR_READ)
        return;
    memcpy(data, season_profile_dummy, sizeof(season_profile_dummy));
    //interpret_template(data, direction, Season_Profile_Dummy, sizeof(Season_Profile_Dummy));
}

/*
 * week_profile_table_active
 */
void access_active_week_profile_table(void *data, int direction)
{
    /********** This is a dummy function *****************/
    if (direction != ATTR_READ)
        return;
    memcpy(data, week_profile_table_dummy, sizeof(week_profile_table_dummy));
    //interpret_template(data, direction, Week_Profile_Table_Dummy, sizeof(Week_Profile_Table_Dummy));
}

/*
 * day_profile_table_active
 */
void access_active_day_profile_table(void *data, int direction)
{
 //   interpret_template(data, direction, Day_Profile_Table_Dummy, sizeof(Day_Profile_Table_Dummy));
  if (direction == ATTR_READ || direction ==ATTR_WRITE){
    msg_info.template=day_profile_table_template;
    msg_info.sz_template=sizeof(day_profile_table_template);
    msg_info.num_entries=3; //we only have three day profiles
    msg_info.start_entry=1;
    msg_info.column_szs=NULL;
  }
  else
  {
   __asm__ __volatile__ (" NOP");
  }

}

/*
 * season_profile_passive
 */
void access_passive_season_profile(void *data, int direction)
{
    /********* This is a dummy function ******************/
    if (direction != ATTR_READ)
        return;
    memcpy(data, season_profile_dummy, sizeof(season_profile_dummy));
    //interpret_template(data, direction, Season_Profile_Dummy, sizeof(Season_Profile_Dummy));
}

/*
 * week_profile_table_passive
 */
void access_passive_week_profile_table(void *data, int direction)
{
    /********** This is a dummy function *****************/
    if (direction != ATTR_READ)
        return;
    memcpy(data, week_profile_table_dummy, sizeof(week_profile_table_dummy));
    //interpret_template(data, direction, Week_Profile_Table_Dummy, sizeof(Week_Profile_Table_Dummy));
}

/*
 * day_profile_table_passive
 */
void access_passive_day_profile_table(void *data, int direction)
{
 //   interpret_template(data, direction, Day_Profile_Table_Dummy, sizeof(Day_Profile_Table_Dummy));
  if (direction == ATTR_READ || direction ==ATTR_WRITE){
    msg_info.template=passive_day_profile_table_template;
    msg_info.sz_template=sizeof(passive_day_profile_table_template);
    msg_info.num_entries=3; //we only have three day profiles
    msg_info.start_entry=1;
    msg_info.column_szs=NULL;
  }
  else
  {
   __asm__ __volatile__ (" NOP");
  }

}

/*
 * Callback function for setting passive calendar activation time
 */
void set_get_passive_calendar_activation_time(void *data,int direction)//TODO. activePassiveCalenderTime is not initialized
{
  uint8_t *datap = data;

  if(direction == ATTR_READ)
  {
    activePassiveCalenderTime.year = 2015;
    activePassiveCalenderTime.month = 5;
    activePassiveCalenderTime.hour = 5;
    activePassiveCalenderTime.minute = 5;
    activePassiveCalenderTime.second = 5;

    uint8_t data2[] = {12,INJECT16(activePassiveCalenderTime.year),activePassiveCalenderTime.month,
                          activePassiveCalenderTime.day,get_weekday(&activePassiveCalenderTime),
                          activePassiveCalenderTime.hour,activePassiveCalenderTime.minute,activePassiveCalenderTime.second,0xff,INJECT16(120), 0x00};

    memcpy(data,data2,13);
  }
  else if(direction == ATTR_WRITE)
  {

      datap++; //length
      activePassiveCalenderTime.year = *datap++;
      activePassiveCalenderTime.year |= (*datap++<<8);

      activePassiveCalenderTime.month =  *datap++;
      activePassiveCalenderTime.day = *datap++;
      datap++;//day of the week
      activePassiveCalenderTime.hour = *datap++;
      activePassiveCalenderTime.minute = *datap++;
      activePassiveCalenderTime.second = *datap++;

  }
}

/*
 * method function for activity calendar object
 */
void activate_passive_calendar(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{

}

/*
 * Callback function for Special Days Table object
 */
void get_special_days_list(void *data,int direction)
{
   //memcpy(data, Tariff_Scripts_Dummy, sizeof(Tariff_Scripts_Dummy));
    //   interpret_template(data, direction, Day_Profile_Table_Dummy, sizeof(Day_Profile_Table_Dummy));
    if (direction == ATTR_READ || direction == ATTR_WRITE){
        msg_info.template=special_days_list_template;
        msg_info.sz_template=sizeof(special_days_list_template);
        msg_info.num_entries = 24; // we only have 24 holidays
        msg_info.start_entry = 0;
        msg_info.column_szs = special_days_column_size;
    }
    else
    {
      __asm__ __volatile__ (" NOP");
    }
}
/*
 * Method function for Special Days Table object
 * to insert special days
 */
void insert_special_day(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
/*
 * Method function for Special Days Table object
 * to delete special days
 */
void delete_special_day(uint8_t *data, uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
}
