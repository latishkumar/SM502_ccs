/*
 * PLC.c
 *
 *  Created on: Jun 3, 2013
 *      Author: Donayam
 */

#include "PLC.h"
#include "Configration.h"
#include "PLC_UART.h"
#include "Data_Transfer.h"  
#include "System_Info.h"
#include "Shutdown.h"
#include "PHY_PIB.h"
#include "MAC_PIB.h"
#include "Alarm.h"
#ifndef ATTACH_H
  #include "Attach.h"
#endif 
#include "Detach.h"
#include "Blob.h"

#include "../Logger.h"
#include "../self_diagnosis.h"
#include "../Schaduler.h"   

#include <stdio.h>
   
#define MAX_PLC_RETRAY 3
   
extern PLC_Message RecivedMessage;
extern System_Info_t system_info;
extern GET_PHY_PID_Replay PHY_PIB_replay;
extern GET_MAC_PID_Replay MAC_PIB_replay;
extern Alarm_Indication_t AlarmIndiation;
extern HardwareStatus hardware_status;

extern Attach_conf confirm;
extern Attach_req req;

extern Detach_conf de_confirm;

uint8_t plc_state = 0;
volatile uint8_t statusRecived = 0;
uint8_t plc_status = 0;
volatile uint8_t plc_counter = 0;//updated in per_second_activity in emeter-main.c 
#define  plc_wait_time  2 //wait for 2 second 

extern uint8_t device_identifier[];

extern uint8_t keepAliveTaskNumber;
extern uint16_t keepAliveTimeOutTime_ms;
extern uint8_t plc_request_status;


uint8_t current_tx_timeout = 0;
uint8_t currentMessageType = 0;
#define maxRetryCount  5;
uint8_t RetryCount = 0;
uint8_t plc_comm_status = 0;

extern Blob_Message_t readBlobMessage;
extern Blob_Message_t writeBlobMessage;

extern uint8_t device_identifier[];
extern uint8_t EUI[];
void PLC_Start_SendBuffer();
/*
* routine that wiaits for responce from PLC processor 
* whould be used only inside the PLC initialization mehod.
*/
uint8_t wait_StatusAsync()
{
      if(plc_counter > plc_wait_time)
      {
           return 0;
      }
      if(statusRecived == 1)
      {
           return 1;
      }
      return 2;//Not yet timeout
}

volatile uint8_t read_blob_flag=0;
volatile uint8_t PLC_INIT_STATE=1;
volatile uint8_t messageSent=0;
volatile uint8_t count=0;
void InitPLC2()
{
  uint8_t temp;
   switch(PLC_INIT_STATE)
   {
     case 1: /* Initialization, Port configuration */

           if(messageSent == 0)
           {
              plc_state = INITIALIZING;
              statusRecived =0;
              
              Load_System_Config_Request(Port_designation_config,'A');
              plc_counter = 0;
              messageSent=1;
           }
           else
           {
             temp = wait_StatusAsync();
             if(temp ==2)
             {
             }
             else if(temp == 0)//Timed out
             {
               messageSent = 0;//resend the message
               count++;
               if(count >=PLC_INIT_MAX_RETRAY)
               {
                 PLC_INIT_STATE++;
                 hardware_status.PLC_Status = 0;
                 count=0;
               }
             }
             else if(temp == 1)
             {
               hardware_status.PLC_Status = 1;
               count=0;
               messageSent = 0;
               PLC_INIT_STATE++;               
             }

           }
       break;
     case 2:   /* Load system configuration */
          if(messageSent == 0)
           {
             statusRecived =0;
             
              Load_System_Config_Request(System_Configuration_config,'A');
              messageSent=1;
              plc_counter = 0;
           }
           else
           {
             temp = wait_StatusAsync();
             if(temp == 2)
             {               
             }
             else if(temp == 0)
             {
               messageSent = 0;//resend the message
               count++;
               if(count >=PLC_INIT_MAX_RETRAY)
               {
                 PLC_INIT_STATE++;
                 hardware_status.PLC_Status = 0;
                 count=0;
               }
             }
             else if(temp == 1)
             {
               hardware_status.PLC_Status = 1;
               count=0;
               messageSent = 0;
               PLC_INIT_STATE++;               
             }
            
           }
       
       break;
     case 3:   /* Load prime MAC configuration */
       
          if(messageSent == 0)
           { 
              statusRecived = 0;
              
              Load_System_Config_Request(PRIME_MAC_Configuration,'A');
             plc_counter = 0;
              messageSent=1;
           }
           else
           {
             temp = wait_StatusAsync();
             if(temp == 2)
             {               
             }
             else if(temp == 0)
             {
               messageSent = 0;//resend the message
               count++;
               if(count >=PLC_INIT_MAX_RETRAY)
               {
                 PLC_INIT_STATE++;
                 hardware_status.PLC_Status = 0;
                 count=0;
               }
             }
             else if(temp == 1)
             {
               hardware_status.PLC_Status = 1;
               count=0;
               messageSent = 0;
               PLC_INIT_STATE++;               
             }

           }
       break;
     case 4:  /* Soft Reset */
          if(messageSent == 0)
           {
             plc_state = INITIALIZING;
             statusRecived = 0;
             
              Shutdown_Request(SOFT_RESET);
              messageSent=1;
              plc_counter = 0;
           }
           else
           { 
             temp = wait_StatusAsync();
             if(temp == 2)
             {               
             }
             else if(temp == 0)
             {
               messageSent = 0;//resend the message
               count++;
               if(count >=PLC_INIT_MAX_RETRAY)
               {
                 PLC_INIT_STATE++;
                 hardware_status.PLC_Status = 0;
                 count=0;
                 
               }
             }
             else if(temp == 1)
             {
               hardware_status.PLC_Status = 1;
               count=0;
               messageSent = 0;
               PLC_INIT_STATE++;               
             }

           }       
       break;
     case 5:  /* waiting for ???? */
         
         statusRecived = 0;
         temp = wait_StatusAsync();//wait for 2 Seconds
         if(temp == 0)
         {
           count=0;
           PLC_INIT_STATE++;
         }
     break;
     case 6: 
          if(messageSent == 0)
           {
             statusRecived = 0;
             
              SETUP_ALARM_Request(Network_Deregistered,0);
              plc_counter = 0;
              messageSent=1;
           }
           else
           {
             temp = wait_StatusAsync();
             if(temp == 2)
             {               
             }             
             else if(temp == 0)
             {
               messageSent = 0;//resend the message
               count++;
               if(count >= PLC_INIT_MAX_RETRAY)
               {
                 PLC_INIT_STATE++;
                 hardware_status.PLC_Status = 0;
                 count=0;
               }
             }
             else if(temp == 1)
             {
               hardware_status.PLC_Status = 1;
               count=0;
               messageSent = 0;
               PLC_INIT_STATE++;               
             }

           }        
       
       break;
     case 7:
          if(messageSent == 0)
           {
               statusRecived =0;
            
               SETUP_ALARM_Request(CL_Alarm,0);
               plc_counter = 0;
              messageSent=1;
           }
           else
           {
             temp = wait_StatusAsync();
             if(temp == 2)
             {               
             }
             else if(temp == 0)
             {
               messageSent = 0;//resend the message
               count++;
               if(count >=PLC_INIT_MAX_RETRAY)
               {
                 PLC_INIT_STATE++;
                 hardware_status.PLC_Status = 0;
                 count=0;
               } 
               
             }
             else if(temp == 1)
             {
               hardware_status.PLC_Status = 1;
               count=0;
               messageSent = 0;
               PLC_INIT_STATE++;               
             }

           }       
       break;
     case 8:
           if(messageSent == 0)
           {
              statusRecived = 0;
              
              GetSystemInfoRequest(); 
              plc_counter = 0;
              messageSent=1;
           }
           else
           {
             temp = wait_StatusAsync();
             if(temp == 2)
             {               
             }             
             else if(temp == 0)
             {
               messageSent = 0;//resend the message
               count++;
               if(count >=PLC_INIT_MAX_RETRAY)
               {
                 PLC_INIT_STATE++;
                 hardware_status.PLC_Status = 0;
                 count=0;
                 plc_counter = 0;
                 statusRecived = 0;
               }
             }
             else if(temp == 1)
             {
               hardware_status.PLC_Status = 1;
               count=0;
               messageSent = 0;
               plc_counter = 0;
               statusRecived = 0;
               PLC_INIT_STATE++;               
             }

           } 
       break;
     case 9:

       
           //PLC self diagnosis result 
           if(hardware_status.PLC_Status == 0)
           {
             AddError(PLC_Error);
           }
       
       
           req.device_Id_Length = 13;
           req.device_identifier = device_identifier;
         
           Attach_Request(&req);//requires 30 Min Timeout 
           statusRecived =0;

           ScaduleTask(AttachTimedOut,AttachRequestTimeout,ATTACH_TIMEOUT_TASK);
           PLC_INIT_STATE++; 
           
       break;
     default:
          plc_state = IDEAL;
       break;
   }
}

void InitPLC()
{
    validation_arg_t noValidation = validation_arg_t_default;
    read_from_eeprom(device_identifier,(uint8_t *)0,getLogicDeviceName,&noValidation);
   // read_from_eeprom(EUI,(uint8_t *)0,getMacAddress,&noValidation);
    read_from_eeprom(&mac_address[1],(uint8_t *)0,getMacAddress,&noValidation);
#ifdef CUSTOM_LOGGER
       printf("  -> Init PLC UART\n");
#endif   
        InitPLCUART();  
        
#ifdef CUSTOM_LOGGER
       printf("  -> Init PLC Modum\n");
#endif        
  
        InitPLC2();
 
   
}



uint8_t processStatus()
{
  return 1;
}

void CRC16_UpdateChecksum(unsigned short *pcrcvalue, const void *data, int length)
{
  unsigned short crc;
  const unsigned char *buf = (const unsigned char *) data;

  crc = *pcrcvalue;
  while (length--)
  {
    crc = (crc << 8) ^ crctable[(crc >> 8) ^ *buf++];
  }
  *pcrcvalue = crc ;//^0xEC06
}

unsigned short CRC16_BlockChecksum(const void *data, int length)
{
  unsigned short crc;

  crc = 0;
  CRC16_UpdateChecksum(&crc, data, length);
  return crc;
}
extern volatile funcPointers tasks[MaxScheduledTasks];
extern uint8_t jump_cancel_getMACPIB;

int SendMessage(PLC_Message *Message)
{
    
     //if there is communication 
  if(tasks[KEEP_ALIVE_TASK].time_in_ms !=0 && jump_cancel_getMACPIB!=1)
  {
      CancelTask2(Get_Mac_PIB_TimedOut);
      keepAliveTaskNumber = ScaduleTask(KeepAlive,keepAliveTimeOutTime_ms,KEEP_ALIVE_TASK);//SchaduleTask(KeepAlive,keepAliveTimeOutTime_ms);
      jump_cancel_getMACPIB = 0;
  }
  
  
  
  
    AddtoTXBuffer(Message->header.Message_Type);
    AddtoTXBuffer(Message->header.Message_flags);

    
    unsigned int x= Message->header.Payload_Length +4; //add the length of the CRC filds
    AddtoTXBuffer(x);
    AddtoTXBuffer(x>>8);

    unsigned char header_array[4];

    header_array[0] = (Message->header.Message_Type);
    header_array[1] = (Message->header.Message_flags);
    
    x= Message->header.Payload_Length +4; //add the length of the CRC filds
    header_array[2] = (x);
    header_array[3] = (x>>8);


    Message->header.Message_HeaderCRC= CRC16_BlockChecksum(header_array, 4);
    Message->header.Message_PayloadCRC = CRC16_BlockChecksum(Message->Payload, Message->header.Payload_Length);
        //header.Message_HeaderCRC = CRC16_BlockChecksum(header_array, 8);
    
    AddtoTXBuffer(Message->header.Message_HeaderCRC);
    AddtoTXBuffer(Message->header.Message_HeaderCRC>>8);

    AddtoTXBuffer(Message->header.Message_PayloadCRC);
    AddtoTXBuffer(Message->header.Message_PayloadCRC>>8);

    //messageLength = 8;

    int i=0;
    for(;i<Message->header.Payload_Length;i++)
    {
    	AddtoTXBuffer(*(Message->Payload));
    	Message->Payload++;
    	//i++;
    }

    if(i%2 != 0)
    	AddtoTXBuffer(0x00); //add padding 

    PLC_Start_SendBuffer();
    
    return 1;  
}




/*
  Method called when a Full Frame has been receved through the PLC 
  The Method checks the message type and processes the message accordingly 
*/
extern uint8_t plc_request_status;
unsigned int HeaderCRC,PayloadCRC;
DataTX_Indication ind;
DataTX_Request_Confirm conf;
int Mode = 0;

void ProcessRecivedPLCMessage()
{
     uint8_t i=0;
     long temp;
     uint16_t status;
     Rx_Done = 1; 
    //check for header CRC
    //chekc for payload CRC 
     uint8_t header_array[4];

    header_array[0] = (RecivedMessage.header.Message_Type);
    header_array[1] = (RecivedMessage.header.Message_flags);
    
    HeaderCRC = RecivedMessage.header.Payload_Length+4; //use the HeaderCRC as a temporary location b/c 
    header_array[2] = (HeaderCRC);                      //it will be replaced on the following statments before it is used again
    header_array[3] = (HeaderCRC>>8);


    HeaderCRC= CRC16_BlockChecksum(header_array, 4);
    PayloadCRC = CRC16_BlockChecksum(RecivedMessage.Payload, RecivedMessage.header.Payload_Length);
        //header.Message_HeaderCRC = CRC16_BlockChecksum(header_array, 8);
    
    if((RecivedMessage.header.Message_HeaderCRC == HeaderCRC || RecivedMessage.header.Message_HeaderCRC== 0) && 
       (RecivedMessage.header.Message_PayloadCRC == PayloadCRC||RecivedMessage.header.Message_PayloadCRC==0))
    {
       //we have good CRC, Notify we have recived message 
       //call the appropriate methods for each case 
      
      if(RecivedMessage.header.Message_Type == PLC_DATA_TRANSFER)
      {
           CancelTask2(dataTransferTimedout);
           plc_request_status = REQUIEST_STATUS_OK;
          //reset dataTransfer time out 
           Mode  = *(RecivedMessage.Payload++);
           Mode |=  (*(RecivedMessage.Payload++))<<8;
           
           if( Mode == ModeDataTX_Confirm)
           {
             conf.Mode = Mode;
             conf.Dest_LSAP = *(RecivedMessage.Payload++);
             conf.Sour_LSAP = *(RecivedMessage.Payload++);
             conf.Des_Address = *(RecivedMessage.Payload++);
             conf.Des_Address |=(*(RecivedMessage.Payload++))<<8;
             for(i=0;i<=3;i++)
                    RecivedMessage.Payload++;
             
             conf.status = *(RecivedMessage.Payload++);
             conf.status|=*(RecivedMessage.Payload++)<<8;
             statusRecived = 1;
             plc_status = conf.status;
             if(conf.status !=0 )
             {
               int y=0;
               y++;
             }
             if(conf.status == 0x4009)
             {

             }
#ifdef MONITOR_HP_COM
             printf("TX Confirm");
#endif 
           }
           else if(Mode == ModeDataTX_Indication)
           {   
             #ifdef MONITOR_HP_COM
              printf("TX Indicat");
             #endif 
                ind.Mode = Mode;
                ind.Dest_LSAP = *(RecivedMessage.Payload++);
                ind.Sour_LSAP = *(RecivedMessage.Payload++);
                ind.Des_Address = *(RecivedMessage.Payload++);
                ind.Des_Address |=  (*(RecivedMessage.Payload++))<<8;
           
                for(i=0;i<=3;i++)
                    RecivedMessage.Payload++;
                ind.Sour_Add = *(RecivedMessage.Payload++);
                ind.Sour_Add |=  (*(RecivedMessage.Payload++))<<8;
                for(i=0;i<=3;i++)
                    RecivedMessage.Payload++;
           
                ind.data = (RecivedMessage.Payload++);
                DataTransferIndication(&ind,RecivedMessage.header.Payload_Length-16);
           }

      }
      else if(RecivedMessage.header.Message_Type == PLC_ATTACH)
      {
             #ifdef MONITOR_HP_COM
              printf("Attach Confirm");
             #endif 
             CancelTask2(AttachTimedOut);
             
             temp = *(RecivedMessage.Payload++);
             confirm.status = temp;
             temp = *(RecivedMessage.Payload++);
             confirm.status |= (temp<<8);
             
             temp = *(RecivedMessage.Payload++);
             confirm.device_id_length = temp;
             temp = *(RecivedMessage.Payload++);
             confirm.device_id_length |= (temp<<8);
             
             confirm.device_identifier = (RecivedMessage.Payload++);
             
             for(temp =0;temp<confirm.device_id_length;temp++)
             {
               RecivedMessage.Payload++;
             }
             temp = *RecivedMessage.Payload++;
             confirm.source_address = temp;
             temp = *RecivedMessage.Payload++;
             confirm.source_address |= (temp<<8);

             temp = *RecivedMessage.Payload++;
             confirm.base_address = temp;
             temp = *RecivedMessage.Payload++;
             confirm.base_address |= (temp<<8);  
             
             statusRecived = 1;
             plc_status = confirm.status;
             Attach_Confirm(&confirm); 
      }
      else 
      {
          CancelTask2(otherPLCRequestsTimeOut);
          
          if(RecivedMessage.header.Message_Type == PLC_GET_SYSTEM_INFO)
          {
             #ifdef MONITOR_HP_COM
              printf("Get SystemInfo Replay");
             #endif 
                       // temp=*(RecivedMessage.Payload++);
            //temp;
            system_info.Firmware_Version[0] = *(RecivedMessage.Payload++);
            system_info.Firmware_Version[1] = *(RecivedMessage.Payload++);
            system_info.Firmware_Version[2] = *(RecivedMessage.Payload++);
            system_info.Firmware_Version[3] = *(RecivedMessage.Payload++);
            
            temp=*(RecivedMessage.Payload++);
            system_info.Device_Serial_Number_Length = temp;//*(RecivedMessage.Payload++)| ((*(RecivedMessage.Payload++))<<8);
            temp=*(RecivedMessage.Payload++);
            system_info.Device_Serial_Number_Length |=temp<<8;
            
             i=0;
            for(;i<=15;i++)   //16 byte device serial number 
            {
              temp =((*(RecivedMessage.Payload++)));
              system_info.Device_Serial_Number[i] =  temp;//*(RecivedMessage.Payload++)|((*(RecivedMessage.Payload++))<<8);     
            }
            
            system_info.Device_Type =  *(RecivedMessage.Payload++);
            system_info.Device_Mode =  *(RecivedMessage.Payload++);
            
            temp =((*(RecivedMessage.Payload++)));
            system_info.HardwareRevision = temp;//*(RecivedMessage.Payload++)|((*(RecivedMessage.Payload++))<<8);
            temp =((*(RecivedMessage.Payload++)));
            system_info.HardwareRevision |=temp<<8;
            
            
            for(i=0;i<=5;i++)
            {
               temp =((*(RecivedMessage.Payload++)));
               system_info.MAC_Address[i] =  temp;//*(RecivedMessage.Payload++)|((*(RecivedMessage.Payload++))<<8);              
            }
            
            system_info.PortAssignments = *(RecivedMessage.Payload++);
            system_info.PHY_Settings = *(RecivedMessage.Payload++);
            
            for(i=0;i<=5;i++) //reserved bits 
            {
              RecivedMessage.Payload++;
            }
            system_info.ARQ_PAC_Flags = *(RecivedMessage.Payload++);
            system_info.Security_Profile= *(RecivedMessage.Payload++);
            
            
            //they are reserved from this point forward 
            //on the new Protocol specification
            system_info.Source_IPv4_Add = *(RecivedMessage.Payload++);
             temp =((*(RecivedMessage.Payload++)));
            system_info.Source_IPv4_Add|= temp<<8;//((*(RecivedMessage.Payload++))<<8);
             temp =((*(RecivedMessage.Payload++)));
            system_info.Source_IPv4_Add|= temp<<16;//((*(RecivedMessage.Payload++))<<16);
             temp =((*(RecivedMessage.Payload++)));
            system_info.Source_IPv4_Add|= temp<<24;//((*(RecivedMessage.Payload++))<<24);
            
            
            system_info.Net_Mask = *(RecivedMessage.Payload++);            
             temp =((*(RecivedMessage.Payload++)));
            system_info.Net_Mask|= temp<<8;//((*(RecivedMessage.Payload++))<<8);
             temp =((*(RecivedMessage.Payload++)));
            system_info.Net_Mask|= temp<<16;//((*(RecivedMessage.Payload++))<<16);
             temp =((*(RecivedMessage.Payload++)));
            system_info.Net_Mask|= temp<<24;//((*(RecivedMessage.Payload++))<<24);
            
            system_info.Gateway = *(RecivedMessage.Payload++);
             temp =((*(RecivedMessage.Payload++)));
            system_info.Gateway|= temp<<8;//((*(RecivedMessage.Payload++))<<8);
             temp =((*(RecivedMessage.Payload++)));
            system_info.Gateway|= temp<<16;//((*(RecivedMessage.Payload++))<<16);
             temp =((*(RecivedMessage.Payload++)));
            system_info.Gateway|= temp<<24;//((*(RecivedMessage.Payload++))<<24);
            
            system_info.UPD_Port = *(RecivedMessage.Payload++);
             temp =((*(RecivedMessage.Payload++)));
            system_info.UPD_Port|= temp<<8;//((*(RecivedMessage.Payload++))<<8);
            statusRecived = 1;
            GetSystemInfoResponse();//indicates a system info response is available

          }
          else if(RecivedMessage.header.Message_Type == PLC_LOAD_SYSTEM_CONFIG)
          {
             #ifdef MONITOR_HP_COM
              printf("Load System Config Replay");
             #endif          
             temp = *(RecivedMessage.Payload++);
             plc_status = temp;
             temp = *(RecivedMessage.Payload++);
             temp <<= 8;
             
             statusRecived = 1;
             plc_status = temp;
          }
          else if(RecivedMessage.header.Message_Type == PLC_GET_PHY_PIB)
          {
             #ifdef MONITOR_HP_COM
              printf("Get Mack PHY Replay");
             #endif
                temp=*(RecivedMessage.Payload++);
             PHY_PIB_replay.status = temp;
             temp=*(RecivedMessage.Payload++);
             temp<<=8;
             PHY_PIB_replay.status |= temp;
                 
             temp=*(RecivedMessage.Payload++);
             PHY_PIB_replay.AttribuitID = temp;
             temp=*(RecivedMessage.Payload++);
             temp<<=8;
             PHY_PIB_replay.AttribuitID |= temp;         
             
             temp=*(RecivedMessage.Payload++);
             PHY_PIB_replay.AttributLength = temp;
             temp=*(RecivedMessage.Payload++);
             temp<<=8;
             PHY_PIB_replay.AttributLength |= temp;
             
             PHY_PIB_replay.AttributeValue = RecivedMessage.Payload++;
             
             GET_PHY_PIB_rep(&PHY_PIB_replay);
          }
          else if(RecivedMessage.header.Message_Type == PLC_GET_MAC_PIB)
          {
             #ifdef MONITOR_HP_COM
              printf("Get Mack PIB Replay");
             #endif
             temp=*(RecivedMessage.Payload++);
             MAC_PIB_replay.status = temp;
             temp=*(RecivedMessage.Payload++);
             temp<<=8;
             MAC_PIB_replay.status |= temp;
                 
             temp=*(RecivedMessage.Payload++);
             MAC_PIB_replay.AttribuitID = temp;
             temp=*(RecivedMessage.Payload++);
             temp<<=8;
             MAC_PIB_replay.AttribuitID |= temp;         
             
             temp=*(RecivedMessage.Payload++);
             MAC_PIB_replay.AttributLength = temp;
             temp=*(RecivedMessage.Payload++);
             temp<<=8;
             MAC_PIB_replay.AttributLength |= temp;
             
             MAC_PIB_replay.AttributeValue = (RecivedMessage.Payload++);             
             GET_MAC_PIB_rep(&MAC_PIB_replay);
          }
          else if(RecivedMessage.header.Message_Type == PLC_SET_INFO)
          {
             #ifdef MONITOR_HP_COM
              printf("SET Info PIB Replay");
             #endif
            status =*(RecivedMessage.Payload++);
            temp = *(RecivedMessage.Payload++);
            temp<<=8;
            status|=temp;
            statusRecived = 1;
            plc_status = status;
            //process status heare 
          }
          else if(RecivedMessage.header.Message_Type == PLC_SETUP_ALRAM)
          {
             #ifdef MONITOR_HP_COM
              printf("Setup Alarm Replay");
             #endif
             temp = *(RecivedMessage.Payload++);
             plc_status = temp;
             temp = *(RecivedMessage.Payload++);
             temp <<= 8;
             statusRecived = 1;
             plc_status = temp;
          }
          else if(RecivedMessage.header.Message_Type == PLC_ALARM)
          {
             #ifdef MONITOR_HP_COM
              printf("Alarm Replay");
             #endif
             AlarmIndiation.Alarm_Type = *(RecivedMessage.Payload++);
             temp = *(RecivedMessage.Payload++);
             temp<<=8;        
             AlarmIndiation.Alarm_Type |= temp; 
             
             AlarmIndiation.Alarm_Length = *(RecivedMessage.Payload++);
             temp = *(RecivedMessage.Payload++);
             temp<<=8;        
             AlarmIndiation.Alarm_Length |= temp; 
            
             AlarmIndiation.Alarm_Value = (RecivedMessage.Payload++);
             ALARM_Indication(&AlarmIndiation);
          }
          else if(RecivedMessage.header.Message_Type == PLC_SHUTDOWN)
          {
             #ifdef MONITOR_HP_COM
              printf("PLC shutdown");
             #endif
             temp = *(RecivedMessage.Payload++);
             plc_status = temp;
             temp = *(RecivedMessage.Payload++);
             temp <<= 8;
             
             statusRecived = 1;
             plc_status = temp;
          }
          else if(RecivedMessage.header.Message_Type == PLC_ATTACH)
          {
             #ifdef MONITOR_HP_COM
              printf("PLC attach replay");
             #endif
             temp = *(RecivedMessage.Payload++);
             confirm.status = temp;
             temp = *(RecivedMessage.Payload++);
             confirm.status |= (temp<<8);
             
             temp = *(RecivedMessage.Payload++);
             confirm.device_id_length = temp;
             temp = *(RecivedMessage.Payload++);
             confirm.device_id_length |= (temp<<8);
             
             confirm.device_identifier = (RecivedMessage.Payload++);
             
             for(temp =0;temp<confirm.device_id_length;temp++)
             {
               RecivedMessage.Payload++;
             }
             temp = *RecivedMessage.Payload++;
             confirm.source_address = temp;
             temp = *RecivedMessage.Payload++;
             confirm.source_address |= (temp<<8);

             temp = *RecivedMessage.Payload++;
             confirm.base_address = temp;
             temp = *RecivedMessage.Payload++;
             confirm.base_address |= (temp<<8);  
             
             statusRecived = 1;
             plc_status = confirm.status;
             Attach_Confirm(&confirm); 
          }
          else if(RecivedMessage.header.Message_Type == PLC_DETACH)
          {
             #ifdef MONITOR_HP_COM
              printf("PLC detach replay");
             #endif
             temp = *RecivedMessage.Payload++;
             de_confirm.status = temp;
             temp = *RecivedMessage.Payload++;
             de_confirm.status |= (temp<<8);

             temp = *RecivedMessage.Payload++;
             de_confirm.dest_address = temp;
             temp = *RecivedMessage.Payload++;
             de_confirm.dest_address |= (temp<<8); 
             Detach_Confirm(&de_confirm);
          }
          else if(RecivedMessage.header.Message_Type == PLC_READ_BLOB)
          {
            readBlobMessage.blob_id = *RecivedMessage.Payload++;
            readBlobMessage.blob_id |= (*RecivedMessage.Payload<<8);
            RecivedMessage.Payload++;
            readBlobMessage.blob_length = *RecivedMessage.Payload++;
            readBlobMessage.blob_length |= (*RecivedMessage.Payload<<8);
            RecivedMessage.Payload++;
            readBlobMessage.blobdata = RecivedMessage.Payload;
            ProcessReadBlob(&readBlobMessage);
          }
          else if(RecivedMessage.header.Message_Type == PLC_WRITE_BLOB)
          {
            writeBlobMessage.blob_id = *RecivedMessage.Payload++;
            writeBlobMessage.blob_id |= (*RecivedMessage.Payload++<<8);
            writeBlobMessage.blob_length = *RecivedMessage.Payload++;
            writeBlobMessage.blob_length |= (*RecivedMessage.Payload++<<8);
            writeBlobMessage.blobdata = RecivedMessage.Payload;
            ProcessWriteBlob(&readBlobMessage);
            
          }
      }
    }
    else
    {
             #ifdef MONITOR_HP_COM
               printf("CRC Failer");
             #endif
      //error happened
    }
}

uint8_t PLCOtherRequestsRetryCount = 0;

/* Schadule this task for Every request except attach and datatransfer  */
void otherPLCRequestsTimedout()
{
  PLCOtherRequestsRetryCount++;
  if(PLCOtherRequestsRetryCount >= MAX_PLC_RETRAY)
  {
    PLCOtherRequestsRetryCount = 0;
    otherPLCRequestsTimeOut();
  }
  else 
  {
  }
}


extern uint8_t plc_request_status;
void otherPLCRequestsTimeOut()
{  
         plc_request_status = 0;
         PLC_INIT_STATE = 1;
         InitPLC2();
 
}

void ACK_Message()
{
  PLC_Message Message;
  Message.header = RecivedMessage.header;
  Message.header.Payload_Length = 0;
  SendMessage(&Message);
}

#ifndef KIT


#define PLC_RESET_DIR P3DIR
#define PLC_RESET_OUT P3OUT
#define PLC_RESET_BIT BIT3

void ResetPLC_Processor()
{
    PLC_RESET_DIR |= PLC_RESET_BIT;
    PLC_RESET_OUT &=~PLC_RESET_BIT;
}
void StartPLC_Processor()
{
    PLC_RESET_DIR |= PLC_RESET_BIT;
    PLC_RESET_OUT |= PLC_RESET_BIT;
}

#else
#endif 
