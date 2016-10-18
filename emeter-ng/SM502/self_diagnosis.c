

//#include "graphics.h"
#include "Logger.h"
#include "self_diagnosis.h"
#include "utilities.h"
#include "../emeter-rtc.h"
#include <stdlib.h>

HardwareStatus hardware_status;

uint8_t sdTemp;
uint8_t sdTempData;

extern rtc_t rtc_temp;
extern uint8_t MAX_LOG_RETRAY ;
extern volatile uint8_t rtc_init;
/**
* this method checks if EEPROM 1 and EEPROM 2 are fine 
* and rtc time is ok or not by comparing it with log taken before entering LPM
*/
uint8_t DiagnosisMeter()
{
  uint8_t z=0;
  sdTemp = 0;
  
  //eeprom 1
  for(;z<MAX_LOG_RETRAY;z++)
  {
    sdTemp = EEPROM2_WriteInt8('A',EEPROM1_DiagAddressStart, 1);
    if(sdTemp == 0)
    {      
      continue;
    }
    else
    {     
     sdTemp = EEPROM2_ReadInt8(EEPROM1_DiagAddressStart,1,&sdTempData);
     if(sdTemp!=0 && (sdTempData == 'A'))
     {
       break;
     }
     else
     {       
       continue;
     }
    }
  }
  
  if(z<MAX_LOG_RETRAY)
  {
    hardware_status.EEPROM1_Status = 1;
  }
  else
  {
    hardware_status.EEPROM1_Status = 0;
  }
  
  //eeprom 2
  for(z=0;z<MAX_LOG_RETRAY;z++)
  {
    sdTemp = EEPROM2_WriteInt8('A',EEPROM2_DiagAddressStart, 1);
    if(sdTemp == 0)
    {
      hardware_status.EEPROM2_Status = 0;
      continue;
    }
    else
    {
     
     sdTemp = EEPROM2_ReadInt8(EEPROM2_DiagAddressStart,1,&sdTempData);
     if(sdTemp!=0 && (sdTempData == 'A'))
     {
        hardware_status.EEPROM2_Status = 1;
       break;
     }
     else
     {
       
       hardware_status.EEPROM2_Status = 0;
       continue;
     }
    }
  }
  
  if(z<MAX_LOG_RETRAY)
  {
    hardware_status.EEPROM2_Status = 1;
  }
  else
  {
    hardware_status.EEPROM2_Status = 0;
  }
  
  
  //rtc 
 //TODO. RTC check is dependent on the EEPROM 
  if(hardware_status.EEPROM1_Status == 1) //Do RTC Diagnosis only if  EEPROM 1 is ok 
  {
      z =  getRTCCheck(&rtc_temp);
      while(rtc_init ==0)
      {
             #if defined(USE_WATCHDOG)
                kick_watchdog();
             #endif
      }
       
      if(is_time_valid(&rtc_temp))
      {

      }
      else{
        
      }
  }


  return 0;
}
