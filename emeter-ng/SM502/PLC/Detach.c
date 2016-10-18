

#include "PLC.h"
#include "Detach.h"

Detach_conf de_confirm;
void Detach_Request(uint16_t des_address)
{
    PLC_Message message;
    PLC_Header header;
    uint8_t payload[2];
    
    header.Message_Type = PLC_DETACH;
    header.Message_flags = 0;
    header.Message_flags |= ORG_flag | SEQ_flag(1) ;
    header.Message_flags &= ~RPY_flag;
    
    header.Payload_Length = 12;
    
     
     payload[0] = des_address;
     payload[1] = des_address>>8;

    message.header = header;
    message.Payload = payload;  
          
    SendMessage(&message); 
  
  
}
void Detach_Confirm(Detach_conf *confirm)
{
  
}