/*
 * PLC.h
 *
 *  Created on: Jun 3, 2013
 *      Author: Donayam
 */

#ifndef PLC_H_
#define PLC_H_

#include<stdint.h>

/**
* Read TI host message specification
*/


#define PLC_INIT_MAX_RETRAY 5   
   
#undef _USE_G3_
#define _PRIME_IPV4_

#define ORG_BIT 0x80
#define RPY_BIT 0x40  
   
enum plc_status_enum{
  
   DISCONNECTED=1,
   SENDING_ATTACH_REQUEST,
   ATTACHED,
   ACTIVELY_BEING_MONITORE,
   REQUIEST_STATUS_NOK,
   REQUIEST_STATUS_OK
   
};

#define AttachRequestTimeout 1800 //msec   
#define DataTransferRequestTimeout 60000 //msec sec
#define OtherPLCTimeOut  5000 //msec    
   
   
static const unsigned short crctable[256] = {
	0x0000,	0x1021,	0x2042,	0x3063,	0x4084,	0x50a5,	0x60c6,	0x70e7,
	0x8108,	0x9129,	0xa14a,	0xb16b,	0xc18c,	0xd1ad,	0xe1ce,	0xf1ef,
	0x1231,	0x0210,	0x3273,	0x2252,	0x52b5,	0x4294,	0x72f7,	0x62d6,
	0x9339,	0x8318,	0xb37b,	0xa35a,	0xd3bd,	0xc39c,	0xf3ff,	0xe3de,
	0x2462,	0x3443,	0x0420,	0x1401,	0x64e6,	0x74c7,	0x44a4,	0x5485,
	0xa56a,	0xb54b,	0x8528,	0x9509,	0xe5ee,	0xf5cf,	0xc5ac,	0xd58d,
	0x3653,	0x2672,	0x1611,	0x0630,	0x76d7,	0x66f6,	0x5695,	0x46b4,
	0xb75b,	0xa77a,	0x9719,	0x8738,	0xf7df,	0xe7fe,	0xd79d,	0xc7bc,
	0x48c4,	0x58e5,	0x6886,	0x78a7,	0x0840,	0x1861,	0x2802,	0x3823,
	0xc9cc,	0xd9ed,	0xe98e,	0xf9af,	0x8948,	0x9969,	0xa90a,	0xb92b,
	0x5af5,	0x4ad4,	0x7ab7,	0x6a96,	0x1a71,	0x0a50,	0x3a33,	0x2a12,
	0xdbfd,	0xcbdc,	0xfbbf,	0xeb9e,	0x9b79,	0x8b58,	0xbb3b,	0xab1a,
	0x6ca6,	0x7c87,	0x4ce4,	0x5cc5,	0x2c22,	0x3c03,	0x0c60,	0x1c41,
	0xedae,	0xfd8f,	0xcdec,	0xddcd,	0xad2a,	0xbd0b,	0x8d68,	0x9d49,
	0x7e97,	0x6eb6,	0x5ed5,	0x4ef4,	0x3e13,	0x2e32,	0x1e51,	0x0e70,
	0xff9f,	0xefbe,	0xdfdd,	0xcffc,	0xbf1b,	0xaf3a,	0x9f59,	0x8f78,
	0x9188,	0x81a9,	0xb1ca,	0xa1eb,	0xd10c,	0xc12d,	0xf14e,	0xe16f,
	0x1080,	0x00a1,	0x30c2,	0x20e3,	0x5004,	0x4025,	0x7046,	0x6067,
	0x83b9,	0x9398,	0xa3fb,	0xb3da,	0xc33d,	0xd31c,	0xe37f,	0xf35e,
	0x02b1,	0x1290,	0x22f3,	0x32d2,	0x4235,	0x5214,	0x6277,	0x7256,
	0xb5ea,	0xa5cb,	0x95a8,	0x8589,	0xf56e,	0xe54f,	0xd52c,	0xc50d,
	0x34e2,	0x24c3,	0x14a0,	0x0481,	0x7466,	0x6447,	0x5424,	0x4405,
	0xa7db,	0xb7fa,	0x8799,	0x97b8,	0xe75f,	0xf77e,	0xc71d,	0xd73c,
	0x26d3,	0x36f2,	0x0691,	0x16b0,	0x6657,	0x7676,	0x4615,	0x5634,
	0xd94c,	0xc96d,	0xf90e,	0xe92f,	0x99c8,	0x89e9,	0xb98a,	0xa9ab,
	0x5844,	0x4865,	0x7806,	0x6827,	0x18c0,	0x08e1,	0x3882,	0x28a3,
	0xcb7d,	0xdb5c,	0xeb3f,	0xfb1e,	0x8bf9,	0x9bd8,	0xabbb,	0xbb9a,
	0x4a75,	0x5a54,	0x6a37,	0x7a16,	0x0af1,	0x1ad0,	0x2ab3,	0x3a92,
	0xfd2e,	0xed0f,	0xdd6c,	0xcd4d,	0xbdaa,	0xad8b,	0x9de8,	0x8dc9,
	0x7c26,	0x6c07,	0x5c64,	0x4c45,	0x3ca2,	0x2c83,	0x1ce0,	0x0cc1,
	0xef1f,	0xff3e,	0xcf5d,	0xdf7c,	0xaf9b,	0xbfba,	0x8fd9,	0x9ff8,
	0x6e17,	0x7e36,	0x4e55,	0x5e74,	0x2e93,	0x3eb2,	0x0ed1,	0x1ef0
};
void CRC16_UpdateChecksum(unsigned short *pcrcvalue, const void *data, int length);

unsigned short CRC16_BlockChecksum(const void *data, int length);

extern uint8_t plc_status ;
extern uint8_t plc_state;
extern volatile uint8_t plc_counter;
/*
 * Message Type
Message Type identifier.

ORG:Message origination.  A value of 1 represents the message originated from the
host, and value of 0 represents the message originated from the receiver.

RPY: Acknowledgement reply bit.  If this bit is set, the receiver shall acknowledge
receipt of the message by echoing back the message header, with either new
CRCs supplied or the CRCs set to 0, with the RPY bit set, and no additional
payload.  The RPY is only used in DATA_TRANSFER.indicate.

RESV: Reserved bits, ignored.

SEQ :- Sequence Number.  This is the 4 bit message sequence number, starting with
0 and incrementing by 1 for each message sequence.  The Sequence Number
should be set to 0 unless specified in the message definition.

Message Payload Length:- A 16 bit number representing the length of the message payload plus the
length of the Message Header CRC16 and Message Payload CRC16 (a
message with no payload will have a length of 4). The maximum message
payload length supported is 1500Bytes.

Message Header CRC16:-  The CRC16 of the message header, from the Message Type to the Message
Payload Length, but not including the Message Header CRC or the Message
Payload CRC.A value of 0 denotes no CRC16 present.  See CRC16 implementation in the
appendix for CRC details.


Message Payload CRC16:- The CRC16 of the message payload data.
A value of 0 denotes no CRC16 present.  See CRC16 implementation in the
appendix for CRC details.
 */

#define ORG_flag 0x80
#define RPY_flag 0x40
#define SEQ_flag(x)  x

typedef struct
{

   uint8_t Message_Type;//8 bit number indicating message type
   uint8_t Message_flags;
//    		bit1_t ORG;// message origination  0 from the receiver, Message is received
//     	 	 	 	 	 	 	 	 //  1 from host, the message is to be transmitted through PLC
//
//    		bit1_t RPY;// ACK Replay bit
//
//    		uint4_t SEQ;// Sequence number

    uint16_t Payload_Length; // 16 bit Payload length

    uint16_t Message_HeaderCRC; // 16 bit header CRC
    uint16_t Message_PayloadCRC; //16 bit Payload CRC

}PLC_Header;

typedef struct
{
	PLC_Header header;
	uint8_t *Payload;
        
}PLC_Message;

//API List  for Message_Type
#define PLC_DATA_TRANSFER 0x00   //Application specific data messages
#define PLC_GET_SYSTEM_INFO 0x01 //get system info (HW/SW)
#define PLC_GET_PHY_PIB 0x02  //get PHY PIB attributes from PLC device
#define PLC_GET_MAC_PIB 0x03  //Get MAC PIB attributes from PLC device
#define PLC_SET_INFO 0x04 //Set certain configuration to PLC device
#define PLC_SHUTDOWN 0x05 //reset PLC device
#define PLC_SETUP_ALRAM 0x06 //setup Alarm notifications
#define PLC_ALARM 0x07  //Alarm Notification

#ifdef _USE_G3_
	#define PLC_NETWORK_START 0x08 //Initiate network registration process
#else
	#define PLC_NW_REGISTER 0x08 //Initiate network registration process
	#define PLC_NW_UNREGISTER 0x09 //Initiate network un-registration process
#endif

#define PLC_CONNECT 0x0A //MAC Initiate connection setup process
#define PLC_DISCONNECT 0x0B //MAC Initiate connection teardown process

#define PLC_LOAD_SYSTEM_CONFIG 0x0C // Load system configuration data
#define PLC_SET_MAC_PIB 0x0d   //Set MAC PIB attributes from PLC device
#define PLC_CLEAR_PHY_PIB 0x0E //Clear certain PHY PIB attributes.
#define PLC_CLEAR_MAC_PIB 0x0f //Clear certain MAC PIB attributes.
#define PLC_ATTACH 0x10	//PRIME CL-432 Establish Request and Confirm
#define PLC_DETACH 0x11 //PRIME CL-432 Release Request and Confirm

#ifdef _USE_G3_
	#define PLC_DISCOVER 0x12  //Network Discovery
#endif

#ifndef _USE_G3_
#define PLC_FIRMWARE_UPGRADE 0x13 // FW upgrade Process
#endif

#define PLC_GET_INFO 0x14 //Gets miscellaneous PLC data

#ifndef _USE_G3_
        #define PLC_Diagnostic_Messages_start 0x80
        #define PLC_Diagnostic_Messages_end 0xFE
#endif

#define PLC_READ_BLOB 0x96
#define PLC_WRITE_BLOB 0x97

enum MessageStatusCodes
{
    STATUS_OK = 0, //Message request success
    STATUS_INVALID_PARAMETER = 0x02 , // Invalid Message Parameters
    STATUS_FEATURE_UNSUPPORTED = 0x03,//Future is not supported
    STATUS_PAYLOAD_CRC_ERROR = 0x04, //Payload CRC error
    STATUS_NO_EEPROM = 0x05,//No EEPROM on the device
    STATUS_HEADER_CRC_ERROR = 0x06, //Message Header CRC error
    STATUS_INVALID_HANDLE =0x07, //the handle is invalid
    STATUS_INVALID_FORMAT =0x08,//The message format is incorrect
    STATUS_RPY_NOT_BLOCKED =0x0A,//AN RPY unblock msg was recv'd but not currently blocked
    STATUS_INVALID_MESSAGE_LENGTH = 0x0B,//The message lenght is invalid
    STATUS_ERR_NO_HANDLER = 0xF3, //No handler for the message type
    STAUTS_PENDING =0xFC,//The operation is pending
    STATUS_ERR_TIMEOUT = 0xFD,//The operation timed-out
    STATUS_ERR_NO_MEMORY = 0xFE,//out of memory
    STATUS_GENERAL_FAILURE = 0xFF// General failure of error
};

#define STATUS_PHY_ERROR_CODES_START 0x1
#define STATUS_MAC_ERROR_CODES_START 0x2
#define STATUS_CL_ERRIOR_CODES_START 0x3
#define STATUS_ECA_ERROR_CODES_START 0x4




extern void GetSystemInfoRequest();
extern void DataTransferRequest(uint8_t *dataPayload,int length);

/***************************************/
/*  Methods for GET_PHY_PIB      */
/***************************************/

//extern void GET_PHY_PIB_Request(uint8_t att_id);
//extern void GET_PHY_PIB_rep(GET_PHY_PID_Replay *Rreplay);


/***************************************/
/*  Methods for GET_MAC_PIB      */
/***************************************/

//extern void GET_MAC_PIB_Request(uint8_t att_id);
//extern void GET_MAC_PIB_rep(GET_MAC_PID_Replay *Rreplay);




enum PLC_STATE {
  
    IDEAL = 0,  
    WAITTING_REPLAY ,
    SENDING_COMMAND ,    
    INITIALIZING
    
};

extern uint8_t plc_state;
extern volatile uint8_t statusRecived;

//uint8_t wait_Status();
//void PRIME_SystemIntialize();
//void PLC_INIT_step4_to_8();

uint8_t processStatus();
int  SendMessage(PLC_Message *Message);
void ProcessRecivedPLCMessage();
void otherPLCRequestsTimeOut();
void ACK_Message();

void InitPLC();
void InitPLC2();

#ifndef KIT

void ResetPLC_Processor();
void StartPLC_Processor();

#else
#endif 

#endif /* PLC_H_ */
