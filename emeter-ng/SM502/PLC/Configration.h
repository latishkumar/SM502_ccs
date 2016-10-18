/*
 * Configration.h
 *
 *  Created on: Jun 4, 2013
 *      Author: Elec_Lab
 */

#ifndef CONFIGRATION_H_
#define CONFIGRATION_H_
/**
* Read TI host message specification
*/

/***************************************/
/*  Methods for LOAD_SYSTEM_CONFIG     */
/***************************************/

enum Configration_Type
{
	Port_designation_config = 0x01,
	System_Configuration_config = 0x03,
        PHY_Configuration = 0x04,
        PRIME_MAC_Configuration = 0x05,
        CL_IPv4_v6_Configuration=0x06,
        PRIME_LLC_Config = 0x07,
	G3_configuration_config = 0x08,
        Flex_LITE_Band_Config = 0x09  
};

typedef struct
{
	int Config_Type;
	int Config_lenght;
	int *Configuration;
}System_Config_t;

#define Diag_Port_SCI_A 0
#define Diag_Port_SCI_B 4

#define Data_Port_SCI_A 0
#define Data_Port_SCI_B 1

void Load_System_Config_Request(int configType,char port);
//void Load_System_Config_Request2(int Config_Type);


#endif /* CONFIGRATION_H_ */
