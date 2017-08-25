/*
 * Logger.h
 *
 *  Created on: Apr 16, 2013
 *      Author: Donayam
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define Seg1START 0
#define Seg1END 10

#include <stdint.h>
#include "Tariff.h"

#include "utilities.h"   
//#include <emeter-toolkit.h>
#include "I2C2EEPROM.h"   
#include "emeter-structs.h"
#include "config.h"
#include "Status.h"
#include "errors.h"
#include "EventTypes.h"
#define EEPROM_INIT_MAX_RETRAY 5    
#define Max_DeviceAddress 32

extern uint8_t mac_address[];
extern MeterStatus status;
/*
 *
 *  16 byte password
 *  5 passwords
 *  which gives
 *
 * a total of
 * 16 * 5 = 80 bytes we will use one page for all the  passwrods
 *
 The following passwords are supported
                 ==>Public Password                   :- has the permission to read what is displayed on the LCD 
                 ==>Reading password                  :- has the permission to read configration and setting of the meter, except password reading and setting 
                 ==>Management Password               :- has the permission to read and write configration data and Reading and Tamper Clearing Password,and also meter resetting to the manufacturer's default setting and configration 
                 ==>firmware upgrading password       :- has the permission to to upgrade the meter firmware and to change its own password 

 */

enum password_types{
    Public_Password = 0,
    Reading_Password = 1,   
    Management_Password = 3,
    Firmware_Upgrading_Password = 4
};

typedef struct
{
	uint8_t password[18];//password number, lenght, password 
        
}Password_128;

#define MaxPasswords 4
#define Password_Length 18
static const Password_128 default_passwords[4]= {
  {0,14,'D','E','F','A','U','L','T','P','A','S','S','W','O','R'},
  {1, 8,'H','E','L','L','O','D','V','E',' ',' ',' ',' ',' ',' '},
  {2,14,'D','E','F','A','U','L','T','P','A','S','S','W','O','R'},
  {2,14,'D','E','F','A','U','L','T','P','A','S','S','W','O','R'}
};


#define Password_keys_Length 16 
static const Password_128 default_keys[4] = {
  {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10},
  {0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20},
  {0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30},
  {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40}
};




extern unsigned long FirmwareVersion; // 4 byte

extern unsigned long last_hourly_energy_log_address;
extern unsigned long last_standard_event_log_address;
extern unsigned long LastEnergyBillingCuttOffLogAddress;
extern unsigned long last_daily_snapshot_log_address;
extern unsigned long last_fraud_event_log_address;
extern unsigned long last_power_qual_event_log_address;
extern unsigned long last_common_event_log_address;
extern unsigned long last_firmware_event_log_address;
extern unsigned long last_synchronization_event_log_address;
extern unsigned long last_disconnect_event_log_address;

/*
 *	EventCode:-  will have 1byte length, 
 * ==> A total of 128 events will be supported
 *	Each Event log will require a total of 12 byte memory space
 * 
 */
typedef struct
{
   uint32_t value;     // 4 byte 
   uint8_t EventGroup; // 1 byte
   uint8_t EventCode;  // 1 byte
   uint8_t Checksum;   // 1 byte
   TimeStump timeStump;// 5 byte
   //unsigned long EventData;//4byte 
   //unsigned long StartAddress;
}EventLog;
#define EventLogSize 12
typedef struct
{
	uint8_t event_code;   // 1 byte
	uint8_t checksum;     // 1 byte
	TimeStump time_stamp; // 5 byte
}event_log; //for standard, fraud detection, power quality, demand, communication events

typedef struct
{
	uint8_t event_code;			  // 1 byte
    uint8_t checksum;             // 1 byte
	TimeStump begin_time_stamp;   // 5 byte
	TimeStump end_time_stamp;	  // 5 byte
}time_bound_event_log; //finished power quality,synchronization event
typedef struct
{
	uint8_t event_code;			// 1 byte
	uint8_t checksum;			// 1 byte
	uint8_t active_firmware[5]; // 5 byte
	//uint8_t former_firmware[5]; // 5 byte
	TimeStump time_stamp;		// 5 byte
}firmware_event_log;
typedef struct
{
	uint8_t event_code;			       // 1 byte
	uint8_t checksum;			       // 1 byte
	uint8_t disconnect_control_status; // 1 byte
	TimeStump time_stamp;			   // 5 byte
}disconnect_event_log;
#define EVENT_LOG_TYPE_SIZE 7
#define TIME_BOUND_EVENT_LOG_TYPE_SIZE 12
#define FIRMWARE_EVENT_LOG_TYPE_SIZE 12
#define DISCONNECT_EVENT_LOG_TYPE_SIZE 8

#define init_common_event_logs {0, 0, {0, 0}}
#define init_time_bounded_event_logs {0, 0, {0, 0}, {0, 0}}
#define init_firmware_event_log {0, 0, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0}}
#define init_disconnect_event_log {0, 0, 0, {0, 0}}
#define init_EventLog {0, 0,0, 0, { 0, 0}}

enum EventGroup_Types{
 
  DISCONNECT_CONTROLL_EVENT_TYPES = 2,
  POWER_QUALITY_EVENT_TYPES = 3,
  FRAUD_EVENT_TYPES = 4,
  COMMEN_EVENT_TYPES = 6,
};


#define EnergyLogSize 22

/*
 * Active Energy 4byte
 * Reactive Energy 4byte
 * timeStump  5byte
 * CRC  1byte
 *
 * this requires a total of 22byte
 *
 * to store energy reading for 45 days at a rate of 15 minute log
 * a total of 61kbyte memory is required
 */
typedef struct
{
    unsigned long active_energy; //4byte   // consumed active energy import
    unsigned long reactive_energy_QI; //4byte // positive reactive energy ,
    unsigned long active_power;//4 byte
    unsigned long reactive_energy_QIV; //4 byte [E.E]-was Voltage and changed to support negative reactive energy

    TimeStump timeStump; //5byte

    uint8_t CRC; //1byte
}EnergyLog;
#define init_EnergyLog {0, 0, 0, 0, { 0, 0}, 0}
/*
 * Hourly load profile
 * Incremental energy
 * 18 bytes
 */
#define INCREMENTAL_ENERGY_LOG_TYPE_SIZE 18
typedef struct
{
    uint16_t inc_active_import_energy; //2 byte    consumed active energy import
    uint16_t inc_active_export_energy; //2 byte    consumed active energy export
    uint16_t inc_reactive_energy_QI;   //2 byte    consumed reactive energy QI
    uint16_t inc_reactive_energy_QII;  //2 byte    consumed reactive energy QII
    uint16_t inc_reactive_energy_QIII; //2 byte    consumed reactive energy QIII
    uint16_t inc_reactive_energy_QIV;  //2 byte    consumed reactive energy QIV
    TimeStump timestamp;               //5 byte    timestamp
    uint8_t crc;                       //1 byte    checksum
}hourly_energy_log_t;
#define init_hourly_energy_log {0, 0, 0, 0, 0, 0,{ 0, 0}, 0}

typedef struct
{
    uint32_t active_import_energy; //4 byte    consumed active energy import
    uint32_t active_export_energy; //4 byte    consumed active energy export
    uint32_t reactive_energy_QI;   //4 byte    consumed reactive energy QI
    uint32_t reactive_energy_QII;  //4 byte    consumed reactive energy QII
    uint32_t reactive_energy_QIII; //4 byte    consumed reactive energy QIII
    uint32_t reactive_energy_QIV;  //4 byte    consumed reactive energy QIV
    TimeStump timestamp;           //5 byte    time stump
    uint8_t crc;                   //1 byte
} daily_energy_log_t;
#define init_daily_energy_log {0, 0, 0, 0, 0, 0,{ 0, 0}, 0}
#define DAILY_SNAPSHOT_LOG_TYPE_SIZE 30
typedef struct
{
    const uint8_t *template;
    const uint16_t *log_column_size;
    uint16_t sz_template;
    uint32_t start_log_address;
    unsigned long last_log_address;
    unsigned long end_log_address;
    uint16_t maximum_event_logs;
    uint8_t overlap_status;
    uint8_t log_size;
    uint8_t offset;
}log_search_params_t;

log_search_params_t log_search_params;
#define EnergyBillingCutoffDayLogSize 47
typedef struct
{
    TimeStump BillingPeriodStart; //5byte
    TimeStump BillingPeriodEnd; //5byte
    
    uint32_t ActiveEnergy_QI;
    uint32_t ActiveEnergy_QII;
    uint32_t ActiveEnergy_QIII;
    uint32_t ActiveEnergy_QIV;
    
    uint32_t ReActiveEnergy_QI;
    uint32_t ReActiveEnergy_QII;
    uint32_t ReActiveEnergy_QIII;
    uint32_t ReActiveEnergy_QIV;
    
    uint32_t balance;    
}EnergyBillingCutoffDayLog;



/*
 *	each tamper count will be of size 8 bit
 *  this will be stored in single location and updated when tamper occurs
 */
typedef struct
{
	uint8_t Magnetic_Count; // 8 bit
	uint8_t UpperCoverRemoved_Count; // 8 bit
	uint8_t LowerCoverRemoved_Count; //8 bit
	uint8_t Neutral_Count; //8 bit
	uint8_t Tampercount_Checksum; // 8 bit
	uint8_t ConfigrationPort_Count;
}Tamper_Count;


/*
 *
 * This is the last status of the logger module
 * used by the program to retransmit the data when an error occurs
 *
 */
typedef struct
{
    uint32_t  LastLogAddress;
    uint8_t isWritting;
    uint8_t isOperationCompleate;
    EventLog lastLogEvent;
    EnergyLog lastLogEnergy;

}loggStatus;

enum OperatingModes{
  SHIPPING_MODE = 0,
  ACTIVE_MODE,
  CONFIGURATION_MODE,
  LOW_POWER_MODE
};

/*
* Data read from eeprom 
*  outputData : data read frpm eeprom 
*  type : data size , 8 byte(1),16 byte(2). 32 byte(3)
*  address:- where to read the data 
*/
typedef struct{
    void *outputData;
    uint8_t type;
    uint32_t address;
}ReadArgument_t;

/*
*  Data written from eeprom 
*  outputData : data read frpm eeprom 
*  type : data size , 8 byte(1),16 byte(2). 32 byte(3)
*  address:- where to read the data 
*/
typedef struct{
  
    const void *outputData;
    uint8_t type;
    uint32_t address;
    
}WriteArgument_t;

/*
* max:- the maxium allowed value for this data
* min:- the minimum allowed value for the data
* default_val :- 
*/
typedef struct{
  
   uint32_t max;
   uint32_t min;
   uint32_t default_val;
   uint32_t validate;
   uint32_t validate_by_reading_back;//for write 
   uint8_t arg_size;
}validation_arg_t;

#define validation_arg_t_default {0,0,0,0,0,0}

//
//int write_to_eeprom(int8_t(*write)(void *writeArgumentType),WriteArgument_t *writeType);
//int read_from_eeprom(int8_t(*read)(void *readArgumentType),ReadArgument_t *readType);

int write_to_eeprom(void *writeArgument_1,void *writeArgument_2,int8_t(*write)(void *writeArgument_1,void *writeArgument_2));//,void *validation_arg
int read_from_eeprom(void *readArgument_1,void *readArgument_2,int8_t(*read)(void *readArgument1,void *readArgument2),void *validation_arg);



/*
 * Last used Segment Address is used to store the last energy and event segment addresses
 * that where accessed. The next data to be written should start from one address higher than the current address.
 *
 * All numbers grater than 8byte in memory should be place with List Significant Byte First Order
 * Checksum is placed as the last byte of each memory
 */



#define MemoryFormattedIndicatorCode {0x2d,0x2d,0x2d,0x2d,0x2d} //0x2F

#define CONFIGURATION_BASE_ADDRESS 128ul //leave Address 0 out

#define MemoryFormattedAddressStart CONFIGURATION_BASE_ADDRESS
#define MEMORY_FORMATTED_INDICATOR_SIZE 8ul

#define MemoryFormattedAddressBackUpStart MemoryFormattedAddressStart + MEMORY_FORMATTED_INDICATOR_SIZE
#define MEMORY_FORMATTED_INDICATOR_BACK_SIZE 8ul

#define LastEnergyLogAddressStart  MemoryFormattedAddressBackUpStart + MEMORY_FORMATTED_INDICATOR_BACK_SIZE      //requires 4 byte.So end is address 4
#define LastEnergyLogAddress_SIZE 4ul 


#define LastEventLogAddressStart   LastEnergyLogAddressStart + LastEnergyLogAddress_SIZE   //5   //requires 4 byte.So end is address 9
#define LastEventLogAddress_SIZE 4ul


#define CurrentBalanceStart  LastEventLogAddressStart + LastEventLogAddress_SIZE// 24 //14		  //requires 5 byte.so end is 18
#define CurrentBalance_SIZE 5ul

#define FirmwareVersionStart  CurrentBalanceStart + CurrentBalance_SIZE //29 //19 		  //requires 4 byte.so end is 32
#define FirmwareVersion_SIZE  4ul

#define TamperCountAddressStart  FirmwareVersionStart + FirmwareVersion_SIZE //33 //20
#define TamperCountAddress_SIZE 8ul



#define EnergyOverlapFlagAddressStart TamperCountAddressStart + TamperCountAddress_SIZE //41 //requires 1 bit so use 1 byte. end 41
#define EnergyOverlapFlagAddress_SIZE 1ul

#define EventOverlapFlagAddressStart EnergyOverlapFlagAddressStart + EnergyOverlapFlagAddress_SIZE //42  //29  //requires 1 bit so use 1 byte. end 42
#define EventOverlapFlagAddress_SIZE 1ul

#define TempLastEnergyValueStart  EventOverlapFlagAddressStart + EventOverlapFlagAddress_SIZE //43       //Is last billed energy required 4 byte 
#define TempLastEnergyValue_SIZE  4ul  

//#define TempLastEnergyValueStart  LastEnergyValueStart + LastEnergyValue_SIZE //57    //Requires 4 byte , used by the tariff module 

#define RTC_CheckStart TempLastEnergyValueStart + TempLastEnergyValue_SIZE //47 //requires 8 byte stores last rtc_t 
#define RTC_Check_SIZE 8ul                          



#define BillingCutoffDateStart RTC_CheckStart + RTC_Check_SIZE //55  //requires 5 byte next is at 73   
#define BillingCutoffDate_SIZE 5ul //TODO. find out why this can't be

#define BillingCutoffEnergyLogOverlapFlagStart (BillingCutoffDateStart + BillingCutoffDate_SIZE) //60 //requires 1 byte next is at 75
#define BillingCutoffEnergyLogOverlapFlag_SIZE 1ul


#define LastBillingCuttoffLogAddressStart BillingCutoffEnergyLogOverlapFlagStart + BillingCutoffEnergyLogOverlapFlag_SIZE //61 //requires 4 byte next is at 80
#define LastBillingCuttoffLogAddress_SIZE 4ul

   
   
#define Manufacturing_DateTimeStart LastBillingCuttoffLogAddressStart + LastBillingCuttoffLogAddress_SIZE //65  //requires 8 byte next is at 88,  Test this 
#define Manufacturing_DateTime_SIZE 8ul

//TODO. write methods for this logger functions 

#define NumberOfConfigrationCounterStart Manufacturing_DateTimeStart + Manufacturing_DateTime_SIZE //73 //next is at 89
#define NumberOfConfigrationCounter_SIZE 1ul


#define lastConfigrationEventLogAddressStart NumberOfConfigrationCounterStart + NumberOfConfigrationCounter_SIZE //74 //requires 4 byte next is at 93
#define lastConfigrationEventLogAddress_SIZE 4ul

#define lastCalibrationEventLogAddressStart lastConfigrationEventLogAddress_SIZE + lastConfigrationEventLogAddressStart //78 //requires 4 byte next is at 97
#define lastCalibrationEventLogAddress_SIZE 4ul


#define NumberOfPowerFailerCountStart lastCalibrationEventLogAddressStart + lastCalibrationEventLogAddress_SIZE //82 //requires 2 byte next is at 99
#define NumberOfPowerFailerCount_SIZE 2ul
//end of write method 


#define ReferanceVoltageStart NumberOfPowerFailerCountStart + NumberOfPowerFailerCount_SIZE //84  //requires 1 byte next is at 100
#define ReferanceVoltage_SIZE 1ul

#define NominalCurrentStart ReferanceVoltageStart + ReferanceVoltage_SIZE //85 requires 1 byte next is at 101
#define NominalCurrent_SIZE 1ul

#define ReferanceFrequencyStart  NominalCurrentStart + NominalCurrent_SIZE // 86  //requires 1 byte next is at 102
#define ReferanceFrequency_SIZE 1ul


#define MinRMSVoltageStart  ReferanceFrequencyStart + ReferanceFrequency_SIZE // 87 //requires 2 byte next is at 104 . This value is the minimum voltage deviation in percent 
#define MinRMSVoltage_SIZE 2ul


#define MaxRMSVoltageStart MinRMSVoltageStart + MinRMSVoltage_SIZE //89 //requires 2 byte 
#define MaxRMSVoltage_SIZE 2ul


#define MaxRMSCurrentStart MaxRMSVoltageStart + MaxRMSVoltage_SIZE //91 //requires 1 byte 
#define MaxRMSCurrent_SIZE 1ul


#define MaxFrequencyStart MaxRMSCurrentStart + MaxRMSCurrent_SIZE //92 //requires 1 byte 
#define MaxFrequency_SIZE 1ul


#define MAXActivePowerTripPointStart MaxFrequencyStart + MaxFrequency_SIZE //93  //requires 2 byte next is at 110
#define MAXActivePowerTripPoint_SIZE 2ul

#define BillingSchemaStart MAXActivePowerTripPointStart + MAXActivePowerTripPoint_SIZE //95 //requires 1 byte 
#define BillingSchema_SIZE 1ul


#define ActivePowerTripDurationStart   BillingSchemaStart + BillingSchema_SIZE //96 // requires 2 byte in seconds 
#define ActivePowerTripDuration_SIZE     2ul


#define OverCurrentTripDurationStart   ActivePowerTripDurationStart + ActivePowerTripDuration_SIZE //98 //requires 2 byte 
#define OverCurrentTripDuration_SIZE  2ul

#define OverVoltageTripDurationStart   OverCurrentTripDurationStart + OverCurrentTripDuration_SIZE //100 //requires 2 byte 
#define OverVoltageTripDuration_SIZE  2ul

#define OverFrequencyTripDurationStart  OverVoltageTripDurationStart + OverVoltageTripDuration_SIZE //102 //requires 2 byte 
#define OverFrequencyTripDuration_SIZE 2ul


#define UnderVoltageTripDurationStart  OverFrequencyTripDurationStart + OverFrequencyTripDuration_SIZE //104 //requires 2 byte 
#define UnderVoltageTripDuration_SIZE  2ul 


#define UnderFrequencyTripDurationStart UnderVoltageTripDurationStart +  UnderVoltageTripDuration_SIZE // 106 //requires 2 byte 
#define UnderFrequencyTripDuration_SIZE 2ul

#define CoverRemovalTripDurationStart  UnderFrequencyTripDurationStart +  UnderFrequencyTripDuration_SIZE // 108 //requires 2 byte next is at 125 
#define CoverRemovalTripDuration_SIZE 2ul
 

#define NominalActivePowerStart CoverRemovalTripDurationStart + CoverRemovalTripDuration_SIZE //110  //requires 2 byte next is at 127
#define NominalActivePower_SIZE 2ul



///*
// *
// */
//static const SegmentAddress ConfigrationSegAddressStart = {128,128}; //starting from page 1
//static const SegmentAddress ConfigrationSegAddressEnd=    {1279,103};//A Total of 9 pages

#define PowerFaileEnergyLogStart NominalActivePowerStart + NominalActivePower_SIZE // //112 //requires 64 byte + 4 byte crc = 68 next is at 196
#define PowerFaileEnergyLog_SIZE 68ul


#define EnergyLoggingTimeStart PowerFaileEnergyLogStart + PowerFaileEnergyLog_SIZE //180 //requires 1 byte next is at 197 time interval for logging energy
#define EnergyLoggingTime_SIZE 1ul


#define CalibrationFactorsStart EnergyLoggingTimeStart + EnergyLoggingTime_SIZE //181 //requires 46 byte + 4 byte check sum = 50 next is at 247 but let's make this 250
#define CalibrationFactors_SIZE 50ul


#define StatusStart CalibrationFactorsStart + CalibrationFactors_SIZE //231  //requires 4 byte next is 254
#define Status_SIZE 4ul


#define Password_start StatusStart + Status_SIZE //235   //start address for is page 5   requires 1 pages
#define Password_SIZE 128ul


//#define TariffRate_start 768ul		//starts from page 6
#define ConsumptionSinceLastBillingStart Password_start + Password_SIZE //363 //requires 4 byte 
#define ConsumptionSinceLastBilling_SIZE 4ul

#define MinFrequencyStart ConsumptionSinceLastBillingStart + ConsumptionSinceLastBilling_SIZE //367 //requires 2 byte 
#define MinFrequency_SIZE 2ul

#define BackToAutoModeTimeStart MinFrequencyStart + MinFrequency_SIZE //369 //requires 2 byte
#define BackToAutoModeTime_SIZE 2ul


#define LanguageSettingStart BackToAutoModeTimeStart + BackToAutoModeTime_SIZE //371 //requires 1 byte AM,EN or AM_EN
#define LanguageSetting_SIZE 1ul


#define MeterConnectedPhaseStart LanguageSettingStart + LanguageSetting_SIZE //372 // requires 1 byte 
#define MeterConnectedPhase_SIZE 1ul

#define EnergyLogTimeStart MeterConnectedPhaseStart + MeterConnectedPhase_SIZE //373 //requires 1 byte 
#define EnergyLogTime_SIZE 1ul 


#define AutoDisplayTimeStart  EnergyLogTimeStart + EnergyLogTime_SIZE //374 // requires 4 byte
#define AutoDisplayTime_SIZE 4ul

//TODO. check whether this 5 bytes are corrept or not
#define DeviceIdentifierStart AutoDisplayTimeStart + AutoDisplayTime_SIZE +5//378 //requires 14 byte
#define DeviceIdentifier_SIZE 14 //including one byte CRC 

#define MACAddressStart DeviceIdentifierStart + DeviceIdentifier_SIZE //392 //requires 6 byte 
#define MACAddress_SIZE 6ul


#define OperatingModeStart MACAddressStart + MACAddress_SIZE //398 //requires 1 byte
#define OperatingMode_SIZE 1ul


#define EventLogBufferStart OperatingModeStart + OperatingMode_SIZE //399    // requires 14 byte
#define EventLogBuffer_SIZE 14ul


#define EnergyLogBufferStart EventLogBufferStart + EventLogBuffer_SIZE // 413   //requires 14 byte
#define EnergyLogBuffer_SIZE 14ul


#define BillingLogBufferStart EnergyLogBufferStart + EnergyLogBuffer_SIZE //427  //requires 14 byte
#define BillingLogBuffer_SIZE 14ul



#define PublicHoliday_Start  BillingLogBufferStart + BillingLogBuffer_SIZE //441	//starts from page 7, requires 2 page 
#define PublicHoliday_SIZE   256ul

//#define TimeSlots_Start    1024 	//starts from page 8
#define LastLoggedEnergyStart PublicHoliday_Start + PublicHoliday_SIZE
#define LastLoggedEnergy_SIZE EnergyLogSize 

#define TimeSlots_Start    LastLoggedEnergyStart + LastLoggedEnergy_SIZE //697 	//starts from page 58 requires app. 1 pages 
#define TimeSlots_SIZE 128


#define Tariff_Rates_start TimeSlots_Start + TimeSlots_SIZE //825         //start of page 59 // requires 4 pages 
#define Tariff_Rates_SIZE 512ul




#define TimeSlots_StartPassive   Tariff_Rates_start + Tariff_Rates_SIZE //1337 	  //starts from page 63 requires app. 1 pages 
#define TimeSlots_StartPassive_SIZE 128ul


#define Tariff_Rates_startPassive TimeSlots_StartPassive + TimeSlots_StartPassive_SIZE //1465    //start of page 64 // requires 4 pages 
#define Tariff_Rates_startPassive_SIZE 512ul





#define power_breaks_start  Tariff_Rates_startPassive + Tariff_Rates_startPassive_SIZE //1977         //requires 320 byte which is 2.5 page ==> 3 page 
#define power_breaks_SIZE 384ul


#define SerialNumberStart power_breaks_start +  power_breaks_SIZE //2361  //0 serial no requires 12 byte
#define SerialNumber_SIZE 11ul//12ul[E.E] Serial number variable defined is 11 bytes

#define Manufacturer_NameStart SerialNumberStart + SerialNumber_SIZE //2373 //1 manufacturer name requires 21byte
#define Manufacturer_Name_SIZE 21ul

#define DeviceID2Start Manufacturer_NameStart + Manufacturer_Name_SIZE //2394   //2 device id 2 requires 7 byte
#define DeviceID2_SIZE 7ul

#define DeviceID3Start DeviceID2Start + DeviceID2_SIZE //2401  //3 device id 3 requires 19 byte
#define DeviceID3_SIZE 19ul

#define DeviceID4Start DeviceID3Start + DeviceID3_SIZE //2420  //4 device id 4 requires 21 byte
#define DeviceID4_SIZE 21ul

#define DeviceID5Start DeviceID4Start + DeviceID4_SIZE //2441  //5 device id 5 requires 14 byte
#define DeviceID5_SIZE 14ul

#define DeviceID6Start DeviceID5Start + DeviceID5_SIZE //2455   //6 device id 6 requires 25 byte
#define DeviceID6_SIZE 25ul

#define DeviceID7Start DeviceID6Start + DeviceID6_SIZE //2480  //7 device id 7 requires 6 byte
#define DeviceID7_SIZE 6ul

#define DeviceID8Start DeviceID7Start + DeviceID7_SIZE //2486  //8 device id 8*/ requires 2 byte
#define DeviceID8_SIZE 2ul

//#define DeviceIDsEnd DeviceID8Start + DeviceID8_SIZE //9225  //start of page 72


#define ThrusholdLongPowerFaileStart DeviceID8Start + DeviceID8_SIZE //2488 //requires 2 byte 
#define ThrusholdLongPowerFaile_SIZE 2ul


#define LongPowerFaileTripDurationStart ThrusholdLongPowerFaileStart + ThrusholdLongPowerFaile_SIZE //2490 //requires 2 byte 
#define LongPowerFaileTripDuration_SIZE 2ul


#define RelayStatusStart LongPowerFaileTripDurationStart + LongPowerFaileTripDuration_SIZE //2492 //requires 1 byte
#define RelayStatus_SIZE 1ul

#define RelayStatusBackUpStart RelayStatusStart + RelayStatus_SIZE //2493 //requires 1 byte
#define RelayStatusBackUp_SIZE 1ul


#define TariffRateCRCStart RelayStatusBackUpStart + RelayStatusBackUp_SIZE //2494 //requires 4 byte
#define TariffRateCRC_SIZE 4ul

#define CurrentBalanceCRC  TariffRateCRCStart + TariffRateCRC_SIZE //2498 //requires 4 byte 
#define CurrentBalanceCRC_SIZE 4ul

#define blob_start CurrentBalanceCRC + CurrentBalanceCRC_SIZE //2502 // for 10 blob types
#define blob_SIZE 40ul


#define EnergyBillingCuttOffLogAddress_start blob_start + blob_SIZE //2542    //start from page 9  required 40 pages for storing at list 20 years data                                                        //let's ends this at page 49 which is ,actually at 6192 last address 
#define EnergyBillingCuttOffLogAddress_SIZE 10080ul                        
                                                                                /* 42 - size of log , 12 months, 20 years
                                                                                  (12*20*42) = 10080
                                                                                */

#define EnergyBillingCuttOffLogAddress_end EnergyBillingCuttOffLogAddress_start + EnergyBillingCuttOffLogAddress_SIZE //12622 



#define STANDARD_EVENT_LOG_ADDRESS_START EnergyBillingCuttOffLogAddress_start + EnergyBillingCuttOffLogAddress_SIZE //12622 //starts from page 77, the billing cutoff energy log can be used for 5 years without overflow
                                                /*
                                                    Total Events = (131070  - 12622) / (11) ==> (EEPROM1_END - EVENT_Start) / EVENT_SIZE = 10,770
                                                */
#define MAX_STANDARD_EVENT_LOGS 6000//5701ul

#define STANDARD_EVENT_LOG_SIZE 42000//68412 //5701 LOGS * 12 BYTES //10768ul
#define STANDARD_EVENT_LOG_ADDRESS_END			STANDARD_EVENT_LOG_ADDRESS_START + STANDARD_EVENT_LOG_SIZE //+ EventLogSize * 10000ul //131071

#define LAST_FRAUD_EVENT_ADDRESS_START 			STANDARD_EVENT_LOG_ADDRESS_END
#define LAST_FRAUD_EVENT_SIZE 4ul

#define FRAUD_EVENT_OVERLAP_ADDRESS_START  		LAST_FRAUD_EVENT_ADDRESS_START + LAST_FRAUD_EVENT_SIZE
#define FRAUD_EVENT_OVERLAP_SIZE 1ul

#define FRAUD_EVENT_LOG_ADDRESS_START 			FRAUD_EVENT_OVERLAP_ADDRESS_START + FRAUD_EVENT_OVERLAP_SIZE
#define MAX_FRAUD_EVENT_LOGS 1773ul
#define FRAUD_EVENT_LOG_SIZE 12411UL //1773LOGS * 7 bytes
#define FRAUD_EVENT_LOG_ADDRESS_END 			FRAUD_EVENT_LOG_ADDRESS_START + FRAUD_EVENT_LOG_SIZE

#define LAST_POWER_QUAL_EVENT_ADDRESS_START  	FRAUD_EVENT_LOG_ADDRESS_END
#define LAST_POWER_QUAL_EVENT_SIZE 4ul

#define POWER_QUAL_EVENT_OVERLAP_ADDRESS_START  LAST_POWER_QUAL_EVENT_ADDRESS_START + LAST_POWER_QUAL_EVENT_SIZE
#define POWER_QUAL_EVENT_OVERLAP_SIZE 1ul

#define POWER_QUAL_LOG_ADDRESS_START			POWER_QUAL_EVENT_OVERLAP_ADDRESS_START + POWER_QUAL_EVENT_OVERLAP_SIZE
#define MAX_POWER_QUAL_EVENT_LOGS 2000ul
#define POWER_QUAL_EVENT_LOG_SIZE 14000UL //2000LOGS * 7 bytes
#define POWER_QUAL_LOG_ADDRESS_END 				POWER_QUAL_LOG_ADDRESS_START + POWER_QUAL_EVENT_LOG_SIZE

#define LAST_COMMON_EVENT_ADDRESS_START			POWER_QUAL_LOG_ADDRESS_END
#define LAST_COMMON_EVENT_SIZE 4ul

#define COMMON_EVENT_OVERLAP_ADDRESS_START		LAST_COMMON_EVENT_ADDRESS_START + LAST_COMMON_EVENT_SIZE
#define COMMON_EVENT_OVERLAP_SIZE 1ul

#define COMMON_LOG_ADDRESS_START				COMMON_EVENT_OVERLAP_ADDRESS_START + COMMON_EVENT_OVERLAP_SIZE
#define MAX_COMMON_EVENT_LOGS 3000ul
#define COMMON_EVENT_LOG_SIZE 21000UL //3000LOGS * 7 bytes
#define COMMON_LOG_ADDRESS_END 					COMMON_LOG_ADDRESS_START + COMMON_EVENT_LOG_SIZE

#define LAST_FIRMWARE_EVENT_ADDRESS_START 		COMMON_LOG_ADDRESS_END
#define LAST_FIRMWARE_EVENT_SIZE 4ul

#define FIRMWARE_EVENT_OVERLAP_ADDRESS_START	LAST_FIRMWARE_EVENT_ADDRESS_START + LAST_FIRMWARE_EVENT_SIZE
#define FIRMWARE_EVENT_OVERLAP_SIZE 1ul

#define FIRMWARE_LOG_ADDRESS_START				FIRMWARE_EVENT_OVERLAP_ADDRESS_START + FIRMWARE_EVENT_OVERLAP_SIZE
#define MAX_FIRMWARE_EVENT_LOGS 1000ul
#define FIRMWARE_EVENT_LOG_SIZE 7000UL //1000LOGS * 7 bytes
#define FIRMWARE_LOG_ADDRESS_END 				FIRMWARE_LOG_ADDRESS_START + FIRMWARE_EVENT_LOG_SIZE

#define LAST_SYNCHRONIZATION_EVENT_ADDRESS_START 		FIRMWARE_LOG_ADDRESS_END
#define LAST_SYNCHRONIZATION_EVENT_SIZE 4ul

#define SYNCHRONIZATION_EVENT_OVERLAP_ADDRESS_START		LAST_SYNCHRONIZATION_EVENT_ADDRESS_START + LAST_SYNCHRONIZATION_EVENT_SIZE
#define SYNCHRONIZATION_EVENT_OVERLAP_SIZE 1ul

#define SYNCHRONIZATION_LOG_ADDRESS_START				SYNCHRONIZATION_EVENT_OVERLAP_ADDRESS_START + SYNCHRONIZATION_EVENT_OVERLAP_SIZE
#define MAX_SYNCHRONIZATION_EVENT_LOGS 1000ul
#define SYNCHRONIZATION_EVENT_LOG_SIZE 7000UL //1000LOGS * 7 bytes
#define SYNCHRONIZATION_LOG_ADDRESS_END 				SYNCHRONIZATION_LOG_ADDRESS_START + SYNCHRONIZATION_EVENT_LOG_SIZE

#define LAST_DISCONNECT_EVENT_ADDRESS_START				SYNCHRONIZATION_LOG_ADDRESS_END
#define LAST_DISCONNECT_EVENT_SIZE 4ul

#define DISCONNECT_EVENT_OVERLAP_ADDRESS_START			LAST_DISCONNECT_EVENT_ADDRESS_START + LAST_DISCONNECT_EVENT_SIZE
#define DISCONNECT_EVENT_OVERLAP_SIZE 1ul

#define DISCONNECT_LOG_ADDRESS_START					DISCONNECT_EVENT_OVERLAP_ADDRESS_START + DISCONNECT_EVENT_OVERLAP_SIZE
#define MAX_DISCONNECT_EVENT_LOGS 1000ul
#define DISCONNECT_EVENT_LOG_SIZE 7000UL //1000LOGS * 7 bytes
#define DISCONNECT_LOG_ADDRESS_END 						DISCONNECT_LOG_ADDRESS_START + DISCONNECT_EVENT_LOG_SIZE



#ifdef EEPROM_REV_2
#define EEPROM1_DiagAddressStart  262143ul
#else
#define EEPROM1_DiagAddressStart  131071ul  //end of EEPROM 1
#endif 


//EnergyLogAddress_Start
//EnergyLog_SIZE
//EnergyLog_SIZE2
//EnergyLogAddress_End
//Hourly load profile
#define HOURLY_ENERGY_LOG_ADDRESS_START 131072ul  //   262143 - 131072 / 18  = 7157 logs, log is every 15 minute,  74.5 days
#define HOURLY_ENERGY_MAX_LOGS   7131ul//
#define HOURLY_ENERGY_SIZE  128358ul  // 128358/18 = 7131 logs, every 15 minute 7131/(4*24)= 74.2 days
#define HOURLY_ENERGY_LOG_ADDRESS_END HOURLY_ENERGY_LOG_ADDRESS_START + HOURLY_ENERGY_SIZE //262126ul//((uint32_t)((EnergyLogAddress_Start)+((uint32_t)(EnergyLog_SIZE * EnergyLogSize))))

//Daily snapshot
#define LAST_DAILY_SNAPSHOT_LOG_ADDRESS_START   HOURLY_ENERGY_LOG_ADDRESS_END
#define LAST_DAILY_SNAPSHOT_LOG_ADDRESS_SIZE 4ul
#define DAILY_SNAPSHOT_LOG_OVER_LAP_ADDRESS_START LAST_DAILY_SNAPSHOT_LOG_ADDRESS_START + LAST_DAILY_SNAPSHOT_LOG_ADDRESS_SIZE
#define DAILY_SNAPSHOT_LOG_OVER_LAP_SIZE 1ul
#define DAILY_SNAPSHOT_LOG_ADDRESS_START     DAILY_SNAPSHOT_LOG_OVER_LAP_ADDRESS_START + DAILY_SNAPSHOT_LOG_OVER_LAP_SIZE
#define DAILY_SNAPSHOT_LOG_SIZE 2700   // 2700/30 = 90logs, every single day 90 days
#define DAILY_SNAPSHOT_MAX_LOGS 90
#define DAILY_SNAPSHOT_LOG_ADDRESS_END DAILY_SNAPSHOT_LOG_ADDRESS_START + DAILY_SNAPSHOT_LOG_SIZE

#ifdef EEPROM_REV_2
                                                  
                                                  
#define EEPROM2_Start 262144ul
#define FirmwareInfo_Start EEPROM2_Start + 128 //leave the first sector untouched
#define ImageIdentifier_Start FirmwareInfo_Start
#define ImageIdentifier_Size 16

#define ImageSize_Start ImageIdentifier_Start + ImageIdentifier_Size   
#define ImageSize_Size 4
                                                  
#define ImageTransferSatus_Start ImageSize_Start + ImageSize_Size
#define ImageTransferSatus_Size 1

#define ImageBlocksStatus_Start   ImageTransferSatus_Start +  ImageTransferSatus_Size
#define ImageBlocksStatusSize 256 //assume 256 bytes is enough for this for 128k image size with 64 byte block size

#define PerviousBlockNumber_Start ImageBlocksStatus_Start + ImageBlocksStatusSize
#define PerviousBlockNumber_Size 4
                                                  
#define FirmwareFirstByteStart PerviousBlockNumber_Start + PerviousBlockNumber_Size//1024 //leave 1kb space for the firmware info



#define EEPROM2_DiagAddressStart 524287ul //end of EEPROM2
                                                  
                                                  
#else
#define EEPROM2_DiagAddressStart 262143ul//524287ul //end of EEPROM2
#endif 

/**
* Initializes the EEPROM module 
* formats the eeprom if it is new
* diagnosis the EEPROM
*/
void InitLogg();
/**
* Loads the configurations from the EEPROM. reads parameters and copy the to working variable
*/
void LoadConfigurations();
/**
*  @param year:- the year
*  @param month:-
*  @param date:-
*  @param hour:-
*  @param minute:-
*  @param minute:-
*  @param second:-
*  @return a time stump (concatenated list of bytes)from the given date and time.
*/
TimeStump getTimeStamp(uint16_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t minute,uint8_t second);
/**
* Converts Timestamp type to the rtc_t type
* @return the converted time
*/
rtc_t  getTime(TimeStump *t);  
/**
 * Accepts a 32 bit number and returns 8bit checksum of the each byte.
 * @param number:- the number to calculate the checksum for
 * @length :-  the number of bytes in the number
 */
uint32_t getCheckSum(void *number,uint8_t length);


/*
* log event to the eeprom
* @param l: the event to log 
* @param dummy:dummy value 
*/
int8_t logEvent(void *l,void *dummy);

/*
* reads event from EERPOM
* @param l: event to log
* @param StartAddress: the start address to log the event at 
*/
uint8_t getEvent(EventLog *l,unsigned long StartAddress);


/**
* @param l(EventLog type) the event to log
* @param StartAddress: the address to write the event at 
*/
//int8_t logEvent2(void *l,void *StartAddress);
/**
* @see logEvent2
*/
int8_t get_standard_event(void *l,uint32_t StartAddress);

int8_t log_daily_energy_snapshot(void *l2,void *dummy);
int8_t get_daily_snapshot_energy_profile(void *lt,uint32_t EntryNumber);
uint8_t get_daily_snapshot_overlap_flag(uint8_t *flag);
/*
 * get the time stamp of daily energy log
 * of the given entry
 */
uint8_t get_daily_energy_log_time_stamp(void *lt,uint32_t EntryNumber);
/*
 * Logs hourly energy profile
 */
int8_t log_hourly_energy_profile(void *l2,void *dummy);
/*
 * Get hourly energy profile
 */
int8_t get_hourly_energy_profile(void *lt,uint32_t EntryNumber);

/*
 * restores the last backup data from eeprom
 */
int8_t restore_eeprom_backup(void *phase_temp2,void *dummy);

/*
 * Backups current energy registers and counters to eeprom
 */
int8_t perform_eeprom_backup(void *phase_temp2,void *dummy);
/*
 * get the time stamp of log
 * of the given entry
 */
uint8_t get_log_time_stamp(void *lt,uint32_t EntryNumber,log_search_params_t *search_params);

uint8_t get_captured_log_by_time_range(const sSA_Range *startRange,const sSA_Range *endRange,uint16_t *startEntryNumber,
                                              uint16_t *numOfEntries,log_search_params_t *search_items);

/*
 * function returns logs based on the
 * access selector specified in the request
 */
void get_captured_logs(log_search_params_t *log_search_items);
/**
* Id2: pointer to the array to copy the id to
* type: the type of id to get 
* ID's 
* 0 serial no
* 1 manufacturer name 
* 2 device id 2
* 3 device id 3
* 4 device id 4
* 5 device id 5
* 6 device id 6
* 7 device id 7
* 8 device id 8
*/
//uint8_t setDeviceIDs(const uint8_t *id,uint8_t type);
int8_t setDeviceIDs(void *id2,void *type2);
/*
* @see setDeviceIDs
*/
int8_t getDeviceIDs(void *id,void *type);

/**
* sets the current balance 
* @see  getCurrentBalance
*/
int8_t setCurrentBalance(void *b2,void *dummy);

/**
* reads the current balance 
* @param b2: the balance
* @param dummy : not used
*/
int8_t getCurrentBalance(void *b2,void *dummy);
/**
* reads the current firmware version
* @param version : the firmware version to write
*/
uint8_t getFirmwareVersion(unsigned long *version);
/**
* @see getFirmwareVersion
*/
int8_t setFirmwareVersion(void *version2,void *dummy); 


/**
* @see getTamperCount
*/
int8_t setTamperCount(void *count2,void *TamperType2);

/*
* The tamper count to read and write 
* @param count: uint8_t 
* @param TamperType: uint8_t 
*              @see the tamper module for the tamper types
*/
int8_t getTamperCount(void *count,void *TamperType);

/*
* reads the passwords stored in the EEPROM
* @param p: Password_128
* @param type: uint8_t
*/
int8_t getPassword( void *p,void *type);

/**
* @see getPassword
*/
int8_t setPassword(void *pass2,void *type2); 


/*
 * Get the configured holidays
 * @param Holiday Number : from 1 to 24 one of the holidays on the schedule
 */
uint8_t getPublicHoliday(uint8_t holidayNumber,Public_Holiday_t *t);
/**
* @see getPublicHoliday
*/
uint8_t setPublicHoliday(uint8_t holidayNumber,Public_Holiday_t *t);



/*
* get the schadules(when the scadule start and ends and the rate for the scadule) for the billing module 
*   3 day profiles 
*   4 scadules per day 
*   ==> 12 total of scadules 
*  @param sheduleNumber 0 - 11
*/
uint8_t getTariffSchaduleTimeslot(uint8_t sheduleNumber,Schedule_timeslot_t *t);

/**
* @see getTariffSchaduleTimeslot
*/
int8_t setTariffSchaduleTimeslot(void *t2,void *scheduleNumber2);

/**
* @see getTariffSchaduleTimeslot
*/
uint8_t getTariffSchaduleTimeslotPassive(uint8_t sheduleNumber,Schedule_timeslot_t *t);
/**
* @see getTariffSchaduleTimeslot
*/
int8_t setTariffSchaduleTimeslotPassive(void *t2,void *scheduleNumber2);

/*
*  gets the billing rate  e.g 0.2730birr/day
*   3 day profiles 
*   4 time slots pre day profiles 
*   10 rates pre time slots 

*  ==> 3*4 * 10 = 120 rates 
*  rateNumber 0 to 119
    
*  @param rateNumber: uint8_t the rate number to get
*  @param rate    : uint32_t rate
*/
int8_t getRate(void *rate,void *rateNumber);
/**
* @see getRate
*/
int8_t setRate(void *rate2,void *rateNumber2 );

/*
*   read the power brack points for the billing module according to the EEU
*   current billing schema
*   @param Number from 0 to 9
*   @param value : uint32_t the value   
*/
int8_t getPowerBreaks(void *value,void *Number );
/**
* @see getPowerBreaks
*/
int8_t setPowerBreaks(void *value2,void *Number2);     

/**
 * reads last log addresses 
 * data: 32bit integer
 * type: 0 energy log address
 *       1 event log address
 *       2 Billing Cuttoff log address 
 *       
 */
int8_t getLastLogAddress(void *data,void *type);
/**
* @see getLastLogAddress
*/
int8_t setLastLogAddress(void *data2,void *type2);

/**
* @see getBillingSchema
*/
int8_t setBillingSchema(void *val2,void *dummy);

/*
* reads the billing schema from EEPROM 
*        Prepaid or Postpaid 
* See the billing module for values
* @param flag:- is 8 bit number
*               
* @param dummy:- not used
*/
int8_t getBillingSchema(void *flag,void *dummy);

/**
* sets the relay status flag
* @param flag: uint8_t relay status flag 
*              1 - connected 
*              0 - disconnected
* @param auto_connect: uint8_t 
*        
*/
int8_t getRelayStatus(void *flag,void *auto_connect);
/**
* @see getRelayStatus 
*/
int8_t setRelayStatus(void *val,void *auto_connect);

/**
* sets or clears the Energy log Overlap Flag
* @param val2: the value to set
* @param dummy: not used
*/
int8_t setEnergyOverlapFlag(void *val2,void *dummy);
/**
* @see setEnergyOverlapFlag
*/
uint8_t getEnergyOverlapFlag(uint8_t *flag);

/**
* sets or clears the Event Overlap Flag
* @param val2: the value to set 
* @param dummy: not used
*/
int8_t setEventOverlapFlag(void *val2,void * dummy);
/**
* @see setEventOverlapFlag
*/
uint8_t get_event_overlap_flag(uint8_t *flag, uint8_t *type);

/**
* sets or clears the billing cutoff log overlap indicator flag
* @param value2 : the value to set 
* @dummy unused variable
*/
int8_t setBillingCutOffLogOverlapFlag(void *value2,void *dummy);
/**
* @see setBillingCutOffLogOverlapFlag
*/
uint8_t getBillingCutOffLogOverlapFlag(uint8_t *flag);

/**
* writes the meters manufactured date and time to the EEPROM
* @param rtc_t the manufacturing date time
*/
uint8_t setManufacturer_DateTime(rtc_t *value);
/**
* @see setManufacturer_DateTime
*/
uint8_t getManufacturer_DateTime(rtc_t *value);


/**
* reads a billing value log from eeprom
* @param EnergyBillingCutoffDayLog
* @param startAddress the address where the log starts from
*/
uint8_t getBillingCutoffDateLog(EnergyBillingCutoffDayLog *l,unsigned long startAddress);
/**
* @see getBillingCutoffDateLog
*/
uint8_t logBillingCutoffDateLog(EnergyBillingCutoffDayLog *l);




/**
* stores the energy reading to eeprom when a long power fail condition is detected
* @param phase_temp2:phase_parms_s type pointer
* @dummy dummy variable
*/
int8_t logPowerFailEnergy(void *phase_temp2,void *dummy); 
/*
* @see logPowerFailEnergy 
*/
int8_t restorePowerFailEnergyLog(void *phase_temp,void *data);

/*
* Writes the calibration values to EEPROM
*@param param :nv_parms_s type 
*@param dummy unused variable
*/
int8_t getCalibrationParams(void *param,void *dummy);
/**
* @see getCalibrationParams
*/
int8_t setCalibrationParams(void *param2,void *dummy);




/*
*@param value2 uint16_t -> vlue to set
*@param type:  uint8_t -> the type of the value
*               0 voltage 
*               1 current 
*               2 frequency 
*               3 active power 
*               4 Long Power Faile  
*
*/
int8_t setNominalValues(void *value2,void *type2 );
/**
* @see setNominalValues for parameter description 
*/
int8_t getNominalValues(void *value,void *type);

/**
* @see setNominalValues for parameter description 
*/
int8_t setMaxValues(void *value2,void *type2);

/**
* @see setNominalValues for parameter description 
*/
int8_t getMaxValues(void *value,void *type);
/**
* @see setNominalValues for parameter description 
*/
int8_t setMinValues(void *value2,void *type2);
/**
* @see setNominalValues for parameter description 
*/
int8_t getMinValues(void *value,void *type);

/**
* Writes the energy loggin time to EEPROM
* @param value2 uint8_t the value to set 
* @parma dummy not used 
*/
int8_t setEnergyLogTime(void *value2,void *dummy);
/**
* 
* @see setEnergyLogTime for parameter description
*/
int8_t getEnergyLogTime(void *value,void *dummy);
/**
*  used by the billing module for backing up the reading that has already been billed for.
*  this value is reset to zero at the end of billing period       
* @param value2(uint32_t):- The value to write to EEPROM
* @param dummy : unused variable                        
*/
int8_t setTempLastEnergyValue(void *value2,void *dummy);

/*
* @see setTempLastEnergyValue for detail
*
*/
int8_t getTempLastEnergyValue(void *value,void *dummy);

/*
* gets the auto display time configuration 
*  @param value(uint32_t) : auto display time  
*  @param dummy : pointer to null
*/
int8_t getAutoDisplayTime(void *value,void *dummy);
/**
* @see getAutoDisplayTime for detail
*/
int8_t setAutoDisplayTime(void *value2,void *dummy);

/*
*  timeout to change the lcd display from manual scrolling to auto scrolling 
*  @param value2: the display back to auto mode time 
*  @param dummy: null pointer
*/
int8_t getBackToAutoModeTime(void *value2,void *dummy);
/*
* @see getBackToAutoModeTime for details
*/
int8_t setBackToAutoModeTime(void *value,void *dummy);

/*
*  sets the current display language of themeter
*  @param value: 1byte number representing the language selection
*                0: for amharic 
*                1: for english 
*                2: for amharic and english
*  @param dummy:null pointer: not used
*/
int8_t getLanguageSelection(void *value,void *dummy);
/**
* @see getLanguageSelection
*/
int8_t setLanguageSelection(void *value,void *dummy);

/**
* sets the meter connected phase, this  function is not used 
* @Param value: 1byte number indicating phase  
* @Param dummy:null pointer, not used
*/
int8_t getMeterConnectedPhase(void  *value,void *dummy);
/**
** @see getMeterConnetedPhase
*/
int8_t setMeterConnectedPhase(void *value2,void *dummy);
/**
*  @param value2(uint32_t) :- This months energy consumption 
*  @dummy not used
*/
int8_t setConsumptionSinceLastBilling(void *value2,void *dummy);
/**   
* @see setConsumptionSinceLastBilling 
*/
int8_t getTempConsumptionSinceLastBilling(void *value,void *dummy);

/** 
Sets the time out for detection of events 
*  @param Value2:-  the value to set the trip duration 2    
*                   ActivePowerTripDuration      3  
*                   OverCurrentTripDuration      1 
*                   OverVoltageTripDuration      0 

*                   OverFrequencyTripDuration    2 
*                   UnderVoltageTripDuration     4 
*                   UnderFrequencyTripDuration   5 

*                   CoverRemovalTripDuration     6 
*                   LongPowerFaileTripDuration   7 
*/
int8_t setTripDuration(void *value2,void *type2);

/*
* @see setTripDuration
*
*/

int8_t getTripDuration(void *duration,void *type);



/*  
*  Stores the the last rtc value before power down used for self diagnosis
*  @param rtcc2(rtc_t) the rtc value to store
*         dummy not used 
*/
int8_t setRTCCheck(void *rtcc2,void *dummy);
/**
* @see setRTCCheck
*
*/
uint8_t getRTCCheck(rtc_t *rtc_temp);

/*
 * Read the meter status from the eeprom 
 * @param m_status2(MeterStatus) the meter status 
 */
int8_t getMeterStatus(void *m_status2,void *dummy);

/**
* @see getMeterStatus
*/
int8_t logMeterStatus(void *m_status,void *dummy);

/**
* reads the devices Identifier from EEPROM
* @param deviceIdentifier : Array of size 13 
* @param dummy: pointer to null. no used 
*/
int8_t getLogicDeviceName(void *deviceIdentifier,void *dummy);

/**
*  @see getLogicDeviceName 
*/
int8_t setLogicDeviceName(void *deviceIdentifier2,void *dummy);

/*
*reads the devices macAddress from EEPROM
* @param macAddress: Array of size 6
* @param dummy:pointer to null.no used 
*/
int8_t getMacAddress(void *macAddress,void *dummy);

/**
* @see setMacAddress
*/
int8_t setMacAddress(void *macAddress,void *dummy);

/*
*reads the systems opperating mode from EEPROM
* @param mode: 1byte mode varialbe 
* @param dummy: pointer to null: not used
*/
int8_t getOperatingMode(void *mode,void * dummy);
/**
* @see getOperationMode
*/
int8_t setOperatingMode(void *mode2,void *dmummy);

/**
* reads and writes blob values from EEPROM and to EEPROM
* @param blob: the blob value to write
* @param blob_id: the blob type identifier
*/
uint8_t setBlob(const uint32_t *blob,uint16_t blob_id);
/**
* @see setBlob
*/
uint8_t getBlob(uint32_t *blob,uint16_t blob_id);

/**
* put the Firmware block, block_number to EEPROM
* @param block: - the block data
* @param block_number:- the block number
* @return 0: if failed 
*         1: if successfull       
*/
uint8_t writeFirmwareBlock(uint8_t *block, uint32_t block_number);
/**
* Read firmware block form EEPROM
* @see writeFirmwareBlock
*/
uint8_t readFirmwareBlock(uint8_t *block,uint32_t block_number);
/**
* Writes the firmware image identifier to EEPROM 
* @param imageIdentifier: the image identifier 
* @param dummy: unused variable 
* @return 0: if failed 
*         1: is successfull         
*/
uint8_t writeImageIdentifier( uint8_t *imageIdentifier, uint8_t dummy );
/**
* Reads image identifer 
* @see readImageIdentifier
*/
uint8_t readImageIdentifier ( uint8_t *imageIdentifier, uint8_t dummy );

/**
* writes the image size to eeprom 
* @param imageSize:- the size of the firmware image
* @param dummy:- unused variable
*/
uint8_t writeImageSize(uint32_t imageSize,uint8_t dummy );
/**
* reads size of the firmware image
* @see writeImageSize
*/
uint8_t readImageSize(uint32_t *imageSize,uint8_t dummy);
/**
* sets image transfer status bytes to 0
* @returns 1: success
*          0: failer
*/
uint8_t EEPROM_reformatImageTransferStatusBytes();
/**
* writes the last firmware image block number downloaded to eeprom
* @param data :-  the block number 
* @param dummy:-  unused variable  
*/
uint8_t writePreviousImageNumber(uint32_t blockNumber,uint8_t dummy);
/**
* read previous image number 
* @see writePreviousImageNumber
*/
uint8_t readPreviousImageNumber(uint32_t *data,uint8_t dummy); 

/**
* updates the status of firmware block 
* @param blockNumber: the block number 
* @param status : the status of the block 
*                 1: recived 
*                 0: not recived  
*/
uint8_t updatateFirwmareBlockStatus(uint32_t blockNumber,uint8_t status);
/**
* reads  all the status bytes for the firmware blocks and writes them to blocks array
* @param blocks: the read out data
* @param dummy: unused varialbe  
*/
uint8_t readAllFirwmareBlockStatus(uint8_t *blocks,uint8_t dummy);
/**
* reads the status of a single  firmware blocks 
* @param statusBlock : the status block
*/
uint8_t readFirwmareBlockStatus(uint8_t *statusBlock,uint8_t blockNumber);
/*
 * Logs different types of events into the eeprom
 */
int8_t log_events(void *l2,void *type);
int8_t compare_fraud_event(void *event0,void *event1);
uint8_t get_fraud_event_log(event_log *l,unsigned long StartAddres);
int8_t get_fraud_event(void *lt,uint32_t EntryNumber);
uint8_t get_power_qual_event_log(event_log *l,unsigned long StartAddres);
int8_t get_power_qual_event(void *lt,uint32_t EntryNumber);
uint8_t get_common_event_log(event_log *l,unsigned long StartAddres);
int8_t get_common_event(void *lt,uint32_t EntryNumber);
int8_t compare_firmware_event(void *event0,void *event1);
uint8_t get_firmware_event_log(firmware_event_log *l,unsigned long StartAddres);
int8_t get_firmware_event(void *lt,uint32_t EntryNumber);
int8_t compare_synchronization_event(void *event0,void *event1);
uint8_t get_synchronization_event_log(time_bound_event_log *l,unsigned long StartAddres);
int8_t get_synchronization_event(void *lt,uint32_t EntryNumber);
int8_t compare_disconnect_event(void *event0,void *event1);
uint8_t get_disconnect_event_log(disconnect_event_log *l,unsigned long StartAddres);
int8_t get_disconnect_event(void *lt,uint32_t EntryNumber);















#endif /* LOGGER_H_ */
