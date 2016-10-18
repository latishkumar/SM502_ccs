


#include "Relay.h"
#include "Status.h"
#include "EventTypes.h"
#include "Logger.h"
#include <stdio.h>
#include "IEC62056/IEC62056_UART.h"
//#include "graphics.h"


extern MeterStatus status;
extern uint32_t ConsumptionSinceLastBilling;
extern uint32_t power_brakes[];
int Disconnect = 0;
uint8_t temp;
extern rtc_t rtcc;

uint8_t AutoConnect = 1; 
uint8_t LastDisconnectReason; // EEPROM

uint8_t RelayTariffAutoConDisconEnabled = 1; //from EERPOM

uint8_t connect_timeout  = 10;
volatile uint8_t connect_counter = 0;
volatile uint8_t start_connect_timeout = 0;


/*
  time to auto reset the rellay ports to off state in second
*/
volatile uint8_t reset_timeout = 1;
volatile uint8_t reset_counter = 0;
volatile uint8_t reset_relay_required = 0;

uint8_t relayState = 0;


extern rms_current_t over_current_temp ;

extern rms_voltage_t under_voltage_temp;
extern rms_voltage_t over_voltage_temp ;

extern frequency_t over_frequency_temp ;
extern frequency_t under_frequency_temp;
extern power_t over_power_temp  ;
extern int32_t lowtariff_temp;

uint32_t desconnect_val;
uint8_t disconnect_verified = 0;
/** Initaializes the realy module by configuring the relay ports to output and reading the last relay status from EEPROM and 
*         changing the current relay state to that state.  
*
*/
void InitRelay()
{
  
  RelayOffDir |=RelayOffBIT;
  RelayOnDir |=RelayOnBIT;
  
  RelayOffPort &= ~RelayOffBIT;
  RelayOnPort &= ~RelayOnBIT;
  
  status.ConnectCommandRecived = 0;
  status.DisconnectCommandRecived = 0;
  
  uint8_t rstatus=0;
//   z = getRelayStatus(&rstatus,&AutoConnect);//read relay status from EEPROM 
  validation_arg_t validation_arg;
  validation_arg.validate = 0;
  uint8_t z= read_from_eeprom(&rstatus,&AutoConnect,getRelayStatus,&validation_arg); 
  if(z == 1) //the reading from the EEPROM is fine so use it 
  {
    status.RelayStatus = rstatus;
    
    if(status.RelayStatus == 0 && AutoConnect == 0)
    {
      DisconnectMeter(MeterDisconnectAfterRestart);
      disconnect_verified=0;
    }
    else if( AutoConnect == 1 )
    {
      LastDisconnectReason = MeterConnectAfterRestart;
      status.RelayStatus = 0; //this is to use the existing method  to
      disconnect_verified =1;
    }
    
  }
  else //the reading is not right, so just auto connect the Relay
  {
     status.RelayStatus = 0;
     AutoConnect = 1;
     //TODO. save this error condition 
  }
}
/** reads the current system status and open or close the relay.
*   The relay has theree main states.
*       1. connected : this is the state when the relay is closed            
*       2. disconnected : this is the state where the relay is open 
*       3. ready to auto connect : this is the state where the relay is open but will auto connect if the reason for disconnecting is removed. 
*                                  the relay is moves to this when transitioning from disconnected to connected state. At this state the system will 
*                                  check if other operating condigions in the meter are fine (@see CheckConnectionIsEnabled) before connecting the meter       
*/
void UpdateRelayStatus()
{
     if(status.RelayStatus == 0) //relay disconnected      
     {
         if(AutoConnect == 1)
         {
           status.RelayStatus = 2; //ready to connect: the 
         }
         
         if(status.ConnectCommandRecived == 1)
         {
           status.ConnectCommandRecived = 0;
           status.DisconnectCommandRecived = 0;
           
           
           ConnectMeter(MeterCommandConnected);
           AutoConnect = 1;
//           status.ConnectCommandRecived = 0;            
         }
     }
     else if(status.RelayStatus == 1) //relay is connected 
     {
        if(IsDisconnectRequired(&temp))
        {
            status.DisconnectCommandRecived = 0;
            status.ConnectCommandRecived = 0;
            
            LastDisconnectReason = temp;
            DisconnectMeter(temp);	            
        }
     }
     else if(status.RelayStatus == 2) //relay is ready to AutoConnect, the realy will connect if the reason for the disconnect is removed 
     {
       if(CheckConnectionIsEnabled(&temp) == 1)
       {
         if(start_connect_timeout ==0)
         {
            start_connect_timeout = 1;
         }
         if(connect_counter >= connect_timeout) 
         {
            
            ConnectMeter(temp);
            connect_counter = 0;
            disconnect_verified = 0;
            start_connect_timeout = 0;
         }
       }
       else
       {
         if(disconnect_verified ==0)
         {
           DisconnectMeter(MeterDisconnectAfterRestart);
           start_connect_timeout = 0;
           connect_counter = 0;
           disconnect_verified = 1;
         }
       }
     }
     else
     {
     }
}

/** check the system status to see if the relay can be connected safely. this event is called in when the relay is dissconnected
*       
*/
uint8_t CheckConnectionIsEnabled(uint8_t *EventCode)
{
  //check for the tariff rate
  //chekc for connect command 
  //configration removed
  uint8_t tm= 0;
  if(RelayTariffAutoConDisconEnabled == 1)
    tm = CheckTariff(power_brakes,&ConsumptionSinceLastBilling);//status.LowTariff;
  
  if( (tm == 0 && status.ConfigurationDetectedStatus == 0 && status.OverCurrentStatus == 0 && status.OverVoltageStatus == 0 &&
       status.ActivePowerExcededStatus == 0 ))
  {
    *EventCode = LastDisconnectReason;//modify this 
    return 1;
  }
  
  return 0;
}

/** Closes the remote switch and logs the connect event
*   @param EventCode: the reason why the meter is connected 
*/
void ConnectMeter(uint8_t EventCode) {

    
    relayState = 1;
    reset_relay_required = 1; 
    
#ifdef PHYSICAL_RELAY_DISCONNECT_ENABLED    
    RelayOffPort &= ~RelayOffBIT;
    RelayOnPort &= ~RelayOnBIT;
//    WriteChar('C',20,2,font5x8);
    RelayOnPort |= RelayOnBIT;
    
#endif    
    reset_counter = 0;
      
    /*
      log connect event 
    */
   //log disconnect event
    EventLog l;
    l.EventCode = EventCode;
    l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
    l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
                    + l.timeStump.TimestampUp);
    l.value = 0;
    //l.StartAddress = 0;	//last event log address
//	uint8_t z = logEvent(&l);
    uint8_t z = write_to_eeprom(&l,(uint8_t *)0,logEvent);
    if(z==0)
    {
      AddError(EEPROM1_Error);
      //eeprom Error 
    }   
    status.RelayStatus = 1;
    
    if(MeterCommandConnected == EventCode)
    {
//      setRelayStatus(status.RelayStatus,1);
      uint8_t temp8 = status.RelayStatus;
      uint8_t aconn = 1;
      write_to_eeprom(&temp8,&aconn,setRelayStatus);
    }
}

/** Opens the remote switch and logs disconnect event with the reason of disconnection
*   @param EventCode: the reason why the meter is disconnected 
*/
void DisconnectMeter(uint8_t EventCode) {
    
#ifdef LOG_
//  printf("\nR:-%d",EventCode);
  IEC_AddtoTXBuffer('\n');
  IEC_AddtoTXBuffer('R');
  IEC_AddtoTXBuffer(':');
  IEC_AddtoTXBuffer('-');
  IEC_AddtoTXBuffer(EventCode);
  IEC_Start_SendBuffer();            
#endif
  
  
    relayState = 0;
    reset_relay_required = 1;
//    WriteChar('D',20,2,font5x8);  
#ifdef PHYSICAL_RELAY_DISCONNECT_ENABLED      
    RelayOffPort &= ~RelayOffBIT;
    RelayOnPort &= ~RelayOnBIT;
    RelayOffPort |= RelayOffBIT;
#endif  
    
    reset_counter = 0;
    LastDisconnectReason = EventCode;
    //log disconnect event
    EventLog l;
    l.value = desconnect_val;
    l.EventCode = EventCode;
    l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
    l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
                    + l.timeStump.TimestampUp);
    //l.StartAddress = 0;	//last event log address
//    uint8_t z = logEvent(&l);
    uint8_t z = write_to_eeprom(&l,(uint8_t *)0,logEvent);
    if(z==0)
    {
      AddError(EEPROM1_Error);
      //eeprom Error 
    }
    status.RelayStatus = 0;//relay is disconnected 
    
    if(MeterCommandDisconnect == EventCode)
    {
//      setRelayStatus(status.RelayStatus,0);
      uint8_t temp8 = status.RelayStatus;
      uint8_t aconn = 0;
      write_to_eeprom(&temp8,&aconn,setRelayStatus);
    }    
}

/** check wheter the sytem need to open the remote switch or not based on the sytem status. 
*   @return  1 if the system need to open the remote swith
*            0 otherwise      
*/  
uint8_t IsDisconnectRequired(uint8_t *EventCode)
{
     if( status.DisconnectCommandRecived==1 )
     {
        *EventCode = MeterCommandDisconnect;
        AutoConnect = 0;
        return 1;
     }
     else if( status.LowTariff == 1 && RelayTariffAutoConDisconEnabled == 1)
     {
        *EventCode = MeterTariffDisconnected;
        desconnect_val = lowtariff_temp;
        AutoConnect = 1;      
	return 1;       
     }
     else if(status.OverVoltageTimedOutStatus == 1)
     {
       *EventCode = MeterOverVoltageDisconnect;
        AutoConnect = 1; 
        desconnect_val = over_voltage_temp;
        
       return 1;
     }
     else if(status.OverCurrentTimedOutStatus == 1)
     {
       *EventCode = MeterOverCurrentDisconnect;
        AutoConnect = 1; 
        desconnect_val = over_current_temp;
       return 1;
     }
     else if(status.ActivePowerExcededTimedOutStatus == 1)
     {
        *EventCode = MeterOverPowerDisconnect;
        AutoConnect = 1; 
        desconnect_val = over_power_temp;
       return 1;
     }
     else if(status.ConfigurationDetectedStatus == 1)
     {
       *EventCode = MeterConfigrationDisconnect;
       AutoConnect = 1; 
       return 1;
     }
      
     *EventCode  = 0;
     return 0;
}

/** sets the outputs on the relay ports to zero so that the relay winding is deenergized. 
*   This method is called every time after a connect or disconnect command is executed.    
*/
void ResetRelayPort()
{

  RelayOnPort &= ~RelayOnBIT;
  RelayOffPort &= ~RelayOffBIT;
  reset_relay_required = 0;
  reset_counter = 0;
  
}
