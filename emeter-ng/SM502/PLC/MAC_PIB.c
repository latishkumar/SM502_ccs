

#include "MAC_PIB.h"
#include "PLC.h"
#include <stdio.h>
#include "Attach.h"

#include "Shutdown.h"
#include <stdint.h>
#include <Scheduler.h>

GET_MAC_PID_Replay MAC_PIB_replay;


uint8_t device_identifier[13] = {'D','V','E','0','0','0','0','0','0','0','0','0','0'}; //Meter one

                              
                              
extern Attach_req req;

uint8_t mac_pid_failer_count = 0;
uint8_t keepAliveTaskNumber  = 0;
uint16_t keepAliveTimeOutTime_ms = 65535; //1 minute & 5 sec
uint8_t plc_connection_status = 0;
uint8_t pib_status = 0;
uint8_t plc_request_status = 0;

void KeepAlive()
{
#ifdef CUSTOM_LOGGER
       printf("-> Keep Alive Sent\n");
#endif 
       
   //if there is communication 
      //CancelTask2(Get_Mac_PIB_TimedOut);
      //keepAliveTaskNumber = ScaduleTask(KeepAlive,keepAliveTimeOutTime_ms,KEEP_ALIVE_TASK);//SchaduleTask(KeepAlive,keepAliveTimeOutTime_ms);

   //else
       
       
   if(plc_connection_status!= SENDING_ATTACH_REQUEST)  
   {
        GET_MAC_PIB_Request(0x24);
        keepAliveTaskNumber = schedule_task(Get_Mac_PIB_TimedOut,keepAliveTimeOutTime_ms,MAC_PIB_TIMEOUT_TASK);//SchaduleTask(Get_Mac_PIB_TimedOut,keepAliveTimeOutTime_ms);
   }
   
}
uint8_t jump_cancel_getMACPIB=0;

void GET_MAC_PIB_Request(uint8_t att_id)
{
    PLC_Message message;
    PLC_Header header;
    uint8_t payload[2];
    
    header.Message_Type = PLC_GET_MAC_PIB;
    header.Message_flags = 0;
    header.Message_flags |= ORG_flag | SEQ_flag(1) ;
    header.Message_flags &= ~RPY_flag;
    
    header.Payload_Length = 2;
    
    payload[0] =  att_id;
    payload[1] =  att_id>>8;
      
    message.header = header;
    message.Payload = payload;  
          jump_cancel_getMACPIB = 1;
    SendMessage(&message); 
}

extern volatile uint8_t PLC_INIT_STATE;
void GET_MAC_PIB_rep(GET_MAC_PID_Replay *Replay)
{

  if(Replay->AttribuitID == 0x24)
  {
    if(*(Replay->AttributeValue) == 0) //Disconnected 
    {
#ifdef CUSTOM_LOGGER
printf("-> Keep Alive Response: Meter Disconnected\n");
#endif 

        cancel_task(Get_Mac_PIB_TimedOut);
//       PLC_INIT_step4_to_8();
       
       
       PLC_INIT_STATE=4;
       InitPLC2();
       
       
       
       pib_status = PIB_DISCONNECTED;
    }
    else if(*(Replay->AttributeValue) == 4) //CL_Close
    {
#ifdef CUSTOM_LOGGER
printf("-> Keep Alive Response: CL Closed\n");
#endif       
       cancel_task(Get_Mac_PIB_TimedOut);
       Attach_Request(&req);
       schedule_task(AttachTimedOut,AttachRequestTimeout,ATTACH_TIMEOUT_TASK);//SchaduleTask(AttachTimedOut,AttachRequestTimeout);
       pib_status = PIB_CL_CLOSE;
    }
    else if((*(Replay->AttributeValue) == 1) || (*(Replay->AttributeValue) == 2))
    {
#ifdef CUSTOM_LOGGER
printf("-> Keep Alive Response: OK\n");
#endif 
          cancel_task(Get_Mac_PIB_TimedOut);
          keepAliveTaskNumber = schedule_task(KeepAlive,keepAliveTimeOutTime_ms,KEEP_ALIVE_TASK);//SchaduleTask(KeepAlive,keepAliveTimeOutTime_ms);
          pib_status = PIB_OK;
    }
  }  
}
uint8_t temp_counter =0;
void Get_Mac_PIB_TimedOut()
{
  temp_counter++;
#ifdef CUSTOM_LOGGER
printf("-> Keep Alive Timed Out\n");
#endif 
  if(temp_counter == 3)
  {
     // do this asyncroniously 
     // CancelTask2(Get_Mac_PIB_TimedOut);
     cancel_task(KeepAlive);
     cancel_task(otherPLCRequestsTimeOut);
     temp_counter = 0;
     otherPLCRequestsTimeOut();
     pib_status = PIB_NOK;
  }
  else
  {
     keepAliveTaskNumber = schedule_task(KeepAlive,keepAliveTimeOutTime_ms,KEEP_ALIVE_TASK);//SchaduleTask(KeepAlive,keepAliveTimeOutTime_ms);
//     pib_status = PIB_NOK;
  }
}
