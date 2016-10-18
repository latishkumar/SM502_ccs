

#include "Alarm.h"
#include "PLC.h"

Alarm_Indication_t AlarmIndiation;
ConnectDisconnectRequestAlarm_t ConnectRequestAlarm;
ConnectDisconnectRequestAlarm_t DisconnectRequestAlarm;
void SETUP_ALARM_Request(uint8_t alarm_type,uint8_t value)
{
    PLC_Message message;
    PLC_Header header;

    uint8_t Payload[5];
       
    //int z=0;
    
    header.Message_Type = PLC_SETUP_ALRAM;
    header.Message_flags |= ORG_flag | SEQ_flag(0) & ~RPY_flag ;
    
    
    Payload[0]= alarm_type;
    Payload[1]= alarm_type>>8;
    
    Payload[2]=1; //all set ups are one byte 
    Payload[3]=0;
    
    Payload[4]= value;
    
    header.Payload_Length = 5;
    

    
    message.header = header;
    message.Payload = Payload;  
    
       
    SendMessage(&message);     
}

void SETUP_ALARM_Reply();

void ALARM_Indication(Alarm_Indication_t *AI)
{
    long temp;
    unsigned int Reason;
    switch(AI->Alarm_Type)
    {
        case Network_Deregistered://Prime 
            Reason = *AI->Alarm_Value;
            temp  = *(AI->Alarm_Value++);
            temp<<=8;
            Reason |= temp;
            //processs reason 
            
             //step 4 in the intialization process
            
          break;
        case Connect_Request:  //prime 
          ConnectRequestAlarm.HCT_Connection_Type=*AI->Alarm_Value;
          temp = *(AI->Alarm_Value++);
          temp <<=8;
          ConnectRequestAlarm.HCT_Connection_Type|=temp;
          
          ConnectRequestAlarm.HCT_Connection_Handle=*(AI->Alarm_Value++);
          temp = *(AI->Alarm_Value++);
          temp <<=8;
          ConnectRequestAlarm.HCT_Connection_Handle|=temp;
          
          ConnectRequestAlarm.Connection_Data_Length=*(AI->Alarm_Value++);
          temp = *(AI->Alarm_Value++);
          temp <<=8;
          ConnectRequestAlarm.Connection_Data_Length|=temp;
          
          ConnectRequestAlarm.Connection_Data.ARQ = *(AI->Alarm_Value++);
          ConnectRequestAlarm.Connection_Data.MAC_Connection_Type = *(AI->Alarm_Value++);
          ConnectRequestAlarm.Connection_Data.CfBytes = *(AI->Alarm_Value++);
          temp=0;
          for(;temp<=5;temp++)
          {
            ConnectRequestAlarm.Connection_Data.DestinationAddress[temp] = *(AI->Alarm_Value++);
          }
          
          ConnectRequestAlarm.Connection_Data.AditionalData = (AI->Alarm_Value++);
          
          break;
        case Disconnect_Indication://PRIME
          DisconnectRequestAlarm.HCT_Connection_Type=*AI->Alarm_Value;
          temp = *(AI->Alarm_Value++);
          temp <<=8;
          DisconnectRequestAlarm.HCT_Connection_Type|=temp;
          
          DisconnectRequestAlarm.HCT_Connection_Handle=*(AI->Alarm_Value++);
          temp = *(AI->Alarm_Value++);
          temp <<=8;
          DisconnectRequestAlarm.HCT_Connection_Handle|=temp;
          
          DisconnectRequestAlarm.Connection_Data_Length=*(AI->Alarm_Value++);
          temp = *(AI->Alarm_Value++);
          temp <<=8;
          DisconnectRequestAlarm.Connection_Data_Length|=temp;
          
          DisconnectRequestAlarm.Reason = *(AI->Alarm_Value++);
          temp = *(AI->Alarm_Value++);
          temp <<=8;
          DisconnectRequestAlarm.Reason |=temp;
          
          break;
        case Network_Registration_Started: //no additional data
          break;
        case Network_Registration_Complete://no additional data
          break;
	//Connect_Complete =0x06, not available
        case G3_Network_Status_Indication://not implimented yet 
          break;
        case PHY_received_PPDU_header_CRC_fail_detected://no additional data
          break;
        case PHY_received_PPDU_header_syntax_error_detected://no additional data
          break;
        case Terminal_node_is_promoted_to_switch_node://no additional data
          break;
        case Switch_node_is_demoted_to_terminal_node://no additional data
          break;
        case CL_Alarm:
          
          //step 8 in the intialization process
          break;
        case General_Error://no additional data
          break;      
    }  
}

void ProcessAlarmValue(uint8_t alarmType)
{
	switch(alarmType)
	{
		case G3_Network_Status_Indication:
			break;
		case PHY_received_PPDU_header_CRC_fail_detected:
			break;
		case PHY_received_PPDU_header_syntax_error_detected:
			break;
		case Terminal_node_is_promoted_to_switch_node:
			break;
		default:
			break;
	}
}