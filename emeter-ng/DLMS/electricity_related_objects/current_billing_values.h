/*
 * current_billing_values.h
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */

#ifndef DLMS_ELECTRICITY_RELATED_OBJECTS_CURRENT_BILLING_VALUES_H_
#define DLMS_ELECTRICITY_RELATED_OBJECTS_CURRENT_BILLING_VALUES_H_

extern const uint32_t billing_profile_entries_in_use;

extern const uint32_t billing_profile_entries;

extern const uint32_t billing_profile_capture_period;

extern const uint8_t billing_profile_sort_method;

extern const uint16_t billing_profile_column_szs[];

extern const uint8_t billing_profile_puffer_template[];

extern const uint8_t billing_profile_capture_objects[];

void capture_billing_profile_data(void *data, int direction);




#endif /* DLMS_ELECTRICITY_RELATED_OBJECTS_CURRENT_BILLING_VALUES_H_ */
