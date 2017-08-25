#include "battery_monitor.h"

uint16_t CurrentBatteryLevel = 7;
uint16_t btemp;
extern uint8_t OperatingMode;
/**
* checks the battery level using the internal hardware battery monitor
* TODO. this does not tell us the correct level of the battery, Because battery's discharge 
*       varies from one type of battery to the other.
*       so come up with some algorithm that takes the actual property of the battery into consideration
*       and find the battery level. 
*
*/

uint8_t CheckBattery()
{
    AUXCTL0 = AUXKEY ;
    uint8_t z =0,w=AUXCTL1;
    btemp = AUXCTL2;
    CurrentBatteryLevel  = (btemp>>8) & 7; 
    if((w & AUX2OK) == AUX2OK)
    {
       z = 1;
    }
    //battery not ok for the CurrentLevel so check for the next lower level

    if(z == 0)  
    {
          if(CurrentBatteryLevel>=1)
          {
            AUXCTL2 = AUXCTL2 & 0xF8FF;
            AUXCTL2  |= ((CurrentBatteryLevel-1)<<8);
          }
          CurrentBatteryLevel-=1;
    }
    else
    {
          if(CurrentBatteryLevel<=6)
          {
            AUXCTL2 = AUXCTL2 & 0xF8FF;
            AUXCTL2  |= ((CurrentBatteryLevel+1)<<8);
          }
    }
    
    AUXCTL0 = 0;  
    
    return CurrentBatteryLevel;  
}

