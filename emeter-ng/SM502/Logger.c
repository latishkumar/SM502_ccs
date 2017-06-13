/*
 * Logger.c
 *
 *  Created on: Apr 23, 2013
 *      Author: Donayam
 */

#include "Logger.h"
#include "I2C2EEPROM.h"
#include "Tariff.h"
#include "Tamper.h"
#include "self_diagnosis.h"
#include "emeter-rtc.h"
#include "TI_aes.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "LPBU.h"   
#include "power_quality.h"
#include "long_power_failures.h"
#include "configuration_defaults.h"
/*
 * The version of the software loaded on the meter
 * This is 1byte data
 */
//uint8_t FirmwareVersion   = 1; // 1 byte
extern uint8_t AutoConnect;
unsigned long FirmwareVersion = 0101; //major version 0 and sub version 1
rtc_t Manufacturer_DateTime = {2014,5,6,12,25,24,12};

extern uint32_t TempLastEnergyValue;
extern uint32_t ConsumptionSinceLastBilling;

uint8_t MAX_LOG_RETRAY = 5;

unsigned long LastEnergyLogAddress = EnergyLogAddress_Start;
unsigned long LastEventLogAddress  = EventLogAddress_Start;
unsigned long LastEnergyBillingCuttOffLogAddress = EnergyBillingCuttOffLogAddress_start;
unsigned long last_daily_snapshot_log_address = DAILY_SNAPSHOT_LOG_ADDRESS_START;
unsigned long last_fraud_event_log_address  = FRAUD_EVENT_LOG_ADDRESS_START;
unsigned long last_power_qual_event_log_address  = POWER_QUAL_LOG_ADDRESS_START;
unsigned long last_common_event_log_address  = COMMON_LOG_ADDRESS_START;
unsigned long last_firmware_event_log_address  = FIRMWARE_LOG_ADDRESS_START;
unsigned long last_synchronization_event_log_address  = SYNCHRONIZATION_LOG_ADDRESS_START;
unsigned long last_disconnect_event_log_address  = DISCONNECT_LOG_ADDRESS_START;

Tamper_Count TamperCount;

//extern  struct info_mem_s nv_parms;

extern HardwareStatus hardware_status;
extern rtc_t rtcc;
rtc_t rtc_temp;
uint8_t EEPROM_InitError = 0;


extern CurrentBalance Current_balance;

extern uint16_t ActivePowerTripDuration  ; //in seconds 
extern uint16_t OverCurrentTripDuration  ;
extern uint16_t over_voltage_trip_duration  ;

extern uint16_t OverFrequencyTripDuration  ;
extern uint16_t under_voltage_trip_duration   ;
extern uint16_t UnderFrequencyTripDuration ;


extern uint16_t CoverRemovalTripDuration   ;

extern uint32_t rates[40];

extern uint32_t power_brakes[10];

extern uint8_t Meter_Sr_No[];
extern uint8_t Manufacturer_Name[];
extern uint8_t Device_ID_2[];
extern uint8_t Device_ID_3[];
extern uint8_t Device_ID_4[];
extern uint8_t Device_ID_5[];
extern uint8_t Device_ID_6[];
extern uint8_t Device_ID_7[];
extern uint8_t Device_ID_8[];

extern uint32_t AutoDisplayTime ;//msec 
extern uint16_t BackToAutoModeTime ;//msec 
extern uint8_t Language ;

extern uint8_t meter_connected_phase;
extern uint8_t EnergyLoggingTime;

extern uint16_t voltage_sag_threshold ;//7% percent
extern uint16_t voltage_swell_threshold ;//2% percent
extern uint16_t voltage_threshold_for_long_power_failure ;

extern uint8_t min_vrms;
extern uint16_t max_vrms ;
extern uint16_t ThresholdLongPowerFaile;
extern uint16_t nominal_voltage;

extern uint8_t billingSchema;
extern uint8_t device_identifier[];
extern uint8_t EUI[];//MAC Address  02,00,01,00,00,03
extern uint8_t OperatingMode;



extern rtc_t default_time;

//ENCRIPTED PASSWORD'S FOR TEST PURPOSE ONLY 
Password_128 Passwords[4] = {
  
  {0,14,'D','E','F','A','U','L','T','P','A','S','S','W','O','R','D','1'},
  {1, 8,'H','E','L','L','O','D','V','E',' ',' ',' ',' ',' ',' ',' ',' '},
  {2,14,'D','E','F','A','U','L','T','P','A','S','S','W','O','R','D','3'},
  {2,14,'D','E','F','A','U','L','T','P','A','S','S','W','O','R','D','4'},
  
};

rtc_t rtc_temp;

/*
 * Initialize EEPROM Communication
 *
 * Read EEPROM Formatted String
 *
 * if EEPROM is Formatted
 *    read all needed configurations
 *         ==> last energy and event log addresses
 *         ==> tamper counts        done in tamper module
 *         ==> current balance 		done in tariff module
 *         ==> todays tariff  	    done in tariff module
 *
 *         ==> calibration values
 * else
 * 	 Format EEPROM from the hard coded initialization variables
 *
 *
 */
extern volatile uint8_t system_init_delay;
extern volatile uint8_t rtc_init;
void reFormatMemoryFormatIndicator(uint32_t address);
uint8_t firtstBoot = 1;
/**
* function to determine wheather a memory has been formatted or not 
* @param address:- the address for the memory indicator code 
*/
int8_t isMemFormatted(uint32_t address)
{

    uint8_t eeprom_values[5]={0,0,0,0,0}; 
    uint8_t mindicaiton[] = MemoryFormattedIndicatorCode;
    
    EEPROM2_ReadInt8(address,0,&eeprom_values[0]);
    EEPROM2_ReadNextInt8(0,&eeprom_values[1]);
    EEPROM2_ReadNextInt8(0,&eeprom_values[2]);
    EEPROM2_ReadNextInt8(0,&eeprom_values[3]);
    EEPROM2_ReadNextInt8(1,&eeprom_values[4]);
                  
    
    uint8_t pc=0;
    if(eeprom_values[0] == mindicaiton[0])
    {
      pc++;
    }
    if(eeprom_values[1] == mindicaiton[1])
    {
      pc++;
    }
    if(eeprom_values[2] == mindicaiton[2])
    {
      pc++;
    }        
    if(eeprom_values[3] == mindicaiton[3])
    {
      pc++;
    }
    if(eeprom_values[4] == mindicaiton[4])
    {
      pc++;
    }
    
    if(pc==5)
    {
      return 1;
    }
    if(pc >= 3 && pc < 5)
    {
       return 0;
    }
    else 
    {
      return -1;
    }   
}
/**
* functin to check if memory has been formatted or not and to format the memory
*/
int8_t isMFormatRequired()
{   
   int8_t x =  isMemFormatted(MemoryFormattedAddressStart);
   if( x == -1 || x == 0 )//not formatted or something is suspisions
   {
     int8_t y = isMemFormatted(MemoryFormattedAddressBackUpStart);//veryify it 
     if(y == -1) //format the EEPROM
     {
       return 1;
     }
     else 
     {
       reFormatMemoryFormatIndicator(MemoryFormattedAddressStart);
       reFormatMemoryFormatIndicator(MemoryFormattedAddressBackUpStart); 
       return 0;
     }
   }
   else //x==1
   {
     return 0;
   }   
}
/**
* Rewrites the memory formatted indicator 
* @param address:- the address where the memory format indicator is located at
*/
void reFormatMemoryFormatIndicator(uint32_t address)
{
       uint8_t mindication[] = MemoryFormattedIndicatorCode;

       EEPROM2_WriteInt8(mindication[0],address,0);
       EEPROM2_WriteNextInt8(mindication[1], 0);
       EEPROM2_WriteNextInt8(mindication[2], 0);
       EEPROM2_WriteNextInt8(mindication[3], 0);
       EEPROM2_WriteNextInt8(mindication[4], 1);                                                    
}

void InitLogg()
{
    while(rtc_init == 0)
    {
         #if defined(USE_WATCHDOG)
         kick_watchdog();
         #endif
    }
      
	EEPROM2Init();
    int8_t MemNotFormatted = 0;//memory is not formatted
    MemNotFormatted = isMFormatRequired();//0;
      //  MemNotFormatted=1; //format
        
	if(MemNotFormatted == 1) //Format is required
	{
          status.ActivePowerExcededTimedOutStatus = 0;
          status.AutoDisconnectEnabled = 1;
          status.BattryStatus = 0;
          status.ConnectCommandRecived = 0;
          status.DisconnectCommandRecived = 0;
          status.DisplayMode  =0;
          status.DisplayState  =0;
          status.NeutralTamperStatus = 0;
          status.NeutralTamperTimedoutStatus = 0;
          status.OverCurrentStatus = 0;
          status.OverCurrentTimedOutStatus = 0;
          status.OverFreqStatus = 0;
          status.OverFreqTimedOutStatus = 0;
          status.OverVoltageStatus = 0;
          status.OverVoltageTimedOutStatus = 0;
          status.RelayStatus = 0;
          AutoConnect = 1;
          status.TamperDetectedStatus = 0;
          status.UnderFreqStatus = 0;
          status.UnderFreqTimedOutStatus = 0;
          status.UpperCoverRemovedTamperStatus = 0;
          status.LowerCoverRemovedTamperStatus = 0;
          status.MangneticTamperStatus = 0;


          setBlob(0,0);
          setBlob(0,1);
          setBlob(0,2);
          setBlob(0,3);

          write_to_eeprom(&status,(uint8_t *)0,logMeterStatus);

          uint8_t temp8 = status.RelayStatus = 1;
                          AutoConnect = 1;
          write_to_eeprom(&temp8,&AutoConnect,setRelayStatus);

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                      
          write_to_eeprom(&device_identifier,(uint8_t *)0,setLogicDeviceName);

          //write_to_eeprom(&EUI,(uint8_t *)0,setMacAddress);
          write_to_eeprom(&mac_address[1],(uint8_t *)0,setMacAddress);

          write_to_eeprom(&OperatingMode,(uint8_t *)0,setOperatingMode);

          write_to_eeprom(&AutoDisplayTime,(uint8_t *)0,setAutoDisplayTime);

          write_to_eeprom(&BackToAutoModeTime,(uint8_t *)0,setBackToAutoModeTime);

          write_to_eeprom(&Language,(uint8_t *)0,setLanguageSelection);

          write_to_eeprom(&billingSchema,(uint8_t *)0,setBillingSchema);

          write_to_eeprom(&meter_connected_phase,(uint8_t *)0,setMeterConnectedPhase);

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                       
          temp8 =0;
          write_to_eeprom(&voltage_sag_threshold,&temp8,setMinValues);


          temp8 = 0;
          write_to_eeprom(&voltage_swell_threshold,&temp8,setMaxValues);

          temp8 = 4;
          write_to_eeprom(&voltage_threshold_for_long_power_failure,&temp8,setNominalValues);

          temp8 = 0;
          write_to_eeprom(&nominal_voltage,&temp8,setNominalValues);

                      
          min_vrms = nominal_voltage - (((voltage_sag_threshold/100) * nominal_voltage)/100);
          max_vrms = nominal_voltage +( ((voltage_swell_threshold/100) * nominal_voltage)/100);
          ThresholdLongPowerFaile = nominal_voltage - (((voltage_threshold_for_long_power_failure/100) * nominal_voltage)/100);
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                      
                      
          temp8 = 0;
          write_to_eeprom(&Meter_Sr_No,&temp8,setDeviceIDs);

          temp8 = 1;
          write_to_eeprom(&Manufacturer_Name,&temp8,setDeviceIDs);

          temp8 = 2;
          write_to_eeprom(&Device_ID_2,&temp8,setDeviceIDs);

          temp8 = 3;
          write_to_eeprom(&Device_ID_3,&temp8,setDeviceIDs);

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                       
          temp8 = 4;
          write_to_eeprom(&Device_ID_4,&temp8,setDeviceIDs);

          temp8 = 5;
          write_to_eeprom(&Device_ID_5,&temp8,setDeviceIDs);

          temp8 = 6;
          write_to_eeprom(&Device_ID_6,&temp8,setDeviceIDs);

          temp8 = 7;
          write_to_eeprom(&Device_ID_7,&temp8,setDeviceIDs);

          temp8 = 8;
          write_to_eeprom(&Device_ID_8,&temp8,setDeviceIDs);

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                      
          write_to_eeprom(&EnergyLoggingTime,(uint8_t *)0,setEnergyLogTime);

          temp8 = 0;
          write_to_eeprom(&over_voltage_trip_duration,&temp8,setTripDuration);

          temp8 = 1;
          write_to_eeprom(&OverCurrentTripDuration,&temp8,setTripDuration);

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                       
          temp8 = 2;
          write_to_eeprom(&OverFrequencyTripDuration,&temp8,setTripDuration);

          temp8 = 3;
          write_to_eeprom(&ActivePowerTripDuration,&temp8,setTripDuration);

          temp8 = 4;
          write_to_eeprom(&under_voltage_trip_duration,&temp8,setTripDuration);

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                      
          temp8 = 5;
          write_to_eeprom(&UnderFrequencyTripDuration,&temp8,setTripDuration);

          temp8 = 6;
          write_to_eeprom(&CoverRemovalTripDuration,&temp8,setTripDuration);


          temp8 = 0;
          uint32_t tmp32 = EnergyLogAddress_Start;
          LastEnergyLogAddress = EnergyLogAddress_Start;
          write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

          temp8 = 1;
          tmp32 = DAILY_SNAPSHOT_LOG_ADDRESS_START;
          last_daily_snapshot_log_address = DAILY_SNAPSHOT_LOG_ADDRESS_START;
          write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

          temp8 = 2;
          tmp32 = EnergyBillingCuttOffLogAddress_start;
          LastEnergyBillingCuttOffLogAddress = tmp32;
          write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

          temp8 = 3;
          tmp32 = EventLogAddress_Start;
          LastEventLogAddress = tmp32;
          write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

          temp8 = 4;
		  tmp32 = FRAUD_EVENT_LOG_ADDRESS_START;
		  last_fraud_event_log_address = tmp32;
		  write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

		  temp8 = 5;
		  tmp32 = POWER_QUAL_LOG_ADDRESS_START;
		  last_power_qual_event_log_address = tmp32;
		  write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

		  temp8 = 6;
		  tmp32 = COMMON_LOG_ADDRESS_START;
		  last_common_event_log_address = tmp32;
		  write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

		  temp8 = 7;
		  tmp32 = FIRMWARE_LOG_ADDRESS_START;
		  last_firmware_event_log_address = tmp32;
		  write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

		  temp8 = 8;
		  tmp32 = SYNCHRONIZATION_LOG_ADDRESS_START;
		  last_synchronization_event_log_address = tmp32;
		  write_to_eeprom(&tmp32,&temp8,setLastLogAddress);

		  temp8 = 9;
		  tmp32 = DISCONNECT_LOG_ADDRESS_START;
		  last_disconnect_event_log_address = tmp32;
		  write_to_eeprom(&tmp32,&temp8,setLastLogAddress);


         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif 


          write_to_eeprom(&FirmwareVersion,(uint8_t *)0,setFirmwareVersion);
                        
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                        
          /*
           * set the tamper counts to 0
           */
          uint8_t temp82 = 0;
          temp8 = MagneticTamperType;
          write_to_eeprom(&temp82,&temp8,setTamperCount);

          temp8 = UC_TamperType;
          write_to_eeprom(&temp82,&temp8,setTamperCount);

          temp8 = LC_TamperType;
          write_to_eeprom(&temp82,&temp8,setTamperCount);

          temp8 = NeutralTamperType;
          write_to_eeprom(&temp82,&temp8,setTamperCount);

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                       
          temp8 = EarthTamperType;
          write_to_eeprom(&temp82,&temp8,setTamperCount);

          temp8 = ReverseConnactionTamperType;
          write_to_eeprom(&temp82,&temp8,setTamperCount);

          temp8 = ConfigrationPortTamperType;
          write_to_eeprom(&temp82,&temp8,setTamperCount);

          temp8 = MeterBypassTamperType;
          write_to_eeprom(&temp82,&temp8,setTamperCount);
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif
                        
           //set Current balance to 0
          Current_balance.balance = 9000000;
          write_to_eeprom(&Current_balance,(uint8_t *)0,setCurrentBalance);
                        
          //clear event and energy log overlap flag
          temp8  = 0;
          temp82 = 3;//type
          write_to_eeprom(&temp8,&temp82,setEventOverlapFlag);
          temp82 = 4;//type
          write_to_eeprom(&temp8,&temp82,setEventOverlapFlag);
          temp82 = 5;//type
          write_to_eeprom(&temp8,&temp82,setEventOverlapFlag);
          temp82 = 6;//type
          write_to_eeprom(&temp8,&temp82,setEventOverlapFlag);
          temp82 = 7;//type
          write_to_eeprom(&temp8,&temp82,setEventOverlapFlag);
          temp82 = 8;//type
          write_to_eeprom(&temp8,&temp82,setEventOverlapFlag);
          temp82 = 9;//type
          write_to_eeprom(&temp8,&temp82,setEventOverlapFlag);
          temp82 = 0;//type
          write_to_eeprom(&temp8,&temp82,setEnergyOverlapFlag);

          write_to_eeprom(&temp8,&temp82,setBillingCutOffLogOverlapFlag);

          temp8=0;
          temp82 = 1;//type
          write_to_eeprom(&temp8,&temp82,setEnergyOverlapFlag);

                        
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                        
          TempLastEnergyValue = 0;
          write_to_eeprom(&TempLastEnergyValue,(uint8_t *)0,setTempLastEnergyValue);

          write_to_eeprom(&ConsumptionSinceLastBilling,(uint8_t *)0,setConsumptionSinceLastBilling);
            //passwords
            //intial key's and intial datas

          uint8_t j,i;
          for(i=0;i<MaxPasswords;i++)
          {
              for(j=0;j<Password_Length;j++)
                 Passwords[i].password[j] = default_passwords[i].password[j];

              aes_enc_dec((unsigned char *)(Passwords[i].password+2),(unsigned char*) (default_keys[i].password),0);
                          
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                          
                          
              write_to_eeprom(&Passwords[i],&i,setPassword);

              for(j=0;j<Password_Length;j++)
                 Passwords[i].password[j] = default_passwords[i].password[j];
            }
            //write EEPROM

            phase->active_energy_counter_QI = 0;
            phase->active_energy_counter_QII = 0;
            phase->active_energy_counter_QIII = 0;
            phase->active_energy_counter_QIV = 0;

            phase->reactive_energy_counter_QI = 0;
            phase->reactive_energy_counter_QII = 0;
            phase->reactive_energy_counter_QIII = 0;
            phase->reactive_energy_counter_QIV = 0;


            phase->consumed_active_energy_QI = 0;
            phase->consumed_active_energy_QII = 0;
            phase->consumed_active_energy_QIII = 0;
            phase->consumed_active_energy_QIV = 0;

            phase->consumed_reactive_energy_QI =0;
            phase->consumed_reactive_energy_QII =0;
            phase->consumed_reactive_energy_QIII =0;
            phase->consumed_reactive_energy_QIV = 0;

            write_to_eeprom(phase,(uint8_t *)0,logPowerFailEnergy);

            //initialize rates
            for(i=0,j=0;i<120;i++,j++)
            {
               if(j == 40)
                    j=0;

               write_to_eeprom(&rates[j],&i,setRate);
               #if defined(USE_WATCHDOG)
                  kick_watchdog();
                #endif
//                           setRate(i , &rates[j]);
            }
                        
            //initialize power breaks
            for(i=0;i<10;i++)
            {
               write_to_eeprom(&power_brakes[i],&i,setPowerBreaks);
//                           setPowerBreaks(i, &power_brakes[i]);
                #if defined(USE_WATCHDOG)
                    kick_watchdog();
                #endif
            }

            //set schadule time slots , set the time slots six hours apart
             Schedule_timeslot_t t;
             uint8_t start_hour[] = {0,6,12,18};
                   
             for (j = 0; j < 12; j++) {

                      t.start_Hour = start_hour[j%4];
                      t.start_minute = 00;
                      t.rate_values_start_pointer = j*10;
                      write_to_eeprom(&t,&j,setTariffSchaduleTimeslot);
//                                  setTariffSchaduleTimeslot(j, &t);
                       write_to_eeprom(&t,&j,setTariffSchaduleTimeslotPassive);
                        #if defined(USE_WATCHDOG)
                          kick_watchdog();
                       #endif
//                                  setTariffSchaduleTimeslotPassive(j, &t);  
             }
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                             

            reFormatMemoryFormatIndicator(MemoryFormattedAddressStart);
            reFormatMemoryFormatIndicator(MemoryFormattedAddressBackUpStart);

            EventLog l;
            l.EventCode = EEPROM_Formatted;
            l.timeStump = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
            l.Checksum  =(getCheckSum(&(l.timeStump.TimestampLow),4) + l.timeStump.TimestampUp + l.EventCode)&0xff;
            l.value = 0;
            temp8 = 3;
            write_to_eeprom(&l,&temp8,log_events);
                        
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                        
        }
        else //log power out and power back logs from flash, set last RTC_CHECK_VALUE
        {
          firtstBoot = 0;
        }


}

void LoadConfigurations()
{
        EEPROM_InitError = 0;
        validation_arg_t validation_arg;
        validation_arg.validate = 1;
        validation_arg.validate_by_reading_back = 0;
        validation_arg_t noValidation = validation_arg_t_default;
        uint8_t temp_did=0;
        uint8_t tmp2 = 0;
        getEnergyOverlapFlag(&temp_did);
        status.energy_log_overlapped = temp_did;
        
        get_daily_snapshot_overlap_flag(&temp_did);
        status.daily_snapshot_energy_overlapped = temp_did;
        tmp2 = 3;
        get_event_overlap_flag(&temp_did, &tmp2); // standard event log
        status.standard_event_log_overlapped = temp_did;
        tmp2 = 4;
        get_event_overlap_flag(&temp_did, &tmp2); // fraud event log
		status.fraud_event_log_overlapped = temp_did;
		tmp2 = 5;
		get_event_overlap_flag(&temp_did, &tmp2); // power quality event log
		status.power_qual_event_log_overlapped = temp_did;
		tmp2 = 6;
		get_event_overlap_flag(&temp_did, &tmp2); // common  event log
		status.common_event_log_overlapped = temp_did;
		tmp2 = 7;
		get_event_overlap_flag(&temp_did, &tmp2); // firmware event log
		status.firmware_event_log_overlapped = temp_did;
		tmp2 = 8;
		get_event_overlap_flag(&temp_did, &tmp2); // synchronization event log
		status.synchronization_event_log_overlapped = temp_did;
		tmp2 = 9;
		get_event_overlap_flag(&temp_did, &tmp2); // disconnect control event log
		status.disconnect_event_log_overlapped = temp_did;

        validation_arg.min = OperatingModeMin;
        validation_arg.max = OperatingModeMax;
        validation_arg.default_val = OperatingMode;
        validation_arg.arg_size = 1;
        read_from_eeprom(&OperatingMode,(uint8_t *)0,getOperatingMode,&validation_arg);
                     
        validation_arg.min = AutoDisplayTimeMin;
        validation_arg.max = AutoDisplayTimeMax;
        validation_arg.default_val = AutoDisplayTime;
        validation_arg.arg_size = 4;
        read_from_eeprom(&AutoDisplayTime,(uint8_t *)0,getAutoDisplayTime,&validation_arg);

        validation_arg.min = BackToAutoModeTimeMin;
        validation_arg.max = BackToAutoModeTimeMax;
        validation_arg.default_val = BackToAutoModeTime;
        validation_arg.arg_size = 2;
        read_from_eeprom(&BackToAutoModeTime,(uint8_t *)0,getBackToAutoModeTime,&validation_arg);
                     
        validation_arg.min = LanguageMin;
        validation_arg.max = LanguageMax;
        validation_arg.default_val = Language;
        validation_arg.arg_size = 1;
        read_from_eeprom(&Language,(uint8_t *)0,getLanguageSelection,&validation_arg);

        validation_arg.min = meter_connected_phaseMin;
        validation_arg.max = meter_connected_phaseMax;
        validation_arg.default_val = meter_connected_phase;
        validation_arg.arg_size = 1;
        read_from_eeprom(&meter_connected_phase,(uint8_t *)0,getMeterConnectedPhase,&validation_arg);
                     
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif
            
        temp_did=0;
        read_from_eeprom(Meter_Sr_No,&temp_did,getDeviceIDs,&noValidation);
        temp_did= 1;
        read_from_eeprom(Manufacturer_Name,&temp_did,getDeviceIDs,&noValidation);
        temp_did= 2;
        read_from_eeprom(Device_ID_2,&temp_did,getDeviceIDs,&noValidation);
        temp_did= 3;
        read_from_eeprom(Device_ID_3,&temp_did,getDeviceIDs,&noValidation);
        temp_did= 4;
        read_from_eeprom(Device_ID_4,&temp_did,getDeviceIDs,&noValidation);
        temp_did= 5;
        read_from_eeprom(Device_ID_5,&temp_did,getDeviceIDs,&noValidation);
        temp_did= 6;
        read_from_eeprom(Device_ID_6,&temp_did,getDeviceIDs,&noValidation);
        temp_did= 7;
        read_from_eeprom(Device_ID_7,&temp_did,getDeviceIDs,&noValidation);
        temp_did= 8;
        read_from_eeprom(Device_ID_8,&temp_did,getDeviceIDs,&noValidation);

        temp_did = 0;
        validation_arg.min = ThrusholdVoltageSagPerMin;
        validation_arg.max = ThrusholdVoltageSagPerMax;
        validation_arg.default_val = voltage_sag_threshold;
        validation_arg.arg_size = 2;
        read_from_eeprom(&voltage_sag_threshold,&temp_did,getMinValues,&validation_arg);

        validation_arg.min = ThrusholdVoltageSwellPerMin;
        validation_arg.max = ThrusholdVoltageSwellPerMax;
        validation_arg.default_val = voltage_swell_threshold;
        validation_arg.arg_size = 2;
        read_from_eeprom(&voltage_swell_threshold,&temp_did,getMaxValues,&validation_arg);

        validation_arg.min = Nominal_VoltageMin;
        validation_arg.max = Nominal_VoltageMax;
        validation_arg.default_val = nominal_voltage;
        validation_arg.arg_size = 2;
        read_from_eeprom(&nominal_voltage,&temp_did,getNominalValues,&validation_arg);

        temp_did = 4;
        validation_arg.min = VoltageThresholdLongPowerFailePerMin;
        validation_arg.max = VoltageThresholdLongPowerFailePerMax;
        validation_arg.default_val = voltage_threshold_for_long_power_failure;
        validation_arg.arg_size = 2;
        read_from_eeprom(&voltage_threshold_for_long_power_failure,&temp_did,getNominalValues,&validation_arg);

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                      
                     
        min_vrms = nominal_voltage - (((voltage_sag_threshold/100) * nominal_voltage)/100);
        max_vrms = nominal_voltage +( ((voltage_swell_threshold/100)* nominal_voltage)/100);
        ThresholdLongPowerFaile = nominal_voltage - (((voltage_threshold_for_long_power_failure/100) * nominal_voltage)/100);

        validation_arg.min = billingSchemaMin;
        validation_arg.max = billingSchemaMax;
        validation_arg.default_val = billingSchema;
        validation_arg.arg_size = 1;
        read_from_eeprom(&billingSchema,(uint8_t *)0,getBillingSchema,&validation_arg);

        validation_arg.min = EnergyLoggingTimeMin;
        validation_arg.max = EnergyLoggingTimeMax;
        validation_arg.default_val = EnergyLoggingTime;
        validation_arg.arg_size = 1;
        read_from_eeprom(&EnergyLoggingTime,(uint8_t *)0,getEnergyLogTime,&validation_arg);

         temp_did = 0;
         validation_arg.min = OverVoltageTripDurationMin;
         validation_arg.max = OverVoltageTripDurationMax;
         validation_arg.default_val = over_voltage_trip_duration;
         validation_arg.arg_size = 2;
         read_from_eeprom(&over_voltage_trip_duration,&temp_did,getTripDuration,&validation_arg);

         temp_did = 1;
         validation_arg.min = OverCurrentTripDurationMin;
         validation_arg.max = OverCurrentTripDurationMax;
         validation_arg.default_val = OverCurrentTripDuration;
         validation_arg.arg_size = 2;
         read_from_eeprom(&OverCurrentTripDuration,&temp_did,getTripDuration,&validation_arg);

         temp_did = 2;
         validation_arg.min = OverFrequencyTripDurationMin;
         validation_arg.max = OverFrequencyTripDurationMax;
         validation_arg.default_val = OverFrequencyTripDuration;
         validation_arg.arg_size = 2;
         read_from_eeprom(&OverFrequencyTripDuration,&temp_did,getTripDuration,&validation_arg);

         temp_did = 3;
         validation_arg.min = ActivePowerTripDurationMin;
         validation_arg.max = ActivePowerTripDurationMax;
         validation_arg.default_val = ActivePowerTripDuration;
         validation_arg.arg_size = 2;
         read_from_eeprom(&ActivePowerTripDuration,&temp_did,getTripDuration,&validation_arg);

         temp_did = 4;
         validation_arg.min = UnderVoltageTripDurationMin;
         validation_arg.max = UnderVoltageTripDurationMax;
         validation_arg.default_val = under_voltage_trip_duration;
         validation_arg.arg_size = 2;
         read_from_eeprom(&under_voltage_trip_duration,&temp_did,getTripDuration,&validation_arg);

         temp_did = 5;
         validation_arg.min = UnderFrequencyTripDurationMin;
         validation_arg.max = UnderFrequencyTripDurationMax;
         validation_arg.default_val = UnderFrequencyTripDuration;
         validation_arg.arg_size = 2;
         read_from_eeprom(&UnderFrequencyTripDuration,&temp_did,getTripDuration,&validation_arg);

         temp_did = 6;
         validation_arg.min = CoverRemovalTripDurationMin;
         validation_arg.max = CoverRemovalTripDurationMax;
         validation_arg.default_val = CoverRemovalTripDuration;
         validation_arg.arg_size = 2;
         read_from_eeprom(&CoverRemovalTripDuration,&temp_did,getTripDuration,&validation_arg);
                      
          #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                    
         //load last energy log address
         temp_did = 0;
         read_from_eeprom(&LastEnergyLogAddress,&temp_did,getLastLogAddress,&noValidation);
         //load last daily snapshot energy log address
         temp_did = 1;
         read_from_eeprom(&last_daily_snapshot_log_address,&temp_did,getLastLogAddress,&noValidation);
         //load last energy billing cuttoff log adddress
         temp_did = 2;
         read_from_eeprom(&LastEnergyBillingCuttOffLogAddress,&temp_did,getLastLogAddress,&noValidation);
         // load last standard event log address
         temp_did = 3;
		 read_from_eeprom(&LastEventLogAddress,&temp_did,getLastLogAddress,&noValidation);
         //load last fraud event log address
         temp_did = 4;
         read_from_eeprom(&last_fraud_event_log_address,&temp_did,getLastLogAddress,&noValidation);
         //load last power quality event log address
         temp_did = 5;
         read_from_eeprom(&last_power_qual_event_log_address,&temp_did,getLastLogAddress,&noValidation);
         //load last common event log address
         temp_did = 6;
         read_from_eeprom(&last_common_event_log_address,&temp_did,getLastLogAddress,&noValidation);
         //load last firmware event log address
         temp_did = 7;
         read_from_eeprom(&last_firmware_event_log_address,&temp_did,getLastLogAddress,&noValidation);
         //load last synchronization event log address
         temp_did = 8;
         read_from_eeprom(&last_synchronization_event_log_address,&temp_did,getLastLogAddress,&noValidation);
         //load last disconnect control event log address
         temp_did = 9;
         read_from_eeprom(&last_disconnect_event_log_address,&temp_did,getLastLogAddress,&noValidation);

         //initialize rates
         int i,j;
         for(i=0,j=0;i<120;i++,j++)
         {
             if(j == 40)
                 j=0;
             read_from_eeprom(&rates[j],&i,getRate,&noValidation);
         }

         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                        
         //initialize power breaks
         for(i=0;i<10;i++)
         {
             read_from_eeprom(&power_brakes[i],&i,getPowerBreaks,&noValidation);
         }

         temp_did =   MagneticTamperType;
         read_from_eeprom(&TamperCount.Magnetic_Count,&temp_did,getTamperCount,&noValidation);

         temp_did =   UC_TamperType;
         read_from_eeprom(&TamperCount.UpperCoverRemoved_Count,&temp_did,getTamperCount,&noValidation);

         temp_did =   LC_TamperType;
         read_from_eeprom(&TamperCount.LowerCoverRemoved_Count,&temp_did,getTamperCount,&noValidation);

         temp_did =   NeutralTamperType;
         read_from_eeprom(&TamperCount.Neutral_Count,&temp_did,getTamperCount,&noValidation);
                      
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif                       
                                         
         i=0;
         //read the passwords ,uncomment this at the end of testing
         for(i=0;i<4;i++)
         {
             read_from_eeprom(&Passwords[i],&i,getPassword,&noValidation);
             aes_enc_dec((unsigned char *)(Passwords[i].password+2),(unsigned char*) (default_keys[i].password),1);
           #if defined(USE_WATCHDOG)
              kick_watchdog();
           #endif
        }
}


int write_to_eeprom(void *writeArgument_1,void *writeArgument_2,int8_t(*write)(void *writeArgument_1,void *writeArgument_2))
{
      	uint8_t x=0,i=0;
        
        for(;i<MAX_LOG_RETRAY;i++)
        {
           x =  write(writeArgument_1,writeArgument_2);
           if(x == TRUE)
             break;
        }
        
        if(i>=MAX_LOG_RETRAY) //the logging was not successfull 
          return 0;
 
   return 1;
}
uint32_t reading_error_counter = 0;
validation_arg_t *v;// = (validation_arg_t *)validation_arg;
//#define Log_read_from_eeprom

int read_from_eeprom(void *readArgument_1,void *readArgument_2,int8_t(*read)(void *readArgument1,void *readArgument2),void *validation_arg)
{
    uint8_t x=0,i=0,t=0;
    
    v = (validation_arg_t *)validation_arg;
    t = v->arg_size;
    for(;i<MAX_LOG_RETRAY;i++)
    {
        #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif
       x =  read(readArgument_1,readArgument_2);
       
        #ifdef Log_read_from_eeprom
               printf("\nx=%d",x);
        #endif 
         
       if(x == TRUE)
       {
          if(v->validate == 1) //check for the argument range
          {
           

            #ifdef Log_read_from_eeprom
               printf("\nd_s=%d",t);
            #endif 
               
            if(t==1)
            {        
                uint8_t *data_8 = (uint8_t *)readArgument_1;
  //            uint32_t *data = (uint32_t*)readArgument_1;
                if(*data_8 > v->max || *data_8 < v->min)
                {
                   continue;
                }

            }
            else if(t==2)
            {
                uint16_t *data_16 = (uint16_t *)readArgument_1;
                if(*data_16 > v->max || *data_16 < v->min)
                {
                   continue;
                }

            } 
            else if(t==4)
            {
                uint32_t *data_32 = (uint32_t *)readArgument_1;
                if(*data_32 > v->max || *data_32 < v->min)
                {
                   continue;
                }
            }
            
          }
         break;
       }
    }
    
    if(i>=MAX_LOG_RETRAY) //the logging was not successfull 
    {
      reading_error_counter++;
    #ifdef Log_read_from_eeprom
          printf("\nFailer");
    #endif      
      if(v->validate == 1)
      {
          if(t==1)
            {        
                uint8_t *temp_data = (uint8_t *)readArgument_1;
                *temp_data = v->default_val; //return the default value 
            }
            else if(t==2)
            {
                uint16_t *temp_data = (uint16_t *)readArgument_1;
                *temp_data = v->default_val; //return the default value 
            } 
            else if(t==4)
            {
                uint32_t *temp_data = (uint32_t *)readArgument_1;
                *temp_data = v->default_val; //return the default value 
            }
      }
      return 0;
    }
    
    #ifdef Log_read_from_eeprom
      else{
          printf("\nSUCESS");
      }
    #endif

   return 1;

}

rtc_t temp_rtc0;
rtc_t temp_rtc1;
/**
* compare if two energy log values have the same time stamp or not 
* @param energy0: the first energy value
* @param energy1: the second energy value
* @see compaer_time for the return values
*/
int8_t compare_energy(void *energy0,void *energy1)
{
  EnergyLog *el = (EnergyLog*)energy0;
  EnergyLog *el2 = (EnergyLog*)energy1;
  
 temp_rtc0 = getTime(&el->timeStump);
 temp_rtc1 = getTime(&el2->timeStump);
 
 return compare_time(&temp_rtc0,&temp_rtc1);
}
/**
* compare if two event log values have the same time stamp or not 
* @param event0: the first energy value
* @param evnet1: the second energy value
* @see compaer_time for the return values
*/
int8_t compare_event(void *event0,void *event1)
{
  EventLog *el = (EventLog*)event0;
  EventLog *el2 = (EventLog*)event1;
  
 temp_rtc0 = getTime(&el->timeStump);
 temp_rtc1 = getTime(&el2->timeStump);
 
 return compare_time(&temp_rtc0,&temp_rtc1);
}

/**
* Updates the address where the next energy is logged. 
* @param type:- the type of log
*               0 ernergy log
*               1 daily snapshot energy profile log
*               2 billing cutoff log 
*               3 standard event log
*               4 fraud event log
*               5 power quality event log
*               6 common event log
*               7 firmware event log
*               8 synchronization event log
*               9 dsiconnect control event log
*/
uint8_t updateNextLogAddress(uint8_t type)
{

    uint32_t tmp;
    uint8_t tmp2;
	if(type == 0) // incremental energy log
	{
        LastEnergyLogAddress += EnergyLogSize;//was 14
        if(LastEnergyLogAddress >=  EnergyLogAddress_End)
          {
              tmp2 = 1;
              uint8_t z = setEnergyOverlapFlag(&tmp2,&type);

                if(z==0)
                    return 0;
                  LastEnergyLogAddress = EnergyLogAddress_Start;
          }
            tmp = LastEnergyLogAddress;
           //setLastLogAddress(&tmp,&type); //for no battery back up
	}
	else if(type == 1)
    {
        last_daily_snapshot_log_address += DAILY_SNAPSHOT_LOG_SIZE;//was 14
        if(last_daily_snapshot_log_address >=  DAILY_SNAPSHOT_LOG_ADDRESS_END)
        {
             tmp2 = 1;
             uint8_t z = setEnergyOverlapFlag(&tmp2,&type);

               if(z==0)
                   return 0;
               last_daily_snapshot_log_address = DAILY_SNAPSHOT_LOG_ADDRESS_START;
        }
        tmp = last_daily_snapshot_log_address;
    }
    else if(type == 2) //billing cutoff date energy log //test this
    {
      LastEnergyBillingCuttOffLogAddress += EnergyBillingCutoffDayLogSize;//4*8+4+5 +1
      if(LastEnergyBillingCuttOffLogAddress >= EnergyBillingCuttOffLogAddress_end)
      {
             tmp2 = 1;
             uint8_t z = setBillingCutOffLogOverlapFlag(&tmp2,(uint8_t *)0);
     if(z==0)
         return 0;
        LastEnergyBillingCuttOffLogAddress = EnergyBillingCuttOffLogAddress_start;

      }
      tmp = LastEnergyBillingCuttOffLogAddress;
      //setLastLogAddress(&tmp,&type);//for no battery back up
    }
    else if(type == 3) //standard event log
	{
        LastEventLogAddress += EventLogSize;
		if(LastEventLogAddress > EventLog_End)
		{
            tmp2 = 1;
			uint8_t z = setEventOverlapFlag(&tmp2,&type);
			if(z==0)
				return 0;
			LastEventLogAddress = EventLogAddress_Start;
		}

		tmp =  LastEventLogAddress;
	}

	else if(type == 4) // fraud event log
    {
		last_fraud_event_log_address += EVENT_LOG_TYPE_SIZE;

        if(last_fraud_event_log_address >  FRAUD_EVENT_LOG_ADDRESS_END)
        {
        	tmp2 = 1;
			uint8_t z = setEventOverlapFlag(&tmp2,&type);

		    if(z==0)
			   return 0;
		    last_fraud_event_log_address = FRAUD_EVENT_LOG_ADDRESS_START;
        }
        tmp = last_fraud_event_log_address;
    }
	else if(type == 5) // power quality event log
    {
		last_power_qual_event_log_address += EVENT_LOG_TYPE_SIZE;

        if(last_power_qual_event_log_address >  POWER_QUAL_LOG_ADDRESS_END)
        {
        	tmp2 = 1;
			uint8_t z = setEventOverlapFlag(&tmp2,&type);

		    if(z==0)
			   return 0;
		    last_power_qual_event_log_address = POWER_QUAL_LOG_ADDRESS_START;
        }
        tmp = last_power_qual_event_log_address;
    }
	else if(type == 6) // common event log
    {
		last_common_event_log_address += EVENT_LOG_TYPE_SIZE;

        if(last_common_event_log_address >  COMMON_LOG_ADDRESS_END)
        {
        	tmp2 = 1;
			uint8_t z = setEventOverlapFlag(&tmp2,&type);

		    if(z==0)
			   return 0;
		    last_common_event_log_address = COMMON_LOG_ADDRESS_START;
        }
        tmp = last_common_event_log_address;
    }
	else if(type == 7) // firmware event log
    {
		last_firmware_event_log_address += FIRMWARE_EVENT_LOG_TYPE_SIZE;

        if(last_firmware_event_log_address >  FIRMWARE_LOG_ADDRESS_END)
        {
        	tmp2 = 1;
			uint8_t z = setEventOverlapFlag(&tmp2,&type);

		    if(z==0)
			   return 0;
		    last_firmware_event_log_address = FIRMWARE_LOG_ADDRESS_START;
        }
        tmp = last_firmware_event_log_address;
    }
	else if(type == 8) // synchronization event log
    {
		last_synchronization_event_log_address += TIME_BOUND_EVENT_LOG_TYPE_SIZE;

        if(last_synchronization_event_log_address >  SYNCHRONIZATION_LOG_ADDRESS_END)
        {
        	tmp2 = 1;
			uint8_t z = setEventOverlapFlag(&tmp2,&type);

		    if(z==0)
			   return 0;
		    last_synchronization_event_log_address = SYNCHRONIZATION_LOG_ADDRESS_START;
        }
        tmp = last_synchronization_event_log_address;
    }
	else if(type == 9) // disconnect control event log
    {
		last_disconnect_event_log_address += DISCONNECT_EVENT_LOG_TYPE_SIZE;

        if(last_disconnect_event_log_address >  DISCONNECT_LOG_ADDRESS_END)
        {
        	tmp2 = 1;
			uint8_t z =	setEventOverlapFlag(&tmp2,&type);

		    if(z==0)
			   return 0;
		    last_disconnect_event_log_address = DISCONNECT_LOG_ADDRESS_START;
        }
        tmp = last_disconnect_event_log_address;
    }
	setLastLogAddress(&tmp,&type);//for no battery back up
	return 1;
}


int8_t logEvent(void *l2,void *dummy)
{
        uint8_t x=0;//,i=0;
        EventLog *l = (EventLog *)l2;
        
        if(LastEventLogAddress<EventLogAddress_Start || LastEventLogAddress>EventLog_End)
          LastEventLogAddress = EventLogAddress_Start;
        
//        if(status.rtc_init == 1)
//        {
  //        for(;i<MAX_LOG_RETRAY;i++)
  //        {
          x= EEPROM2_WriteInt8(l->EventCode,LastEventLogAddress,0);
  //	if(x==0)
  //	     continue;
          
          x= EEPROM2_WriteNextLong(l->value,0);
  //        if(x==0)
  //             continue;
          x = EEPROM2_WriteNextInt8(l->EventGroup,0);
  //        if(x==0)
  //        	continue;        
          x= EEPROM2_WriteNextLong(l->timeStump.TimestampLow,0);
  //        if(x==0)
  //             continue;

          x = EEPROM2_WriteNextInt8(l->timeStump.TimestampUp,0);
  //        if(x==0)
  //        	continue;

          x= EEPROM2_WriteNextInt8(l->Checksum,1);
          if(x==0)
            return 0;

          updateNextLogAddress(3);//1 for event log
//        }else
//          return 0;
        
        return 1;
}

/*
 * Logs different types of events into the eeprom
 */
int8_t log_events(void *l2,void *type)
{
	uint8_t x = 0;
    uint8_t *ptr = (uint8_t *)type;
	uint8_t val = *ptr;
    if(val == 3) // standard event log
    {
    	EventLog *l = (EventLog *)l2;
    	if(LastEventLogAddress < EventLogAddress_Start || LastEventLogAddress > EventLog_End)
    		LastEventLogAddress = EventLogAddress_Start;

        x = EEPROM2_WriteInt8(l->EventCode,LastEventLogAddress,0);
	    x = EEPROM2_WriteNextLong(l->value,0);
		x = EEPROM2_WriteNextInt8(l->EventGroup,0);
	    x = EEPROM2_WriteNextLong(l->timeStump.TimestampLow,0);
        x = EEPROM2_WriteNextInt8(l->timeStump.TimestampUp,0);
        x = EEPROM2_WriteNextInt8(l->Checksum,1);
    	if(x==0)
    		return 0;
    }
    else if(val == 4) // fraud event log
    {
    	event_log *l = (event_log *)l2;
    	if(last_fraud_event_log_address < FRAUD_EVENT_LOG_ADDRESS_START || last_fraud_event_log_address > FRAUD_EVENT_LOG_ADDRESS_END)
           last_fraud_event_log_address = FRAUD_EVENT_LOG_ADDRESS_START;

        x = EEPROM2_WriteInt8(l->event_code,last_fraud_event_log_address,0);
        x = EEPROM2_WriteNextLong(l->time_stamp.TimestampLow,0);
        x = EEPROM2_WriteNextInt8(l->time_stamp.TimestampUp,0);
        x = EEPROM2_WriteNextInt8(l->checksum,1);
        if(x==0)
        	return 0;
    }
    else if(val == 5) // power quality event log
    {
    	event_log *l = (event_log *)l2;
    	if(last_power_qual_event_log_address < POWER_QUAL_LOG_ADDRESS_START || last_power_qual_event_log_address > POWER_QUAL_LOG_ADDRESS_END)
    		last_power_qual_event_log_address = POWER_QUAL_LOG_ADDRESS_START;

        x = EEPROM2_WriteInt8(l->event_code,last_power_qual_event_log_address,0);
        x = EEPROM2_WriteNextLong(l->time_stamp.TimestampLow,0);
        x = EEPROM2_WriteNextInt8(l->time_stamp.TimestampUp,0);
        x = EEPROM2_WriteNextInt8(l->checksum,1);
        if(x==0)
        	return 0;
    }
    else if(val == 6) // common event log
    {
    	event_log *l = (event_log *)l2;
    	if(last_common_event_log_address < COMMON_LOG_ADDRESS_START || last_common_event_log_address > COMMON_LOG_ADDRESS_END)
    		last_common_event_log_address = COMMON_LOG_ADDRESS_START;

        x = EEPROM2_WriteInt8(l->event_code,last_common_event_log_address,0);
        x = EEPROM2_WriteNextLong(l->time_stamp.TimestampLow,0);
        x = EEPROM2_WriteNextInt8(l->time_stamp.TimestampUp,0);
        x = EEPROM2_WriteNextInt8(l->checksum,1);
        if(x==0)
        	return 0;
    }
    else if(val == 7) // firmware event log
    {
    	firmware_event_log *l = (firmware_event_log *)l2;
    	if(last_firmware_event_log_address < FIRMWARE_LOG_ADDRESS_START || last_firmware_event_log_address > FIRMWARE_LOG_ADDRESS_END)
    		last_firmware_event_log_address = FIRMWARE_LOG_ADDRESS_START;

        x = EEPROM2_WriteInt8(l->event_code,last_firmware_event_log_address,0);
        x = EEPROM2_WriteNextInt8(l->active_firmware[0],0);
        x = EEPROM2_WriteNextInt8(l->active_firmware[1],0);
        x = EEPROM2_WriteNextInt8(l->active_firmware[2],0);
        x = EEPROM2_WriteNextInt8(l->active_firmware[3],0);
        x = EEPROM2_WriteNextInt8(l->active_firmware[4],0);
        x = EEPROM2_WriteNextLong(l->time_stamp.TimestampLow,0);
        x = EEPROM2_WriteNextInt8(l->time_stamp.TimestampUp,0);
        x = EEPROM2_WriteNextInt8(l->checksum,1);
        if(x==0)
        	return 0;
    }
    else if(val == 8) // synchronization event log
    {
    	time_bound_event_log *l = (time_bound_event_log *)l2;
    	if(last_synchronization_event_log_address < SYNCHRONIZATION_LOG_ADDRESS_START || last_synchronization_event_log_address > SYNCHRONIZATION_LOG_ADDRESS_END)
    		last_synchronization_event_log_address = SYNCHRONIZATION_LOG_ADDRESS_START;

        x = EEPROM2_WriteInt8(l->event_code,last_synchronization_event_log_address,0);
        x = EEPROM2_WriteNextLong(l->begin_time_stamp.TimestampLow,0);
	    x = EEPROM2_WriteNextInt8(l->begin_time_stamp.TimestampUp,0);
	    x = EEPROM2_WriteNextLong(l->end_time_stamp.TimestampLow,0);
		x = EEPROM2_WriteNextInt8(l->end_time_stamp.TimestampUp,0);
        x = EEPROM2_WriteNextInt8(l->checksum,1);
        if(x==0)
        	return 0;
    }
	else if(val == 9) // disconnect control event log
	{
		disconnect_event_log *l = (disconnect_event_log *)l2;
		if(last_disconnect_event_log_address < DISCONNECT_LOG_ADDRESS_START || last_disconnect_event_log_address > DISCONNECT_LOG_ADDRESS_END)
			last_disconnect_event_log_address = DISCONNECT_LOG_ADDRESS_START;

		x = EEPROM2_WriteInt8(l->event_code,last_disconnect_event_log_address,0);
		x = EEPROM2_WriteNextInt8(l->disconnect_control_status,0);
		x = EEPROM2_WriteNextLong(l->time_stamp.TimestampLow,0);
		x = EEPROM2_WriteNextInt8(l->time_stamp.TimestampUp,0);
		x = EEPROM2_WriteNextInt8(l->checksum,1);
		if(x==0)
			return 0;
	}

    updateNextLogAddress(val);
    return 1;
}
//uint8_t logEnergy(EnergyLog *l)
int8_t logEnergy(void *l2,void *dummy)
{
        EnergyLog *l = (EnergyLog *)l2;
    	uint8_t x=0;//,i=0;
        if(LastEnergyLogAddress<EnergyLogAddress_Start || LastEnergyLogAddress>EnergyLogAddress_End)
          LastEnergyLogAddress = EnergyLogAddress_Start;
          
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
		//x=EEPROM2_WriteLong(l->ActiveEnergy,l->startAddress,0);
    	x=EEPROM2_WriteLong(l->active_energy,LastEnergyLogAddress,0);
//	if(x==0)
//		continue;

	x= EEPROM2_WriteNextLong(l->reactive_energy_QI, 0);
//	if(x==0)
//		continue;
//       Uncomment this if power and voltage log is enabled 
        x= EEPROM2_WriteNextLong(l->active_power, 0);
//	if(x==0)
//		continue;
        
        x= EEPROM2_WriteNextLong(l->reactive_energy_QIV, 0);
//	if(x==0)
//		continue;
        
	x= EEPROM2_WriteNextLong(l->timeStump.TimestampLow,0);
//        if(x==0)
//        	continue;

        x = EEPROM2_WriteNextInt8(l->timeStump.TimestampUp,0);
//        if(x==0)
//        	continue;

        x= EEPROM2_WriteNextInt8(l->CRC,1);
        if(x==0)
          return 0;
//        	continue;
//        else
//          break;
//        }
        
//        if(i>=MAX_LOG_RETRAY) //the logging was not successfull 
//          return 0;
        
        updateNextLogAddress(0);//0 for energy log
        return 1;
}

int8_t log_daily_energy_snapshot(void *l2,void *dummy)
{
        EnergyLog *l = (EnergyLog *)l2;
        uint8_t x=0;//,i=0;
        if(last_daily_snapshot_log_address<DAILY_SNAPSHOT_LOG_ADDRESS_START || last_daily_snapshot_log_address>DAILY_SNAPSHOT_LOG_ADDRESS_END)
            last_daily_snapshot_log_address = DAILY_SNAPSHOT_LOG_ADDRESS_START;

        x=EEPROM2_WriteLong(l->active_energy,last_daily_snapshot_log_address,0);
        x= EEPROM2_WriteNextLong(l->reactive_energy_QI, 0);
        x= EEPROM2_WriteNextLong(l->active_power, 0);
        x= EEPROM2_WriteNextLong(l->reactive_energy_QIV, 0);
        x= EEPROM2_WriteNextLong(l->timeStump.TimestampLow,0);
        x = EEPROM2_WriteNextInt8(l->timeStump.TimestampUp,0);
        x= EEPROM2_WriteNextInt8(l->CRC,1);
        if(x==0)
          return 0;
        updateNextLogAddress(1);//1 for energy daily snaphot log
        return 1;
}
uint8_t get_daily_snapshot_energy(EnergyLog *l,unsigned long StartAddress)
{
    uint8_t x=0,i=0;

    for(;i<MAX_LOG_RETRAY;i++)
    {
        x=EEPROM2_ReadLong(StartAddress,0,&(l->active_energy));
        if(x==0)
        {
            continue;
        }

        x= EEPROM2_ReadNextLong(0,&(l->reactive_energy_QI));
        if(x==0)
        {
            continue;
        }

        x= EEPROM2_ReadNextLong(0,&(l->active_power));
        if(x==0)
        {
            continue;
        }

        x= EEPROM2_ReadNextLong(0,&(l->reactive_energy_QIV));
        if(x==0)
        {
            continue;
        }
        x= EEPROM2_ReadNextLong(0,&(l->timeStump.TimestampLow));
        if(x==0)
        {
            continue;
        }

        x = EEPROM2_ReadNextInt8(0,&(l->timeStump.TimestampUp));
        if(x==0)
        {
            continue;
        }

        x= EEPROM2_ReadNextInt8(1,&(l->CRC));
        if(x==0)
        {
            continue;
        }

        else
          break;
    }

        if(i>=MAX_LOG_RETRAY)
          return 0;

        return 1;
}
uint8_t getEnergy(EnergyLog *l,unsigned long StartAddress)
{
   	uint8_t x=0,i=0;
        
        for(;i<MAX_LOG_RETRAY;i++)
        {
	x=EEPROM2_ReadLong(StartAddress,0,&(l->active_energy));
	if(x==0)
		continue;

	x= EEPROM2_ReadNextLong(0,&(l->reactive_energy_QI));
	if(x==0)
		continue;

//       Uncomment this if power and voltage log is enabled 
        x= EEPROM2_ReadNextLong(0,&(l->active_power));
	if(x==0)
		continue;
        
        x= EEPROM2_ReadNextLong(0,&(l->reactive_energy_QIV));
	if(x==0)
		continue;                        
		x= EEPROM2_ReadNextLong(0,&(l->timeStump.TimestampLow));
        if(x==0)
        	continue;

        x = EEPROM2_ReadNextInt8(0,&(l->timeStump.TimestampUp));
        if(x==0)
        	continue;

        x= EEPROM2_ReadNextInt8(1,&(l->CRC));
        if(x==0)
        	continue;
        else
          break;
        }
        
        if(i>=MAX_LOG_RETRAY)
          return 0;
        
        return 1;
}

uint8_t getEvent(EventLog *l,unsigned long StartAddres)
{
    	uint8_t x=0,i=0;
        for(;i<MAX_LOG_RETRAY;i++)
        {
	x= EEPROM2_ReadInt8(StartAddres,0,&(l->EventCode));
	if(x==0)
		continue;
        
	x= EEPROM2_ReadNextLong(0,&(l->value));
        if(x==0)
        	continue;
        
        x = EEPROM2_ReadNextInt8(0,&(l->EventGroup));
        if(x==0)
        	continue;
        
	x= EEPROM2_ReadNextLong(0,&(l->timeStump.TimestampLow));
        if(x==0)
        	continue;

        x = EEPROM2_ReadNextInt8(0,&(l->timeStump.TimestampUp));
        if(x==0)
        	continue;

        x= EEPROM2_ReadNextInt8(1,&(l->Checksum));
        if(x==0)
        	continue;
        else
          break;
        }
        
        if(i>=MAX_LOG_RETRAY)
          return 0;
        
        return 1;
}







/*
int8_t logEvent2(void *lt,void *StartAddress)
{
          EventLog *l = (EventLog*)lt;
          uint8_t x=0;
        uint32_t *start_add = StartAddress;
        uint32_t add = *start_add;
        
	x= EEPROM2_WriteInt8(l->EventCode,add,0);
	x= EEPROM2_WriteNextLong(l->value,0);
        x = EEPROM2_WriteNextInt8(l->EventGroup,0);     
	x= EEPROM2_WriteNextLong(l->timeStump.TimestampLow,0);
        x = EEPROM2_WriteNextInt8(l->timeStump.TimestampUp,0);
        l->Checksum = l->EventCode + l->EventGroup + 
                      l->timeStump.TimestampLow + 
                      l->timeStump.TimestampUp + 
                      l->value;
        x= EEPROM2_WriteNextInt8(l->Checksum,1);
        x++;

        return 1;
}
*/

int8_t logEnergy2(void *lt,uint32_t StartAddress)
{
        EnergyLog *l = (EnergyLog*)lt;
    	uint8_t x=0;//,i=0;
        

    	x=EEPROM2_WriteLong(l->active_energy,StartAddress,0);


    	x= EEPROM2_WriteNextLong(l->reactive_energy_QI, 0);

        x= EEPROM2_WriteNextLong(l->active_power, 0);

        
        x= EEPROM2_WriteNextLong(l->reactive_energy_QIV, 0);

        
        x= EEPROM2_WriteNextLong(l->timeStump.TimestampLow,0);


        x = EEPROM2_WriteNextInt8(l->timeStump.TimestampUp,0);


        x= EEPROM2_WriteNextInt8(l->CRC,1);
        if(x==0)
        	return 0;

        
        return 1;
}



int8_t getEnergy2(void *lt,uint32_t EntryNumber)
{
       int32_t en = --EntryNumber;
       if(en < 0)
          EntryNumber = 0;
       
       //--EntryNumber;//handle zero based index 
       EnergyLog *l = (EnergyLog*)lt;
//       uint8_t x=0;//,i=0;
       
       uint32_t StartAddress;
       if(status.energy_log_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (EnergyLogSize*EntryNumber);
          if(val > EnergyLogAddress_End)
          {
             StartAddress = val + EnergyLogAddress_Start - EventLog_End;            
          }
          else
          {
             StartAddress = (LastEnergyLogAddress + val); 
          }
       }
       else       
          StartAddress = EnergyLogAddress_Start + (EnergyLogSize*EntryNumber);
       
       if( getEnergy(l,StartAddress) == 0 )
         return 0;

        return 1;
}

int8_t get_daily_snapshot_energy_profile(void *lt,uint32_t EntryNumber)
{
       int32_t en = --EntryNumber;
       if(en < 0)
       {
           EntryNumber = 0;
       }
       EnergyLog *l = (EnergyLog*)lt;
       uint32_t StartAddress;
       if(status.daily_snapshot_energy_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (DAILY_SNAPSHOT_LOG_SIZE*EntryNumber);
          if(last_daily_snapshot_log_address + val > DAILY_SNAPSHOT_LOG_ADDRESS_END)
          {
              StartAddress = last_daily_snapshot_log_address + val - DAILY_SNAPSHOT_LOG_ADDRESS_END;
             //StartAddress = val + DAILY_SNAPSHOT_LOG_ADDRESS_START - EventLog_End;
          }
          else{
             StartAddress = (last_daily_snapshot_log_address + val);
          }
       }
       else
          StartAddress = DAILY_SNAPSHOT_LOG_ADDRESS_START + (DAILY_SNAPSHOT_LOG_SIZE * EntryNumber);
       if( get_daily_snapshot_energy(l,StartAddress) == 0 )
         return 0;
       return 1;
}
int8_t getEvent2(void *lt,uint32_t EntryNumber)
{
      --EntryNumber;//handle zero based index 
       EventLog *l = (EventLog*)lt;
//       uint8_t x=0;//,i=0;
       
       uint32_t StartAddress;
       if(status.standard_event_log_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (EventLogSize*EntryNumber);
          if(val + LastEventLogAddress > EventLog_End)
          {
             StartAddress = val + LastEventLogAddress - EventLog_End;
          }
          else{
             StartAddress = (LastEventLogAddress + val); //TODO it jumps the first entry
          }
       }
       else       
          StartAddress = EventLogAddress_Start + (EventLogSize*EntryNumber);
       if(getEvent(l,StartAddress)==0)
         return 0;

        return 1;
}
int8_t compare_fraud_event(void *event0,void *event1)
{
	event_log *el = (event_log*)event0;
	event_log *el2 = (event_log*)event1;

	temp_rtc0 = getTime(&el->time_stamp);
	temp_rtc1 = getTime(&el2->time_stamp);

	return compare_time(&temp_rtc0,&temp_rtc1);
}
uint8_t get_fraud_event_log(event_log *l,unsigned long StartAddres)
{
	uint8_t x=0,i=0;
    for(;i<MAX_LOG_RETRAY;i++)
    {
    	x= EEPROM2_ReadInt8(StartAddres,0,&(l->event_code));
		if(x==0)
			continue;
		x= EEPROM2_ReadNextLong(0,&(l->time_stamp.TimestampLow));
		if(x==0)
        	continue;

        x = EEPROM2_ReadNextInt8(0,&(l->time_stamp.TimestampUp));
        if(x==0)
        	continue;

        x= EEPROM2_ReadNextInt8(1,&(l->checksum));
        if(x==0)
        	continue;
        else
          break;
        }
        if(i>=MAX_LOG_RETRAY)
          return 0;
        return 1;
}
int8_t get_fraud_event(void *lt,uint32_t EntryNumber)
{
      --EntryNumber;//handle zero based index
       event_log *l = (event_log*)lt;
       uint32_t StartAddress;
       if(status.fraud_event_log_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (EVENT_LOG_TYPE_SIZE*EntryNumber);
          if(val + last_fraud_event_log_address > FRAUD_EVENT_LOG_ADDRESS_END)
          {
             StartAddress = val + last_fraud_event_log_address - FRAUD_EVENT_LOG_ADDRESS_END;
          }
          else{
             StartAddress = (last_fraud_event_log_address + val); //TODO it jumps the first entry
          }
       }
       else
          StartAddress = FRAUD_EVENT_LOG_ADDRESS_START + (EVENT_LOG_TYPE_SIZE*EntryNumber);
       if(get_fraud_event_log(l,StartAddress)==0)
         return 0;

        return 1;
}
uint8_t get_power_qual_event_log(event_log *l,unsigned long StartAddres)
{
	uint8_t x=0,i=0;
    for(;i<MAX_LOG_RETRAY;i++)
    {
    	x= EEPROM2_ReadInt8(StartAddres,0,&(l->event_code));
		if(x==0)
			continue;
		x= EEPROM2_ReadNextLong(0,&(l->time_stamp.TimestampLow));
		if(x==0)
        	continue;
        x = EEPROM2_ReadNextInt8(0,&(l->time_stamp.TimestampUp));
        if(x==0)
        	continue;
        x= EEPROM2_ReadNextInt8(1,&(l->checksum));
        if(x==0)
        	continue;
        else
          break;
        }
        if(i>=MAX_LOG_RETRAY)
          return 0;
        return 1;
}
int8_t get_power_qual_event(void *lt,uint32_t EntryNumber)
{
      --EntryNumber;//handle zero based index
       event_log *l = (event_log*)lt;
       uint32_t StartAddress;
       if(status.power_qual_event_log_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (EVENT_LOG_TYPE_SIZE*EntryNumber);
          if(val + last_power_qual_event_log_address > POWER_QUAL_LOG_ADDRESS_END)
          {
             StartAddress = val + last_power_qual_event_log_address - POWER_QUAL_LOG_ADDRESS_END;
          }
          else{
             StartAddress = (last_power_qual_event_log_address + val); //TODO it jumps the first entry
          }
       }
       else
          StartAddress = POWER_QUAL_LOG_ADDRESS_START + (EVENT_LOG_TYPE_SIZE*EntryNumber);
       if(get_power_qual_event_log(l,StartAddress)==0)
         return 0;

        return 1;
}
uint8_t get_common_event_log(event_log *l,unsigned long StartAddres)
{
	uint8_t x=0,i=0;
    for(;i<MAX_LOG_RETRAY;i++)
    {
    	x= EEPROM2_ReadInt8(StartAddres,0,&(l->event_code));
		if(x==0)
			continue;
		x= EEPROM2_ReadNextLong(0,&(l->time_stamp.TimestampLow));
		if(x==0)
        	continue;
        x = EEPROM2_ReadNextInt8(0,&(l->time_stamp.TimestampUp));
        if(x==0)
        	continue;
        x= EEPROM2_ReadNextInt8(1,&(l->checksum));
        if(x==0)
        	continue;
        else
          break;
        }
        if(i>=MAX_LOG_RETRAY)
          return 0;
        return 1;
}
int8_t get_common_event(void *lt,uint32_t EntryNumber)
{
      --EntryNumber;//handle zero based index
       event_log *l = (event_log*)lt;
       uint32_t StartAddress;
       if(status.common_event_log_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (EVENT_LOG_TYPE_SIZE*EntryNumber);
          if(val + last_common_event_log_address > COMMON_LOG_ADDRESS_END)
          {
             StartAddress = val + last_common_event_log_address - COMMON_LOG_ADDRESS_END;
          }
          else{
             StartAddress = (last_common_event_log_address + val); //TODO it jumps the first entry
          }
       }
       else
          StartAddress = COMMON_LOG_ADDRESS_START + (EVENT_LOG_TYPE_SIZE*EntryNumber);
       if(get_common_event_log(l,StartAddress)==0)
         return 0;

        return 1;
}
int8_t compare_firmware_event(void *event0,void *event1)
{
	firmware_event_log *el = (firmware_event_log*)event0;
	firmware_event_log *el2 = (firmware_event_log*)event1;

	temp_rtc0 = getTime(&el->time_stamp);
	temp_rtc1 = getTime(&el2->time_stamp);

	return compare_time(&temp_rtc0,&temp_rtc1);
}
uint8_t get_firmware_event_log(firmware_event_log *l,unsigned long StartAddres)
{
	uint8_t x=0,i=0;
    for(;i<MAX_LOG_RETRAY;i++)
    {
    	x= EEPROM2_ReadInt8(StartAddres,0,&(l->event_code));
		if(x==0)
			continue;
		x = EEPROM2_ReadNextInt8(0,&(l->active_firmware[0]));
		if(x==0)
			continue;
		x = EEPROM2_ReadNextInt8(0,&(l->active_firmware[1]));
		if(x==0)
			continue;
		x = EEPROM2_ReadNextInt8(0,&(l->active_firmware[2]));
		if(x==0)
			continue;
		x = EEPROM2_ReadNextInt8(0,&(l->active_firmware[3]));
		if(x==0)
			continue;
		x = EEPROM2_ReadNextInt8(0,&(l->active_firmware[4]));
		if(x==0)
			continue;
		x= EEPROM2_ReadNextLong(0,&(l->time_stamp.TimestampLow));
		if(x==0)
        	continue;
        x = EEPROM2_ReadNextInt8(0,&(l->time_stamp.TimestampUp));
        if(x==0)
        	continue;
        x= EEPROM2_ReadNextInt8(1,&(l->checksum));
        if(x==0)
        	continue;
        else
          break;
	}
	if(i>=MAX_LOG_RETRAY)
	  return 0;
	return 1;
}
int8_t get_firmware_event(void *lt,uint32_t EntryNumber)
{
      --EntryNumber;//handle zero based index
       firmware_event_log *l = (firmware_event_log*)lt;
       uint32_t StartAddress;
       if(status.firmware_event_log_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (FIRMWARE_EVENT_LOG_TYPE_SIZE * EntryNumber);
          if(val + last_firmware_event_log_address > FIRMWARE_LOG_ADDRESS_END)
          {
             StartAddress = val + last_firmware_event_log_address - FIRMWARE_LOG_ADDRESS_END;
          }
          else{
             StartAddress = (last_firmware_event_log_address + val); //TODO it jumps the first entry
          }
       }
       else
          StartAddress = FIRMWARE_LOG_ADDRESS_START + (FIRMWARE_EVENT_LOG_TYPE_SIZE*EntryNumber);
       if(get_firmware_event_log(l,StartAddress)==0)
         return 0;

        return 1;
}
int8_t compare_synchronization_event(void *event0,void *event1)
{
	time_bound_event_log *el = (time_bound_event_log*)event0;
	time_bound_event_log *el2 = (time_bound_event_log*)event1;

	temp_rtc0 = getTime(&el->end_time_stamp);
	temp_rtc1 = getTime(&el2->end_time_stamp);

	return compare_time(&temp_rtc0,&temp_rtc1);
}
uint8_t get_synchronization_event_log(time_bound_event_log *l,unsigned long StartAddres)
{
	uint8_t x=0,i=0;
    for(;i<MAX_LOG_RETRAY;i++)
    {
    	x= EEPROM2_ReadInt8(StartAddres,0,&(l->event_code));
		if(x==0)
			continue;
		x= EEPROM2_ReadNextLong(0,&(l->begin_time_stamp.TimestampLow));
		if(x==0)
        	continue;
		x= EEPROM2_ReadNextInt8(0,&(l->begin_time_stamp.TimestampUp));
		if(x==0)
        	continue;
		x= EEPROM2_ReadNextLong(0,&(l->end_time_stamp.TimestampLow));
		if(x==0)
        	continue;
        x = EEPROM2_ReadNextInt8(0,&(l->end_time_stamp.TimestampUp));
        if(x==0)
        	continue;
        x= EEPROM2_ReadNextInt8(1,&(l->checksum));
        if(x==0)
        	continue;
        else
          break;
	}
	if(i>=MAX_LOG_RETRAY)
	  return 0;
	return 1;
}
int8_t get_synchronization_event(void *lt,uint32_t EntryNumber)
{
      --EntryNumber;//handle zero based index
      time_bound_event_log *l = (time_bound_event_log*)lt;
       uint32_t StartAddress;
       if(status.synchronization_event_log_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (TIME_BOUND_EVENT_LOG_TYPE_SIZE * EntryNumber);
          if(val + last_synchronization_event_log_address > SYNCHRONIZATION_LOG_ADDRESS_END)
          {
             StartAddress = val + last_synchronization_event_log_address - SYNCHRONIZATION_LOG_ADDRESS_END;
          }
          else{
             StartAddress = (last_synchronization_event_log_address + val); //TODO it jumps the first entry
          }
       }
       else
          StartAddress = SYNCHRONIZATION_LOG_ADDRESS_START + (TIME_BOUND_EVENT_LOG_TYPE_SIZE*EntryNumber);
       if(get_synchronization_event_log(l,StartAddress)==0)
         return 0;
        return 1;
}
int8_t compare_disconnect_event(void *event0,void *event1)
{
	disconnect_event_log *el = (disconnect_event_log*)event0;
	disconnect_event_log *el2 = (disconnect_event_log*)event1;

	temp_rtc0 = getTime(&el->time_stamp);
	temp_rtc1 = getTime(&el2->time_stamp);

	return compare_time(&temp_rtc0,&temp_rtc1);
}
uint8_t get_disconnect_event_log(disconnect_event_log *l,unsigned long StartAddres)
{
	uint8_t x=0,i=0;
    for(;i<MAX_LOG_RETRAY;i++)
    {
    	x= EEPROM2_ReadInt8(StartAddres,0,&(l->event_code));
		if(x==0)
			continue;
		x= EEPROM2_ReadNextInt8(0,&(l->disconnect_control_status));
		if(x==0)
        	continue;
		x= EEPROM2_ReadNextLong(0,&(l->time_stamp.TimestampLow));
		if(x==0)
        	continue;
        x = EEPROM2_ReadNextInt8(0,&(l->time_stamp.TimestampUp));
        if(x==0)
        	continue;
        x= EEPROM2_ReadNextInt8(1,&(l->checksum));
        if(x==0)
        	continue;
        else
          break;
	}
	if(i>=MAX_LOG_RETRAY)
	  return 0;
	return 1;
}
int8_t get_disconnect_event(void *lt,uint32_t EntryNumber)
{
      --EntryNumber;//handle zero based index
      disconnect_event_log *l = (disconnect_event_log*)lt;
       uint32_t StartAddress;
       if(status.disconnect_event_log_overlapped == 1)//handle cirular buffer
       {
          uint32_t val = (DISCONNECT_EVENT_LOG_TYPE_SIZE * EntryNumber);
          if(val + last_disconnect_event_log_address > DISCONNECT_LOG_ADDRESS_END)
          {
             StartAddress = val + last_disconnect_event_log_address - DISCONNECT_LOG_ADDRESS_END;
          }
          else{
             StartAddress = (last_disconnect_event_log_address + val); //TODO it jumps the first entry
          }
       }
       else
          StartAddress = DISCONNECT_LOG_ADDRESS_START + (DISCONNECT_EVENT_LOG_TYPE_SIZE*EntryNumber);
       if(get_disconnect_event_log(l,StartAddress)==0)
         return 0;
        return 1;
}
//uint8_t setDeviceIDs(const uint8_t *id,uint8_t type)
int8_t setDeviceIDs(void *id2,void *type2)
{
  
     uint8_t *id = (uint8_t *)id2;
     uint8_t *ptr = (uint8_t *)type2;
     uint8_t type = *ptr;
     
     uint8_t z= 0,j=0;//i=0,
     uint32_t address = 0;
     const uint8_t *data = id;
     uint8_t length = 0;
     
        if(type == 0) //serial number
          {
              address  = SerialNumberStart;
              length = SerialNumber_SIZE;
          }
        else if(type == 1)//manufacturer name
          {
              address  = Manufacturer_NameStart;
              length = Manufacturer_Name_SIZE;
          }
        else if(type == 2) //device id 2
          {
              address = DeviceID2Start;
              length = DeviceID2_SIZE;
          }
        else if(type == 3) //device id 3
          {
              address = DeviceID3Start;
              length = DeviceID3_SIZE;
          }
        else if(type == 4) //device id 4
          {
              address = DeviceID4Start;
              length = DeviceID4_SIZE;
          }  
        else if(type == 5) //f id 5
          {
              address = DeviceID5Start;
              length = DeviceID5_SIZE;
          } 
        else if(type == 6) //device id 6
          {
              address = DeviceID6Start;
              length = DeviceID6_SIZE;
          }        
        else if(type == 7) //device id 7
          {
              address = DeviceID7Start;
              length = DeviceID7_SIZE;
          }         
        else if(type == 8) //device id 8
          {
              address = DeviceID8Start;
              length = DeviceID8_SIZE;
          }
           
           
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
          
            z = EEPROM2_WriteInt8(*data++,address, 0); // write length
            length-=1;
            for(j=0;j<length;j++)
            {
               z = EEPROM2_WriteNextInt8(*data++, 0); //write middle data
            }
            
            z = EEPROM2_WriteNextInt8(*data++, 1);       //write last data     
            if(z == 0)
            {
                  return 0;
            }
            
//        }
     return 1;
}

int8_t getDeviceIDs(void *id2,void *type2)
{
         uint8_t z= 0;//,i=0;
         uint32_t address = 0;
         uint8_t length,temp,j=0;
         uint8_t *id = (uint8_t *)id2;
         uint8_t *ptr = type2;
         uint8_t type = *ptr;
         
        if(type == 0) //serial number
          {
              address  = SerialNumberStart;
              length = SerialNumber_SIZE;
          }
        else if(type == 1)//manufacturer name
          {
              address  = Manufacturer_NameStart;
              length = Manufacturer_Name_SIZE;
          }
        else if(type == 2) //device id 2
          {
              address = DeviceID2Start;
              length = DeviceID2_SIZE;
          }
        else if(type == 3) //device id 3
          {
              address = DeviceID3Start;
              length = DeviceID3_SIZE;
          }
        else if(type == 4) //device id 4
          {
              address = DeviceID4Start;
              length = DeviceID4_SIZE;
          }  
        else if(type == 5) //device id 5
          {
              address = DeviceID5Start;
              length = DeviceID5_SIZE;
          } 
        else if(type == 6) //device id 6
          {
              address = DeviceID6Start;
              length = DeviceID6_SIZE;
          }        
        else if(type == 7) //device id 7
          {
              address = DeviceID7Start;
              length = DeviceID7_SIZE;
          }         
        else if(type == 8) //device id 8
          {
              address = DeviceID8Start;
              length = DeviceID8_SIZE;
          }
              
        
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
          z = EEPROM2_ReadInt8(address,0,&temp);
          //length = temp;
          *id = temp;
           id++;
           
          for( j=0;j<length-1;j++)
          {           
            z = EEPROM2_ReadNextInt8( 0,&temp);
            *id = temp;
            id++;
          }   
          
          z = EEPROM2_ReadNextInt8( 1,&temp);
          *id = temp;

            if(z == 0)
//            {
             return FALSE;
//            } 
//        }
        
     return TRUE;
}

//uint8_t getCurrentBalance(CurrentBalance *b)
int8_t getCurrentBalance(void *b2,void *dummy)
{
        CurrentBalance *b = (CurrentBalance *)b2;
	uint8_t x;
//	uint8_t z;
	unsigned long data = 0;//,i=0;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	EEPROM2_ReadLong((unsigned long)CurrentBalanceStart,0,&data);
//          if(z==0)
//            continue;

          EEPROM2_ReadNextInt8(1,&x);
          b->balance = data;
          b->checksum = x;

          //check if the checksum is correct
          if(getCheckSum(&(b->balance),4) != b->checksum )
              return 0;
//          else break;
//        }
//        if(i>=MAX_LOG_RETRAY)
//          return 0;
    return 1;
}

//uint8_t setCurrentBalance(CurrentBalance *b)
int8_t setCurrentBalance(void *b2,void *dummy)
{
   CurrentBalance *b =(CurrentBalance *)b2;
    uint8_t z;//,i=0;
//    for(;i<MAX_LOG_RETRAY;i++)
//    {
    z = EEPROM2_WriteLong(b->balance,(unsigned long)CurrentBalanceStart,0);   
//    if(z == 0)
//      continue;
    
    b->checksum =  getCheckSum(&(b->balance),4);
    z = EEPROM2_WriteNextInt8(b->checksum,1);
    if(z == 0)
      return 0;
//    else 
//     return 1;
//    }
    
//    if(i>=MAX_LOG_RETRAY)
//      return 0;
    
    return 1;
}



uint8_t getFirmwareVersion(unsigned long *version)
{
	uint8_t z;//,i=0;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	   z = EEPROM2_ReadLong(FirmwareVersionStart,1,version);
	   if(z==0)
             return 0;
//	      continue;
//           else break;
//        }
        
//        if(i>=MAX_LOG_RETRAY)
          
        
	return 1;
}

//uint8_t setFirmwareVersion(unsigned long version)
int8_t setFirmwareVersion(void *version2,void *dummy) 
{

        uint32_t *ptr = (uint32_t *)version2;
        uint32_t version = *ptr;        
	uint8_t z;//,i=0;
        
//        for(;i<MAX_LOG_RETRAY;i++)
//        {

	z = EEPROM2_WriteLong(version,FirmwareVersionStart,1);

	if(z==0)
		return 0;
//        else break;
//        }
//        
//        if(i>=MAX_LOG_RETRAY)
//          return 0;
        
	return 1;
}

/*
 * type : enum of type password Types
 */
int8_t getPassword(void *p2,void *type2) //changed test this 
{
        Password_128 *p = (Password_128 *)p2;
        uint8_t *ptr=(uint8_t*)type2;
        uint8_t type;
        type = *ptr;
        
        if(type > MaxPasswords)
            return 0;

        uint8_t z,i=0;//,j=0;
        uint32_t x;
//        for(;j<=MAX_LOG_RETRAY;j++)
//        {
          i=0;
          
           x= Password_start + (Password_Length * type);//x= Password_start + (18 * (type));

            z= EEPROM2_ReadInt8(x,0,&(p->password[0]));
//            if(z==0)
//                    continue;
            i++;

            for(;i<Password_Length-1;i++)//14 was 31
            {
                    z= EEPROM2_ReadNextInt8(0,&(p->password[i]));
//                    if(z==0)
//                        break;
            }
//        
//            if(z==0)
//                continue;
        
            z= EEPROM2_ReadNextInt8(1,&(p->password[Password_Length-1]));
            if(z==0)
                return FALSE;
//        }
//        
//        if(j>MAX_LOG_RETRAY)
//          return 0;
        
        return TRUE;
}


//uint8_t setPassword(Password_128 *pass,uint8_t type) //changed test this 
int8_t setPassword(void *pass2,void *type2) 
{
        Password_128 *pass = (Password_128 *)pass2;
        uint8_t *ptr =(uint8_t *)type2;
        uint8_t type = *ptr;
        
        if(type > MaxPasswords)
            return 0;

        uint8_t z,i=0;//,j=0;
        uint32_t x;
        
//        for(;j<MAX_LOG_RETRAY;j++)
//        {
          i=0;
        x= Password_start+ ((Password_Length)*type);//(128 * (type));

        z= EEPROM2_WriteInt8(pass->password[0],x,0);
//        if(z==0)
//            continue;
        i++;
        
        for(;i<Password_Length-1;i++)
        {
            z= EEPROM2_WriteNextInt8(pass->password[i],0);
            if(z==0)
                    break;
        }
//           if(z==0)
//             continue;
        
        z= EEPROM2_WriteNextInt8(pass->password[Password_Length-1],1);
        if(z==0)
            return 0;
//        else break;
//        }
//        if(j>=MAX_LOG_RETRAY)
//          return 0;
        
        return 1;
}

/*
 * Holiday Number : from 0 to 23 one of the holidays on the schedule
 */
uint8_t getPublicHoliday(uint8_t holidayNumber,Public_Holiday_t *t)
{
	if(holidayNumber>=24)
		return 0;

	unsigned long number=0,temp,i=0;
        for(;i<MAX_LOG_RETRAY;i++)
        {
          unsigned long address = PublicHoliday_Start +(holidayNumber*3);
          uint8_t x=0;
          uint8_t z;
          z = EEPROM2_ReadInt8(address,0,&x);
          if(z==0)
                  continue;

          number = x;
          z = EEPROM2_ReadNextInt8(0,&x);
          if(z==0)
                  continue;
          temp=x;

          number |=x<<8;

          z = EEPROM2_ReadNextInt8(1,&x);
          if(z==0)
                  continue;

           temp= number;
           t->Checksum = x;
           t->day   = (uint8_t)(temp & 0x1F);
           temp= number;
           t->month =	(temp&0x1E0) >>5;
           temp= number;
           t->year  =  (temp&0xFE00) >> 9;
          
           uint8_t chek= (t->year + t->month+t->day)&0xff;
            t->year += 2000;
           if(chek !=t->Checksum)
                   continue;
           else break;
        }
        
        if(i>=MAX_LOG_RETRAY)
          return 0;
	return 1;
}
/*
 * holidayNumber : from 0 to 23
 */
uint8_t setPublicHoliday(uint8_t holidayNumber,Public_Holiday_t *t)
{
	if(holidayNumber>=24)
		return 0;
       uint8_t i=0;
       for(;i<MAX_LOG_RETRAY;i++)
       {
         
	unsigned int number=0;
	number = t->day|  (t->month<<5)|(t->year<<9);
	unsigned long address = PublicHoliday_Start +(holidayNumber*3);
	uint8_t z;
        
	z = EEPROM2_WriteInt8(number,address,0);
	if(z==0)
		continue;

	z = EEPROM2_WriteNextInt8(number>>8,0);
	if(z==0)
		continue;

	z = EEPROM2_WriteNextInt8(t->Checksum,1);
	if(z==0)
		continue;
        else break;
        
       }
       
       if(i>=MAX_LOG_RETRAY)
         return 0;
       
	return 1;
}



//possible of 3*4 = 12 schedules each with 8 byte length, 
//scheduleNumber :from 0 to 11
uint8_t getTariffSchaduleTimeslot(uint8_t scheduleNumber,Schedule_timeslot_t *t)
{
	if(scheduleNumber>=12)
		return 0;

	unsigned long CRC = 0,i = 0;
//        uint8_t w=0;
	unsigned long address = TimeSlots_Start +(scheduleNumber*8);
	uint8_t x=0;
	uint8_t z;
        
        for(;i<MAX_LOG_RETRAY;i++)
        {
          t->checksum = 0;
	z = EEPROM2_ReadInt8(address,0,&x);
	if(z==0)
		continue;
        t->start_Hour = x;
        t->checksum += x;
	
	z = EEPROM2_ReadNextInt8(0,&x);
	if(z==0)
		continue;
        t->start_minute = x;
        t->checksum +=x;

//	for(w = 0 ;w<10;w++)
//        {
          
//         z = EEPROM2_ReadNextInt8(0,&x);
//	 if(z==0)
//		continue;
//         t->rates[w] = x;
//         t->checksum += temp;
         
         z = EEPROM2_ReadNextInt8(0,&x);
	 if(z==0)
		continue;
         t->rate_values_start_pointer = x;
        
         z = EEPROM2_ReadNextInt8(0,&x);
	 if(z==0)
		continue;
         t->rate_values_start_pointer |=(uint16_t) (x<<8);
         t->checksum += t->rate_values_start_pointer;
//        }

	   z = EEPROM2_ReadNextLong(0,&CRC);
	   if(z==0 || CRC != t->checksum)
		 continue;
           else 
                 break;
        }
        
        if(i>=MAX_LOG_RETRAY)
          return 0;
	return 1;
}

/*
 * scheduleNumber: from 0 to 11
 */
//uint8_t setTariffSchaduleTimeslot(uint8_t scheduleNumber,Schedule_timeslot_t *t)
int8_t setTariffSchaduleTimeslot(void *t2,void *scheduleNumber2)
{
        Schedule_timeslot_t *t = (Schedule_timeslot_t *)t2;
        uint8_t *ptr = (uint8_t *)scheduleNumber2;
        uint8_t scheduleNumber = *ptr;
          
	if(scheduleNumber>=12) // 3 scadules 4 scadules per day 
		return 0;

	uint16_t z=0;//i=0,
	unsigned long address = TimeSlots_Start +(scheduleNumber*8);
        
        t->checksum = 0;

        t->checksum = t->rate_values_start_pointer;
        
        t->checksum += t->start_Hour;
        t->checksum += t->start_minute;
        

	z = EEPROM2_WriteInt8(t->start_Hour,address,0);


	z = EEPROM2_WriteNextInt8(t->start_minute,0);

          
          
        z = EEPROM2_WriteNextInt8(t->rate_values_start_pointer,0);

        z = EEPROM2_WriteNextInt8(t->rate_values_start_pointer>>8,0);

        
        

	  z = EEPROM2_WriteNextLong(t->checksum,1);
	  if(z==0)
		return 0;

        
	return 1;
}







//possible of 3*4 = 12 schedules each with 8 byte length, 
//scheduleNumber :from 0 to 11
uint8_t getTariffSchaduleTimeslotPassive(uint8_t scheduleNumber,Schedule_timeslot_t *t)
{
	if(scheduleNumber>=12)
		return 0;

	unsigned long CRC = 0,i = 0;
//        uint8_t w=0;
	unsigned long address = TimeSlots_StartPassive +(scheduleNumber*8);
	uint8_t x=0;
	uint8_t z;
        
        for(;i<MAX_LOG_RETRAY;i++)
        {
          t->checksum = 0;
	z = EEPROM2_ReadInt8(address,0,&x);
	if(z==0)
		continue;
        t->start_Hour = x;
        t->checksum += x;
	
	z = EEPROM2_ReadNextInt8(0,&x);
	if(z==0)
		continue;
        t->start_minute = x;
        t->checksum +=x;

        
         z = EEPROM2_ReadNextInt8(0,&x);
	 if(z==0)
		continue;
         t->rate_values_start_pointer = x;
        
         z = EEPROM2_ReadNextInt8(0,&x);
	 if(z==0)
		continue;
         t->rate_values_start_pointer |=(uint16_t) (x<<8);
         t->checksum += t->rate_values_start_pointer;
//        }

	   z = EEPROM2_ReadNextLong(0,&CRC);
	   if(z==0 || CRC != t->checksum)
		 continue;
           else 
                 break;
        }
        
        if(i>=MAX_LOG_RETRAY)
          return 0;
	return 1;
}

/*
 * scheduleNumber: from 0 to 11
 */
//uint8_t setTariffSchaduleTimeslotPassive(uint8_t scheduleNumber,Schedule_timeslot_t *t)
int8_t setTariffSchaduleTimeslotPassive(void *t2,void *scheduleNumber2)
{
        Schedule_timeslot_t *t = (Schedule_timeslot_t *)t2;
        uint8_t *ptr = (uint8_t *)scheduleNumber2;
        uint8_t scheduleNumber = *ptr;
        
	if(scheduleNumber>=12) // 3 scadules 4 scadules per day 
		return 0;

	uint16_t z=0;//i=0,
	unsigned long address = TimeSlots_StartPassive +(scheduleNumber*8);
        
        t->checksum = 0;
//        for(;i<10;i++)
//        {
          t->checksum = t->rate_values_start_pointer;
//        }
        
        t->checksum += t->start_Hour;
        t->checksum += t->start_minute;
        
	//uint8_t x=0;
//	uint8_t w;



//        for(i=0;i<MAX_LOG_RETRAY;i++)
//        {
	z = EEPROM2_WriteInt8(t->start_Hour,address,0);
//	if(z==0)
//		continue;

	z = EEPROM2_WriteNextInt8(t->start_minute,0);
                   
          z = EEPROM2_WriteNextInt8(t->rate_values_start_pointer,0);
//	  if(z==0)
//		continue;
          z = EEPROM2_WriteNextInt8(t->rate_values_start_pointer>>8,0);

                

	  z = EEPROM2_WriteNextLong(t->checksum,1);
	  if(z==0)
		return 0;

        
	return 1;
}



/*
  rate number from 0 to 119
*/
int8_t getRate(void *rate2,void *rateNumber2 )
{
  	uint8_t z,rateNumber;//i=0,
        uint32_t *rate = (uint32_t *)rate2;
        uint8_t *ptr = (uint8_t*)rateNumber2;
        rateNumber = *ptr;
        
	unsigned long address=Tariff_Rates_start +(rateNumber*4);

	z= EEPROM2_ReadLong(address,1,rate);
	if(z==0)
		return 0;

        
	return 1;
}
                         
/*
  rate number from 0 to 119
*/
//uint8_t setRate(uint8_t rateNumber, const uint32_t *rate)
int8_t setRate(void *rate2,void *rateNumber2 )
{
        uint32_t *rate =(uint32_t *)rate2;
        uint8_t *rnumber = (uint8_t *)rateNumber2;
        uint8_t rateNumber = *rnumber;
        
  	uint8_t z;//,i=0;
        
	unsigned long address=Tariff_Rates_start +(rateNumber*4);
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	z= EEPROM2_WriteLong(*rate,address,1);
	if(z==0)
	    return 0;

	return 1;
}


/*
   number from 0 to 9
*/
int8_t  getPowerBreaks(void *value2,void *Number2 )
{
  	uint8_t z,Number;//i=0;
        uint32_t *value = (uint32_t *)value2;
        uint8_t *ptr = (uint8_t*)Number2;
        Number = *ptr;
	unsigned long address=power_breaks_start +(Number*4);
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	z= EEPROM2_ReadLong(address,1,value);
	if(z==0)
		return FALSE;

        
	return TRUE;
}
                         
/*
   number from 0 to 9
*/
//uint8_t setPowerBreaks(const uint32_t *value,uint8_t Number )
int8_t setPowerBreaks(void *value2,void *Number2 )
{
        uint32_t *value = (uint32_t *)value2;
        uint8_t *num = (uint8_t *)Number2;
        uint8_t Number = *num;
        
  	uint8_t z;//,i=0;
        
	unsigned long address=power_breaks_start +(Number*4);
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	z= EEPROM2_WriteLong(*value,address,1);
	if(z==0)
		return 0;

	return 1;
}


//uint8_t setTamperCount(uint8_t TamperType,uint8_t count)
int8_t setTamperCount(void *count2,void *TamperType2)
{
        uint8_t *tType = (uint8_t *)TamperType2;
        uint8_t TamperType = *tType;
        
        uint8_t *ptr = (uint8_t *)count2;
        uint8_t count = *ptr;
        
	uint8_t z;//,i=0;       
	unsigned long address=TamperCountAddressStart +(TamperType);

	z= EEPROM2_WriteInt8(count,address,1);
	if(z==0)
		return 0;

	return 1;
}

int8_t getTamperCount(void *count2,void *TamperType2)
{
	uint8_t z,TamperType;//i=0;
        uint8_t *count = (uint8_t *)count2;
        uint8_t *ptr = (uint8_t *)TamperType2;
        TamperType = *ptr;
        
	unsigned long address=TamperCountAddressStart +(TamperType);
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	z= EEPROM2_ReadInt8(address,1,count);
	if(z==0)
		return FALSE;

        
	return TRUE;
}

int8_t getLastLogAddress(void *data2,void *type2)
{
    uint32_t *data = (uint32_t *)data2;
	uint8_t z,type;
    uint8_t *ptr= (uint8_t*)type2;
    type = *ptr;
    uint32_t address;

	if(type == 0)
        address = LastEnergyLogAddressStart;
	else if(type ==1)
		address = LAST_DAILY_SNAPSHOT_LOG_ADDRESS_START;
	else if(type ==2)
        address = LastBillingCuttoffLogAddressStart;
	else if(type == 3)
		address = LastEventLogAddressStart;
	else if(type == 4)
		address = LAST_FRAUD_EVENT_ADDRESS_START;
	else if(type == 5)
		address = LAST_POWER_QUAL_EVENT_ADDRESS_START;
	else if(type == 6)
		address = LAST_COMMON_EVENT_ADDRESS_START;
	else if(type == 7)
		address = LAST_FIRMWARE_EVENT_ADDRESS_START;
	else if(type == 8)
		address = LAST_SYNCHRONIZATION_EVENT_ADDRESS_START;
	else if(type == 9)
		address = LAST_DISCONNECT_EVENT_ADDRESS_START;
	z= EEPROM2_ReadLong(address,1,data);
	if(z==0)
		return 0;

	return 1;
}

//uint8_t setLastLogAddress(uint8_t type,uint32_t data)
int8_t setLastLogAddress(void *data2,void *type2)
{
        uint32_t *tmp = (uint32_t *)data2;
        uint32_t data = *tmp;
        uint8_t *ptr = (uint8_t *)type2;
        uint8_t type = *ptr;
        uint8_t z;//,i=0;
        uint32_t address;
//       
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	   if(type == 0)
            address = LastEnergyLogAddressStart;
	   else if(type == 1)
            address = LAST_DAILY_SNAPSHOT_LOG_ADDRESS_START;
       else if(type ==2)
            address = LastBillingCuttoffLogAddressStart;
       else if(type == 3)
    	   address = LastEventLogAddressStart;
       else if(type == 4)
    	   address = LAST_FRAUD_EVENT_ADDRESS_START;
       else if(type == 5)
    	   address = LAST_POWER_QUAL_EVENT_ADDRESS_START;
       else if(type == 6)
    	   address = LAST_COMMON_EVENT_ADDRESS_START;
       else if(type == 7)
    	   address = LAST_FIRMWARE_EVENT_ADDRESS_START;
       else if(type == 8)
    	   address = LAST_SYNCHRONIZATION_EVENT_ADDRESS_START;
       else if(type == 9)
    	   address = LAST_DISCONNECT_EVENT_ADDRESS_START;

       z= EEPROM2_WriteLong(data,address,1);
	   if( z == 0 )
	       return 0;
	return 1;
}
//uint8_t setBillingSchema(uint8_t val)
int8_t setBillingSchema(void *val2,void *dummy)
{
        uint8_t *ptr =(uint8_t *)val2;
        uint8_t val = *ptr;
  	uint8_t z;//,i=0;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	  z= EEPROM2_WriteInt8(val,BillingSchemaStart,1);
	  if(z==0)
		return 0;

        
	return 1;
}
int8_t getBillingSchema(void *flag2,void *dummy)
{ 
        uint8_t *flag= (uint8_t *)flag2;
  	uint8_t z;//,i=0;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
	z= EEPROM2_ReadInt8(BillingSchemaStart,1,flag);
	if(z==0)
		return 0;

	return 1;
}
//uint8_t setEnergyOverlapFlag(uint8_t val)
int8_t setEnergyOverlapFlag(void *val,void *type)
{
        uint8_t *tmp = (uint8_t *)val;
        uint8_t val1 = *tmp;
        uint8_t *tmp1 = (uint8_t *) type;
        uint8_t val2 = *tmp1;
	    uint8_t z;//,i=0;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
          if(val2 == 0)
          {
            status.energy_log_overlapped = val1;
            z= EEPROM2_WriteInt8(val1,EnergyOverlapFlagAddressStart,1);
            if(z==0)
            return 0;
          }
          else if(val2 == 1)
          {
              status.daily_snapshot_energy_overlapped = val1;
              z= EEPROM2_WriteInt8(val1,DAILY_SNAPSHOT_LOG_OVER_LAP_ADDRESS_START,1);
             if(z==0)
              return 0;
          }

	return 1;
}

uint8_t getEnergyOverlapFlag(uint8_t *flag)
{

	uint8_t z,i=0;
        for(;i<MAX_LOG_RETRAY;i++)
        {
	z= EEPROM2_ReadInt8(EnergyOverlapFlagAddressStart,1,flag);
	if(z==0)
		continue;
        else 
             break;
        }

        if(i>=MAX_LOG_RETRAY)
          return 0;
	return 1;

}

uint8_t get_daily_snapshot_overlap_flag(uint8_t *flag)
{
    uint8_t z,i=0;
    for(;i<MAX_LOG_RETRAY;i++)
    {
        z= EEPROM2_ReadInt8(DAILY_SNAPSHOT_LOG_OVER_LAP_ADDRESS_START,1,flag);
        if(z==0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    if(i>=MAX_LOG_RETRAY)
    {
        return 0;
    }
    return 1;
}
//uint8_t setEventOverlapFlag(uint8_t val)
int8_t setEventOverlapFlag(void *data,void * type)
{
    uint8_t *ptr = (uint8_t *)data;
    uint8_t val = *ptr;
	uint8_t *ptr2 = (uint8_t *)type;
	uint8_t val2 = *ptr2;
	uint32_t tmp;
	switch(val2)
	{
	case 3: // standard event log
		status.standard_event_log_overlapped = val;
		tmp = EventOverlapFlagAddressStart;
		break;

	case 4: // fraud event log
		status.fraud_event_log_overlapped = val;
		tmp = FRAUD_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 5: // power quality event log
		status.power_qual_event_log_overlapped = val;
		tmp = POWER_QUAL_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 6: // common event log
		status.common_event_log_overlapped = val;
		tmp = COMMON_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 7: // firmware event log
		status.fraud_event_log_overlapped = val;
		tmp = FIRMWARE_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 8: // synchronization event log
		status.synchronization_event_log_overlapped = val;
		tmp = SYNCHRONIZATION_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 9: // disconnect control event log
		status.disconnect_event_log_overlapped = val;
		tmp = DISCONNECT_EVENT_OVERLAP_ADDRESS_START;
		break;

	default:
		break;
	}
	uint8_t z;
	z= EEPROM2_WriteInt8(val,tmp,1);
	if(z==0)
		return 0;

	return 1;
}

uint8_t get_event_overlap_flag(uint8_t *flag, uint8_t *type)
{
	uint8_t *ptr2 = (uint8_t *)type;
	uint8_t val2 = *ptr2;
	uint32_t tmp;
	switch(val2)
	{
	case 3: // standard event log
		tmp = EventOverlapFlagAddressStart;
		break;

	case 4: // fraud event log
		tmp = FRAUD_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 5: // power quality event log
		tmp = POWER_QUAL_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 6: // common event log
		tmp = COMMON_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 7: // firmware event log
		tmp = FIRMWARE_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 8: // synchronization event log
		tmp = SYNCHRONIZATION_EVENT_OVERLAP_ADDRESS_START;
		break;

	case 9: // disconnect control event log
		tmp = DISCONNECT_EVENT_OVERLAP_ADDRESS_START;
		break;

	default:
		break;
	}

	uint8_t z,i=0;
    for(;i<MAX_LOG_RETRAY;i++)
	{
    	z = EEPROM2_ReadInt8(tmp,1,flag);
	    if(z==0)
	    	continue;
        else
           break;
	}
        
    if(i>=MAX_LOG_RETRAY)
         return 0;
        
	return 1;
}
//uint8_t setRTCCheck(rtc_t *rtcc) 
//TODO . Calculate Check sum 
int8_t setRTCCheck(void *rtcc2,void *dummy) 
{
      rtc_t *rtcc = (rtc_t *)rtcc2;
      uint8_t z;
      uint8_t temp;//,i=0;
//      for(;i<MAX_LOG_RETRAY;i++)
//      {
      z = EEPROM2_WriteInt8(rtcc->year,RTC_CheckStart,0);
//      if(z==0)
//        continue;
      temp=rtcc->year>>8;
      z = EEPROM2_WriteNextInt8(temp,0);
//      if(z==0)
//        continue;
      z = EEPROM2_WriteNextInt8(rtcc->month,0);
//      if(z==0)
//        continue;
      z = EEPROM2_WriteNextInt8(rtcc->day,0);
//      if(z==0)
//        continue;
      z = EEPROM2_WriteNextInt8(rtcc->hour,0);
//        if(z==0)
//          continue;
      z = EEPROM2_WriteNextInt8(rtcc->minute,0);
//      if(z==0)
//        continue;
      z = EEPROM2_WriteNextInt8(rtcc->second,0);
//      if(z==0)
//        continue;
      z = EEPROM2_WriteNextInt8(rtcc->sumcheck,1); 
      if(z==0)
        return 0;
//      }
      
//      if(i>=MAX_LOG_RETRAY)
//        return 0;
      return 1;
}
uint8_t getRTCCheck(rtc_t *rtc_temp)//test this 
{
      uint8_t z;
      unsigned int temp;
      uint8_t temp2;//,i=0;
//      for(;i<MAX_LOG_RETRAY;i++)
//      {
      z = EEPROM2_ReadInt8(RTC_CheckStart,0,&temp2); 
//      if(z==0)
//        continue;
      rtc_temp->year = temp2;
      z = EEPROM2_ReadNextInt8(0,&temp2);
//      if(z==0)
//        continue;

      temp= temp2;
      rtc_temp->year |= temp<<8;

      z = EEPROM2_ReadNextInt8(0,&rtc_temp->month);
//      if(z==0)
//        continue;
      z = EEPROM2_ReadNextInt8(0,&rtc_temp->day);
//      if(z==0)
//        continue;
      z = EEPROM2_ReadNextInt8(0,&rtc_temp->hour);
//        if(z==0)
//        return 0;
      z = EEPROM2_ReadNextInt8(0,&rtc_temp->minute);
//      if(z==0)
//        continue;
      z = EEPROM2_ReadNextInt8(0,&rtc_temp->second);
//      if(z==0)
//        continue;
      z = EEPROM2_ReadNextInt8(1,&rtc_temp->sumcheck); 
       if(z==0)
         return 0;
//       else 
//         break;
//      }
//
//      if(i>=MAX_LOG_RETRAY)
//        return 0;
      return 1;
}

uint8_t logBillingCutoffDateLog(EnergyBillingCutoffDayLog *l)//test this 
{
      	uint8_t x=0, CRC=0;//i=0,
        
        
        if(LastEnergyBillingCuttOffLogAddress<EnergyBillingCuttOffLogAddress_start || LastEnergyBillingCuttOffLogAddress>EnergyBillingCuttOffLogAddress_end)
          LastEnergyBillingCuttOffLogAddress = EnergyBillingCuttOffLogAddress_start;
        
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
		//x=EEPROM2_WriteLong(l->ActiveEnergy,l->startAddress,0);
    	x=EEPROM2_WriteLong(l->ActiveEnergy_QI,LastEnergyBillingCuttOffLogAddress,0);
//	if(x==0)
//		continue;
        CRC += l->ActiveEnergy_QI;
        
        
	x= EEPROM2_WriteNextLong(l->ActiveEnergy_QII, 0);
//	if(x==0)
//		continue;
        CRC += l->ActiveEnergy_QII;
        
//       Uncomment this if power and voltage log is enabled 
        x= EEPROM2_WriteNextLong(l->ActiveEnergy_QIII, 0);
//	if(x==0)
//		continue;        
        CRC += l->ActiveEnergy_QIII;
        
        x= EEPROM2_WriteNextLong(l->ActiveEnergy_QIV, 0);
//	if(x==0)
//		continue;
        CRC += l->ActiveEnergy_QIV;
        
        
        
        
        x=EEPROM2_WriteNextLong(l->ReActiveEnergy_QI,0);
//	if(x==0)
//		continue;
        CRC +=l->ReActiveEnergy_QI;
        

	x= EEPROM2_WriteNextLong(l->ReActiveEnergy_QII, 0);
//	if(x==0)
//		continue;
        CRC += l->ReActiveEnergy_QII;
        
        
//       Uncomment this if power and voltage log is enabled 
        x= EEPROM2_WriteNextLong(l->ReActiveEnergy_QIII, 0);
//	if(x==0)
//		continue;
        CRC += l->ReActiveEnergy_QIII;
        
        
        x= EEPROM2_WriteNextLong(l->ReActiveEnergy_QIV, 0);
//	if(x==0)
//		continue;
        CRC += l->ReActiveEnergy_QIV;
        
        
        x= EEPROM2_WriteNextLong(l->balance, 0);
//	if(x==0)
//		continue;
        CRC += l->balance;
        
        
	x= EEPROM2_WriteNextLong(l->BillingPeriodStart.TimestampLow,0);
//        if(x==0)
//        	continue;
        CRC += l->BillingPeriodStart.TimestampLow;
        

        x = EEPROM2_WriteNextInt8(l->BillingPeriodStart.TimestampUp,0);
//        if(x==0)
//        	continue;
        CRC += l->BillingPeriodStart.TimestampUp;

	x= EEPROM2_WriteNextLong(l->BillingPeriodEnd.TimestampLow,0);
//        if(x==0)
//        	continue;
        CRC += l->BillingPeriodEnd.TimestampLow;
        

        x = EEPROM2_WriteNextInt8(l->BillingPeriodEnd.TimestampUp,0);
//        if(x==0)
//        	continue;
        CRC += l->BillingPeriodEnd.TimestampUp;        
        

        x= EEPROM2_WriteNextInt8(CRC,1);
        if(x==0)
           return 0;
//        else
//          break;
//        }
//        
//        if(i>=MAX_LOG_RETRAY) //the logging was not successfull 
//          return 0;
        
        updateNextLogAddress(2);//0 for billing log
        
        return 1;
        
}

uint8_t getBillingCutoffDateLog(EnergyBillingCutoffDayLog *l,unsigned long startAddress)//test this method 
{
       	uint8_t x=0,CRC,CRCRead;//i=0,
        
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
        CRC = 0;
		//x=EEPROM2_WriteLong(l->ActiveEnergy,l->startAddress,0);
        x= EEPROM2_ReadLong(startAddress,0,&(l->ActiveEnergy_QI));    	
//	if(x==0)
//		continue;
        CRC += l->ActiveEnergy_QI;
        
        
        x= EEPROM2_ReadNextLong(0,&(l->ActiveEnergy_QII)); 
//	if(x==0)
//		continue;
        CRC += l->ActiveEnergy_QII;
        
//       Uncomment this if power and voltage log is enabled 
        x= EEPROM2_ReadNextLong(0,&(l->ActiveEnergy_QIII));
//	if(x==0)
//		continue;
        CRC += l->ActiveEnergy_QIII;
        
        
        x= EEPROM2_ReadNextLong(0,&(l->ActiveEnergy_QIV));
//	if(x==0)
//		continue;
        CRC += l->ActiveEnergy_QIV;
        
        x=EEPROM2_ReadNextLong(0,&(l->ReActiveEnergy_QI));
//	if(x==0)
//		continue;
        CRC += l->ReActiveEnergy_QI;
        
        

	x= EEPROM2_ReadNextLong(0,&(l->ReActiveEnergy_QII));
//	if(x==0)
//		continue;
        CRC += l->ReActiveEnergy_QII;
        
        
//       Uncomment this if power and voltage log is enabled 
        x= EEPROM2_ReadNextLong(0,&(l->ReActiveEnergy_QIII));
//	if(x==0)
//		continue;
        CRC += l->ReActiveEnergy_QIII;
        
        x= EEPROM2_ReadNextLong(0,&(l->ReActiveEnergy_QIV));
//	if(x==0)
//		continue;
        CRC += l->ReActiveEnergy_QIV;
        
        
        x= EEPROM2_ReadNextLong(0,&(l->balance));
//        if(x==0)
//        	continue;
        CRC += l->balance;
        
        
	x= EEPROM2_ReadNextLong(0,&(l->BillingPeriodStart.TimestampLow));
//        if(x==0)
//        	continue;
        CRC += l->BillingPeriodStart.TimestampLow;
        
        x = EEPROM2_ReadNextInt8(0,&(l->BillingPeriodStart.TimestampUp));
//        if(x==0)
//        	continue;
        CRC += l->BillingPeriodStart.TimestampUp;

	x= EEPROM2_ReadNextLong(0,&(l->BillingPeriodEnd.TimestampLow));
//        if(x==0)
//        	continue;
        CRC += l->BillingPeriodEnd.TimestampLow;
        
        

        x = EEPROM2_ReadNextInt8(0,&(l->BillingPeriodEnd.TimestampUp));
//        if(x==0)
//        	continue;
        CRC += l->BillingPeriodEnd.TimestampUp;        

        x= EEPROM2_ReadNextInt8(1,&(CRCRead));
        if(x==0 || CRCRead != CRC)
        	return 0;
//        else
//          break;
//        }
//        
//        if(i>=MAX_LOG_RETRAY) //the logging was not successfull 
//          return 0;
 
        return 1;
        
}
//uint8_t setBillingCutOffLogOverlapFlag(uint8_t value)
int8_t setBillingCutOffLogOverlapFlag(void *value2,void *dummy)
{
         uint8_t *ptr =(uint8_t *)value2;
         uint8_t value = *ptr;
         
         uint8_t x=0;//,i=0;       
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
          x= EEPROM2_WriteInt8(value,BillingCutoffEnergyLogOverlapFlagStart,1);
          if(x==0)
        	return 0;
//          else
//           break;
//        }
        
//        if(i>=MAX_LOG_RETRAY) //the logging was not successfull 
//          return 0;
                
        return 1;     
}
uint8_t getBillingCutOffLogOverlapFlag(uint8_t *flag)
{
         	uint8_t x=0,i=0;
        
        for(;i<MAX_LOG_RETRAY;i++)
        {
          x= EEPROM2_ReadInt8(BillingCutoffEnergyLogOverlapFlagStart,1,flag);
          if(x==0)
        	continue;
          else
           break;
        }
        
        if(i >= MAX_LOG_RETRAY) //the logging was not successfull 
          return 0;
                
        return 1;
}


uint8_t setManufacturer_DateTime(rtc_t *value){ //test this 

             	uint8_t x=0,i=0,z=0;
                //uint16_t temp = 0;
        for(;i<MAX_LOG_RETRAY;i++)
        {
          z= value->year>>8;
          x= EEPROM2_WriteInt8(z,Manufacturing_DateTimeStart,0);          
          if(x==0)
        	continue;
          z= value->year;
          x = EEPROM2_WriteNextInt8(z,0);
          if(x==0)
        	continue;
           
          z= value->month;
          x = EEPROM2_WriteNextInt8(z,0);
          if(x==0)
        	continue;          
          
          z= value->day;
            
          x = EEPROM2_WriteNextInt8(z,0);
          if(x==0)
        	continue;          
          
          z= value->hour;
          x = EEPROM2_WriteNextInt8(z,0);
          if(x==0)
        	continue;   
          
          z = value->minute;
          x = EEPROM2_WriteNextInt8(z,0);
          if(x==0)
        	continue;          
          
          z= value->second;
          x = EEPROM2_WriteNextInt8(z,0);
          if(x==0)
        	continue;          
          
          z= value->sumcheck;
          x = EEPROM2_WriteNextInt8(z,1);

          if(x==0)
        	continue;                    
          else
            break;          
        }
        
        if(i >= MAX_LOG_RETRAY) //the logging was not successfull 
          return 0;
                
        return 1;  
}

uint8_t getManufacturer_DateTime(rtc_t *value){ //test this 
           	uint8_t x=0,i=0,z=0;
                uint16_t temp = 0;
        for(;i<MAX_LOG_RETRAY;i++)
        {
          x= EEPROM2_ReadInt8(Manufacturing_DateTimeStart,1,&z);          
          if(x==0)
        	continue;
          temp = z;
          temp <<=8;
          x = EEPROM2_ReadNextInt8(0,&z);
          if(x==0)
        	continue;
          temp |= z;
          value->year = temp;
          
          x = EEPROM2_ReadNextInt8(0,&z);
          if(x==0)
        	continue;          
          
          value->month = x;
          x = EEPROM2_ReadNextInt8(0,&z);
          if(x==0)
        	continue;          
          
          value->day = x;
          x = EEPROM2_ReadNextInt8(0,&z);
          if(x==0)
        	continue;          
          
          value->hour = x;
          x = EEPROM2_ReadNextInt8(0,&z);
          if(x==0)
        	continue;          
          
          value->minute = x;
          x = EEPROM2_ReadNextInt8(0,&z);
          if(x==0)
        	continue;          
          
          value->second = x;
          x = EEPROM2_ReadNextInt8(1,&z);
             value->sumcheck = x;
          if(x==0)
        	continue;                    
          else
            break;
          
        }
        
        if(i >= MAX_LOG_RETRAY) //the logging was not successfull 
          return 0;
                
        return 1;
  
}

//uint8_t logPowerFailEnergy(phase_parms_s *phase_temp) //test this method
int8_t logPowerFailEnergy(void *phase_temp2,void *dummy) //test this method 
{
        struct phase_parms_s *phase_temp = (struct phase_parms_s *)phase_temp2;
        uint8_t x=0;//,i=0;
        uint32_t CRC=0;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
          
          
        CRC =0;
    	x=EEPROM2_WriteLong(phase_temp->consumed_active_energy_QI,PowerFaileEnergyLogStart,0);
//	if(x==0)
//		continue;
        CRC +=phase_temp->consumed_active_energy_QI;
        
	x= EEPROM2_WriteNextLong(phase_temp->consumed_active_energy_QII, 0);
//	if(x==0)
//		continue;
        CRC+=phase_temp->consumed_active_energy_QII;
        
        x= EEPROM2_WriteNextLong(phase_temp->consumed_active_energy_QIII, 0);
//	if(x==0)
//		continue;     
        CRC += phase_temp->consumed_active_energy_QIII;
        
        x= EEPROM2_WriteNextLong(phase_temp->consumed_active_energy_QIV, 0);
//	if(x==0)
//		continue;
        CRC += phase_temp->consumed_active_energy_QIV;
        
        
        
        
        
        x=EEPROM2_WriteNextLong(phase_temp->consumed_reactive_energy_QI,0);
//	if(x==0)
//		continue;
        CRC+=phase_temp->consumed_reactive_energy_QI;
        
	x= EEPROM2_WriteNextLong(phase_temp->consumed_reactive_energy_QII, 0);
//	if(x==0)
//		continue;
        CRC+=phase_temp->consumed_reactive_energy_QII;
        
        x= EEPROM2_WriteNextLong(phase_temp->consumed_reactive_energy_QIII, 0);
//	if(x==0)
//		continue;   
        CRC+=phase_temp->consumed_reactive_energy_QIII;
        
        x= EEPROM2_WriteNextLong(phase_temp->consumed_reactive_energy_QIV, 0);
//	if(x==0)
//		continue;
        CRC+=phase_temp->consumed_reactive_energy_QIV;
        
        
        x= EEPROM2_WriteNextLong(phase_temp->active_energy_counter_QI, 0);
//	if(x==0)
//		continue;
        CRC += phase_temp->active_energy_counter_QI;
        
        x= EEPROM2_WriteNextLong(phase_temp->active_energy_counter_QII, 0);
//	if(x==0)
//		continue;
        CRC += phase_temp->active_energy_counter_QII;
        
        x= EEPROM2_WriteNextLong(phase_temp->active_energy_counter_QIII, 0);
//	if(x==0)
//		continue;
        CRC += phase_temp->active_energy_counter_QIII;
        
        x= EEPROM2_WriteNextLong(phase_temp->active_energy_counter_QIV, 0);
//	if(x==0)
//		continue;
        CRC += phase_temp->active_energy_counter_QIV;
        
        x= EEPROM2_WriteNextLong(phase_temp->reactive_energy_counter_QI, 0);
//	if(x==0)
//		continue;              
        CRC += phase_temp->reactive_energy_counter_QI;
        
        x= EEPROM2_WriteNextLong(phase_temp->reactive_energy_counter_QII, 0);
//	if(x==0)
//		continue;    
        CRC += phase_temp->reactive_energy_counter_QII;
        
        x= EEPROM2_WriteNextLong(phase_temp->reactive_energy_counter_QIII, 0);
//	if(x==0)
//		continue;       
        CRC += phase_temp->reactive_energy_counter_QIII;
        
        x= EEPROM2_WriteNextLong(phase_temp->reactive_energy_counter_QIV, 0);
//	if(x==0)
//		continue;
        CRC += phase_temp->reactive_energy_counter_QIV;


        
        x= EEPROM2_WriteNextLong(CRC,1);
        if(x==0)
        	return 0;
//        else
//             break;
//        }
//        
//        if(i>=MAX_LOG_RETRAY) //the logging was not successfull 
//          return 0;
        
        
        return 1;
}
int8_t restorePowerFailEnergyLog(void *phase_temp2,void *dummy) //test this method
{
        uint8_t x=0;//,i=0;
        uint32_t tempCRC ,CRC;
        uint32_t temp32 =0;
        
        struct phase_parms_s *phase_temp = (struct phase_parms_s*)phase_temp2; 
//        
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
          tempCRC = 0;
          temp32 = 0;
        x= EEPROM2_ReadLong(PowerFaileEnergyLogStart,0,&temp32);    	
//	if(x==0)
//		continue;
        phase_temp->consumed_active_energy_QI = temp32;
        tempCRC +=temp32;//phase_temp->consumed_active_energy_QI;
        
        x= EEPROM2_ReadNextLong(0,&(temp32)); 	
//	if(x==0)
//		continue;
        phase_temp->consumed_active_energy_QII = temp32;
        tempCRC +=temp32;//phase_temp->consumed_active_energy_QII;
        
        
        x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;        
        phase_temp->consumed_active_energy_QIII = temp32;
        tempCRC +=temp32;//phase_temp->consumed_active_energy_QIII;
        
        
        x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->consumed_active_energy_QIV = temp32;        
        tempCRC +=temp32;//phase_temp->consumed_active_energy_QIV;
        
        
        x=EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->consumed_reactive_energy_QI = temp32;
        tempCRC += temp32;//phase_temp->consumed_reactive_energy_QI;
        
        
	x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->consumed_reactive_energy_QII = temp32;
        tempCRC += temp32;//phase_temp->consumed_reactive_energy_QII;
        
        
        x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;      
        phase_temp->consumed_reactive_energy_QIII = temp32;
        tempCRC +=temp32;//phase_temp->consumed_reactive_energy_QIII;
        
        
        x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->consumed_reactive_energy_QIV = temp32;
        tempCRC +=temp32;//phase_temp->consumed_reactive_energy_QIV;
        
        
        x=EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->active_energy_counter_QI = temp32;
        tempCRC +=temp32;//phase_temp->active_energy_counter_QI;
        
        
	x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->active_energy_counter_QII = temp32;
        tempCRC +=temp32;//phase_temp->active_energy_counter_QII;
        
        
        x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;        
        phase_temp->active_energy_counter_QIII = temp32;
        tempCRC +=temp32;//phase_temp->active_energy_counter_QIII;
        
        
        x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->active_energy_counter_QIV = temp32;
        tempCRC +=temp32;//phase_temp->active_energy_counter_QIV;
        
        
        
        x=EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->reactive_energy_counter_QI = temp32;
        tempCRC +=temp32;//phase_temp->reactive_energy_counter_QI;
        
	x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->reactive_energy_counter_QII = temp32;
        tempCRC +=temp32;//phase_temp->reactive_energy_counter_QII;
        
        
        x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;        
        phase_temp->reactive_energy_counter_QIII = temp32;
        tempCRC +=temp32;//phase_temp->reactive_energy_counter_QIII;
        
        
        x= EEPROM2_ReadNextLong(0,&(temp32));
//	if(x==0)
//		continue;
        phase_temp->reactive_energy_counter_QIV = temp32;
        tempCRC +=temp32;//phase_temp->reactive_energy_counter_QIV;
        
        
        x= EEPROM2_ReadNextLong(1,&(CRC));

//        	continue;
//        
        if(CRC != tempCRC || x==0)
         return FALSE;
        
//        }
        
//        if(i>=MAX_LOG_RETRAY) //the logging was not successfull 
//          return 0;
                
        return TRUE;
        
}

//uint8_t setCalibrationParams(nv_parms_s *param)
int8_t setCalibrationParams(void *param2,void *dummy)
{
       struct nv_parms_s *param = (struct nv_parms_s *)param2;
       
       uint8_t z= 0,i=0;
       uint32_t temp32;
        for(;i<MAX_LOG_RETRAY;i++)
        {
                  z = EEPROM2_WriteInt8(param->meter_uncalibrated,CalibrationFactorsStart, 0);  
                  
                  z = EEPROM2_WriteNextLong(param->chan1.current.I_rms_scale_factor,0);
                  
                  z=  EEPROM2_WriteNextLong((uint32_t)param->chan1.current.ac_offset,0);
                  
                  z = EEPROM2_WriteNextInt8(param->chan1.current.P_scale_factor[0], 0);
                  z = EEPROM2_WriteNextInt8((param->chan1.current.P_scale_factor[0]>>8), 0);
                  z = EEPROM2_WriteNextInt8(param->chan1.current.P_scale_factor[1], 0);
                  z = EEPROM2_WriteNextInt8((param->chan1.current.P_scale_factor[1]>>8), 0);     
                  z=  EEPROM2_WriteNextLong(param->chan1.current.initial_dc_estimate,0);
                  
                  z = EEPROM2_WriteNextInt8((uint16_t)param->chan1.current.phase_correction[0], 0);
                  z = EEPROM2_WriteNextInt8(((uint16_t)param->chan1.current.phase_correction[0]>>8), 0);
                  
                   
                  #if defined(NEUTRAL_MONITOR_SUPPORT)
                  
//                  z = EEPROM2_WriteNextInt8(param->neutral.I_rms_scale_factor, 0);
//                  z = EEPROM2_WriteNextInt8((param->neutral.I_rms_scale_factor>>8), 0);
//                  z = EEPROM2_WriteNextInt8((param->neutral.I_rms_scale_factor>>16), 0);
//                  z = EEPROM2_WriteNextInt8((param->neutral.I_rms_scale_factor>>24), 0);
                  
                  z=  EEPROM2_WriteNextLong((uint32_t)param->neutral.I_rms_scale_factor,0);
                  
                  z=  EEPROM2_WriteNextLong(param->neutral.ac_offset,0);
                  
                  z = EEPROM2_WriteNextInt8(param->neutral.P_scale_factor[0], 0);
                  z = EEPROM2_WriteNextInt8((param->neutral.P_scale_factor[0]>>8), 0);
                  
                  z = EEPROM2_WriteNextInt8(param->neutral.P_scale_factor[1], 0);
                  z = EEPROM2_WriteNextInt8((param->neutral.P_scale_factor[1]>>8), 0);   
                  
                  z=  EEPROM2_WriteNextLong(param->neutral.initial_dc_estimate,0);   
                 #else
                  z = EEPROM2_WriteNextInt8(0, 0);
                  z = EEPROM2_WriteNextInt8(0, 0);
                  z=  EEPROM2_WriteNextLong(0,0);
                  z = EEPROM2_WriteNextInt8(0, 0);
                  z = EEPROM2_WriteNextInt8(0, 0);
                  z = EEPROM2_WriteNextInt8(0, 0);
                  z = EEPROM2_WriteNextInt8(0, 0);     
                  z=  EEPROM2_WriteNextLong(0, 0); 
                 #endif
  
                    
                  z = EEPROM2_WriteNextInt8(param->chan1.V_rms_scale_factor, 0);
                  z = EEPROM2_WriteNextInt8((param->chan1.V_rms_scale_factor>>8), 0);
                  
                  z = EEPROM2_WriteNextInt8(param->chan1.initial_v_dc_estimate, 0);
                  z = EEPROM2_WriteNextInt8((param->chan1.initial_v_dc_estimate>>8), 0);
                  
                  
                  //z = EEPROM2_WriteNextInt8(param->temperature_at_calibration, 0);
                  
                  //z = EEPROM2_WriteNextInt8(param->temperature_offset, 0);                  
                 // z = EEPROM2_WriteNextInt8(param->temperature_offset>>8, 0);
                  
                 //z = EEPROM2_WriteNextInt8(param->temperature_scaling, 0 );                
                 //z = EEPROM2_WriteNextInt8(param->temperature_scaling >>8, 0);                  
                 //z = EEPROM2_WriteNextLong(param->crystal_base_correction,0);
                  
                   temp32 = param->crystal_base_correction +
                   param->chan1.initial_v_dc_estimate + param->chan1.V_rms_scale_factor + 
#ifdef NEUTRAL_MONITOR_SUPPORT
                     param->neutral.P_scale_factor[0] + param->neutral.P_scale_factor[1] +
                   param->neutral.initial_dc_estimate + param->neutral.ac_offset + param->neutral.I_rms_scale_factor + 
#endif                      
                     param->chan1.current.phase_correction[0] +
                   param->chan1.current.P_scale_factor[1] + param->chan1.current.P_scale_factor[0] + param->chan1.current.ac_offset + 
                   param->chan1.current.I_rms_scale_factor + param->meter_uncalibrated;
                     
                  z =  EEPROM2_WriteNextLong(temp32,1);  
                 
                  
            if(z == 1)
              {
                  return 1;
              } 
        }
     return 0; 
 
  
}

int8_t getCalibrationParams(void *param2,void *dummy){
  
       struct nv_parms_s *param = (struct nv_parms_s *)param2;
       uint8_t z,temp;//i=0,
       uint16_t temp16;
       uint32_t temp32;
       uint32_t crc = 0;
       
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
                  crc = 0;
                  z = EEPROM2_ReadInt8(CalibrationFactorsStart, 0,&temp);   
                  param->meter_uncalibrated = temp;
                  //crc +=  param->meter_uncalibrated;
                  
                  z=  EEPROM2_ReadNextLong(0,&temp32);
                  param->chan1.current.I_rms_scale_factor = (int32_t)temp32;
                  
                  //crc += param->chan1.current.I_rms_scale_factor;
//                  z = EEPROM2_ReadNextInt8( 0,&temp);
//                  temp16 = temp;
//                  z = EEPROM2_ReadNextInt8( 0,&temp);
//                  temp16 |=(temp<<8);                  
//                  param->chan1.current.I_rms_scale_factor =(int16_t)temp16;
                  
                  
                  z=  EEPROM2_ReadNextLong(0,&temp32);
                  param->chan1.current.ac_offset  = (int32_t)temp32;
                  //crc += param->chan1.current.ac_offset;
                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 = temp;
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 |=(temp<<8); 
                  param->chan1.current.P_scale_factor[0] = (int16_t)temp16;
                  //crc += param->chan1.current.P_scale_factor[0];
                  
                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 = temp;
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 |=(temp<<8); 
                  param->chan1.current.P_scale_factor[1] = (int16_t)temp16;
                  //crc += param->chan1.current.P_scale_factor[1];
                  
                                                        
                  z=  EEPROM2_ReadNextLong(0,&temp32);
                  param->chan1.current.initial_dc_estimate  = (int32_t)temp32;
                  //crc += param->chan1.current.initial_dc_estimate;
                  
                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 = temp;
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 |=(temp<<8); 
                  param->chan1.current.phase_correction[0] = (int16_t)temp16;
                  //crc += param->chan1.current.phase_correction[0];
                 
                 
     
                  
                  #if defined(NEUTRAL_MONITOR_SUPPORT)                   
 
                  z=  EEPROM2_ReadNextLong(0,&temp32);                  
                  param->neutral.I_rms_scale_factor =temp32;
                  //crc += param->neutral.I_rms_scale_factor;
                  
                  
                  z=  EEPROM2_ReadNextLong(0,&temp32);
                  param->neutral.ac_offset  = (int32_t)temp32;
                  //crc += param->neutral.ac_offset;
                  
                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 = temp;
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 |=(temp<<8); 
                  param->neutral.P_scale_factor[0] = (int16_t)temp16;
                  //crc += param->neutral.P_scale_factor[0];
                  
                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 = temp;
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 |=(temp<<8); 
                  param->neutral.P_scale_factor[1] = (int16_t)temp16;
                  //crc +=  param->neutral.P_scale_factor[1];                                     
                  
                  z=  EEPROM2_ReadNextLong(0,&temp32);
                  param->neutral.initial_dc_estimate  = (int32_t)temp32; 
                  //crc += param->neutral.initial_dc_estimate;
                  
                 #else
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  z=  EEPROM2_ReadNextLong( 0, &temp32);                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  z = EEPROM2_ReadNextInt8( 0,&temp);                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  z = EEPROM2_ReadNextInt8( 0,&temp);                                                        
                  z=  EEPROM2_ReadNextLong(0,&temp32);
                  #endif
  
                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 = temp;
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 |=(temp<<8);                  
                  param->chan1.V_rms_scale_factor =(int16_t)temp16;
                  //crc += param->chan1.V_rms_scale_factor;
                  
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 = temp;
                  z = EEPROM2_ReadNextInt8( 0,&temp);
                  temp16 |=(temp<<8);                  
                  param->chan1.initial_v_dc_estimate =(int16_t)temp16;
                  //crc += param->chan1.initial_v_dc_estimate;

//                  z = EEPROM2_ReadNextInt8( 0,&temp);        
//                  param->temperature_at_calibration = (uint8_t)temp;
                  //crc += param->temperature_at_calibration;
                  
                  
//                  z = EEPROM2_ReadNextInt8( 0,&temp);
//                  temp16 = temp;
//                  z = EEPROM2_ReadNextInt8( 0,&temp);
//                  temp16 |=(temp<<8);                  
//                  param->temperature_offset =(int16_t)temp16;
                  //crc += param->temperature_offset;
                       
                  
//                  z = EEPROM2_ReadNextInt8( 0,&temp);
//                  temp16 = temp;
//                  z = EEPROM2_ReadNextInt8( 0,&temp);
//                  temp16 |=(temp<<8);                  
//                  param->temperature_scaling =temp16;
                  //crc += param->temperature_scaling;
    
                 
                 z=  EEPROM2_ReadNextLong(0,&temp32);
                 param->crystal_base_correction = (int32_t)temp32;
                 //crc += param->crystal_base_correction;
                  
                 z= EEPROM2_ReadNextLong(1,&temp32);//read crc 
//                 if(z ==0)
//                 {
//                   continue;
//                 }
                 
                 crc = param->crystal_base_correction +
                   
                   param->chan1.initial_v_dc_estimate + param->chan1.V_rms_scale_factor + 
#ifdef NEUTRAL_MONITOR_SUPPORT
                     param->neutral.P_scale_factor[0] + param->neutral.P_scale_factor[1] +
                   param->neutral.initial_dc_estimate + param->neutral.ac_offset + param->neutral.I_rms_scale_factor + 
#endif                     
                     param->chan1.current.phase_correction[0] +
                   param->chan1.current.P_scale_factor[1] + param->chan1.current.P_scale_factor[0] + param->chan1.current.ac_offset + 
                   param->chan1.current.I_rms_scale_factor + param->meter_uncalibrated;
                                  
                 if(crc != temp32 && z!=1)
                          return FALSE;
//        }
//        
//        
//        if(i >= MAX_LOG_RETRAY)
//        {
//          return 0;
//        }
        
        return TRUE;
}
  
//uint8_t setNominalValues(const uint16_t *value,uint8_t type )
int8_t setNominalValues(void *value2,void *type2 )
{
        uint16_t *value = (uint16_t *)value2;
        uint8_t *ptr = (uint8_t *)type2;
        uint8_t type = *ptr;
        uint8_t z= 0;//,i=0;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
            if(type == 0) //voltage 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,ReferanceVoltageStart, 1);   
              }
            else if(type == 1)//current 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,NominalCurrentStart, 1);
              }
            else if(type == 2) //frequency 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,ReferanceFrequencyStart, 1);
              }
            else if(type == 3) //active power 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,NominalActivePowerStart, 0);
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);
              }
            else if(type == 4)
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,ThrusholdLongPowerFaileStart, 0);
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);
              }
            if(z == 0)
              {
                  return 0;
              } 
//        }
     return 1;
}
int8_t getNominalValues(void *value2,void *type2){

        uint8_t z= 0,temp;//i=0,
        uint16_t *value = (uint16_t *)value2;
        uint8_t *ptr = (uint8_t *)type2;
        uint8_t type = *ptr;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
            if(type == 0) //voltage 
              {
                  z = EEPROM2_ReadInt8(ReferanceVoltageStart,1,&temp);  
                  *value = temp;
              }
            else if(type == 1)//current 
              {
                  z = EEPROM2_ReadInt8(NominalCurrentStart,1, &temp);
                  *value = temp;
              }
            else if(type == 2) //frequency 
              {
                  z = EEPROM2_ReadInt8(ReferanceFrequencyStart,1,&temp);
                  *value = temp;
              }
            else if(type == 3) //active power 
              {
                  z = EEPROM2_ReadInt8(NominalActivePowerStart,0,&temp);
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp);
                  *value |= (temp<<8);
              }
            else if(type == 4) //Thrushold long power faile 
              {
                  z = EEPROM2_ReadInt8(ThrusholdLongPowerFaileStart,0,&temp);
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp);
                  *value |= (temp<<8);
              }  
            if(z == 0)
              {
                  return FALSE;
              } 
//        }
     return TRUE;

}

//uint8_t setMaxValues(const uint16_t *value,uint8_t type)
int8_t setMaxValues(void *value2,void *type2)
{
  
        uint16_t *value = (uint16_t *)value2;
        uint8_t *tmp = (uint8_t*)type2;     
        uint8_t type = *tmp;
        
        uint8_t z= 0;//,i=0;

            if(type == 0) //voltage 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,MaxRMSVoltageStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1); 
              }
            else if(type == 1)//current 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,MaxRMSCurrentStart, 1);
              }
            else if(type == 2) //frequency 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,MaxFrequencyStart, 1);
              }
            else if(type == 3) //active power 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,MAXActivePowerTripPointStart, 0);
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);
              }
  
            if(z == 0)
              {
                  return 0;
              } 
//        }
     return 1;  
}

int8_t getMaxValues(void *value2,void *type2){
  
        uint16_t *value =  (uint16_t *)value2;
        uint8_t *type3= (uint8_t *)type2;
        uint8_t type = *type3;
        
        uint8_t z= 0,temp;//i=0,

            if(type == 0) //voltage 
              {
                  z = EEPROM2_ReadInt8(MaxRMSVoltageStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
              }
            else if(type == 1)//current 
              {
                  z = EEPROM2_ReadInt8(MaxRMSCurrentStart, 1,&temp);
                   *value = temp;

              }
            else if(type == 2) //frequency 
              {
                  z = EEPROM2_ReadInt8(MaxFrequencyStart, 1,&temp);
                  *value = temp;
              }
            else if(type == 3) //active power 
              {
                  z = EEPROM2_ReadInt8(MAXActivePowerTripPointStart, 0,&temp);
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
              }  
            if(z == 0)
              {
                  return FALSE;
              } 
//        }
     return TRUE;
}
//uint8_t setMinValues(const uint16_t *value,uint8_t type)
int8_t setMinValues(void *value2,void *type2)
{
        uint16_t *value = (uint16_t *)value2;
        uint8_t *ptr = (uint8_t *)type2;
        uint8_t type = *ptr;
        
        uint8_t z= 0;//,i=0;

            if(type == 0) //voltage 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,MinRMSVoltageStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1); 
//                  z = EEPROM2_WriteInt8((uint8_t)*value,MinRMSVoltageStart, 1);   
                   
              }
            else if(type == 2) //frequency 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,MinFrequencyStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);                
//                  z = EEPROM2_WriteInt8((uint8_t)*value,MinFrequencyStart, 1);
              }

            if(z == 0)
              {
                  return 0;
              } 
//        }
     return 1;
}
int8_t getMinValues(void *value2,void *type2){
        
        uint16_t *value = (uint16_t *)value2;
        uint8_t *type3 = (uint8_t *)type2;
        uint8_t type = *type3;
        
        uint8_t z= 0,temp = 0;//,i=0
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
            if(type == 0) //voltage 
             {
                 z = EEPROM2_ReadInt8(MinRMSVoltageStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
//                 z = EEPROM2_ReadInt8(MinRMSVoltageStart, 1,&temp);  
//                 *value = temp;
             }
            else if(type == 2) //frequency 
             {
                 z = EEPROM2_ReadInt8(MinFrequencyStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
                  
//                 z = EEPROM2_ReadInt8(MinFrequencyStart, 1,&temp);
//                 *value = temp;
             }  
            if(z == 0)
             {
                  return FALSE;
             } 
//        }
     return TRUE;
     
}

//uint8_t setEnergyLogTime(const uint8_t *value)
int8_t setEnergyLogTime(void *value2,void *dummy)
{
        uint8_t *value = (uint8_t *)value2;
        uint8_t z= 0;//,i=0;

        z = EEPROM2_WriteInt8((uint8_t)*value,EnergyLogTimeStart, 1);                     
        if(z == 0)
          {
              return 0;
          } 

     return 1;
}
int8_t getEnergyLogTime(void *value2,void *dummy)
{
  uint8_t *value = (uint8_t *)value2;
      uint8_t z= 0,temp = 0;//i=0,

         z = EEPROM2_ReadInt8(EnergyLogTimeStart, 1,&temp);  
         *value = temp;
         if(z == 0)
         {
              return 0;
         } 

      return 1;  
}


int8_t getAutoDisplayTime(void *value2,void *dummy)
{
          uint8_t z= 0;//,i=0;
          uint32_t *value = (uint32_t *)value2;

          z = EEPROM2_ReadLong(AutoDisplayTimeStart, 0,value);   

  
            if(z == 0)
              {
                  return FALSE;
              } 

     return TRUE;
}

//TODO. test this Method
//uint8_t setAutoDisplayTime(const uint32_t *value)
int8_t setAutoDisplayTime(void *value2,void *dummy)
{
        uint32_t * value = (uint32_t *)value2;
        uint8_t z= 0;//,i=0;

        z = EEPROM2_WriteLong(*value,AutoDisplayTimeStart, 0);   

  
        if(z == 0)
          {
              return 0;
          } 

       return 1;        
}


int8_t getBackToAutoModeTime(void *value2, void *dummy)
{
        uint8_t z= 0,temp;//,i=0;
        uint16_t *value = (uint16_t *)value2;


        z = EEPROM2_ReadInt8(BackToAutoModeTimeStart, 0,&temp);   
        *value = temp;
        z = EEPROM2_ReadNextInt8( 1,&temp); 
        *value |= (temp<<8);

        if(z == 0)
         {
              return FALSE;
         } 

     return TRUE;
}

//uint8_t setBackToAutoModeTime(const uint16_t *value)
int8_t setBackToAutoModeTime(void *value2,void *dummy)
{
        uint16_t *value = (uint16_t *)value2;
        uint8_t z= 0;//,i=0;

        z = EEPROM2_WriteInt8((uint8_t)(*value),BackToAutoModeTimeStart, 0);   
        z = EEPROM2_WriteNextInt8((uint8_t)((*value)>>8), 1); 

  
        if(z == 0)
          {
              return 0;
          } 

       return 1;   
       
}
 
int8_t getLanguageSelection(void *value2,void *dummy)
{
  
	uint8_t z;//,i=0;
        uint8_t *value = (uint8_t *)value2;

	z= EEPROM2_ReadInt8(LanguageSettingStart,1,value);
	if(z==0)
          return FALSE;
        
        return TRUE;
  
}

//uint8_t setLanguageSelection(const uint8_t *value)
int8_t setLanguageSelection(void *value2,void *dummy)
{
        uint8_t *value = (uint8_t *)value2;
        uint8_t z= 0;//,i=0;

          z = EEPROM2_WriteInt8((uint8_t)*value,LanguageSettingStart, 1);
          if(z ==0)
            return 0;
        
        
        return 1;  
}

int8_t getMeterConnectedPhase(void *value2,void *dummy)
{
  	uint8_t z;//,i=0;
        uint8_t *value = (uint8_t *)value2;

	z= EEPROM2_ReadInt8(MeterConnectedPhaseStart,1,value);
	if(z==0)
		return FALSE;

	return TRUE;
}
//uint8_t setMeterConnectedPhase(const uint8_t *value)
int8_t setMeterConnectedPhase(void *value2,void *dummy)
{
          uint8_t *value = (uint8_t *)value2;
          uint8_t z= 0;//,i=0;

          z = EEPROM2_WriteInt8((uint8_t)*value,MeterConnectedPhaseStart, 1);
          if(z ==0)
            return 0;
       
        
        return 1; 
}

//uint8_t setConsumptionSinceLastBilling(const uint32_t *value)
int8_t setConsumptionSinceLastBilling(void *value2,void *dummy)
{
        uint32_t *value = (uint32_t *)value2;
        uint8_t z= 0;//,i=0;

          z=  EEPROM2_WriteLong(*value,ConsumptionSinceLastBillingStart,1);
          if(z ==0)
            return 0;

        
        return 1;
}
int8_t getTempConsumptionSinceLastBilling(void *value2,void *dummy)
{
          uint8_t z= 0;//,i=0;
          uint32_t *value = (uint32_t *)value2;

          z=  EEPROM2_ReadLong(ConsumptionSinceLastBillingStart,1,value);
          if(z ==0)
            return FALSE;

        
        return TRUE;
}

//uint8_t setTempLastEnergyValue(const uint32_t *value)
int8_t setTempLastEnergyValue(void *value2,void *dummy)
{
      uint32_t *value = (uint32_t *)value2;
      uint8_t z= 0;//,i=0;

          z=  EEPROM2_WriteLong(*value,TempLastEnergyValueStart,1);
          if(z ==0)
            return 0;

        

        
        return 1;
  
}
int8_t getTempLastEnergyValue(void *value2,void *dummy)
{
        uint8_t z= 0;//,i=0;
        uint32_t *value = (uint32_t *)value2;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
          z=  EEPROM2_ReadLong(TempLastEnergyValueStart,1,value);
          if(z ==1)
            return 0;
//        }
//        
//        if(i>=MAX_LOG_RETRAY)
//          return 0;
        
        return 1;
}

/*

       ActivePowerTripDurationStart    3 
       OverCurrentTripDurationStart    1
       OverVoltageTripDurationStart    0

       OverFrequencyTripDurationStart  2
       UnderVoltageTripDurationStart   4
       UnderFrequencyTripDurationStart 5 

       CoverRemovalTripDurationStart   6 
*/
//uint8_t setTripDuration(const uint16_t *value,uint8_t type)
int8_t setTripDuration(void *value2,void *type2)
{
        uint16_t *value = (uint16_t *)value2;
        uint8_t *ptr = (uint8_t *)type2; 
        uint8_t type = *ptr;
        uint8_t z= 0;//,i=0;
//        for(;i<MAX_LOG_RETRAY;i++)
//        {
            if(type == 0) //voltage 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,OverVoltageTripDurationStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1); 
              }
            else if(type == 1)//current 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,OverCurrentTripDurationStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1); 
              }
            else if(type == 2) //frequency 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,OverFrequencyTripDurationStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);
              }
            else if(type == 3) //active power 
              {
                  z = EEPROM2_WriteInt8((uint8_t)*value,ActivePowerTripDurationStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);              
              }
            else if(type == 4)
            {
              z = EEPROM2_WriteInt8((uint8_t)*value,UnderVoltageTripDurationStart , 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);
            }
            else if(type == 5)
            {
              z = EEPROM2_WriteInt8((uint8_t)*value,UnderFrequencyTripDurationStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);
            }
            else if(type == 6)
            {
              z = EEPROM2_WriteInt8((uint8_t)*value,CoverRemovalTripDurationStart, 0);   
                  z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);
            }
            else if(type == 7)
            {
              z = EEPROM2_WriteInt8((uint8_t)*value,LongPowerFaileTripDurationStart, 0);   
              z = EEPROM2_WriteNextInt8((uint8_t)(*value>>8), 1);
            }
         
            if(z == 0)
              {
                  return 1;
              } 
//        }
     return 1; 
}

int8_t getTripDuration(void *value2,void *type2){
        uint8_t z= 0,temp,type;//i=0,
        uint8_t *ptr= (uint8_t *)type2;
        type = *ptr;
        uint16_t *value = (uint16_t*)value2;
        

            if(type == 0) //voltage 
              {
                  z = EEPROM2_ReadInt8(OverVoltageTripDurationStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
              }
            else if(type == 1)//current 
              {
                  z = EEPROM2_ReadInt8(OverCurrentTripDurationStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8); 
              }
            else if(type == 2) //frequency 
              {
                  z = EEPROM2_ReadInt8(OverFrequencyTripDurationStart, 0,&temp);   
                   *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
              }
            else if(type == 3) //active power 
              {
                  z = EEPROM2_ReadInt8(ActivePowerTripDurationStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);              
              }
            else if(type == 4)
            {
              z = EEPROM2_ReadInt8(UnderVoltageTripDurationStart , 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
            }
            else if(type == 5)
            {
              z = EEPROM2_ReadInt8(UnderFrequencyTripDurationStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
            }
            else if(type == 6)
            {
              z = EEPROM2_ReadInt8(CoverRemovalTripDurationStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
            }
            else if(type == 7)
            {
              z = EEPROM2_ReadInt8(LongPowerFaileTripDurationStart, 0,&temp);   
                  *value = temp;
                  z = EEPROM2_ReadNextInt8( 1,&temp); 
                  *value |= (temp<<8);
            }         
            if(z == 0)
            {
                  return FALSE;
            } 
//        }
     return TRUE;
}


int8_t getMeterStatus(void *m_status2,void *dummy)
{
         MeterStatus *m_status = (MeterStatus *)m_status2;
          uint8_t z= 0,i=0;
          uint32_t status_temp=0;
        for(;i<MAX_LOG_RETRAY;i++)
        {
          z=  EEPROM2_ReadLong(StatusStart,1,&status_temp);
          if(z ==1)
            break;
        }
        
        if(i>=MAX_LOG_RETRAY)
          return 0;
        

      if((status_temp & 0x01) != 0 )
          m_status->AutoDisconnectEnabled = 1;
      if((status_temp & 0x02) != 0)
          m_status->BattryStatus = 1;
      if((status_temp & 0x04) != 0)
          m_status->ConfigrationPortTamperStatus = 1;

      if((status_temp & 0x10) != 0)
          m_status->LowerCoverRemovedTamperStatus = 1;
      if((status_temp & 0x20) != 0)
        m_status->MangneticTamperStatus = 1;
      if((status_temp & 0x40) != 0)
        m_status->NeutralTamperStatus =1;
      if((status_temp & 0x80) != 0)
        m_status->UpperCoverRemovedTamperStatus = 1;


       
        return 1;
  
}

//uint8_t logMeterStatus(const MeterStatus *m_status)
int8_t logMeterStatus(void *m_status2,void *dummy)
{
    MeterStatus *m_status= (MeterStatus *)m_status2;
    uint32_t status_temp=0;
    if(m_status->AutoDisconnectEnabled == 1)
      status_temp |= 0x01;
    if(m_status->BattryStatus == 1)
      status_temp |= 0x02;
    if(m_status->ConfigrationPortTamperStatus == 1)
      status_temp |= 0x04;
//    if(AutoConnect == 1)
//      status_temp |= 0x08;
    if(m_status->LowerCoverRemovedTamperStatus == 1)
      status_temp |= 0x10;
    if(m_status->MangneticTamperStatus == 1)
      status_temp |= 0x20;
    if(m_status->NeutralTamperStatus == 1)
      status_temp |= 0x40;
    if(m_status->UpperCoverRemovedTamperStatus == 1)
      status_temp |= 0x80;
            
         uint8_t z= 0;//,i=0;

          z=  EEPROM2_WriteLong(status_temp,StatusStart,1);
          if(z ==0)
             return 0;

        
        return 1;
}


//uint8_t setRelayStatus(uint8_t status,uint8_t auto_connect)
int8_t setRelayStatus(void *status2,void *auto_connect2)
{
    uint8_t status_temp=0;
    uint8_t *ptr=(uint8_t *)status2;
    uint8_t status = *ptr;
    ptr = auto_connect2;
    uint8_t auto_connect = *ptr;
    
    if(status == 1)
    {
      status_temp = 0x1;
    }

    if(auto_connect == 1)
    {
      status_temp |= 0x2;
    }
    
  	uint8_t z;//,i=0;
            z= EEPROM2_WriteInt8(status_temp,RelayStatusStart,1);
            z= EEPROM2_WriteInt8(status_temp,RelayStatusBackUpStart,1);
            if(z==0)
              return FALSE;

        
	return TRUE;
}
int8_t getRelayStatus(void *status2,void *auto_connect2)
{
         uint8_t *status = (uint8_t *)status2;
         uint8_t *auto_connect = (uint8_t *)auto_connect2;
         
  	uint8_t z,status_temp=0,status_temp2=0;//i=0,

          z= EEPROM2_ReadInt8(RelayStatusStart,1,&status_temp);
       

          z= EEPROM2_ReadInt8(RelayStatusBackUpStart,1,&status_temp2); //double check 

          
          if(status_temp2 != status_temp && z!=1)
            return FALSE;


       if((status_temp & 0x01)==0x01)
       {
         *status = 1;
       }
       else //if((status_temp & 0x2) != 0)
       {
         *status = 0;
       }

       
       if((status_temp & 0x02) == 0x02)
       {
         *auto_connect = 1;
       }
       else
       {
         *auto_connect=0;
       }
        
	return TRUE;
}

int8_t getLogicDeviceName(void *deviceIdentifier,void *dumy)
{

    	uint8_t z,r=0;//i=0,
        uint8_t *did = deviceIdentifier;
        uint8_t length=DeviceIdentifier_SIZE;
        uint8_t crc=0,crc_r=0;

            z = EEPROM2_ReadInt8(DeviceIdentifierStart,0,did);
            crc+=*did;

              did++;
            length-=2;
            for(r=0;r<length;r++)
            {
            

              z= EEPROM2_ReadNextInt8(0,did); //double check 
              crc+=*did;
              did++;
            }
            

            z= EEPROM2_ReadNextInt8(1,&crc_r); //double check 
            if(z==0 && crc != crc_r)
              return FALSE;

	return TRUE;
}
//uint8_t setLogicDeviceName(const uint8_t *deviceIdentifier)
int8_t setLogicDeviceName(void *deviceIdentifier2,void *dummy)
{    
	uint8_t *deviceIdentifier = (uint8_t *)deviceIdentifier2;
  	uint8_t z,r=0;//i=0,
	const uint8_t *did = deviceIdentifier;
	uint8_t length=DeviceIdentifier_SIZE;
	uint8_t crc=0;

	z= EEPROM2_WriteInt8(*did,DeviceIdentifierStart,0);
    crc+=*did;

	did++;
	length-=2;
	for(r=0;r<length;r++)
	{
		z = EEPROM2_WriteNextInt8(*did,0); //double check
	    crc+=*did;
	    did++;
	}

	z= EEPROM2_WriteNextInt8(crc,1);
	if(z==0 )//&& r!=12
		return FALSE;

	return TRUE;
}

int8_t getMacAddress(void *macAddress,void *dumy)
{
	uint8_t z,r=0;//i=0,
	uint8_t *did = macAddress;
	z = EEPROM2_ReadInt8(MACAddressStart,0,did);
	did++;
	for(r=0;r<4;r++)
	{
		z = EEPROM2_ReadNextInt8(0,did);
        did++;
	}
	if(r != 4)
		return FALSE;
            
	z = EEPROM2_ReadNextInt8(1,did);
	if(z==0)
		return FALSE;

	return TRUE;
}

//uint8_t setMacAddress(const uint8_t *macAddress)
int8_t setMacAddress(void *macAddress2,void *dummy)
{
	uint8_t *macAddress = (uint8_t *)macAddress2;
 	uint8_t z,i=0,r=0;
	const uint8_t *did = macAddress;

	for(;i<MAX_LOG_RETRAY;i++)
	{
		z= EEPROM2_WriteInt8(*did,MACAddressStart,0);
		if(z==0)
			  continue;
			did++;

		for(r=0;r<4;r++)
		{
		  z= EEPROM2_WriteNextInt8(*did,0);
		  if(z==0)
				  break;
		  did++;
		}
		if(r!=4)
		  continue;
		z= EEPROM2_WriteNextInt8(*did,1);
		if(z==0)
			  continue;
		else
		  break;
	}

	if(i>=MAX_LOG_RETRAY)
	  return 0;
        
	return 1;
}

//uint8_t setOperatingMode(const uint8_t *mode)
int8_t setOperatingMode(void *mode2,void *dmummy)
{
        uint8_t *mode = (uint8_t *)mode2;
    	uint8_t z;//,i=0;
	z= EEPROM2_WriteInt8(*mode,OperatingModeStart,1);
	if(z==0)
           return 0;        
	return 1;
    
    
}

int8_t getOperatingMode(void *mode,void *dummy)
{
  
    	uint8_t z;//,i=0;
	z= EEPROM2_ReadInt8(OperatingModeStart,1,mode);
	if(z==0)
		return FALSE;
	return TRUE;
}


uint8_t setBlob(const uint32_t *blob,uint16_t blob_id)
{
      	uint8_t z;//,i=0;
        uint32_t blob_address = blob_start + (blob_id*4);

          z=EEPROM2_WriteLong(*blob,blob_address,1);
	  if(z==0)
            return 0;

        
	return 1;
}
uint8_t getBlob(uint32_t *blob,uint16_t blob_id)
{
    	uint8_t z;//,i=0;
        uint32_t blob_address = blob_start + (blob_id*4);

          z=EEPROM2_ReadLong(blob_address,1,blob);
          if(z==0)
                  return 0;

	return 1;
}


extern uint32_t image_block_size;// = 64;
uint8_t writeFirmwareBlock(uint8_t *block, uint32_t block_number)
{  
      uint8_t z,r=0;
      uint32_t blob_address = FirmwareFirstByteStart + (block_number<<6);//multiplay by image_block_size
      z= EEPROM2_WriteInt8(*block,blob_address,0);
      if(z==0)
         return 0;
          block++;
        
      for(r=1;r<image_block_size-1;r++)
      {   
        z = EEPROM2_WriteNextInt8(*block,0);  
        if(z==0)
                break;
        block++;
      }      
      if(r!=image_block_size-1)
        return 0;            
      z = EEPROM2_WriteNextInt8(*block,1);
          return z;     
}

        
	
  
  

uint8_t readFirmwareBlock(uint8_t *block,uint32_t block_number)
{
  return 1;
}


uint8_t writeImageSize(uint32_t imageSize,uint8_t dummy)
{
  	uint8_t z;//,i=0;
        uint32_t image_address = ImageSize_Start ;

          z=EEPROM2_WriteLong(imageSize,image_address,1);
	  if(z==0)
            return 0;        
	return 1;
}
uint8_t readImageSize(uint32_t *imageSize,uint8_t dummy)
{
        uint8_t z;//,i=0;
        uint32_t image_address = ImageSize_Start;// + (blob_id*4);

        z=EEPROM2_ReadLong(image_address,1,imageSize);
        if(z==0)
             return 0;
	return 1;
}


uint8_t EEPROM_reformatImageTransferStatusBytes()
{      
    uint8_t i=0;
    uint32_t image_address = ImageBlocksStatus_Start;
    EEPROM2_WriteLong(0,image_address,0);
    for(i=1;i<ImageBlocksStatusSize-1;i++)
    {
       EEPROM2_WriteNextLong(0,0);
      
    }
    EEPROM2_WriteNextLong(0,1);
  
   return 1;
}
uint8_t writePreviousImageNumber(uint32_t data,uint8_t dummy)
{
        uint8_t z;//,i=0;
        uint32_t image_address = PerviousBlockNumber_Start ;

          z=EEPROM2_WriteLong(data,image_address,1);
	  if(z==0)
            return 0;
       
	return 1;
}
uint8_t readPreviousImageNumber(uint32_t *data,uint8_t dummy)
{
    uint8_t z;//,i=0;
    uint32_t image_address = PerviousBlockNumber_Start;// + (blob_id*4);

    z=EEPROM2_ReadLong(image_address,1,data);
    if(z==0)
         return 0;
    return 1;
}
uint8_t updatateFirwmareBlockStatus(uint32_t blockNumber,uint8_t status)
{  
    uint32_t blockAddress = ImageBlocksStatus_Start + ( blockNumber >> 3);
    uint32_t blockBit  = blockNumber & 7; // the block bit is in the last 8 bit of the number
    uint8_t x = status<<blockBit;
    uint8_t val=0;
    EEPROM2_ReadInt8(blockAddress,1,&val);
    blockBit|=x;
    EEPROM2_WriteInt8(blockBit,blockAddress,1); 
    
    return 0;
}

uint8_t readAllFirwmareBlockStatus(uint8_t *statusBlocks,uint8_t dummy)
{  
    uint32_t blocksCount  = ImageBlocksStatusSize;//TODO. replace this with calculated blocks count
    uint32_t addressStart = ImageBlocksStatus_Start;
    uint8_t i=0;
    
    
    EEPROM2_ReadInt8(addressStart,0,statusBlocks);
    statusBlocks++;              
    for(;i<blocksCount-1;i++)
    {         
      EEPROM2_ReadNextInt8(0,statusBlocks);  
      statusBlocks++;
    }            
      EEPROM2_ReadNextInt8(1,statusBlocks);  
           
    return 0;
}
uint8_t readFirwmareBlockStatus(uint8_t *statusBlocks,uint8_t blockNumber)
{    
    uint32_t addressStart = ImageBlocksStatus_Start + (blockNumber<<6);
          
    EEPROM2_ReadInt8(addressStart,1,statusBlocks);               
    return 0;
}

uint8_t writeImageIdentifier( uint8_t *imageIdentifier, uint8_t dummy )
{
   uint8_t size = ImageIdentifier_Size,r=0;
   uint32_t address = ImageIdentifier_Start;
   EEPROM2_WriteInt8(*imageIdentifier,address,0);
   imageIdentifier++;              
   for(r=1;r<size-1;r++)
   {        
     EEPROM2_WriteNextInt8(*imageIdentifier,0);  
     imageIdentifier++;
   }
   EEPROM2_WriteNextInt8(*imageIdentifier,1);                       
   return 1;
}
uint8_t readImageIdentifier ( uint8_t *imageIdentifier, uint8_t dummy )
{
  
  return 0;
}
