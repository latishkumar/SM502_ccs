
#include <stdint.h>
#include <msgs.h>
#include <string.h>
#include "../SM502/Logger.h"
#include "../SM502/Tamper.h"

uint8_t Meter_Sr_No[] =
{
    10,'1','1','0','0','0','0','0','0','3','4' //'A','B','C','D','1','2','3','4','5','6',
};

uint8_t Manufacturer_Name[] =
{
    20, 'd','V','e','n','t','u','s',' ','T','e','c','h','n','o','l','o','g','i','e','s'
};


/*
manuf_code[2]+model_code[2]+manuf_year[2]
*/
uint8_t Device_ID_2[] =
{
   6,'d','v','0','1','1','3'
};

uint8_t Device_ID_3[] =
{
   18,'c','o','n','t','a','d','o','r',' ',' ','D','L','M','S','0','1','0','6'
};

uint8_t Device_ID_4[] =
{
   20,'0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9'
};
uint8_t Device_ID_5[] =
{
   13,'d','V','s','A','B','C','D','1','2','3','4','5','6'
};


/*
  Multicast communication identifier 
*/
uint8_t Device_ID_6[] =
{
   24,'0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','1','2','3','4'
};


/* 
  Firmware Version 
*/
uint8_t Device_ID_7[] =
{
   5,'0','0','1','1','1'
};

/* Scroll Display Mode */
uint8_t Device_ID_8[] =
{
   1,'A'
};

/**/
uint8_t DateTimeFormat[] =
{
  12,'Y','Y','M','M','D','D','H','H','M','M','S','S' 
};

uint8_t MAC_Address[] =
{
   6,'0','0','0','0','0','0'
};


uint8_t PLC_FW_Version[] =
{
  6,'0','0','0','7','0','0'//,'0','1','0','0','0','9','0','0','0','1'16
};

uint16_t PLC_Vendor_ID = 1234;

uint8_t PLC_Product_ID = 234;

uint8_t MaxAllowedTimeShift = 30;
uint16_t ClockTimeShiftInvalidLim = 600;

uint16_t TransformerRatio_VoltageNum = 2300;
uint16_t TransformerRatio_VoltageDen = 2300;

uint16_t TransformerRatio_CurrentNum = 500;
uint16_t TransformerRatio_CurrentDen = 500;

//uint16_t TimeThresholdVoltageSag = 180;//sec
//uint16_t TimeThresholdVoltageSwell = 120;

uint16_t TimeThresholdPowerDemandExceed = 120;

uint32_t Active_Demand_Control_Threshold = 11000;
uint16_t DurationOfAllLongPowerFailes = 1300;


extern uint16_t ThrusholdVoltageSagPer ;//7% percent 
extern uint16_t ThrusholdVoltageSwellPer ;

uint16_t ThrusholdPowerDemandCloseToContractPower = 9500;
//uint16_t VoltageReferance = 230;


extern uint16_t VoltageThresholdLongPowerFailePer ;
extern uint16_t NumberOfLongPowerFails ; //use method to read from EERPOM

extern uint8_t ActiveQuadrant;

uint8_t *SpecialDayEntries;

uint8_t CurrentActiveTariff = 1;


uint16_t Output_State = 0;
uint8_t control_state =0;
uint8_t control_mode = 5;


uint32_t ErrorValue = 0;
uint16_t ImportActivePowerOverThrStatus = 0; //boolean
uint16_t ExportActivePowerOverThrStatus = 0; //boolean
uint32_t ImportActivePowerThreshold = 11000;
uint32_t ExportActivePowerThreshold = 11000;

extern uint16_t Nominal_Voltage;
extern uint16_t TimeThresholdLongPowerFaile ;
extern uint8_t MIN_VRMS;
extern uint16_t MAX_VRMS; 
extern uint16_t ThresholdLongPowerFaile;
extern uint16_t UnderVoltageTripDuration;
extern uint16_t OverVoltageTripDuration;
extern uint32_t MAXActivePowerTripPoint ; //in w watt steps 
extern uint32_t MAXActivePowerExportTripPoint ;

extern uint16_t NumberOfLongPowerFails;

extern Tamper_Count TamperCount;
extern uint8_t EnergyLoggingTime; //Minute 

void ChangeReferanceVoltage(void *data,int data_direction)
{  
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      Nominal_Voltage = nv_2;

      //TODO. check this is with in the hardware maximmu limit
      //if(Nominal_Voltage <= AbsoluteMaxVRMS){
        
         MAX_VRMS = Nominal_Voltage +( ((ThrusholdVoltageSwellPer/100) * Nominal_Voltage)/100);
         MIN_VRMS = Nominal_Voltage - (((ThrusholdVoltageSagPer/100) * Nominal_Voltage)/100);  
         ThresholdLongPowerFaile = Nominal_Voltage - (((VoltageThresholdLongPowerFailePer/100) * Nominal_Voltage)/100);
         setNominalValues(&Nominal_Voltage,0);
         
      //}
      //else{
       
      //}
      

      

      
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = Nominal_Voltage & 0xFF;
       datap++;
      *datap = (Nominal_Voltage>>8) & 0xFF;
    }
}

void changeTimeThresholdLongPowerFaile(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);

      TimeThresholdLongPowerFaile = nv_2;
            
//      setTripDuration(&TimeThresholdLongPowerFaile,7);
      uint8_t temp8 = 7;
      write_to_eeprom(&TimeThresholdLongPowerFaile,&temp8,setTripDuration); 
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = TimeThresholdLongPowerFaile & 0xFF;
       datap++;
      *datap = (TimeThresholdLongPowerFaile>>8) & 0xFF;
    }
}

void changeTimeThresholdVoltageSwell(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
            uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);
      
      OverVoltageTripDuration = nv_2;
            
      //write_TimeThreshold_OverVoltage(Nominal_Voltage);
      setTripDuration(&OverVoltageTripDuration,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = OverVoltageTripDuration & 0xFF;
       datap++;
      *datap = (OverVoltageTripDuration>>8) & 0xFF;
    }
}

void changeTimeThresholdVoltageSag(void *data,int data_direction)
{
      uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);
      
      UnderVoltageTripDuration = nv_2;
      
//      UnderVoltageTripDuration = *datap++;
//      UnderVoltageTripDuration |= ((*datap++)<<8);
      
      setTripDuration(&OverVoltageTripDuration,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = UnderVoltageTripDuration & 0xFF;
       datap++;
      *datap = (UnderVoltageTripDuration>>8) & 0xFF;
    }
}

void changeThresholdVoltageSag(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);      
      
      ThrusholdVoltageSagPer= nv_2;
//      ThrusholdVoltageSagPer = *datap++;
//      ThrusholdVoltageSagPer |= ((*datap++)<<8);
      
    
      MIN_VRMS = Nominal_Voltage - ((ThrusholdVoltageSagPer * Nominal_Voltage)/100);  
      setMinValues(&ThrusholdVoltageSagPer,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = ThrusholdVoltageSagPer & 0xFF;
       datap++;
      *datap = (ThrusholdVoltageSagPer>>8) & 0xFF;
    }
}

void changeThresholdVoltageSwell(void *data,int data_direction)
{
  
        uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);       
      
      ThrusholdVoltageSwellPer = nv_2;
//      ThrusholdVoltageSwellPer = *datap++;
//      ThrusholdVoltageSwellPer |= ((*datap++)<<8);
      
    
      MAX_VRMS = Nominal_Voltage +( (ThrusholdVoltageSwellPer * Nominal_Voltage)/100);

      setMaxValues(&ThrusholdVoltageSwellPer,0);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = ThrusholdVoltageSwellPer & 0xFF;
       datap++;
      *datap = (ThrusholdVoltageSwellPer>>8) & 0xFF;
    }
}

void changeThresholdLongPowerFaile(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);  
      
      VoltageThresholdLongPowerFailePer = nv_2;
//      VoltageThresholdLongPowerFailePer = *datap++;
//      VoltageThresholdLongPowerFailePer |= ((*datap++)<<8);
      
    
      ThresholdLongPowerFaile = Nominal_Voltage - ((VoltageThresholdLongPowerFailePer * Nominal_Voltage)/100);

//      setNominalValues(&VoltageThresholdLongPowerFailePer,4 );
      uint8_t temp8 = 4;
      write_to_eeprom(&VoltageThresholdLongPowerFailePer,&temp8,setNominalValues);
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = VoltageThresholdLongPowerFailePer & 0xFF;
       datap++;
      *datap = (VoltageThresholdLongPowerFailePer>>8) & 0xFF;
    }
}

//TODO. implement 
void changeExportActivePowerThreshold(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      uint32_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+3);
      nv_2 |= ((*(ptr2+2))<<8); 
      nv_2 |= ((uint32_t)(*(ptr2+1))<<16);  
      nv_2 |= ((uint32_t)(*(ptr2))<<24);
      
      MAXActivePowerExportTripPoint = nv_2;      
      
      
      
//      MAXActivePowerExportTripPoint = *datap++;
//      MAXActivePowerExportTripPoint |= ((uint32_t)(*datap++)<<8);
//      MAXActivePowerExportTripPoint |= ((uint32_t)(*datap++)<<16);
//      MAXActivePowerExportTripPoint |= ((uint32_t)(*datap++)<<24);
      
      //ThresholdLongPowerFaile = (VoltageThresholdLongPowerFailePer * Nominal_Voltage)/100;
      //write_Threshold_VoltageSwell(ThrusholdVoltageSag);
     
    }
    else if(data_direction == ATTR_READ)
    {
      *datap = MAXActivePowerExportTripPoint & 0xFF;
       datap++;
      *datap = (MAXActivePowerExportTripPoint>>8) & 0xFF;
       datap++;
      *datap = (MAXActivePowerExportTripPoint>>16) & 0xFF;;
      datap++;
      *datap = (MAXActivePowerExportTripPoint>>24) & 0xFF;;;
    }
}
//TODO. implement  
void changeImportActivePowerThreshold(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
            uint32_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+3);
      nv_2 |= ((*(ptr2+2))<<8); 
      nv_2 |= ((uint32_t)(*(ptr2+1))<<16);  
      nv_2 |= ((uint32_t)(*(ptr2))<<24);
      
      MAXActivePowerTripPoint = nv_2;
//      MAXActivePowerTripPoint = *datap++;
//      MAXActivePowerTripPoint |= ((uint32_t)(*datap++)<<8);
//      MAXActivePowerTripPoint |= ((uint32_t)(*datap++)<<16);
//      MAXActivePowerTripPoint |= ((uint32_t)(*datap++)<<24);
      //ThresholdLongPowerFaile = (VoltageThresholdLongPowerFailePer * Nominal_Voltage)/100;
      //write_Threshold_VoltageSwell(ThrusholdVoltageSag);
      
    }
    else if(data_direction == ATTR_READ)
    {
      
      *datap = MAXActivePowerTripPoint & 0xFF;
       datap++;
      *datap = (MAXActivePowerTripPoint>>8) & 0xFF;
       datap++;
      *datap = (MAXActivePowerTripPoint>>16) & 0xFF;;
      datap++;
      *datap = (MAXActivePowerTripPoint>>24) & 0xFF;;
    }
}
//TODO. implement  
void changeNumberOfLongPowerFailes(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      
      uint16_t nv_2=0;
      uint8_t *ptr2 = data;
      nv_2 = *(ptr2+1);
      nv_2 |= ((*(ptr2))<<8);
      NumberOfLongPowerFails = nv_2;
      
//      NumberOfLongPowerFails = *datap++;
//      NumberOfLongPowerFails |= ((uint32_t)(*datap++)<<8);

      //logNumberofLongPowerFailes(ThrusholdVoltageSag);TODO . impliment shis 
    }
    else if(data_direction == ATTR_READ)
    {
      
      *datap = NumberOfLongPowerFails & 0xFF;
       datap++;
      *datap = (NumberOfLongPowerFails>>8) & 0xFF;

    }
}

void changeCumulativeTamperCount(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {

      
//       EnergyLoggingTime = *datap; //Minute 
//      //write to EEPROM
//      setEnergyLogTime(&EnergyLoggingTime);
    }
    else if(data_direction == ATTR_READ)
    {
        uint32_t count = TamperCount.Magnetic_Count + 
                TamperCount.Neutral_Count + 
                TamperCount.UpperCoverRemoved_Count + 
                TamperCount.LowerCoverRemoved_Count;
        
         *datap = count & 0xFF;
         datap++;
         *datap = (count>>8) & 0xFF;
         datap++;
         *datap = (count>>16) & 0xFF;;
         datap++;
         *datap = (count>>24) & 0xFF;;
        
       
//       *datap = EnergyLoggingTime ; //Minute 
    }
}
//TODO: finish this 
void changeLoadProfileCapturePeriod(void *data,int data_direction)
{
    uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      EnergyLoggingTime = *datap;
      write_to_eeprom(&EnergyLoggingTime,(uint8_t *)0,setEnergyLogTime); 
//      MAXActivePowerTripPoint = *datap++;
//      MAXActivePowerTripPoint |= ((uint32_t)(*datap++)<<8);
//      MAXActivePowerTripPoint |= ((uint32_t)(*datap++)<<16);
//      MAXActivePowerTripPoint |= ((uint32_t)(*datap++)<<24);
    }
    else if(data_direction == ATTR_READ)
    {
//       uint32_t temp = TamperCount.Magnetic_Count + TamperCount.Neutral_Count + TamperCount.LowerCoverRemoved_Count + TamperCount.UpperCoverRemoved_Count;
//         
//      *datap = temp & 0xFF;
//       datap++;
//      *datap = (temp>>8) & 0xFF;
//      *datap++;
//      *datap = (temp>>16) & 0xFF;
//      *datap++;
//      *datap = (temp>>24) & 0xFF;
//      *datap++;
      *datap= EnergyLoggingTime;
    }
}

void changeMeterSerialNumber(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;

//      for(;i<=Meter_Sr_No[0];i++)
//        Meter_Sr_No[i] = *datap++;
      if(length < Meter_Sr_No[0])     
      {        
        memcpy(&(Meter_Sr_No[1]),data,length+1);
        //setDeviceIDs(Meter_Sr_No,0);
        uint8_t temp8 = 0;
        write_to_eeprom(&Meter_Sr_No,&temp8,setDeviceIDs);
      }
      else
      {
        //some kind of error 
      } 
    }
    else if(data_direction == ATTR_READ)
    {      
      memcpy(data,Meter_Sr_No,Meter_Sr_No[0]+1);      
    }
}
void changeDeviceId2(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;

//      for(;i<=Device_ID_2[0];i++)
//        Device_ID_2[i] = *datap++;
      if(length < Device_ID_2[0])     
      {
        memcpy(Device_ID_2,data,length+1);
//        setDeviceIDs(Device_ID_2,2);
        uint8_t temp8 = 2;
        write_to_eeprom(&Device_ID_2,&temp8,setDeviceIDs); 
      }
      else
      {
      }
      
    }
    else if(data_direction == ATTR_READ)
    {      
      memcpy(datap,Device_ID_2,Device_ID_2[0]+1);
     
    }
}
void changeDeviceId3(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
    if(data_direction == ATTR_WRITE)
    {
       uint8_t length = *datap++;

//      for(;i<=Device_ID_3[0];i++)
//        Device_ID_3[i] = *datap++;
      if(length < Device_ID_3[0])     
      {
        memcpy(Device_ID_3,data,length+1);
//        setDeviceIDs(Device_ID_3,3);
        uint8_t temp8 = 3;
        write_to_eeprom(&Device_ID_3,&temp8,setDeviceIDs); 
      }
      else
      {
      }
      
    }
    else if(data_direction == ATTR_READ)
    {      
      //memcpy(datap,Device_ID_3,Device_ID_3[0]+1);
    }
}
void changeDeviceId4(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=0;
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;

//      for(;i<=Device_ID_4[0];i++)
//        Device_ID_4[i] = *datap++;

      if(length < Device_ID_4[0])     
      {
        memcpy(Device_ID_4,data,length+1);
//        setDeviceIDs(Device_ID_4,4);
        uint8_t temp8 = 4;
        write_to_eeprom(&Device_ID_4,&temp8,setDeviceIDs); 
      }
      else
      {
      }
      
    }
    else if(data_direction == ATTR_READ)
    {      
      memcpy(datap,Device_ID_4,Device_ID_4[0]+1);
    }
}
void changeDeviceId5(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;

//      for(;i<=Device_ID_5[0];i++)
//        Device_ID_5[i] = *datap++;

      if(length < Device_ID_5[0])     
      {
        memcpy(Device_ID_5,data,length+1);
//        setDeviceIDs(Device_ID_5,5);
        uint8_t temp8 = 5;
        write_to_eeprom(&Device_ID_5,&temp8,setDeviceIDs);
      }
      else
      {
      }

    }
    else if(data_direction == ATTR_READ)
    {      
      memcpy(datap,Device_ID_5,Device_ID_5[0]+1);
    }
}
void changeDeviceId6(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
   
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;
//      for(;i<=Device_ID_6[0];i++)
//        Device_ID_6[i] = *datap++;

      if(length < Device_ID_6[0])     
      {
        memcpy(Device_ID_6,data,length+1);
//        setDeviceIDs(Device_ID_6,6);
        uint8_t temp8 = 6;
        write_to_eeprom(&Device_ID_6,&temp8,setDeviceIDs);
      }
      else
      {
      }
    }
    else if(data_direction == ATTR_READ)
    {      
      memcpy(datap,Device_ID_6,Device_ID_6[0]+1);
    }
}
void changeDeviceId7(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
   
    if(data_direction == ATTR_WRITE)
    {
      uint8_t length = *datap++;
//      for(;i<=Device_ID_7[0];i++)
//        Device_ID_7[i] = *datap++;

      if(length < Device_ID_7[0])     
      {
        memcpy(Device_ID_7,data,length+1);
//        setDeviceIDs(Device_ID_7,7);
        uint8_t temp8 = 7;
        write_to_eeprom(&Device_ID_7,&temp8,setDeviceIDs);        
      }
      else
      {
      }
    }
    else if(data_direction == ATTR_READ)
    {      
      memcpy(datap,Device_ID_7,Device_ID_7[0]+1);
    }
}
void changeDeviceId8(void *data,int data_direction)
{
    uint8_t *datap =  data;
//    uint8_t i=1;
   
    if(data_direction == ATTR_WRITE)
    {
     uint8_t length = *datap++;
//      for(;i<=Device_ID_8[0];i++)
//        Device_ID_8[i] = *datap++;
      if(length < Device_ID_8[0])     
      {
        memcpy(Device_ID_8,data,length+1);
//        setDeviceIDs(Device_ID_8,8);
        uint8_t temp8 = 8;
        write_to_eeprom(&Device_ID_8,&temp8,setDeviceIDs);        
      }
      else
      {
      }
    }
    else if(data_direction == ATTR_READ)
    {      
      memcpy(datap,Device_ID_8,Device_ID_8[0]+1);
    }
}
//uint8_t Temp_EEPROM_data_Buffer[640];