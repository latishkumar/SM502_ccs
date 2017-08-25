
#define __MAIN_PROGRAM__

#include <intrinsics.h>
#include <stdint.h>
#include <stdlib.h>


#include <math.h>
#include <msp430.h>
#include "Logger.h"

#include <configuration_defaults.h>
#include "LCD_C_Graphics.h"
#include "LCD_C.h"
#include "EventTypes.h"
#include "Status.h"

#include "Tamper.h"
#include "I2C2EEPROM.h"
#include "Tariff.h"
#include "UI.h"
#include "PLC.h"
#include "self_diagnosis.h"
#include "Relay.h"
#include "iec62056_uart.h"
#include "errors.h"
#include "battery_monitor.h"
#include <Scheduler.h>

#include <stdio.h>
#include "LCD_C.h"
#include "LPBU.h"

#include "DLMS.h"
#include "iec62056_46_link.h"
#include "iec62056_46_user.h"
#include "uart_comms.h"
#include "daily_load_profile.h"
#include "firmware_loader.h"
void performPowerQualityMessurement(); // some of the power quality management is performed by the

#if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)



uint8_t total_active_energy_pulse_remaining_time;
int32_t total_active_power;
//power_array    total_active_power_array;
int32_t total_active_power_counter;
    #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
int16_t extra_total_active_power_counter;
    #endif
uint32_t total_consumed_active_energy;
#endif

#if defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
uint8_t total_reactive_energy_pulse_remaining_time;
int32_t total_reactive_power;
int32_t total_reactive_power_counter;
    #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
int16_t extra_total_reactive_power_counter;
    #endif
uint32_t total_consumed_reactive_energy;
#endif



// Meter status flag bits.
uint16_t meter_status;

// Current operating mode - normal, limp, power down, etc.
volatile int8_t operating_mode;

/* Persistence check counters for anti-tamper measures. */
// PHASE_REVERSED_DETECTION_SUPPORT
int8_t current_reversed;
// POWER_BALANCE_DETECTION_SUPPORT
int8_t current_unbalanced;



/* The main per-phase working parameter structure */
struct phase_parms_s chan1;

rtc_t rtc;
MeterStatus status;
System_Status_type SystemStatus=SYSTEM_INITIALIZING;
extern HardwareStatus hardware_status;

uint8_t meter_connected_phase = 'R';
//DLMS Variables 
uint32_t P_Reactive_positive ;
uint32_t P_Reactive_negative ;
uint32_t P_Active_Plus;
uint32_t P_Active_Negative;
//End of DLMS Variables


extern uint32_t scroll_display_time;
extern uint32_t AutoDisplayTime;
extern volatile uint8_t rtc_init ;
extern volatile uint8_t secondCounter;
//over voltage and current ranges and Frequency
extern uint8_t  max_irms ;
extern uint16_t  max_vrms;
extern uint8_t  MAX_Frequency;
extern uint32_t MAXActivePowerTripPoint; //in 10m watt steps 

//under voltage and current ranges 
//extern uint8_t MIN_IRMS ;
extern uint8_t min_vrms ;
extern uint8_t MIN_Frequency ;

extern uint16_t Nominal_Voltage ;//V
extern uint8_t Basic_Current ; //A
extern uint8_t Nominal_Frequency ;//HZ

extern uint16_t ThresholdLongPowerFaile;
extern uint32_t TempLastEnergyValue;
extern uint32_t ConsumptionSinceLastBilling;

extern uint16_t output_state;
extern uint8_t control_state;

extern volatile uint8_t lpc ;
//uint8_t AMR_profile_status_SO2;
extern uint8_t active_quadrant;
extern uint8_t phase_presence;
uint8_t energy_export_support = 0;
uint8_t critial_System_error = 0;
uint32_t lastEnergyReadingDuringDisconnect;

extern void updateCalibrationFactor(int16_t ErrorPercent,uint8_t type);
void reset_firmware_upgrate_parameters();
void restoreBackup();
extern EnergyLog LastTxEnergyCopy;
int local_comm_exchange_mode_flag=0; //optical:0; USB:1
/*
 * Backup parameters and functions
 */
uint32_t eeprom_backup_energy_counter;
uint8_t flash_backup_energy_counter;
#define BACKUP_TIME 360000 //msec-6 minutes
#define ENERGY_THRESHOLD_FOR_FLASH_BACKUP 54//KWH
#define NORMAL_BACK_UP 0x02
#define LOW_BATTERY_BACKUP 0x04
const uint32_t ENERGY_KWATT_HOUT_THRESHOLD = ENERGY_WATT_HOUR_THRESHOLD_CUSTOME*1000;
void check_for_backup();
void backup_energy_parameters();
void perform_flash_backup(uint8_t backup_type);
extern uint8_t firtstBoot;
void log_standard_events(uint8_t event_code);
/*******************************
 * test parameters
 */
void test_circular_buffer();
uint16_t overflowsamples, second_counter;
/******************************/

void RESET_ISR(void);
#pragma NOINIT(meter_state)
METER_STATES meter_state;
void main(void)
{
    static int32_t x;
    
    PMMCTL0_H = PMMPW_H;
    SVSMHCTL|=SVSMHRRL_5|SVSHRVL_1|SVMHE|SVSHE; //SVM monitoring level,set reference voltage for switching to auxiliary supply ,
                        //Interrupt is generated when the supply falls below this voltage,
                        //Handle this interrupt to detect power failure

    //PMMRIE =0;//&= ~SVSHPE;
    AUXCTL0 = AUXKEY ;
    AUXCTL1  |= AUX1MD;
    AUXCTL1  &= ~AUX1OK;// | AUX2MD & ~AUX2OK | AUX0MD | AUX0OK; //Auxiliary supply one is disabled, set AUX1OK = 0;
    AUXCTL2  |= AUX0LVL_7|AUX2LVL_7|AUXMR_2;
    AUX3CHCTL = AUXCHKEY | AUXCHC_1 | AUXCHV_1 | AUXCHEN;//comment this for the board Enable Charger for AUX3 to enable RTC
    //configure non maskable Auxiliary interrupt here
    AUXIE    |= AUXSWGIE|AUX2SWIE|AUX0SWIE|AUX2DRPIE;
    AUXIE    &= ~AUXMONIE;//|AUXMONIE; //use maskable interrupt for auxiliary switching
    PMMCTL0_H = 0;

    AUXCTL0 = 0;

    system_setup();

    uint8_t temp_did = 3;
    validation_arg_t noValidation = validation_arg_t_default;
    read_from_eeprom(&last_standard_event_log_address,&temp_did,getLastLogAddress,&noValidation);
    RESET_ISR();
    if(SystemStatus == SYSTEM_RUNNING)
    {
       /* Restore backup
        * first check for valid low battery backup (on flash)
        * second check for valid normal backup on eeprom
        * last check for valid normal backup on flash
        */
        if(firtstBoot)
        {
            //first time boot so there is no backup data
            log_standard_events(FIRST_BOOT);
        }
        else if(backup.seg_a.s.valid_backup == LOW_BATTERY_BACKUP)
        {
            restoreBackup();
            log_standard_events(BACKUP_RESTORED_FROM_FLASH_LB); //backup restored from LOW_BATTERY_BACKUP flash
        }
         else if(restore_eeprom_backup(phase,(void *)0))
        {
            perform_flash_backup(LOW_BATTERY_BACKUP);
            log_standard_events(BACKUP_RESTORED_FROM_EEPROM); //backup restored from eeprom
        }
        else if(backup.seg_a.s.valid_backup == NORMAL_BACK_UP)
        {
            restoreBackup();
            log_standard_events(BACKUP_RESTORED_FROM_FLASH_NB); //backup restored from NORMAL_BACKUP flash
        }
        else
        {
            // Either it is first time boot or neither the flash nor eeprom backup restoration is succeeded
            log_standard_events(BACKUP_RESTORATION_FAILED); //neither the flash nor eeprom backup restoration is succeeded
        }

       LoadConfigurations();
       
       init_dlms();
       status.LogEnergyLogged  = 1;  
      // status.LogEnergy = 0;
       status.task_exec_finished = 1;
      //wait for rtc to finish initializing      
      while(rtc_init == 0)
      {
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif
      }
      
      initTariff(); 
      InitRelay();         
      switch_to_normal_mode();//start Energy reading 
      
      //Moved to Flash
//      validation_arg_t validation_arg;
//      validation_arg.validate = 0;
//      read_from_eeprom(phase,(uint8_t *)0,restorePowerFailEnergyLog,&validation_arg); 
    
      RedLedOff();
      EnergyLedOff();   
      all_segments_off();
      status.DisplayState = 0;
      InitPLC();
      
    }
    else if(SystemStatus == SYSTEM_IN_CRITICAL_ERROR)
    {
      
    }
    
// PHASE_CORRECTION_SUPPORT  &&  !defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)
    set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], 0, phase_nv->current.phase_correction[0]);
    set_sd16_phase_correction(&phase->metrology.neutral.in_phase_correction[0], 1, nv_parms.seg_a.s.neutral.phase_correction[0]);

    //perform_eeprom_backup(phase,(void *) 0);
    schedule_task(backup_energy_parameters,BACKUP_TIME,BACKUP_TASK); //schedule backup process
    for (;;)
    {
            #if defined(USE_WATCHDOG)
            kick_watchdog();
            #endif
             /*process dlms frame */
             uint8_t i=0;       
             i = process_dlms_frame();
             if(i == 0)
             {
              /* There is an error in the received frame*/
             }
             else
             {
             } 

            /* display relay status */
            if(status.RelayStatus == 1) //connected
            {
               output_state = 1;
               control_state = 1;
            }
            else
            {
               output_state = 0;
               control_state = 2;
            }   

             if(operating_mode == OPERATING_MODE_NORMAL)
             {             
                    if ((phase->status & NEW_LOG))
                    {              
						status.DisplayUpdateRequired = 1;
					  /* The background activity has informed us that it is time to
						 perform a block processing operation. */
						  phase->status &= ~NEW_LOG;
						  /* We can only do real power assessment in full operating mode */
						  x = active_power();

						  if(labs(x) < MIN_ACTIVE_POWER_CONSIDERED_ZERO) // if the power reading is less than 2.5w this is an error power so consider it as zero power
						  {
							x = 0;
						  }

						  phase->readings.I_rms = current();
						  phase->readings.V_rms = voltage();
						  if(phase->readings.V_rms > MIN_PHSE_PRESENCE_RMS_VOLTAGE)//TODO. pick a better value for this
						  {
							phase_presence |= BIT0; //BIT0 phase 1 BIT1 phase 2, BIT2 phase 3
						  }
						  else
						  {
							phase_presence &= ~BIT0;
						  }

						 // phase->metrology.neutral.I_rms = neutral_current();

						  /*if the phase reading is less than 3mA and then set the neutral reading to zero.
							This is to prevent from causing neutral tamper status at very small current*/
						  if(phase->metrology.neutral.I_rms < MIN_NEUTRAL_CURRENT_CONSIDERED_ZERO && phase->readings.I_rms <= MIN_PHASE_CURRENT_CONSIDERED_ZERO)
						  {
							phase->metrology.neutral.I_rms = 0;
						  }

						  phase->readings.active_power = x;
						  phase->readings.active_power_pulse = x;
						  if(phase->readings.active_power <= 0)
						  {
							  P_Active_Plus = 0;
							  P_Active_Negative = labs(x);
						  }
						  else
						  {
							P_Active_Plus = x;
							P_Active_Negative = 0;
						  }

						  x = reactive_power();

						  if(labs(x) < MIN_REACTIVE_POWER_CONSIDERED_ZERO)//for reactive power less than 2.5VAr make the reactive power 0
						  {
							phase->readings.reactive_power = 0;
						  }
						  else
						  {
							  phase->readings.reactive_power = x;
						  }

						  if(phase->readings.reactive_power <= 0)
						  {
							  P_Reactive_positive = 0 ;
							  P_Reactive_negative = labs(x);
						  }
						  else
						  {
							  P_Reactive_positive = x;
							  P_Reactive_negative = 0;
						  }
                                  
						  phase->readings.apparent_power = apparent_power();

						  /* The power factor should be calculated last */
						  phase->readings.power_factor = labs(power_factor());

						  //determine the current Quadrant
						  if(phase->readings.active_power >= 0 && phase->readings.reactive_power >= 0 ) //QI
						  {
							active_quadrant = 1;
						  }
						  else if(phase->readings.active_power < 0 && phase->readings.reactive_power >= 0 ) //QII
						  {
							active_quadrant = 2;
						  }
						  else if(phase->readings.active_power < 0 && phase->readings.reactive_power < 0 ) //QIII
						  {
							active_quadrant = 3;
						  }
						  else if(phase->readings.active_power >= 0 && phase->readings.reactive_power < 0 ) //QVI
						  {
							active_quadrant = 4;
						  }

						  if(energy_export_support == 0 &&  phase->readings.active_power < 0)
						  {
							  status.energy_reverse_flow_tamper = 1;
							  phase->readings.active_power = labs(phase->readings.active_power);
							  phase->readings.active_power_pulse = phase->readings.active_power;
						  }

						  /*D. updated this for 4 quadrant energy measurement */
						  if(phase->readings.active_power >= 0 && phase->readings.reactive_power >= 0 ) //QI
						  {
						      if(phase->readings.active_power == 0 && phase->readings.reactive_power == 0)
						      {

						      }
						      else
						      {
                                    //phase->active_energy_import_counter += phase->readings.active_power;// variable used for pulse generation purpose
                                    phase->import_active_energy_counter_QI_QIV += phase->readings.active_power;
                                   // overflowsamples += (phase->metrology.dot_prod_logged.sample_count-4096);
                                   // second_counter++;
                                    //phase->active_energy_counter_QI += mul_32_32(phase->readings.active_power, phase->metrology.dot_prod_logged.sample_count);
                                    while (phase->import_active_energy_counter_QI_QIV >= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)//ENERGY_WATT_HOUR_THRESHOLD)//
                                    {
                                        phase->import_active_energy_counter_QI_QIV -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;//ENERGY_WATT_HOUR_THRESHOLD;//
                                        phase->import_active_energy_QI_QIV++;
                                        phase->inc_import_active_energy++; //hourly energy
                                    }

                                    phase->reactive_energy_counter_QI += phase->readings.reactive_power;
                                    while (phase->reactive_energy_counter_QI > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                    {
                                        phase->reactive_energy_counter_QI -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                        phase->consumed_reactive_energy_QI++;
                                        phase->inc_reactive_energy_QI++; //hourly energy
                                    }
						      }
							 //ActiveQuadrant = 1;
						}
						else if(phase->readings.active_power < 0 && phase->readings.reactive_power >= 0 ) //QII
						{
							 phase->readings.active_power = labs(phase->readings.active_power);
							 //phase->active_energy_export_counter +=phase->readings.active_power;
                             phase->export_active_energy_counter_QII_QIII += phase->readings.active_power;
                             while (phase->export_active_energy_counter_QII_QIII > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
							  {
								  phase->export_active_energy_counter_QII_QIII -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
								  phase->export_active_energy_QII_QIII++;
								  phase->inc_export_active_energy++; //hourly energy
							  }

							 phase->reactive_energy_counter_QII += phase->readings.reactive_power;
							 while (phase->reactive_energy_counter_QII > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
							  {
								  phase->reactive_energy_counter_QII -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
								  phase->consumed_reactive_energy_QII++;
								  phase->inc_reactive_energy_QII++; //hourly energy
							  }
							 //ActiveQuadrant = 2;
						}
						else if(phase->readings.active_power < 0 && phase->readings.reactive_power <= 0 ) //QIII
						{
							 phase->readings.active_power = labs(phase->readings.active_power);
							 phase->readings.reactive_power = labs(phase->readings.reactive_power);

							 //phase->active_energy_export_counter +=phase->readings.active_power;
							 phase->export_active_energy_counter_QII_QIII += phase->readings.active_power;
							 while (phase->export_active_energy_counter_QII_QIII >= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
							 {
								 phase->export_active_energy_counter_QII_QIII -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
								 phase->export_active_energy_QII_QIII++;
								 phase->inc_export_active_energy++; //hourly energy
							 }

							 phase->reactive_energy_counter_QIII += phase->readings.reactive_power;
							 while (phase->reactive_energy_counter_QIII >= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
							 {
								 phase->reactive_energy_counter_QIII -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
								 phase->consumed_reactive_energy_QIII++;
								 phase->inc_reactive_energy_QIII++; //hourly energy
							 }

							//ActiveQuadrant = 3;
						}
						else if(phase->readings.active_power >= 0 && phase->readings.reactive_power < 0 ) //QVI
						{
							 phase->readings.reactive_power = labs(phase->readings.reactive_power);

							 //phase->active_energy_import_counter +=phase->readings.active_power;// * phase->metrology.dot_prod_logged.sample_count;
							 phase->import_active_energy_counter_QI_QIV += phase->readings.active_power;
							 //overflowsamples += (phase->metrology.dot_prod_logged.sample_count-4096);
							 //second_counter++;
							 //phase->active_energy_counter_QI += mul_32_32(phase->readings.active_power, phase->metrology.dot_prod_logged.sample_count);
							  while (phase->import_active_energy_counter_QI_QIV >= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)//ENERGY_WATT_HOUR_THRESHOLD)//
							  {
								  phase->import_active_energy_counter_QI_QIV -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;//ENERGY_WATT_HOUR_THRESHOLD;//
								  phase->import_active_energy_QI_QIV++;
								  phase->inc_import_active_energy++; //hourly energy
							  }
							 /*phase->active_energy_counter_QIV += phase->readings.active_power;
							 while (phase->active_energy_counter_QIV > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
							 {
								 phase->active_energy_counter_QIV -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
								 phase->consumed_active_energy_QIV++;
							 }
							 */
							 phase->reactive_energy_counter_QIV += phase->readings.reactive_power;
							 while (phase->reactive_energy_counter_QIV >= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
							 {
								 phase->reactive_energy_counter_QIV -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
								 phase->consumed_reactive_energy_QIV++;
								 phase->inc_reactive_energy_QIV++; //hourly energy

							 }
  //                           ActiveQuadrant = 4;
						}

						phase->import_reactive_energy_QI_QII   = phase->consumed_reactive_energy_QI + phase->consumed_reactive_energy_QII;
						phase->export_reactive_energy_QIII_QIV = phase->consumed_reactive_energy_QIII + phase->consumed_reactive_energy_QIV;

						/*End of update for 4 quadrant energy measurement */

						check_for_backup(); //check if accumulated energy reaches to energy backup threshold

						if(phase->readings.V_rms < 25)
						  phase->readings.frequency = 0;
						else
						  phase->readings.frequency = frequency();

						/*
						 * Average voltage, power factor and frequency, and peak power calculation routine
						 *
						 */
						phase->voltage_accum      += phase->readings.V_rms;
						phase->power_factor_accum += phase->readings.power_factor;
						phase->frequency_accum    += phase->readings.frequency;
						phase->peak_power          = phase->readings.active_power > phase->peak_power? phase->readings.active_power : phase->peak_power;
                        phase->average_counter++;
                        //test_circular_buffer();
						performPowerQualityMessurement();
                        CalculateBilling();       //perform tariff Update and calculation
                        UpdateDisplayProgressBar_c();
                  }
                  
                  if(status.SecondElapsed == 1)
                  {
                	  per_second_activity();//run activities that needs to be executed every second
                      status.SecondElapsed = 0;
                  }
                  if(status.MiuteElapsed == 1)
                  {
                	  per_minute_activity();//run activities that needs to be executed every minute
                      status.MiuteElapsed = 0;
                  }
                  if(status.DayChanged == 1)
                  {
                	  per_day_activity();//run activities that needs to be executed every day
                	  status.DayChanged = 0;
                	  capture_daily_snapshot();
                  }
                  if(status.MontheChagned == 1){
                	  status.MontheChagned = 0;
                	  per_month_activity(); //run activities that needs to be executed every month
                  }

                  
                  //perform other tasks hear
                  if( plc_state == INITIALIZING)
                  {
                	  InitPLC2();
                  }
                  if (status.PLCCommunicationDetected == 1) // from the communication module.PLC Module 
                  {
                	  ProcessRecivedPLCMessage();
                      status.PLCCommunicationDetected = 0;
                  }

                  //test_circular_buffer();
                  UpdateDisplay_c();
                  
                  if(status.task_exec_finished == 1)
                  {
                      status.task_exec_finished = 0;
                      execute_tasks();
                      status.task_exec_finished = 1;
                  }                  
             }
             else if(operating_mode == OPERATING_MODE_POWERFAIL)
             {
            	 if(lpc == 2)
                 {
            		 custom_power_fail_handler();//custom_power_restore_handler();//TODO
                 }
             }

             if(local_comm_exchange_mode_flag == 0 && (P3IN & BIT0))
             {
            	 configure_uart_port(1,3);  //change to USB
            	 local_comm_exchange_mode_flag = 1;
            	 usb_recv.recv_state = IDLE;
            	 //reset_firmware_upgrate_parameters();
             }
             else if(local_comm_exchange_mode_flag == 1 && !(P3IN & BIT0))
             {
            	 local_comm_exchange_mode_flag = 0;
            	 configure_uart_port(1,0); //change to OPT
             }
    }
}
/**
* writes low power mode entering backup data to Flash
*/
volatile uint8_t __backingUp=0;
void perform_low_battery_backup() //was __monitor function
{
      // back up for no battery conditions
      perform_flash_backup(LOW_BATTERY_BACKUP);
      if(__backingUp == 0)
      {
          //status.LogEnergy = 0;
          status.LogEnergyLogged = 1;
      }
      //end of back up for no battery conditions
}
void perform_flash_backup(uint8_t backup_type)
{
    if(__backingUp == 0)
    {
        __backingUp = 1;
        struct info_mem_a_s backup2;

        backup2.seg_a.s.active_power_counter                  = phase->active_power_counter;
        backup2.seg_a.s.import_active_energy_counter_QI_QIV   = phase->import_active_energy_counter_QI_QIV;
        backup2.seg_a.s.export_active_energy_counter_QII_QIII = phase->export_active_energy_counter_QII_QIII;

       // backup2.seg_a.s.active_energy_export_counter = phase->active_energy_export_counter;
       // backup2.seg_a.s.active_energy_import_counter = phase->active_energy_import_counter;

        backup2.seg_a.s.import_active_energy_QI_QIV   = phase->import_active_energy_QI_QIV;
        backup2.seg_a.s.export_active_energy_QII_QIII = phase->export_active_energy_QII_QIII;

        backup2.seg_a.s.consumed_reactive_energy_QI   = phase->consumed_reactive_energy_QI;
        backup2.seg_a.s.consumed_reactive_energy_QII  = phase->consumed_reactive_energy_QII;
        backup2.seg_a.s.consumed_reactive_energy_QIII = phase->consumed_reactive_energy_QIII;
        backup2.seg_a.s.consumed_reactive_energy_QIV  = phase->consumed_reactive_energy_QIV;

        backup2.seg_a.s.reactive_energy_counter_QI   = phase->reactive_energy_counter_QI;
        backup2.seg_a.s.reactive_energy_counter_QII  = phase->reactive_energy_counter_QII;
        backup2.seg_a.s.reactive_energy_counter_QIII = phase->reactive_energy_counter_QIII;
        backup2.seg_a.s.reactive_energy_counter_QIV  = phase->reactive_energy_counter_QIV;

        backup2.seg_a.s.inc_import_active_energy = chan1.inc_import_active_energy;
        backup2.seg_a.s.inc_export_active_energy = chan1.inc_export_active_energy;
        backup2.seg_a.s.inc_reactive_energy_QI   = chan1.inc_reactive_energy_QI;
        backup2.seg_a.s.inc_reactive_energy_QII  = chan1.inc_reactive_energy_QII;
        backup2.seg_a.s.inc_reactive_energy_QIII = chan1.inc_reactive_energy_QIII;
        backup2.seg_a.s.inc_reactive_energy_QIV  = chan1.inc_reactive_energy_QIV;

        //TODO. Change this to get the Time From hardware,
        //getHardwareTime(&rtcc);
        TimeStump ts = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);

        //Log this to EEPROM when power weak up
        backup2.seg_a.s.RTCHigh = ts.TimestampUp & 0x000000ff;
        backup2.seg_a.s.RTCLow  = ts.TimestampLow;
        //Also Log Power down and Power Up Event

        //backup2.seg_a.s.TempLastEnergyValue = TempLastEnergyValue;
        //backup2.seg_a.s.CurrentBalance      = Current_balance.balance;
        //backup2.seg_a.s.ConsumptionSinceLastBilling = ConsumptionSinceLastBilling;

        backup2.seg_a.s.valid_backup = backup_type;

        void *x = &backup2;
        int32_t * y = (int32_t *)x;
        flashBackup(y,22);

        __backingUp = 0;
    }
}
/**
* Processes Diagnosis results. Adds Errors to the system error list
*/
void ProcessDiagnosisResult()
{
   if(hardware_status.EEPROM1_Status==0)
   {
      AddError(EEPROM1_Error);
      critial_System_error = 1;
   }
   if(hardware_status.EEPROM2_Status==0)
   {
      AddError(EEPROM2_Error);
      
   }

   
      /* CHECK RTC status and add them to Error List */
   if(hardware_status.RTCResetToDefaultStatus == 1)
   {
      AddError(RTC_Reset_Error);
   }
   
   if(hardware_status.RTC_Status == 0 )
   {
      AddError(RTC_Reset_Error);
   }
         
   SystemStatus = SYSTEM_RUNNING;
   meter_state = OPERATIONAL;
}

/**
* Initializes the different peripherals of the system
* 
*/
void custom_initialisation() {  
      
#ifdef CUSTOM_LOGGER
        initIEC62056();
#endif 
        
        
        set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], 0, phase_nv->current.phase_correction[0]);
            #if defined(NEUTRAL_MONITOR_SUPPORT)
        set_sd16_phase_correction(&phase->metrology.neutral.in_phase_correction[0] , 1, nv_parms.seg_a.s.neutral.phase_correction[0]);//&neutral.metrology.current.in_phase_correction[0]
            #endif
        
        scroll_display_time = AutoDisplayTime/1000; // for DLMS Purpose 
            
#ifndef KIT
        
#ifdef CUSTOM_LOGGER
       printf("\n->Initializing\n-> Ports, Backup memory, Error List , Logger Module, LCD, Scaduler,Tamper Switches, Ui and Relay \n");
#endif 
        
        P4DIR = 0xff;
        P5DIR = 0xff;
        P6DIR |= BIT0|BIT6|BIT7;
        P8DIR |= BIT0|BIT1|BIT2|BIT3;
        P2DIR |= BIT6;
        P1DIR |= BIT7|BIT6;
        P9DIR &=~BIT3;//was |= BIT3; chnaged by EEE
#endif 
        
        
        BAKMEM0  = 0;
        BAKMEM1  = 0;
        BAKMEM2  = 0;
        //BAKMEM3  = 0; /*[M.G] Commented out this line. to use this memory for LPM bug debugging Oct 24,2015 17:53 */
        
        init_ErrorList();
        InitGLCD_c();
        all_segments_on();
       __bis_SR_register(GIE);
        InitLogg();
       __bic_SR_register(GIE);          
        status.UpdateDate = 1;        
     
        
        
        init_scheduler();
        initTampperSwitchs();//should be initialized after scheduler because of its dependency
        InitUI();//should be initialized after scheduler because of its dependency
        
        RedLedOn();
        EnergyLedOn();
        
        InitIECUART();

         /*global interrupt is required to initialize PLC,so enable it before initializing PLC
         enable Global Interrupt*/
	__bis_SR_register(GIE);       

        
#ifdef CUSTOM_LOGGER
       printf("-> PLC Module\n");
#endif        
       

       
#ifdef EEPROOM_ENDURANCE_TEST
    Current_balance.balance = 503400;
    int32_t tmp = Current_balance.balance;
    RedLedOff();
    
    while(1)
    {
        tmp = Current_balance.balance;
              //	Current_balance.checksum = 0;
        write_to_eeprom(&Current_balance,(uint8_t *)0,setCurrentBalance);
       
        validation_arg_t validation_arg;
        validation_arg.validate = 0;   
        read_from_eeprom(&Current_balance,(uint8_t *)0,getCurrentBalance,&validation_arg);
       
        if(Current_balance.balance != tmp)
        {
          RedLedOn();
          break;
        }
        counter++;
        Current_balance.balance =~Current_balance.balance;
    };
#endif           
                        
#ifdef CUSTOM_LOGGER
       printf("-> Finished PLC Module init, Init Tariff Module\n");
#endif  
       
        DiagnosisMeter(); 
        ProcessDiagnosisResult();              
}

rms_current_t over_current_temp =0;
rms_voltage_t under_voltage_temp=0;
rms_voltage_t over_voltage_temp =0;

frequency_t over_frequency_temp =0;
frequency_t under_frequency_temp=0;
power_t over_power_temp  = 0;
/**
* This methods handle power quality monitoring.  
* checks the levels with the maximum and minimum values
* and updates the system status 
*/
void performPowerQualityMessurement() 
{     
        uint32_t temp=phase->readings.I_rms;
           temp/=1000;
        if((temp) >= max_irms)
        {
           status.OverCurrentStatus = 1;
           over_current_temp = phase->readings.I_rms;
        }
        else
        {
          status.OverCurrentStatus = 0;
        }
        
        
         temp = phase->readings.V_rms/100;
        if((temp) >= max_vrms)
        {
           
           status.OverVoltageStatus = 1;
           over_voltage_temp = phase->readings.V_rms;
        }
        else
        {
          status.OverVoltageStatus = 0;
        }
        
        //temp = phase->readings.V_rms/100;
        if((temp) <= min_vrms)
        {
           //Event   
           status.UnderVoltageStatus = 1;
            if(temp <= ThresholdLongPowerFaile)
            {
              status.LongPowerFailStatus = 1;
              under_voltage_temp = phase->readings.V_rms;
            }
            else
            {
              status.LongPowerFailStatus = 0;
            }
        }
        else
        {
           status.UnderVoltageStatus = 0;
        }
             
        
        
        temp = phase->readings.frequency/100;
        if((temp) <= MIN_Frequency)
        {
           //Event   
           status.UnderFreqStatus = 1;
           under_frequency_temp = phase->readings.frequency;
        }
        else
        {
          status.UnderFreqStatus = 0;
        }
        
        
        temp =phase->readings.frequency/100;
        if((temp) >= MAX_Frequency)
        {
           status.OverFreqStatus = 1;
           over_frequency_temp = phase->readings.frequency;
        }
        else
        {
           status.OverFreqStatus = 0;
        }        
        
        temp =phase->readings.active_power/100;// phase->readings.active_power/100
        if((temp) >= MAXActivePowerTripPoint) //(temp) >= MAXActivePowerTripPoint*100
        {
            status.ActivePowerExcededStatus = 1;
            over_power_temp =  phase->readings.active_power;        
        }  
        else
          status.ActivePowerExcededStatus = 0;        
}

/**
* method to verify that the relay is disconnected and that the customer is not using power
*/
void verify_normal_operations()
{
    if(status.RelayStatus == 0 && phase->import_active_energy_QI_QIV != lastEnergyReadingDuringDisconnect )
    {
      //error there is something wrong 
    }
}
void time_validity_checker_and_corrector(TimeStump t)
{
    rtc_t time =  getTime(&t);
    uint64_t td = labs(getTimeDifferenceInMinutes(&rtcc,&time));
    if(time.isValid)
    {
        if(compare_time(&rtcc,&time) < 0)
        {
            hardware_status.RTC_Status = 0; //rtc is not ok

            //log invalid RTC event-before set to last saved rtc time
            log_standard_events(RTC_INVALID_TIME_BEFORE_CORRECTED);

            //set it to the last saved rtc time
            adjust_rtc(&time);

            //log invalid RTC event-after set to last saved rtc time
            log_standard_events(RTC_INVALID_TIME_AFTER_CORRECTED);
        }
        else //check whether time has moved too long to the future or to the past
        {
            if(td > MAX_EXPECTED_METER_OFF_TIME_MINUTES)
            {
                hardware_status.RTC_Status = 0; //rtc is not ok

                //log invalid RTC event-before set to last saved rtc time
                log_standard_events(RTC_INVALID_TIME_BEFORE_CORRECTED);

                //set it to the last saved rtc time
                adjust_rtc(&time);

                //log invalid RTC event-after set to last saved rtc time
                log_standard_events(RTC_INVALID_TIME_AFTER_CORRECTED);
            }
        }
    }
    else
    {
        //incorrect time stamp
        log_standard_events(INCORRECT_TIMESTAMP);
    }
}
/**
*  this should be executed before loading configurations from EEPROM, because
*  this will log power out events. it also logs the last energy values to EEPROM.
*  this routin//if(labs(gete reads the last event log address from EEPROM. So EEPROM ports have to be
*  initialized before calling this method.
*
*  writes back up parameters from flash to EEPROM and to the current working variables
*
*
*/
#include <string.h>

void restoreBackup()
{
    TimeStump t = {backup.seg_a.s.RTCLow,(uint8_t)backup.seg_a.s.RTCHigh};
    time_validity_checker_and_corrector(t);
    rtc_t time =  getTime(&t);
    uint8_t temp_did = 3;
    //validation_arg_t noValidation = validation_arg_t_default;
    //read_from_eeprom(&last_standard_event_log_address,&temp_did,getLastLogAddress,&noValidation);

    temp_did = 3;
    //log power is out event
    event_log l;
    l.event_code = PowerOut;
    l.time_stamp = getTimeStamp(time.year, time.month, time.day, time.hour, time.minute, time.second);
    l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
    write_to_eeprom(&l,&temp_did,log_events);
    temp_did = 3;

    //log power is back event
    l.event_code = PowerUp;
    l.time_stamp = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
    l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow  + l.time_stamp.TimestampUp);
    write_to_eeprom(&l,&temp_did,log_events);


    //Current_balance.balance = backup.seg_a.s.CurrentBalance;
    //TempLastEnergyValue = backup.seg_a.s.TempLastEnergyValue;
    //ConsumptionSinceLastBilling = backup.seg_a.s.ConsumptionSinceLastBilling;

    phase->active_power_counter                  = backup.seg_a.s.active_power_counter ;
    phase->import_active_energy_counter_QI_QIV   = backup.seg_a.s.import_active_energy_counter_QI_QIV ;
    phase->export_active_energy_counter_QII_QIII = backup.seg_a.s.export_active_energy_counter_QII_QIII ;

    //phase->active_energy_export_counter = backup.seg_a.s.active_energy_export_counter ;
    //phase->active_energy_import_counter = backup.seg_a.s.active_energy_import_counter ;

    phase->import_active_energy_QI_QIV = backup.seg_a.s.import_active_energy_QI_QIV ;
    phase->export_active_energy_QII_QIII = backup.seg_a.s.export_active_energy_QII_QIII;

    //            phase->reactive_power_counter        = backup.seg_a.s.reactive_power_counter ;
    phase->consumed_reactive_energy_QI   = backup.seg_a.s.consumed_reactive_energy_QI ;
    phase->consumed_reactive_energy_QII  = backup.seg_a.s.consumed_reactive_energy_QII ;
    phase->consumed_reactive_energy_QIII = backup.seg_a.s.consumed_reactive_energy_QIII ;
    phase->consumed_reactive_energy_QIV = backup.seg_a.s.consumed_reactive_energy_QIV ;

    phase->reactive_energy_counter_QI   = backup.seg_a.s.reactive_energy_counter_QI ;
    phase->reactive_energy_counter_QII  = backup.seg_a.s.reactive_energy_counter_QII ;
    phase->reactive_energy_counter_QIII = backup.seg_a.s.reactive_energy_counter_QIII ;
    phase->reactive_energy_counter_QIV  = backup.seg_a.s.reactive_energy_counter_QIV ;

    chan1.inc_import_active_energy = backup.seg_a.s.inc_import_active_energy ;
    chan1.inc_export_active_energy = backup.seg_a.s.inc_export_active_energy;
    chan1.inc_reactive_energy_QI   = backup.seg_a.s.inc_reactive_energy_QI;
    chan1.inc_reactive_energy_QII  = backup.seg_a.s.inc_reactive_energy_QII;
    chan1.inc_reactive_energy_QIII = backup.seg_a.s.inc_reactive_energy_QIII;
    chan1.inc_reactive_energy_QIV  = backup.seg_a.s.inc_reactive_energy_QIV;

    write_to_eeprom(phase,(uint8_t *)0,perform_eeprom_backup);

    //make this value invalid
    struct info_mem_a_s backup2;

    void *x = &backup2;
    const void *y = &backup;
    memcpy(x,y,sizeof(backup));

    backup2.seg_a.s.valid_backup = NORMAL_BACK_UP;
    flashBackup(x,24);

    #if defined(USE_WATCHDOG)
    kick_watchdog();
    #endif
    write_to_eeprom(&rtcc,(uint8_t *)0,setRTCCheck);
}

/*
 * This function checks for 1KWh energy accumulation
 */
void check_for_backup()
{
    eeprom_backup_energy_counter += phase->readings.active_power;
    if(eeprom_backup_energy_counter >= ENERGY_KWATT_HOUT_THRESHOLD) //1kwh
    {
        cancel_task(backup_energy_parameters);
        backup_energy_parameters();
    }

}
/*
 * Backups the current energy registers and counters
 * either on flash or eeprom or on both depending on their threshold
 */
void backup_energy_parameters()
{
    //back up to eeprom
    perform_eeprom_backup(phase,(void *) 0);
    flash_backup_energy_counter++;
    eeprom_backup_energy_counter = eeprom_backup_energy_counter > ENERGY_KWATT_HOUT_THRESHOLD?(eeprom_backup_energy_counter-ENERGY_KWATT_HOUT_THRESHOLD):0;
    if(flash_backup_energy_counter >= ENERGY_THRESHOLD_FOR_FLASH_BACKUP)
    {
        //backup to flash memory
        perform_flash_backup(NORMAL_BACK_UP);
        flash_backup_energy_counter = 0;
    }
    schedule_task(backup_energy_parameters,BACKUP_TIME,BACKUP_TASK);
}

/*
 * Logs Standard events to eeprom
 */
void log_standard_events(uint8_t event_code)
{
    uint8_t temp_did = 3; //event type
    rtc_t time;
    event_log l;
    l.event_code = event_code;
    getHardwareTime(&time);
    l.time_stamp = getTimeStamp(time.year, time.month, time.day, time.hour, time.minute, time.second);
    l.checksum = (int) (l.event_code + l.time_stamp.TimestampLow + l.time_stamp.TimestampUp);
    write_to_eeprom(&l,&temp_did,log_events);
}
