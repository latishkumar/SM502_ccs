/*
 * Configration.c
 *
 *  Created on: Jun 4, 2013
 *      Author: Elec_Lab
 */

#include "Configration.h"
#include "PLC_UART.h"
#include "PLC.h"
#include "../Logger.h"

//extern MeterAddress Meter_serialNumber;
uint8_t EUI[]                  =  {0xB,0,0,0,0,0x11};//Meter One
uint8_t LLC_SOURCE_LSAP[]      = {0,0};
uint8_t LLC_DEF_DEST_LSAP[]    = {0,1};
uint8_t LLC_DEF_Node_Address[] = {0,5};
extern uint8_t Meter_Sr_No[];

void Load_System_Config_Request(int Config_Type,char port)
{
    PLC_Message message;
    PLC_Header header;

    uint8_t Payload[30];
    int z=10;
    long temp =0;
    header.Message_Type = PLC_LOAD_SYSTEM_CONFIG;
    header.Message_flags = 0;
    header.Message_flags |= ORG_flag | SEQ_flag(0) & ~RPY_flag ;

	switch(Config_Type)
	{

	case Port_designation_config:
		Payload[0] = Port_designation_config;
		Payload[1] = Port_designation_config>>8;

		Payload[2] = 1; // configuration length
                Payload[3] = 0;
                if(port == 'A')
                  Payload[4] = Diag_Port_SCI_A | Data_Port_SCI_A;
                else 
                  Payload[4] = Diag_Port_SCI_B | Data_Port_SCI_B;
		  header.Payload_Length = 5;                
		break;
	case System_Configuration_config:

		Payload[0] = System_Configuration_config;
		Payload[1] = 0;
                
		Payload[2] = 0x1A; //configration length
		Payload[3] = 0x00;

		Payload[4] = 0x0A; //serial number length
		Payload[5] = 0x00;
                
		z = 6;
		temp=1;
		for(;z<16;z++,temp++)
		{
		  Payload[z]= Meter_Sr_No[temp];
		}
                
		for(;z<=21;z++)
		Payload[z] = 0;
                
                temp = 1;
		for(;z<=27;z++,temp++)
			Payload[z] = mac_address[temp];//EUI???????????????????????????????????????????????????????????????????????
			//Payload[z] = EUI[temp];//EUI???????????????????????????????????????????????????????????????????????

		Payload[28] = 0;//0 normal mode 3 device mode p2p
		Payload[29] = 0;// 0 No Atuo Mode //0x20 Auto mode enabled, device connects automatically to the network

		header.Payload_Length = 30;
		break;
        case PHY_Configuration:
                Payload[0] = PHY_Configuration;
                Payload[1] = 0;
                
		Payload[2] = 0x1; //configration length
		Payload[3] = 0x0;
                
                Payload[4] = 0x00;//PHY_0|PRM_0;                
                header.Payload_Length = 5;
                
        break;      
        case PRIME_MAC_Configuration:
                Payload[0] = PRIME_MAC_Configuration;
                Payload[1] = 0;
                
		Payload[2] = 0x8; //configration length
		Payload[3] = 0x0;
                z=4;
                for(;z<=9;z++)
                Payload[z] = 0;
                
                Payload[10] = 0;//MAC security profile 
                Payload[11] = 1;// PAC disabled and ARQ          
                header.Payload_Length = 12;  
                
        break;
        case CL_IPv4_v6_Configuration:
        break;
        case PRIME_LLC_Config:
                Payload[0] = PRIME_LLC_Config;
                Payload[1] = 0;
                
		Payload[2] = 0x6; //configration length
		Payload[3] = 0x0;

		Payload[4]= LLC_SOURCE_LSAP[0];
		Payload[5]= LLC_SOURCE_LSAP[1];

		Payload[6]= LLC_DEF_DEST_LSAP[0];
		Payload[7] =LLC_DEF_DEST_LSAP[1];

		Payload[8]= LLC_DEF_Node_Address[0];
		Payload[9] = LLC_DEF_Node_Address[1];

		header.Payload_Length = 10;
        break;
        
	}
    
	message.header  = header;
	message.Payload = Payload;

	SendMessage(&message); // calculate the payload and header CRC on this method 
}

//
//void Load_System_Config_Request2(int Config_Type)
//{
//    PLC_Message message;
//    PLC_Header header;
//
//    uint8_t Payload[30];
//    int z=10;
//    long temp =0;
//    header.Message_Type = PLC_LOAD_SYSTEM_CONFIG;
//    header.Message_flags = 0;
//    header.Message_flags |= ORG_flag | SEQ_flag(0) & ~RPY_flag ;
//
//	switch(Config_Type)
//	{
//
//	case Port_designation_config:
//		Payload[0] = Port_designation_config;
//		Payload[1] = Port_designation_config>>8;
//
//		Payload[2] = 1; // configuration length
//                Payload[3] = 0;
//
//                Payload[4] = Diag_Port_SCI_B | Data_Port_SCI_B;
//		header.Payload_Length = 5;
//                
//		break;
//	case System_Configuration_config:
//
//		Payload[0] = System_Configuration_config;
//                Payload[1] = 0;
//                
//		Payload[2] = 0x1A; //configration length
//		Payload[3] = 0x00;
//
//                Payload[4] = 0x04; //serial number length 
//                Payload[5] = 0x00;
//                
//                temp = Meter_serialNumber.Address;
//		Payload[6] =temp;
//                temp>>=8;
//		Payload[7] =temp;
//                temp>>=8;
//		Payload[8] =temp;
//                temp>>=8;
//		Payload[9] =temp;
//
//                
//		for(;z<=21;z++)
//		Payload[z] = 0;
//                
//                temp = 0;
//		for(;z<=27;z++,temp++)
//                Payload[z] = EUI[temp];//EUI???????????????????????????????????????????????????????????????????????
//
//		Payload[28] = 0;//0 normal mode 3 device mode p2p
//
//		Payload[29] = 0;// 0 No Atuo Mode //0x32 Auto mode enabled, device connects automatically to the network
//
//		header.Payload_Length = 30;
//		break;
//        case PHY_Configuration:
//                Payload[0] = PHY_Configuration;
//                Payload[1] = 0;
//                
//		Payload[2] = 0x1; //configration length
//		Payload[3] = 0x0;
//                
//                Payload[4] = 0x00;//PHY_0|PRM_0;                
//                header.Payload_Length = 5;
//                
//        break;      
//        case PRIME_MAC_Configuration:
//                Payload[0] = PRIME_MAC_Configuration;
//                Payload[1] = 0;
//                
//		Payload[2] = 0x8; //configration length
//		Payload[3] = 0x0;
//                z=4;
//                for(;z<=9;z++)
//                Payload[z] = 0;
//                
//                Payload[10] = 0;//MAC security profile 
//                Payload[11] = 1;// PAC disabled and ARQ          
//                header.Payload_Length = 12;  
//                
//        break;
//        case CL_IPv4_v6_Configuration:
//        break;
//        case PRIME_LLC_Config:
//                Payload[0] = PRIME_LLC_Config;
//                Payload[1] = 0;
//                
//		Payload[2] = 0x6; //configration length
//		Payload[3] = 0x0;
//                         
//                  
//                Payload[4]= LLC_SOURCE_LSAP[0];
//                Payload[5]= LLC_SOURCE_LSAP[1];
//          
//                Payload[6]= LLC_DEF_DEST_LSAP[0];
//                Payload[7] =LLC_DEF_DEST_LSAP[1];
//          
//                Payload[8]= LLC_DEF_Node_Address[0];
//                Payload[9] = LLC_DEF_Node_Address[1];
//          
//                header.Payload_Length = 10;          
//        break;
//        
//	}
//    
//	message.header  = header;
//        message.Payload = Payload;
//
//	SendMessage(&message); // calculate the payload and header CRC on this method 
//}
