

#include "PHY_PIB.h"
#include "PLC.h"
#include <stdint.h>

GET_PHY_PID_Replay PHY_PIB_replay;

void GET_PHY_PIB_Request(uint8_t att_id)
{
    PLC_Message message;
    PLC_Header header;
    uint8_t payload[2];
    
    header.Message_Type = PLC_GET_PHY_PIB;
    header.Message_flags = 0;
    header.Message_flags |= ORG_flag | SEQ_flag(1) ;
    header.Message_flags &= ~RPY_flag;
    
    header.Payload_Length = 2;
    
    payload[0] =  att_id;
    payload[1] =  att_id<<8;
      
    message.header = header;
    message.Payload = payload;  
          
    SendMessage(&message); 
}

void GET_PHY_PIB_rep(GET_PHY_PID_Replay *Rreplay)
{
}