
#include "Data_Transfer.h"
#include "PLC.h"
#include "Status.h"
#include "Logger.h"
#include "System_Info.h"
#include "Tariff.h"
#include "self_diagnosis.h"
#include "Relay.h"
#include "errors.h"
#include "Status.h"
#include "UI.h"
#include "Tamper.h"
#define MaxDataPayloadLength  120  //the protocol allows a max of 1500 byte of data

#include "iec62056_46_link.h"
#include "DLMS.h"
#include "uart_comms.h"

extern async_hdlc_tx_t *txptr[MAX_USCI_PORTS];
extern iec62056_46_link_t link[];
extern async_hdlc_rx_t rx[];
extern uint32_t next_octet_timeout[MAX_USCI_PORTS],next_inactivity_timeout[MAX_USCI_PORTS];



#define DestinationLSAP   89
#define SourceLSAP  90

//extern  phase_parms_s chan1;
//extern uint32_t total_consumed_reactive_energy;
//extern CurrentBalance Current_balance;
//extern MeterAddress Meter_serialNumber;
//extern MeterAddress Meter_ProductNumber;
//extern MeterAddress Meter_address;
//extern int FirmwareVersion;  // 1 byte
extern System_Info_t system_info;//In the,system info table 
extern MeterStatus status;
extern HardwareStatus hardware_status;

uint8_t DataPayload[MaxDataPayloadLength];


//over voltage and current ranges and Frequency,change the trip ranges to percent 
extern uint8_t  MAX_IRMS ;
extern uint16_t MAX_VRMS ; 
extern uint8_t  MAX_Frequency ;

//under voltage and current ranges 
extern uint8_t MIN_VRMS ;
extern uint8_t MIN_Frequency ;



extern uint32_t MAXActivePowerTripPoint ; //in w watt steps 

extern uint8_t Frame_Received[];


void DataTransferRequest(uint8_t *dataPayload,int length)
{
    PLC_Message message;
    PLC_Header header;
    DataTX_Request_Confirm request;
    
    request.Mode = ModeDataTX_Request;
    request.Dest_LSAP = DestinationLSAP;
    request.Sour_LSAP =SourceLSAP;
    request.Des_Address = 0;
    request.data = dataPayload;//(uint8_t[]){"Donayam Nega"};
       
    header.Message_Type = PLC_DATA_TRANSFER;
    header.Message_flags = 0;
    header.Message_flags |= ORG_flag | SEQ_flag(1) ;
    header.Message_flags &= ~RPY_flag;
    
    header.Payload_Length = 10 + length ; //10 for the data transfer header in the LLC mode 
    
    DataPayload[0]=   request.Mode;
    DataPayload[1]=   request.Mode>>8;
    DataPayload[2]=  request.Dest_LSAP;
    DataPayload[3]= request.Sour_LSAP;
    DataPayload[4] = request.Des_Address;
    DataPayload[5] = request.Des_Address>>8;
    //reserved
    DataPayload[6] = 0;
    DataPayload[7] = 0;
    DataPayload[8] = 0;
    DataPayload[9] = 0;
    
    
    int z=10;
    length = length;
    for(;z<=length+10;z++)
    {
      DataPayload[z] = (*request.data);
     
      

       request.data++;
    }


    
    message.header = header;
    message.Payload = DataPayload;  
          
    SendMessage(&message); 
 
 
    
#ifdef DLMS
     txptr[PLC_PORT]= NULL;
#endif      
     
}
//void DataTransferRequest(int ConnectionHandel,int *dataPayload,int length)
//{
//    PLC_Message message;
//    PLC_Header header;
//
//    DataPayload[0]=   ConnectionHandel;
//    DataPayload[1]=   ConnectionHandel>>8;
//   
//    header.Message_Type = PLC_DATA_TRANSFER;
//    header.Message_flags |= ORG_flag | SEQ_flag(0) & ~RPY_flag ;
//    header.Payload_Length = length;
//    int z=2;
//    for(;z<=length+2;z++)
//    {
//      DataPayload[z] = (*dataPayload);
//      dataPayload++;
//    }
//    
//    message.header = header;
//    message.Payload = DataPayload;  
//    
//       
//    SendMessage(&message); 
//}
//int connection_handel= *RecivedMessage.Payload;
//             RecivedMessage.Payload++;
//             connection_handel |=(*RecivedMessage.Payload)<<8;
//             DataTransferRequest(connection_handel,RecivedMessage.Payload,RecivedMessage.header.Payload_Length-2);
/*
 * status: 16 bit number indicating the status of the data transmission.
 *  	   This is of Type "enum MessageStatusCodes"
 */
void DataTransferConfirm(DataTX_Request_Confirm *confirm)
{
  
}

/*
 *message from PLC to host
 */
void DataTransferIndication(DataTX_Indication *indication,int datalength )
{
  
#if defined(DLMS)          
        rx->msg_len = 0;
        
        /* Initialise the header information. */
//        rx->segmented = byte & 0x08;
//        rx->expected_len = (byte & 0x7) << 8;
//        rx->control_field = 0;
        
        rx->dest_msap = indication->Des_Address;
        rx->source_msap = indication->Sour_Add;
        rx->dest_msap_len = 2;
        rx->source_msap_len = 2;                               
//      iec62056_46_link_idle_restart(link);          
        uint8_t i=0;
        
        rx->msg[rx->msg_len++] = 0xE6;
        rx->msg[rx->msg_len++] = 0xE6;
        rx->msg[rx->msg_len++] = 0x00;
        
       for(;i<datalength;i++)
       {
          rx->msg[rx->msg_len++] = *(indication->data);
//         iec62056_46_rx_byte(&link[PLC_PORT], &rx[PLC_PORT], *(indication->data));
          indication->data++;
       }
       
       rx->good_packets++;
       Frame_Received[link->port]=1;
       link->association = 1; // use management association 
       
//       //handle PLC DLMS Timeout        
//       uint32_t now;
//       now = current_time();
//
//       if (now >= next_octet_timeout[IEC_PORT])
//          {
//              if (rx[IEC_PORT].state > 1)
//                // P1OUT ^= BIT1;
//                iec62056_46_rx_timeout(&link[IEC_PORT], &rx[IEC_PORT]);
//          } 
//                   
//       
//       next_octet_timeout[PLC_PORT] = now + ((32768L*TIMEOUT_3_INTRA_FRAME)/1000L);
//       next_inactivity_timeout[PLC_PORT] = now + ((32768L*TIMEOUT_2_INACTIVITY)/1000L);
//       //end of Handle PLC DLMS Timeout
       
       
       i = process_dlms_frame();
       if(i == 0)
       {
         /* There is an error in the recived frame*/
       }
       else
       {
       } 


            

#else  

//        uint8_t data[37],counter=0;
//        unsigned long temp,d;
//        //uint8_t type = *(indication->data);
//    switch(*(indication->data))
//    {
//      case HOST_CMD_GET_READINGS_PHASE_1:
//        
//        data[0] = HOST_CMD_GET_READINGS_PHASE_1;
//        temp= chan1.readings.V_rms;
//        data[1] = temp;
//        temp>>=8;
//        data[2] = temp;
//        temp>>=8;
//        data[3] = temp;
//        temp>>=8;
//        data[4] = temp;
//        
//        temp = chan1.readings.I_rms;
//        data[5]=  temp;
//        temp>>=8;
//        data[6] = temp;
//        temp>>=8;
//        data[7] = temp;
//        temp>>=8;
//        data[8]=temp;
//        
//        temp= chan1.active_energy_import;//chan1.consumed_active_energy;//
//        data[9]= temp;
//        temp>>=8;
//        data[10]= temp;
//        temp>>=8;
//        data[11] = temp;
//        temp>>=8;
//        data[12]= temp; 
//        
//        temp = chan1.readings.power_factor;
//        data[13]= temp;
//        temp>>=8;
//        data[14]= temp;
//        
//        
//        temp = chan1.readings.frequency;
//        data[15]= temp;
//        temp>>=8;
//        data[16]= temp;
//        
//        temp = Current_balance.balance;
//        data[17]= temp;
//        temp>>=8;
//        data[18]= temp;
//        temp>>=8;
//        data[19] = temp;
//        temp>>=8;
//        data[20]= temp; 
//        
//        temp = chan1.readings.active_power;
//        data[21] = temp;
//        temp>>=8;
//        data[22] = temp;
//        temp>>=8;
//        data[23] = temp;
//        temp>>=8;
//        data[24] = temp;
//        
//        temp = chan1.readings.reactive_power;
//        data[25] = temp;
//        temp>>=8;
//        data[26] = temp;
//        temp>>=8;
//        data[27] = temp;
//        temp>>=8;
//        data[28] = temp;       
//        
//        temp = chan1.consumed_reactive_energy_QI;// total_consumed_reactive_energy;
//        data[29] = temp;
//        temp>>=8;
//        data[30] = temp;
//        temp>>=8;
//        data[31] = temp;
//        temp>>=8;
//        data[32] = temp; 
//        
//        temp = 0;
//        
//        if(hardware_status.EEPROM1_Status==1)
//        {
//          temp=1;
//        }
//        if(status.MangneticTamperStatus ==1)
//        {
//          temp |=0x02;
//        }
//        if(status.UpperCoverRemovedTamperStatus ==1)
//        {
//          temp |=0x04;
//        }
//        if(status.LowerCoverRemovedTamperStatus ==1)
//        {
//          temp |= 0x08;
//        }
//        if(status.NeutralTamperTimedoutStatus == 1)
//        {
//          temp |=0x10;
//        }
//        
//        if(hardware_status.Battry_Status == 1)
//        {
//          temp |=0x20; 
//        }
//        if(hardware_status.EEPROM2_Status == 1)
//        {
//          temp |=0x40;
//        }
//        if(hardware_status.LCD_Status == 1)
//        {
//          temp |=0x80;
//        }
//        if(hardware_status.PLC_Status == 1)
//        {
//          temp|=0x100;
//        }
//        if(hardware_status.RTC_Status == 1)
//        {
//          temp |=0x200;
//        }
//        if(status.BattryStatus == 1)
//        {
//          temp |= 0x400;
//        }
//        if(status.RelayStatus == 1)
//        {
//          temp |= 0x800;
//        }
//        
//        
//        data[33] = temp;
//        temp>>=8;
//        data[34] = temp;
//        temp>>=8;
//        data[35] = temp;
//        temp>>=8;
//        data[36] = temp; 
//        
//        
//        DataTransferRequest(data,37);
//      break;
//    case   HOST_CMD_GET_METER_CONFIGURATION:
//
//            data[0] = HOST_CMD_GET_METER_CONFIGURATION;
//            
//            temp = Meter_serialNumber.Address;
//            data[1] = temp;
//            temp>>=8;
//            data[2] = temp;
//            temp>>=8;
//            data[3] = temp;
//            temp>>=8;
//            data[4] = temp;  
//            
//            temp = Meter_ProductNumber.Address;
//            data[5] = temp;
//            temp>>=8;
//            data[6] = temp;
//            temp>>=8;
//            data[7] = temp;
//            temp>>=8;
//            data[8] = temp;  
//            
//            data[9] = FirmwareVersion;
//            
//            //plc firmware version 
//            data [10] = system_info.Firmware_Version[3];//major 
//            data [11] = system_info.Firmware_Version[2];//minor
//            data [12] = system_info.Firmware_Version[1];//revision
//            data [13] = system_info.Firmware_Version[0];//build
//            
//            DataTransferRequest(data,13);
//            
//      break;
//    case HOST_CMD_SET_METER_CONSUMPTION:
//      break;
//    case HOST_CMD_SET_RTC:
//      break;
//    case HOST_CMD_GET_RTC:
//      break;
//    case HOST_CMD_ALIGN_WITH_CALIBRATION_FACTORS:
//      break;
//    case HOST_CMD_SET_PASSWORD:
//      break;
//    case HOST_CMD_GET_READINGS_NEUTRAL:
//      break;
//    case HOST_CMD_ERASE_FLASH_SEGMENT:
//      break;
//    case HOST_CMD_SET_FLASH_POINTER:
//      break;    
//    case HOST_CMD_FLASH_DOWNLOAD:
//      break;
//    case HOST_CMD_FLASH_UPLOAD:
//      break;
//    case HOST_CMD_ZAP_MEMORY_AREA:
//      break;
//    case HOST_CMD_SUMCHECK_MEMORY:
//      break;
//    case HOST_CMD_GET_RAW_ACTIVE_POWER_PHASE_1:
//      break;
//    
//    case HOST_CMD_GET_RAW_REACTIVE_POWER_PHASE_1:
//      break;
//
//    case HOST_CMD_GET_RAW_ACTIVE_POWER_NEUTRAL:
//      break;
//    case HOST_CMD_GET_RAW_REACTIVE_POWER_NEUTRAL:
//      break;
//    case HOST_CMD_CHECK_RTC_ERROR:
//      break;
//    case HOST_CMD_RTC_CORRECTION:
//      break;
//    case HOST_CMD_MULTIRATE_SET_PARAMETERS:
//      break;
//    case HOST_CMD_MULTIRATE_GET_PARAMETERS:
//      break;
//    case HOST_CMD_MULTIRATE_CLEAR_USAGE:
//      break;
//    case HOST_CMD_MULTIRATE_GET_USAGE:
//      break;           
//    case HOST_CMD_TAMPER_GET_STATUS:                 
//        temp = 0;
//        if(status.ReverseConnectionTamperStatus==1)
//        {
//          temp=1;
//        }
//        if(status.MangneticTamperStatus ==1)
//        {
//          temp |=0x02;
//        }
//        if(status.UpperCoverRemovedTamperStatus ==1)
//        {
//          temp |=0x04;
//        }
//        if(status.LowerCoverRemovedTamperStatus ==1)
//        {
//          temp |= 0x08;
//        }
//        if(status.NeutralTamperStatus ==1)
//        {
//          temp |=0x10;
//        }
//        data[0] = HOST_CMD_TAMPER_GET_STATUS;
//        data[1] =temp;
//        
//        DataTransferRequest(data,2);        
//      break;
//    case HOST_CMD_WRITE_EEPROM:
//        temp = *(++indication->data); // start address  unsinged long(4 byte), length  1byte 
//        d = *(++indication->data);
//        d <<=8;
//        temp |= d;
//        d = *(++indication->data); 
//        d <<= 16;
//        temp |= d;
//        d = *(++indication->data); 
//        temp |= (d<<24);
//        
//         counter=0;
//        d = *(++indication->data); //lenght 
//         
//        
//        for(;counter<d;counter++);
//        {
//          
//        }
//        
//      break;
//    case HOST_CMD_SET_POWER_QUALITY_COMMAND:
//          MAX_IRMS = *(++indication->data) ;
//         
//          temp = *(++indication->data);
//          temp <<= 8;
//          temp |= *(++indication->data);
//          MAX_VRMS =temp; //in V
//         
//          
//          MAX_Frequency =*(++indication->data);
//
//         //under voltage and current ranges 
//          MIN_VRMS =*(++indication->data);  //in V
//          MIN_Frequency = *(++indication->data); //in HZ
//          
//          temp = *(++indication->data);
//          temp <<= 8;
//          temp |= *(++indication->data);          
//          MAXActivePowerTripPoint = temp; //in w watt steps          
//      break;
//      case HOST_CMD_GET_POWER_QUALITY_COMMAND:
//          
//          data[0] = HOST_CMD_GET_POWER_QUALITY_COMMAND;
//          data[1] = MAX_IRMS;
//          
//          data[2] = MAX_VRMS;
//          data[3] = MAX_VRMS>>8;
//          
//          data[4] =  MAX_Frequency;
//          
//          data[5] = MIN_VRMS;
//          data[6] = MIN_Frequency;
//          
//          data[7] = MAXActivePowerTripPoint;
//          data[8] = MAXActivePowerTripPoint>>8;
//          DataTransferRequest(data,9); 
//
//         
//      break;
//      
//    case HOST_CMD_CONNECT_RELAY:
//       status.ConnectCommandRecived = 1;
//    break;
//    case HOST_CMD_DISCONNECT_RELAY:
//       status.DisconnectCommandRecived = 1;  
//    break;
//    case HOST_CMD_CLEARTAMPERS_COMMAND:
//      status.UpperCoverRemovedTamperStatus = 0;
//      status.LowerCoverRemovedTamperStatus = 0;
//      status.NeutralTamperTimedoutStatus = 0;
//      status.MangneticTamperStatus = 0;
//
//      break;
//      
//    case HOST_CMD_SETBALANCE_COMMAND:
//      temp = *(++indication->data);
//      temp |= (*(++indication->data)<<8);
//      d= (*(++indication->data));
//      temp |= d<<16;
//      d= (*(++indication->data));      
//      temp |= d<<24;
//      Current_balance.balance = temp;
//      break;
//    case HOST_CMD_GET_BALANCE_COMMAND:
//      
//      temp = Current_balance.balance;
//      data[0]= HOST_CMD_GET_BALANCE_COMMAND;
//      data[1]= temp;
//      temp>>=8;
//      data[2] = temp;
//      temp>>=8;
//      data[3] = temp;
//      temp>>=8;
//      data[4] = temp;
//      DataTransferRequest(data,5);
//     
//      
//      break;      
//      
//    default:
//      
//        data[0] = HOST_CMD_UNSUPPORTED_COMMAND;
//        data[1] = *(indication->data);        
//        DataTransferRequest(data,2);
//        
//      break;
//    }
#endif 
}

void dataTransferTimedout()
{
}
