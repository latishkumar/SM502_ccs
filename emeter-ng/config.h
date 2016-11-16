

#ifndef _CONFIG_H
#define _CONFIG_H
/**
*  Contains list of max and min allowed values for different configurations in the system
*  This are used for validating user configuration data.
*
*/
#define MIN_ACTIVE_POWER_CONSIDERED_ZERO 250 //2.5w
#define MIN_REACTIVE_POWER_CONSIDERED_ZERO 250 //2.5Var

#define MIN_PHASE_CURRENT_CONSIDERED_ZERO 3 //mA
#define MIN_NUTRAL_CURRENT_CONSIDERED_ZERO 20 //mA

#define MIN_PHSE_PRESENCE_RMS_VOLTAGE 50 //V


#define ZeroAcVoltage 100L //v -+ when the ac goes bellow this value we will assume that we have no voltage
#define HardwareVoltageScaler 2694L //*2^-20
#define ZeroAcVoltageDigital ((ZeroAcVoltage * HardwareVoltageScaler)<<3)/281L
#define ZeroAcVoltageDigitalNeg -ZeroAcVoltageDigital




#define OperatingModeMin 1
#define OperatingModeMax 3


#define AutoDisplayTimeMin 10000L //msec 
#define AutoDisplayTimeMax 25000L //msec  


#define BackToAutoModeTimeMin 10000 //msec 
#define BackToAutoModeTimeMax 65000

#define LanguageMin 0
#define LanguageMax 2


#define meter_connected_phaseMin 'R'
#define meter_connected_phaseMax 'T'

#define ThrusholdVoltageSagPerMin 1500 //15%
#define ThrusholdVoltageSagPerMax 4500 //30%


#define ThrusholdVoltageSwellPerMin 1000 //10%
#define ThrusholdVoltageSwellPerMax 2000 //20%

#define Nominal_VoltageMin 200 //V
#define Nominal_VoltageMax 300 


#define VoltageThresholdLongPowerFailePerMin 2000 //20%
#define VoltageThresholdLongPowerFailePerMax 7000 //70%


#define billingSchemaMin 0
#define billingSchemaMax 1


#define EnergyLoggingTimeMin 10 //10min
#define EnergyLoggingTimeMax 25 


#define OverVoltageTripDurationMin 1 //1sec 
#define OverVoltageTripDurationMax 5


#define OverCurrentTripDurationMin 0 //0sec
#define OverCurrentTripDurationMax 5

#define OverFrequencyTripDurationMin 4 //sec
#define OverFrequencyTripDurationMax 15 //sec

#define ActivePowerTripDurationMin 2 //sec
#define ActivePowerTripDurationMax 10

#define UnderVoltageTripDurationMin 5 //sec
#define UnderVoltageTripDurationMax 15 //sec

#define UnderFrequencyTripDurationMin 5 //sec
#define UnderFrequencyTripDurationMax 15 //sec


#define CoverRemovalTripDurationMin 1 //sec
#define CoverRemovalTripDurationMax 10 //sec

#endif 
