/*
 * association_ln.h
 *
 *  Created on: May 12, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_ASSOCIATION_LN_H_
#define DLMS_ABSTRACT_OBJECTS_ASSOCIATION_LN_H_

/* SAP_assignment_list
 * list of logical devices 1 logical device (management logical device)
 * with associations:
 */
extern const uint8_t sap_list[];

/*associated_partners_ids
 * lowest security(NS)
 * Low security (LS)
 * High security (HS)
 */
extern const uint8_t partner_HS[];
extern const uint8_t partner_LS[];
extern const uint8_t partner_NS[];
/*
 * application context name
 */
extern const uint8_t application_context_name_logical_names[];
/*
 * authentication mechanism name
 */
extern const uint8_t auth_mech_name_NS[];
extern const uint8_t auth_mech_name_LS[];
extern const uint8_t auth_mech_name_HS[];
/*association_status
 * type-enum
 */
extern uint16_t uint8val;
/*unique identification of thelogical device
 * type-octet-string[16]
 */
extern uint8_t DeviceIdentifier[];
extern uint8_t device_identifier[]; //TODO Only use one device identifier
extern Password_128 Passwords[4];

void authentication_secret_LS(void *data, int direction);
void get_set_device_identifier(void *data,int data_direction);
void change_hls_secret_NS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void change_hls_secret_HS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void reply_to_hls_auth(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void change_hls_secret_LS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);
void add_object(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_lena);
void remove_object(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len);





#endif /* DLMS_ABSTRACT_OBJECTS_ASSOCIATION_LN_H_ */
