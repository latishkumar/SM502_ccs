#ifndef Hardware_limits_c
#define Hardware_limits_c

#include <stdint.h>




#define AbsoluteMaxIRMS  80    /*[M.G] Changed from 50 to 80 */
#define AbsoluteMaxVRMS  300
#define AbsoluteMaxActivePower 13000 //WH
#define AbsoluteMaxFrequency 70

//over voltage and current ranges and Frequency,change the trip ranges to percent 
//uint8_t  MAX_IRMS = 80000;//mA/*[M.G] Changed from 50 to 80 */ //n
/*uint16_t MAX_VRMS = 280; *///n
uint8_t  MAX_Frequency = 53;
uint32_t MAXActivePowerTripPoint = 11500; //w watt steps //n
uint32_t MAXActivePowerExportTripPoint = 1250000; //in 0.01 w steps

//values in percent 
//uint16_t ThrusholdVoltageSagPer = 3000;//30% percent //n
//uint16_t ThrusholdVoltageSwellPer = 1500;//15% percent //n
//uint16_t VoltageThresholdLongPowerFailePer = 5600;//56% //n


//under voltage and current ranges 
//uint8_t MIN_VRMS = 170; //n
uint8_t MIN_Frequency = 47;

uint16_t ThresholdLongPowerFaile = 115;
//uint16_t TimeThresholdLongPowerFaile = 600; //time in second //n
/*uint16_t Nominal_Voltage = 230;//V*/ //n
uint8_t Basic_Current = 5; //A
uint8_t Nominal_Frequency = 50;//HZ
uint16_t Nominal_ActivePower = 5000; //KW


uint16_t ActivePowerTripDuration    =  5; //in seconds 
uint16_t OverCurrentTripDuration    =  1;
//uint16_t OverVoltageTripDuration    =  1; //n

uint16_t OverFrequencyTripDuration  =  10;
//uint16_t UnderVoltageTripDuration   =  10;//n
uint16_t UnderFrequencyTripDuration =  10;


uint16_t CoverRemovalTripDuration = 1;
uint16_t NeutralTamperTripDuration = 1; //1 min

uint32_t Current_Thrushold = 625; // current balance multiplied by 100 i.e 6.25% & 12.5% 

/*uint16_t NumberOfLongPowerFails = 0; //use method to read from EERPOM*///n

#endif 
