/*
 * billing_periods_and_profiles.c
 *
 *  Created on: May 15, 2017
 *      Author: abi
 */

#include "headers.h"
#include "billing_periods_and_profiles.h"
/*
 * type for Active end of billing period 1 object
 * fixed time, wildcard in date
 */
const uint8_t billing_script_type = 1;

/*
 * script for Predefined Scripts -MDI reset end of billing period
 *
 */
const uint8_t billing_script[] =
{
    1*26 + 1,
        1,
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1), //Script ID - 1
                TAG_ARRAY, 1,
                    TAG_STRUCTURE, 5,   // Invoke Capture Method of Billing Period Profile
                        TAG_ENUM, 2,    // Execute specific method
                        TAG_UINT16, INJECT16(CLASS_ID_PROFILE_GENERIC),
                        TAG_OCTET_STRING, 6, 1, 0, 98, 1, 0, 255,
                        TAG_INT8, 1,
                        TAG_INT8, 0
};
const uint16_t billing_script_execution_time_template_column_szs[] = {16,19,24,29,34,39,44,49,51,56,61};
uint8_t billing_script_execution_time_template[] =
{
            TAG_STRUCTURE, 2,
                TAG_OCTET_STRING, 4, ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME, 0, 0, 0,              // Time-Midnight        //8,9
                TAG_OCTET_STRING, 5, ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY, 0, 0, 0, 0,    // Last day of Month   //17
};
/*
 * executed_script
 * for billing period reset/MDI reset/end of billing period
 */
const uint8_t executed_billing_script[] =
{
    11 + 1,
        2,
            TAG_OCTET_STRING, 6, 0, 0, 10, 0, 1, 255,
            TAG_UINT16, INJECT16(1),
};
/*
 * The activation of this scripts is performed by calling the execute ()
 * method to the script identifier of the corresponding script object.
 * Arguments:
 *  1:Monthly Billing Reset Contract 1
 *  2:Monthly Billing Reset Contract 2
 *  3:Monthly Billing Reset Contract 3
 *  7: Monthly Billing Reset all contracts
 */
void billing_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{

}

void access_billing_script_execution_time(void *data, int direction)
{
 //   interpret_template(data, direction, Day_Profile_Table_Dummy, sizeof(Day_Profile_Table_Dummy));
  if (direction == ATTR_READ || direction ==ATTR_WRITE){
    msg_info.template=billing_script_execution_time_template;
    msg_info.sz_template=sizeof(billing_script_execution_time_template);
    msg_info.num_entries=12;
    msg_info.start_entry=1;
    msg_info.column_szs=billing_script_execution_time_template_column_szs;
  }
  else
  {
   __asm__ __volatile__ (" NOP");
  }

}
