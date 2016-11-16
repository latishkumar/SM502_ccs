

#include "Shutdown.h"
#include "PLC.h"


void Shutdown_Request(uint8_t type)
{
    PLC_Message message;
    PLC_Header header;

    uint8_t Payload[2];
    
    
    header.Message_Type = PLC_SHUTDOWN;
    header.Message_flags = 0;
    header.Message_flags |= ORG_flag | SEQ_flag(0) & ~RPY_flag ;
    header.Payload_Length = 2;
    

    Payload[0] = type;
    Payload[1] = type>>8;
    
    message.header = header;
    message.Payload = Payload;  
    
       
    SendMessage(&message);     
}
