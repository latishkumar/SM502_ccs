

#ifndef ALARM_H_
#define ALARM_H_

#include<stdint.h>
/**
* Read TI host message specification
*/

/***************************************/
/*  Methods for SETUP_ALARM      */
/***************************************/
enum Alarm_Types{
	Network_Deregistered =0x01,
	Connect_Request  = 0x02,
	Disconnect_Indication = 0x03,
	Network_Registration_Started = 0x04,
	Network_Registration_Complete = 0x05,
	//Connect_Complete =0x06, not available
	G3_Network_Status_Indication =0x07,//G3
	PHY_received_PPDU_header_CRC_fail_detected = 0x40,//G3
	PHY_received_PPDU_header_syntax_error_detected = 0x41,//G3
	Terminal_node_is_promoted_to_switch_node = 0x80,//G3
	Switch_node_is_demoted_to_terminal_node = 0x81,
	CL_Alarm = 0xc0,
	General_Error = 0xff
};

#define ALARM_Clear  0x01
typedef struct
{
    int Alarm_Type;
    int Alarm_Length;

    uint8_t Alarm_value;
}Alarm_Request_t;

void SETUP_ALARM_Request(uint8_t alarm_type,uint8_t value);
void SETUP_ALARM_Reply();




/***************************************/
/*  Methods for ALARM      */  //from PLC
/***************************************/

////use the Alarm_Request structure
typedef struct
{
    int Alarm_Type;
    int Alarm_Length;
    uint8_t *Alarm_Value;
}Alarm_Indication_t;
extern Alarm_Indication_t AlarmIndiation;
//0x0007 – G3 Network Status Indication
enum  G3_Status_Codes
{
	Counter_Error = 0xDB,
	Improper_Key_Type = 0xDC,
	Unsupported_Security = 0xDF,
	Sync_loss = 0xE0,
	Security_Error = 0xE4,
	Frame_Too_Long = 0xE5,
	Conflict_PAN_Id = 0xEE,
	Invalid_Index = 0xF9
};


enum AddressMode
{
	No_Address = 0,
	Short_Address = 2,
	Long_Address = 3
};

typedef struct
{
	uint8_t Security_Level;
	uint8_t Key_Id_Mode;
	int key_Index;
	int Pan_Id;
	int Source_Address_Mode;
	uint8_t Address[8];
}Additional_Info;


typedef struct{
  
  uint8_t ARQ;
  uint8_t MAC_Connection_Type;
  unsigned int CfBytes;
  uint8_t DestinationAddress[6];
  uint8_t *AditionalData;
  
}PRIME_MAC_Connection_DataType_t;

typedef struct {
  unsigned int HCT_Connection_Type;
  unsigned int HCT_Connection_Handle;
  unsigned int Connection_Data_Length;
  union {
    PRIME_MAC_Connection_DataType_t Connection_Data;
    unsigned int Reason;
  };
}ConnectDisconnectRequestAlarm_t;

extern ConnectDisconnectRequestAlarm_t ConnectRequestAlarm;
extern ConnectDisconnectRequestAlarm_t DisconnectRequestAlarm;

void ALARM_Indication(Alarm_Indication_t *AI);
void ProcessAlarmValue(uint8_t alarmType);

#endif 