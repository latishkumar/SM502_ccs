#include "BatteryMonitor.h"

#pragma message("compiling 2")
uint16_t CurrentBatteryLevel = 7;
uint16_t btemp;
extern uint8_t OperatingMode;
/**
* checks the battery level using the internal harware battry monitor
* TODO. this does not tell us the correct level of the battery, Because battery's discharge 
*       varies from one type of battery to the other.
*       so come up with some algorithm that takes the actuall property of the battery into consideration
*       and find the battery level. 
*
*/

uint8_t CheckBattery()
{
//  if(OperatingMode == 1){
     //check for AUX2DRP interrupt flag 
    AUXCTL0 = AUXKEY ;
    uint8_t z =0,w=AUXCTL1;
    btemp = AUXCTL2;
    CurrentBatteryLevel  = (btemp>>8) & 7; 
    if((w & AUX2OK) == AUX2OK)
    {
       z = 1;
    }
    #pragma message("compiling 4")
    //battry not ok for the CurrentLevel so check for the next lower level

    
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
    
//  }else
//    return 0;
    
}
#pragma message("compiling 5")