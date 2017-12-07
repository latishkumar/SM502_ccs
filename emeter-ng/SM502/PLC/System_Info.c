

#include "PLC.h"
#include "System_Info.h"



System_Info_t system_info;
unsigned int VendorID = 0;
uint8_t ProductID[16];

void GetSystemInfoRequest()
{
    PLC_Message m;
    m.header.Message_Type   = PLC_GET_SYSTEM_INFO;
    m.header.Message_flags  = ORG_flag | SEQ_flag(0) & ~RPY_flag;
    m.header.Payload_Length = 0;
    SendMessage(&m);
}

void GetSystemInfoResponse()
{
  //the information is in system_info structure so process it
  
}

void SET_INFO_Request(unsigned int info_type)
{
    PLC_Message message;
    PLC_Header header;
    header.Message_Type = PLC_SET_INFO;
     header.Message_flags=0;
    header.Message_flags |= ORG_flag | SEQ_flag(0) & ~RPY_flag ;
    
    uint8_t Payload[19];
    int z=0;
    int i=0;
    switch(info_type)
    {
      
    case PRIME_PHY_TX_Info_Type:
           Payload[0] =info_type;
           Payload[1] =info_type>>8;
           
           Payload[2] = 0x06; //Info length
           Payload[3] = 0;
           
           Payload[4] = 0;//resirved
           Payload[5] = 1;//PRM  non disgnated
                          //ROBO prime mode 
                          //FEC on
           
           Payload[6] = DBPSK;//Modulation type
           Payload[7] = 0;
           
           Payload[8] = 0;// TX_Level Prime specification is required, value form 0 to 7
           Payload[9] = 0;
           
           header.Payload_Length = 10;  
      break;
    case PRIME_PHY_RX_Info_Type:
          Payload[0] =info_type;
          Payload[1] =info_type>>8;
          
           Payload[2] = 0x04; //Info length
           Payload[3] = 0;
           
           Payload[4] = 0;//resirved
           Payload[5] = 1;//ROBO  AGC(Automatic gain controll)
                      
           Payload[6] = 0;// Gain Value Prime specification is required, value form 0 to 7
           Payload[7] = 0;
           
           header.Payload_Length = 8;  
          
      break;
    case G3_PHY_TX_Info_Type:
          Payload[0] =info_type;
          Payload[1] =info_type>>8;
      break;
    case G3_PHY_RX_Info_Type:
          Payload[0] =info_type;
          Payload[1] =info_type>>8;
      break;
    case Vendor_and_Product_Id_Info_Type:
          Payload[0] =info_type;
          Payload[1] =info_type>>8;
           
          Payload[2] = 18;//length 
          Payload[3] = 0;
    
          Payload[4] = VendorID;//assigned by Prime Alliance for PRIME
          Payload[5] = VendorID>>8;
          
          z=6;

          for(;z<=15;z++,i++)
          Payload[z] = ProductID[i];

          header.Payload_Length = 15; 

          
      break;      
    case TX_PHY_Attenuation_Info_Type:
          Payload[0] =info_type;
          Payload[1] =info_type>>8;
    
          Payload[2] = 2;//length 
          Payload[3] = 0;
    
          Payload[4] = 0x1E;// Tx attenuation from 0xc4 to 0x1E
          Payload[5] = 0 ;  // TX PGA  0 = 0 dB
                            // 1 = -3 dB
                            // 2 = -6 dB
           
          header.Payload_Length = 6; 
      break;
      
    }
    
    message.header = header;
    message.Payload = Payload;
    
    SendMessage(&message);  
}
