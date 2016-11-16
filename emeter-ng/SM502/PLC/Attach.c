


#include "Attach.h"
#include "PLC.h"
#include "MAC_PIB.h"
#include "../Schaduler.h"
#include "Configration.h"
#include <stdio.h>



Attach_conf confirm;
Attach_req req;
extern uint8_t keepAliveTaskNumber;
extern uint16_t keepAliveTimeOutTime_ms;


extern uint8_t LLC_SOURCE_LSAP[]      ;
extern uint8_t LLC_DEF_DEST_LSAP[]    ;
extern uint8_t LLC_DEF_Node_Address[] ;

extern uint8_t plc_connection_status;
extern uint8_t device_identifier[];
extern uint8_t Meter_Sr_No[];
void Attach_Request(Attach_req *req)
{
  
    req->device_Id_Length = 13;
    req->device_identifier = device_identifier;//&(Meter_Sr_No[1]);// =
  
#ifdef CUSTOM_LOGGER
       printf("  -> PLC Sending Attach Request \n");
#endif
       
    plc_connection_status = SENDING_ATTACH_REQUEST;
    PLC_Message message;
    PLC_Header header;
    uint8_t payload[17];
    
    header.Message_Type = PLC_ATTACH;
    header.Message_flags = 0;
    header.Message_flags |= ORG_flag | SEQ_flag(1);
    header.Message_flags &= ~RPY_flag;
    
    header.Payload_Length = 15;
         
    payload[0] = req->device_Id_Length;
    payload[1] = req->device_Id_Length>>8;
    uint8_t i=0;
    for(;i<req->device_Id_Length;i++)
       payload[i+2] = (*req->device_identifier++);
            
    message.header = header;
    message.Payload = payload;  
          
    SendMessage(&message); 
}

void Attach_Confirm(Attach_conf *confirm)
{
  uint8_t x = confirm->status;

#ifdef CUSTOM_LOGGER
printf("-> Attach Response %d\n",x);
#endif 

  if(x != 0) //status not sucess 
  {
//      Attach_req req;
//      req.device_Id_Length = 13;
//      req.device_identifier = device_identifier;
      Attach_Request(&req);
      plc_connection_status = SENDING_ATTACH_REQUEST;
  } 
  else 
  {
    
       LLC_DEF_DEST_LSAP[0] =  (uint8_t)(confirm->base_address);
       LLC_DEF_DEST_LSAP[1] =  (uint8_t)(confirm->base_address>>8);
       LLC_SOURCE_LSAP[0]   =  (uint8_t)(confirm->source_address);
       LLC_SOURCE_LSAP[1]   =  (uint8_t)(confirm->source_address>>8);
       LLC_DEF_Node_Address[0] = LLC_DEF_DEST_LSAP[0];
       LLC_DEF_Node_Address[1] = LLC_DEF_DEST_LSAP[1]; 
       
       //Load_System_Config_Request(PRIME_LLC_Config,'A');
         
       //start the keep alive process   
       CancelTask2( AttachTimedOut );
       CancelTask2( Get_Mac_PIB_TimedOut );
       keepAliveTaskNumber = ScaduleTask(KeepAlive,keepAliveTimeOutTime_ms,KEEP_ALIVE_TASK);//SchaduleTask(KeepAlive,keepAliveTimeOutTime_ms);
       plc_connection_status = ATTACHED;
  }
}
uint16_t attachTimeOutCounter = 0;
void AttachTimedOut()
{
  attachTimeOutCounter ++;
  if(attachTimeOutCounter > 1002)//34 1 minute //1002 it has been more than 30 minutes since the attach requiest 
  {
      Attach_Request(&req);
      plc_connection_status = SENDING_ATTACH_REQUEST;
  }
  else
  {       
      
       ScaduleTask(AttachTimedOut,AttachRequestTimeout,ATTACH_TIMEOUT_TASK);//SchaduleTask(AttachTimedOut,AttachRequestTimeout);   
  }
  
}
