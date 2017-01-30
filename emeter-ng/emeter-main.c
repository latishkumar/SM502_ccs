#define __MAIN_PROGRAM__

#include <intrinsics.h>
#include <stdint.h>
#include <stdlib.h>


#include <math.h>
#include <msp430.h>
#include "SM502/Logger.h"

#include "config.h"
#include "SM502/LCD_C_Graphics.h"//graphics.h"
#include "SM502/LCD_C.h"
#include "SM502/EventTypes.h"
#include "SM502/Status.h"

#include "SM502/Tamper.h"
#include "SM502/I2C2EEPROM.h"
#include "SM502/Tariff.h"
#include "SM502/UI.h"
#include "SM502/PLC/PLC.h"
#include "SM502/self_diagnosis.h"
#include "SM502/Relay.h"
#include "SM502/IEC62056/IEC62056.h"       /*[M.G] Restored this line from an older version Oct 22, 2015 - 13:52 */
#include "SM502/Errors.h"
#include "SM502/BatteryMonitor.h"
#include "SM502/Schaduler.h"

#include <stdio.h>
#include "SM502/LCD_C.h"
#include "SM502/LPBU.h"


#if defined(DLMS) || defined(DLMS_IEC_21)
   #include "DLMS/DLMS.h"
   #include "DLMS/core/iec62056_46_link.h"
   #include "DLMS/core/iec62056_46_user.h"
   #include "DLMS/uart_comms.h"
#endif


int __low_level_init(void)
{
   WDTCTL = WDTPW + WDTHOLD; 
   return 1;
}


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



/* Meter status flag bits. */
uint16_t meter_status;

/* Current operating mode - normal, limp, power down, etc. */
volatile int8_t operating_mode;

///* Persistence check counters for anti-tamper measures. */
#if defined(PHASE_REVERSED_DETECTION_SUPPORT)
int8_t current_reversed;
#endif




/* The main per-phase working parameter structure */
#if !defined(SINGLE_PHASE)
struct phase_parms_s chan[NUM_PHASES];
#else
struct phase_parms_s chan1;
#endif

    #if defined(NEUTRAL_MONITOR_SUPPORT)
struct neutral_parms_s neutral_c;
    #endif







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
extern uint8_t  MAX_IRMS ;
extern uint16_t  MAX_VRMS; 
extern uint8_t  MAX_Frequency;
extern uint32_t MAXActivePowerTripPoint; //in 10m watt steps 

//under voltage and current ranges 
extern uint8_t MIN_IRMS ;
extern uint8_t MIN_VRMS ;
extern uint8_t MIN_Frequency ;

extern uint16_t Nominal_Voltage ;//V
extern uint8_t Basic_Current ; //A
extern uint8_t Nominal_Frequency ;//HZ

extern uint16_t ThresholdLongPowerFaile;
extern uint32_t TempLastEnergyValue;
extern uint32_t ConsumptionSinceLastBilling;

extern uint16_t Output_State;
extern uint8_t control_state;

extern volatile uint8_t lpc ;

uint8_t ActiveQuadrant = 0;
uint8_t Phase_Presence = 1;
uint8_t energy_export_support = 0;
uint8_t critial_System_error = 0;
uint32_t lastEnergyReadingDuringDisconnect;

extern void updateCalibrationFactor(int32_t ErrorPercent,uint8_t type);

void restoreBackup();
extern EnergyLog LastTxEnergyCopy;
extern uint8_t OperatingMode;
int local_comm_exchange_mode_flag=0; //optical:0; USB:1
void time_get_test();
void main(void)
{
    static int32_t x;
    
    /*[M.G] moved this section of code(AUX supply configuration) to the begining of main function Oct 22, 2015 - 13:52, to debug LPM bug */ 
       #if defined (__MSP430_HAS_AUX_SUPPLY__)
       #if defined SM502_DISABLE_AUX_SWITHING
          PMMCTL0_H = PMMPW_H;    
          SVSMHCTL|=SVSMHRRL_5|SVSHRVL_1|SVMHE|SVSHE; //SVM monitoring level,set referance voltage for switching to auxilary supplay , 
                                //Interrupt is generated when the supplay falls below this voltage,
                                //Handel this interrupt to detect power failer 
          //PMMRIE =0;//&= ~SVSHPE;
          AUXCTL0   = AUXKEY ;
          AUXCTL1  |= AUX1MD + AUX2MD;  /*[M.G] grouped SET and RESET operations together Oct 22, 2015 - 13:52*/
          AUXCTL1  &= ~(AUX1OK + AUX2OK );// AUX1 and AUX2 is disabled;
          AUXCTL2  |= AUX0LVL_7|AUX2LVL_7|AUXMR_2;
          AUX3CHCTL = AUXCHKEY | AUXCHC_1 | AUXCHV_1 | AUXCHEN;//comment this for the board Enable Charger for AUX3 to enable RTC
          //configure non maskable Auxilary interrupt heare 
          AUXIE    = 0; //[M.G] Disable all interrupts for now, to debug LPM  bug &= ~AUX2DRPIE&~AUXMONIE;//|AUXMONIE; //use maskable interrupt for auxilary switching 
          PMMCTL0_H = 0;  

          AUXCTL0 = 0;       
        #else
          PMMCTL0_H = PMMPW_H;    
          SVSMHCTL|=SVSMHRRL_5|SVSHRVL_1|SVMHE|SVSHE; //SVM monitoring level,set referance voltage for switching to auxilary supplay , 
                                //Interrupt is generated when the supplay falls below this voltage,
                                //Handel this interrupt to detect power failer 
          
          //PMMRIE =0;//&= ~SVSHPE;
          AUXCTL0 = AUXKEY ;
          AUXCTL1  |= AUX1MD;
          AUXCTL1  &= ~AUX1OK;// | AUX2MD & ~AUX2OK | AUX0MD | AUX0OK; //Auxilary supplay one is dissabled, set AUX1OK = 0;
          AUXCTL2  |= AUX0LVL_7|AUX2LVL_7|AUXMR_2;
          AUX3CHCTL = AUXCHKEY | AUXCHC_1 | AUXCHV_1 | AUXCHEN;//comment this for the board Enable Charger for AUX3 to enable RTC
          //configure non maskable Auxilary interrupt heare 
          AUXIE    |= AUXSWGIE|AUX2SWIE|AUX0SWIE|AUX2DRPIE;
          AUXIE    &= ~AUXMONIE;//|AUXMONIE; //use maskable interrupt for auxilary switching
          PMMCTL0_H = 0;  

          AUXCTL0 = 0;
        #endif

    #endif
    system_setup();
 //   time_get_test();
    if(SystemStatus == SYSTEM_RUNNING)
    {
      
#ifdef EEPROM_LOW_LEVEL_METHODS_TEST       
      uint32_t data=0;
      uint32_t pc=0,fc=0;
      uint32_t j=0;
      EEPROM2_WriteLong(0,0,0);
      for(j=1;j< 131071;j++)
      {
         EEPROM2_WriteNextLong(j,0);
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif
      }
      EEPROM2_WriteNextLong(j,1);
      
      EEPROM2_ReadLong(0,0,&data);
      for(uint32_t i=1;i< 131071;i++)
      {
         #if defined(USE_WATCHDOG)
            kick_watchdog();
         #endif
         EEPROM2_ReadNextLong(0,&data);
         
         if(data == i)
           pc++;
         else 
           fc++;
      }
      EEPROM2_ReadNextLong(1,&data);         
      pc++;
      fc++;
#endif       
      
      
       restoreBackup(); 
       LoadConfigurations();
       
       #if defined(DLMS) || defined(DLMS_IEC_21)
         init_dlms();
       #endif 
       status.LogEnergyLogged  = 1;  
       status.LogEnergy = 0;
       status.task_exec_finished = 1;
      //wait for rtc to finish initializing      
      while(rtc_init ==0)
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
    

#if !defined(ESP_SUPPORT)  &&  defined(PHASE_CORRECTION_SUPPORT)  &&  !defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)

    #if defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__)
        set_phase_correction(&phase->metrology.current.in_phase_correction[0], phase_nv->current.phase_correction[0]);
        #if GAIN_STAGES > 1
        set_phase_correction(&phase->metrology.current.in_phase_correction[1], phase_nv->current.phase_correction[1]);
        #endif
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        set_phase_correction(&phase->neutral.in_phase_correction[0], nv_parms.seg_a.s.neutral.phase_correction[0]);
            #if GAIN_STAGES > 1
        set_phase_correction(&phase->neutral.in_phase_correction[1], nv_parms.seg_a.s.neutral.phase_correction[1]);
            #endif
        #endif
    #else
        #if defined(SINGLE_PHASE)
        set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], 0, phase_nv->current.phase_correction[0]);
            #if defined(NEUTRAL_MONITOR_SUPPORT)
        set_sd16_phase_correction(&phase->metrology.neutral.in_phase_correction[0], 1, nv_parms.seg_a.s.neutral.phase_correction[0]);
            #endif
        #else
        set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], ch, phase_nv->current.phase_correction[0]);
        #endif
    #endif

#endif

#ifdef SM502_CODE_TEST_MODE
        TestMain();          /* Run test code if test configuration is active */
#endif


    for (;;)
    {
            #if defined(USE_WATCHDOG)
            kick_watchdog();
            #endif

          #if ( defined(DLMS)) || defined(DLMS_IEC_21)
             uint8_t i=0;       
             i = process_dlms_frame();
             if(i == 0)
             {
              /* There is an error in the recived frame*/
             }
             else
             {
             } 
          #endif
           /* display relay status */
            if(status.RelayStatus == 1) //connected
            {
               Output_State = 1;
               control_state = 1;
            }
            else
            {
               Output_State = 0;
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
                                        Phase_Presence |= BIT0; //BIT0 phase 1 BIT1 phase 2, BIT2 phase 3
                                      }
                                      else
                                      {
                                        Phase_Presence &= ~BIT0;
                                      }
                                      
                                      
      #ifdef NEUTRAL_MONITOR_SUPPORT
                                      phase->metrology.neutral.I_rms = neutral_current();
                                      
                                      /*if the phase reading is less than 3mA and then set the neutral reading to zero.
                                        This is to prevent from causing neutral tamper status at very small current*/
                                      if(phase->metrology.neutral.I_rms < MIN_NUTRAL_CURRENT_CONSIDERED_ZERO && phase->readings.I_rms <= MIN_PHASE_CURRENT_CONSIDERED_ZERO)
                                      {
                                        phase->metrology.neutral.I_rms = 0;
                                      }  
      #endif                                


                                  phase->readings.active_power = x;
                                  phase->readings.active_power_pulse = x;
                                  if(phase->readings.active_power<=0)
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
                                  
                                  
                                  if(labs(phase->readings.reactive_power) < MIN_REACTIVE_POWER_CONSIDERED_ZERO)//for reactive power less than 2.5VAr make the reactive power 0
                                  {
                                    phase->readings.reactive_power=0;
                                    
                                  }
                                  
                                  
                                  phase->readings.reactive_power = x;
                                  if(phase->readings.reactive_power <=0)
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

                                  //determin the current Quadrant       
                                  if(phase->readings.active_power >= 0 && phase->readings.reactive_power >= 0 ) //QI
                                  {
                                    ActiveQuadrant = 1;
                                  }
                                  else if(phase->readings.active_power < 0 && phase->readings.reactive_power >= 0 ) //QII
                                  {
                                    ActiveQuadrant = 2;
                                  }
                                  else if(phase->readings.active_power < 0 && phase->readings.reactive_power < 0 ) //QIII
                                  {
                                    ActiveQuadrant = 3;
                                  }
                                  else if(phase->readings.active_power >= 0 && phase->readings.reactive_power < 0 ) //QVI
                                  {
                                    ActiveQuadrant = 4;
                                  }
                                          
                                  if(energy_export_support == 0 &&  phase->readings.active_power < 0) 
                                  {
                                        status.energy_reverse_flow_tamper = 1;
                                        phase->readings.active_power = labs(phase->readings.active_power);
                                        phase->readings.active_power_pulse = phase->readings.active_power;
                                       
                                  }

                            
                            /*D. updated this for 4 quadrant energy measssuremnt */
                            if(phase->readings.active_power >= 0 && phase->readings.reactive_power >= 0 ) //QI
                            {
                               phase->active_energy_import_counter += phase->readings.active_power;// variable used for pulse generation purpose 
                               phase->active_energy_counter_QI += phase->readings.active_power;
                                 while (phase->active_energy_counter_QI > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                  {
                                      phase->active_energy_counter_QI -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                      phase->consumed_active_energy_QI++;
                                  }
                               
                              phase->reactive_energy_counter_QI += phase->readings.reactive_power;
                                 while (phase->reactive_energy_counter_QI > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                  {
                                      phase->reactive_energy_counter_QI -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                      phase->consumed_reactive_energy_QI++;
                                  }               
                                 //ActiveQuadrant = 1;
                            }
                            else if(phase->readings.active_power < 0 && phase->readings.reactive_power >= 0 ) //QII
                            {
                                 phase->readings.active_power = labs(phase->readings.active_power);
                                 phase->active_energy_export_counter +=phase->readings.active_power;
                                 phase->active_energy_counter_QII += phase->readings.active_power;
                                 while (phase->active_energy_counter_QII > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                  {
                                      phase->active_energy_counter_QII -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                      phase->consumed_active_energy_QII++;
                                  }
                               
                              phase->reactive_energy_counter_QII += phase->readings.reactive_power;
                                 while (phase->reactive_energy_counter_QII > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                  {
                                      phase->reactive_energy_counter_QII -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                      phase->consumed_reactive_energy_QII++;
                                  }     
                                 //ActiveQuadrant = 2;
                            }
                            else if(phase->readings.active_power < 0 && phase->readings.reactive_power < 0 ) //QIII
                            {
                                 phase->readings.active_power = labs(phase->readings.active_power);
                                 phase->readings.reactive_power = labs(phase->readings.reactive_power);
                                 
                                 phase->active_energy_export_counter +=phase->readings.active_power;
                                 phase->active_energy_counter_QIII += phase->readings.active_power;
                                 while (phase->active_energy_counter_QIII > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                  {
                                      phase->active_energy_counter_QIII -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                      phase->consumed_active_energy_QIII++;
                                  }
                               
                                 phase->reactive_energy_counter_QIII += phase->readings.reactive_power;
                                 while (phase->reactive_energy_counter_QIII > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                  {
                                      phase->reactive_energy_counter_QIII -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                      phase->consumed_reactive_energy_QIII++;
                                  }
                                
                                //ActiveQuadrant = 3;
                            }
                            else if(phase->readings.active_power >= 0 && phase->readings.reactive_power < 0 ) //QVI
                            {
                                 phase->readings.reactive_power = labs(phase->readings.reactive_power);
                                 
                                 phase->active_energy_import_counter +=phase->readings.active_power;// * phase->metrology.dot_prod_logged.sample_count;
                                 phase->active_energy_counter_QIV += phase->readings.active_power;
                                 while (phase->active_energy_counter_QIV > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                  {
                                      phase->active_energy_counter_QIV -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                      phase->consumed_active_energy_QIV++;
                                  }
                               
                                 phase->reactive_energy_counter_QIV += phase->readings.reactive_power;
                                 while (phase->reactive_energy_counter_QIV > ENERGY_WATT_HOUR_THRESHOLD_CUSTOME)
                                  {
                                      phase->reactive_energy_counter_QIV -= ENERGY_WATT_HOUR_THRESHOLD_CUSTOME;
                                      phase->consumed_reactive_energy_QIV++;
                                  }            
      //                           ActiveQuadrant = 4;
                            }
                                                        
                             
                            phase->active_energy_import = phase->consumed_active_energy_QI + phase->consumed_active_energy_QIV ;
                            phase->active_energy_export = phase->consumed_active_energy_QII + phase->consumed_active_energy_QIII;
                            
                            /*End of update for 4 quadrant energy meassuremnt */
                                  
                            if(phase->readings.V_rms < 25)
                              phase->readings.frequency = 0;
                            else 
                              phase->readings.frequency = frequency();   
                            
                            
                             performPowerQualityMessurement();
                             CalculateBilling();       //perform tariff Update and calculation                                                  
                             UpdateDisplayProgressBar_c();
                             
                  }
                  
                  if(status.SecondElapsed == 1)
                  {
                     per_second_activity();//run activities that needs to be executed every second 
                     status.SecondElapsed =0;
                  }
                  if(status.MiuteElapsed==1)
                  {
                     per_minute_activity();//run activities that needs to be executed every minute
                     status.MiuteElapsed = 0;
                  }
                  if(status.MontheChagned==1){
                     status.MontheChagned = 0;
                     per_month_activity(); //run activities that needs to be executed every month
                  }
                  if(status.DayChanged == 1)
                  {
                    per_day_activity();//run activities that needs to be executed every day
                    status.DayChanged = 0;
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
                  //Timed Tasks
                  //Interrupt From Timer
                  //15 minute interrupt for RTCC
                  if (status.LoggingTimeIsUp == 1) //Timer
                  {
                      EnergyLog e;
                      e.ActiveEnergy =chan1.active_energy_import;// chan1.consumed_active_energy;
                      e.Reactive_Power = chan1.readings.reactive_power;
                      e.Active_Power = chan1.readings.active_power;
                      e.Voltage = chan1.readings.V_rms;
                      e.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
                      e.CRC = e.ActiveEnergy + e.Reactive_Power + e.timeStump.TimestampLow+ e.timeStump.TimestampUp;			
                      write_to_eeprom(&e,(uint8_t *)0,logEnergy);
                      status.LoggingTimeIsUp =  0;          
                  }
                  
                  
                  UpdateDisplay_c();
                  
                  if(status.task_exec_finished == 1)
                  {
                      status.task_exec_finished = 0;
                      executeTasks();
                      status.task_exec_finished = 1;
                  }                  
             }
             
             else if(operating_mode == OPERATING_MODE_POWERFAIL)
             {                 
                    if(lpc == 2)
                    {
                       custom_power_restore_handler();  
                    }                
             }    
         #if defined OPT_INT     

             if(local_comm_exchange_mode_flag==0 && (P3IN & BIT0)){
               configure_uart_port(1,3);  //change to USB
               local_comm_exchange_mode_flag=1;
             }
             else if(local_comm_exchange_mode_flag==1 && !(P3IN & BIT0)){              
               local_comm_exchange_mode_flag=0;
               configure_uart_port(1,0); //change to OPT

             }
             
         #endif  
    }
}
/**
* writes low power mode entering backup data to Flash
*/
volatile uint8_t __backingUp=0;
 void perform_low_battry_backup() //was __monitor function
{
        // back up for no battery conditions
        if(__backingUp == 0)
        {
            __backingUp=1;
            
            
          struct info_mem_a_s backup2;
          
          backup2.seg_a.s.active_power_counter = phase->active_power_counter;
          backup2.seg_a.s.active_energy_counter = phase->active_energy_counter;          
          backup2.seg_a.s.active_energy_counter_QI = phase->active_energy_counter_QI;
          backup2.seg_a.s.active_energy_counter_QII = phase->active_energy_counter_QII;
          
          backup2.seg_a.s.active_energy_counter_QIII = phase->active_energy_counter_QIII;          
          backup2.seg_a.s.active_energy_counter_QIV = phase->active_energy_counter_QIV;
          backup2.seg_a.s.active_energy_export_counter = phase->active_energy_export_counter;
          backup2.seg_a.s.active_energy_import_counter = phase->active_energy_import_counter;
         
          backup2.seg_a.s.consumed_active_energy_QI = phase->consumed_active_energy_QI;
          backup2.seg_a.s.consumed_active_energy_QII= phase->consumed_active_energy_QII;          
          backup2.seg_a.s.consumed_active_energy_QIII = phase->consumed_active_energy_QIII;
          backup2.seg_a.s.consumed_active_energy_QIV = phase->consumed_active_energy_QIV;
          
          backup2.seg_a.s.reactive_power_counter = phase->reactive_power_counter; 
          backup2.seg_a.s.consumed_reactive_energy_QI = phase->consumed_reactive_energy_QI;
          backup2.seg_a.s.consumed_reactive_energy_QII = phase->consumed_reactive_energy_QII;
          backup2.seg_a.s.consumed_reactive_energy_QIII = phase->consumed_reactive_energy_QIII;
          
          backup2.seg_a.s.consumed_reactive_energy_QIV = phase->consumed_reactive_energy_QIV;       
          backup2.seg_a.s.reactive_energy_counter_QI = phase->reactive_energy_counter_QI;
          backup2.seg_a.s.reactive_energy_counter_QII = phase->reactive_energy_counter_QII;
          backup2.seg_a.s.reactive_energy_counter_QIII = phase->reactive_energy_counter_QIII;
          
          backup2.seg_a.s.reactive_energy_counter_QIV = phase->reactive_energy_counter_QIV;   
          
          //TODO. Change this to get the Time From hardware, 
          //getHardwareTime(&rtcc);
          TimeStump ts = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
          
          //Log this to EEPROM when power weaks up
          backup2.seg_a.s.RTCHigh = ts.TimestampUp & 0x000000ff;
          backup2.seg_a.s.RTCLow = ts.TimestampLow;
          //Also Log Power down and Power Up Event
          
          backup2.seg_a.s.TempLastEnergyValue = TempLastEnergyValue;
          backup2.seg_a.s.CurrentBalance = Current_balance.balance;
                              
          backup2.seg_a.s.ConsumptionSinceLastBilling =   ConsumptionSinceLastBilling;
          backup2.seg_a.s.valid_backup = 1;
          
          void *x = &backup2;
          int32_t * y = (int32_t *)x;
          flashBackup(y,27);
                                                                              
            status.LogEnergy = 0;   
            status.LogEnergyLogged = 1;                                   
            __backingUp =0;
        }
      //end of back up for no battery conditions
}
/**
* Proccesses Diagnosis results. Addes Errors to the system error list
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
}

/**
* Initializes the different periperials of the system
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
        //BAKMEM3  = 0; /*[M.G] Commentedout this line. to use this memory for LPM bug debugging Oct 24,2015 17:53 */
        
        init_ErrorList();
        InitGLCD_c();
        all_segments_on();
       __bis_SR_register(GIE);
        InitLogg();
       __bic_SR_register(GIE);          
        status.UpdateDate = 1;        
     
        
        
        Init_Scheduler();
        initTampperSwitchs();//should be initalized after scheduler because of its dependency
        InitUI();//should be initalized after scheduler because of its dependency
        
        RedLedOn();
        EnergyLedOn(); 
        
#if !defined(CUSTOM_LOGGER ) && !defined(PLC_USCIA0)
        initIEC62056();  /*[M.G] Un-commented this line Oct 22, 2015 - 13:48 */
#endif 
         
         /*global interrupt is required to intialize PLC,so enable it before initizlizing PLC
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
* checkes the levels with the maximum and minimum values
* and updates the system status 
*/
void performPowerQualityMessurement() 
{     
        uint32_t temp=phase->readings.I_rms;
           temp/=1000;
        if((temp) >= MAX_IRMS) 
        {
           status.OverCurrentStatus = 1;
           over_current_temp = phase->readings.I_rms;
        }
        else
        {
          status.OverCurrentStatus = 0;
        }
        
        
         temp = phase->readings.V_rms/100;
        if((temp) >= MAX_VRMS)
        {
           
           status.OverVoltageStatus = 1;
           over_voltage_temp = phase->readings.V_rms;
        }
        else
        {
          status.OverVoltageStatus = 0;
        }
        
        //temp = phase->readings.V_rms/100;
        if((temp) <= MIN_VRMS)
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
* method to verify that the relay is disconnectd and that the customer is not using power
*/
void verify_normal_operations()
{
    if(status.RelayStatus == 0 && phase->active_energy_import != lastEnergyReadingDuringDisconnect )
    {
      //error there is something wrong 
    }
}

/**
*  this should be executed before loading configurations from EEPROM, because 
*  this will log power out events. it also logs the last energy values to EEPROM.
*  this routine reads the last event log address from EEPROM. So EEPROM ports have to be 
*  initialized before calling this method.  
*
*  writes back up parameters from falsh to EEPROM and to the current working variables
*
*
*/
#include <string.h>
extern uint8_t firtstBoot;
void restoreBackup()
{
  if(backup.seg_a.s.valid_backup == 1)
  //if(firtstBoot == 0)
  {
            TimeStump t = {backup.seg_a.s.RTCLow,(uint8_t)backup.seg_a.s.RTCHigh};
            rtc_t time =  getTime(&t);  
            uint64_t td = labs(getTimeDifferenceInMinutes(&rtcc,&time));
            if(time.isValid)
            {
                if(compare_time(&rtcc,&time) < 0)
                {
                   hardware_status.RTC_Status = 0; //rtc is not ok  
                   //set it to the last saved rtc time
                   adjust_rtc(&time,0);
                }
                else //if(labs(getTimeDifferenceInMinutes(&rtcc,&time)) > MAX_EXPECTED_METER_OFF_TIME_MINUTES) //check whether time has moved too long  to the future or to the past 
                {
                   if(td > MAX_EXPECTED_METER_OFF_TIME_MINUTES)
                   {
                      hardware_status.RTC_Status = 0; //rtc is not ok  
                      adjust_rtc(&time,0);
                   }
                } 
            }
            
            uint8_t temp_did = 1;
            validation_arg_t noValidation = validation_arg_t_default;
            read_from_eeprom(&LastEventLogAddress,&temp_did,getLastLogAddress,&noValidation);  
            
            //log power is out event
            EventLog l;
            l.EventCode = PowerOut;
            l.timeStump = getTimeStamp(time.year, time.month, time.day, time.hour, time.minute, time.second);
            l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
                            + l.timeStump.TimestampUp);
            l.value = (uint32_t)td;
            write_to_eeprom(&l,(uint8_t *)0,logEvent);            
            
            
            //log power is back event
            l.EventCode = PowerUp;
            l.timeStump = getTimeStamp(rtcc.year, rtcc.month, rtcc.day, rtcc.hour, rtcc.minute, rtcc.second);
            l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
                            + l.timeStump.TimestampUp);
            l.value = 0;
            write_to_eeprom(&l,(uint8_t *)0,logEvent); 
            
            
//            Current_balance.balance = backup.seg_a.s.CurrentBalance;
//            TempLastEnergyValue = backup.seg_a.s.TempLastEnergyValue;
//            ConsumptionSinceLastBilling = backup.seg_a.s.ConsumptionSinceLastBilling;
            
            
            
            phase->active_power_counter = backup.seg_a.s.active_power_counter ;
            phase->active_energy_counter = backup.seg_a.s.active_energy_counter ;          
            phase->active_energy_counter_QI = backup.seg_a.s.active_energy_counter_QI ;
            phase->active_energy_counter_QII = backup.seg_a.s.active_energy_counter_QII ;
            
            phase->active_energy_counter_QIII = backup.seg_a.s.active_energy_counter_QIII ;          
            phase->active_energy_counter_QIV = backup.seg_a.s.active_energy_counter_QIV ;
            phase->active_energy_export_counter = backup.seg_a.s.active_energy_export_counter ;
            phase->active_energy_import_counter = backup.seg_a.s.active_energy_import_counter ;
           
            phase->consumed_active_energy_QI = backup.seg_a.s.consumed_active_energy_QI ;
            phase->consumed_active_energy_QII = backup.seg_a.s.consumed_active_energy_QII;          
            phase->consumed_active_energy_QIII = backup.seg_a.s.consumed_active_energy_QIII ;
            phase->consumed_active_energy_QIV = backup.seg_a.s.consumed_active_energy_QIV ;
            
            phase->reactive_power_counter = backup.seg_a.s.reactive_power_counter ; 
            phase->consumed_reactive_energy_QI = backup.seg_a.s.consumed_reactive_energy_QI ;
            phase->consumed_reactive_energy_QII = backup.seg_a.s.consumed_reactive_energy_QII ;
            phase->consumed_reactive_energy_QIII = backup.seg_a.s.consumed_reactive_energy_QIII ;
            
            phase->consumed_reactive_energy_QIV = backup.seg_a.s.consumed_reactive_energy_QIV ;       
            phase->reactive_energy_counter_QI = backup.seg_a.s.reactive_energy_counter_QI ;
            phase->reactive_energy_counter_QII = backup.seg_a.s.reactive_energy_counter_QII ;
            phase->reactive_energy_counter_QIII = backup.seg_a.s.reactive_energy_counter_QIII ;
            
            phase->reactive_energy_counter_QIV = backup.seg_a.s.reactive_energy_counter_QIV ;  
            write_to_eeprom(phase,(uint8_t *)0,logPowerFailEnergy);
                                                                       
            //make this value invalid
             struct info_mem_a_s backup2;
         
             void *x = &backup2;
             const void *y = &backup;             
             memcpy(x,y,sizeof(backup));
             
             backup2.seg_a.s.valid_backup = 0;             
             flashBackup(x,27);
                            
               #if defined(USE_WATCHDOG)
                 kick_watchdog();
               #endif
            write_to_eeprom(&rtcc,(uint8_t *)0,setRTCCheck);
  }
}
