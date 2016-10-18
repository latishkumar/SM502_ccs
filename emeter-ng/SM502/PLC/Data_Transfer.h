
#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include <stdint.h>

/**
* Read TI host message specification
*/

//#ifdef DEVICE_MODE_LLC
//
typedef struct 
{
  uint16_t Mode;
  uint8_t Dest_LSAP;
  uint8_t Sour_LSAP;
  uint16_t Des_Address;  //12bit
  //long reserverd
  union{
    uint8_t *data;
    int status;
  };
}DataTX_Request_Confirm;

typedef struct 
{
  uint16_t Mode;
  uint8_t Dest_LSAP;
  uint8_t Sour_LSAP;
  uint16_t Des_Address;  //12bit
  //long reserverd
  uint16_t Sour_Add;
  //long reserved
  uint8_t *data;
}DataTX_Indication;

enum DataTX_Modes{
   ModeDataTX_Request = 0x01,
   ModeDataTX_Confirm = 0x02,
   ModeDataTX_Indication = 0x03
};
//
//#endif 

/***************************************/
/*  Methods for PLC_DATA_TRANSFER   MAC Mode   */
/***************************************/

/*
 * Device mode MAC  from host to PLC
 * ConnectionHandel:- The connection handle returned from the CONNECT message.
 * dataPayload:- the Transfer data
 * length :- the data length
 */
//void DataTransferRequest(int ConnectionHandel,int *dataPayload,int length);  mac mode 
void DataTransferRequest(uint8_t *dataPayload,int length); //LLC mode
//int connection_handel= *RecivedMessage.Payload;
//             RecivedMessage.Payload++;
//             connection_handel |=(*RecivedMessage.Payload)<<8;
//             DataTransferRequest(connection_handel,RecivedMessage.Payload,RecivedMessage.header.Payload_Length-2);
/*
 * status: 16 bit number indicating the status of the data transmission.
 *  	   This is of Type "enum MessageStatusCodes"
 */
void DataTransferConfirm(DataTX_Request_Confirm *confirm);
void dataTransferTimedout();
/*
 *message from PLC to host
 */
//void DataTransferIndication(int ConnectionHandel,uint8_t *dataPayload,int length );

void DataTransferIndication(DataTX_Indication *indication,int datalength );


#endif
