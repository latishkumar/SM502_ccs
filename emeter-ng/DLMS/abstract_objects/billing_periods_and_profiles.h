/*
 * billing_periods_and_profiles.h
 *
 *  Created on: May 15, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_BILLING_PERIODS_AND_PROFILES_H_
#define DLMS_ABSTRACT_OBJECTS_BILLING_PERIODS_AND_PROFILES_H_
extern const uint8_t billing_script_type;
extern const uint8_t executed_billing_script[];
extern const uint8_t billing_script[] ;

void billing_script_execute(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);

void access_billing_script_execution_time(void *data, int direction);





#endif /* DLMS_ABSTRACT_OBJECTS_BILLING_PERIODS_AND_PROFILES_H_ */
