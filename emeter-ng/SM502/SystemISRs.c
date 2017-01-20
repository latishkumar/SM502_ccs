
#include <msp430.h>
#include <stdint.h>
#include "Logger.h"
#include "Tamper.h"
#include "UI.h"
#include "Schaduler.h"
#include <stdio.h>


#include "IEC62056/IEC62056_UART.H"
#include "PLC/PLC_UART.h"

extern MeterStatus status;
extern CurrentBalance Current_balance;
extern unsigned long LastEventLogAddress;
extern unsigned long LastEnergyLogAddress;
extern unsigned long LastEnergyBillingCuttOffLogAddress;


extern volatile uint8_t UCTamperSchaduled ;
extern volatile uint8_t LCTamperSchaduled ;
extern volatile uint8_t MagneticTamperSchaduled ;

extern uint32_t TempLastEnergyValue;

unsigned int csum=0;
uint8_t w = 0;

uint16_t ISR_Debouncer = 5;
uint16_t LC_ISR_Counter = 0;
uint16_t UC_ISR_Counter = 0;
uint8_t test_temp22 = 0;
extern EventLog PowerOutlog;
extern volatile uint8_t IEC_Tx_Done;

extern __monitor void perform_low_battry_backup();

/**
* auxilary power supplay switching interrupt service routine 
* when DVCC is available use dvcc 
* otherwise use auxilary 2 i.e battery 
*/
#pragma vector=AUX_VECTOR
__interrupt void AUX_ISR(void)
{
  BAKMEM3 = AUXIV;  /*[M.G] Added for LPM debugging Oct 24, 2015 17:55 */
 switch(AUXIV)
 {
   
  case AUXIV_AUX0SWIFG://             /* Switched to DVCC interrupt flag */   
    
       
      //switch_to_normal_mode();
      
      #if defined(LOW_POWER_LOG_ENABLED)      
      printf("Waking Up Low Power Mode");
      _EINT();
      IEC_Tx_Done=0;
      while(IEC_Tx_Done==0);
      _DINT();
      
      #endif
      
      
      
      
      
         /* Take control of the EEPROM signals again. */
          enable_eeprom_port();

          #if defined(USE_WATCHDOG)
          kick_watchdog();
          #endif

          
        //Enable RTC Interrupt 
            RTCCTL0_H = RTCKEY_H;
            RTCCTL0_L |= RTCRDYIE_L;
            RTCCTL0_H = 0;   // LOCK RTC
            custom_power_restore_handler();  /*[M.G] Uncommented this line Oct 24, 2015 17:06 */
          
       _BIC_SR_IRQ(LPM4_bits);//wake up from low power mode    
            
    break;
  case AUXIV_AUX1SWIFG://             /* Switched to AUX1 interrupt flag */
    break;
  case AUXIV_AUX2SWIFG://             /* Switched to AUX2 interrupt flag */    
       #if defined(LOW_POWER_LOG_ENABLED)
         printf("Entering Low Power Mode");
         _EINT();
         IEC_Tx_Done=0;
         while(IEC_Tx_Done==0);
         _DINT();
       
       #endif


          //WDTCTL |= WDTPW | WDTHOLD;
        operating_mode = OPERATING_MODE_POWERFAIL;

         /* Note that a power down occured */
         meter_status |= POWER_DOWN;

        /* Turn off all the LEDs. */
        meter_status &= ~(STATUS_REVERSED | STATUS_EARTHED | STATUS_PHASE_VOLTAGE_OK);

        #if defined(__HAS_SD_ADC__)
        disable_analog_front_end();
        #endif
        #if defined(PWM_DITHERING_SUPPORT)
        /* Disable dithering, by disabling Timer B */
        TBCTL = TBCLR | TBSSEL_2;
        #endif

        custom_power_fail_handler();
        _BIS_SR_IRQ(LPM4_bits);

        __no_operation(); 
    break;

  case AUXIV_AUX2DRPIFG://            /* AUX2 dropped below its threshold interrupt flag */    
    break;
  case AUXIV_AUXMONIFG://             /* Supply monitor interrupt flag */
    
              /*
     TODO. fix the hardware bug which shows full battery when there is actually no battery 
           installed on the meter. 
    */
    

    
    
    
    break;
  case AUXIV_AUXSWNMIFG://            /* Supplies switched (non-)maskable interrupt flag */
    break;
 }
}



void Reset_System()
{
  PMMCTL0_H = PMMPW_H;    
  PMMCTL0|=PMMSWPOR;//PMMSWBOR;
  PMMCTL0_H = 0;  
}


/**
* Tamper ports interrupt service routine
*  if the processor is in low power mode store them in backup memory and 
*  log them when power is back
*  othrewise schedule logger and log them to eerpom 
*/
uint8_t temp_pfm=0;
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  switch(P1IV)
  {
    case P1IV_P1IFG1:  //lowercover
      
      if(operating_mode == OPERATING_MODE_POWERFAIL)//temp_pfm==1)//
      {

          
               LC_ISR_Counter= 0;
               BAKMEM0 = LowerCoverRemovedTamperError;
               LC_ISR_Counter = BAKMEM1;
               BAKMEM2 = BAKMEM0 + LC_ISR_Counter;  
               
          #if defined(LOW_POWER_LOG_ENABLED)      
               uint16_t bm2=BAKMEM2;
              printf("Lower Cover Tampered BAKMEM0=%d,BAKMEM1=%d,BAKMEM2=%d",BAKMEM0,LC_ISR_Counter,bm2);
               _EINT();
              IEC_Tx_Done=0;
              while(IEC_Tx_Done==0);
             
              _DINT();
          #endif
              status.LowerCoverRemovedTamperStatus = 1;
           write_to_eeprom(&status,(uint8_t *)0,logMeterStatus);   //comment this 
      }
      else
      {
        if(LCTamperSchaduled == 0)
        {
            LCTamperSchaduled =1;
            ScaduleTask(ProcessTamperLC,100,PROCESS_TAMPER_LC_TASK);//SchaduleTask(&ProcessTamperLC,100);
        }
      }

      break;
    case P1IV_P1IFG0: //upper cover 
            
      if(operating_mode == OPERATING_MODE_POWERFAIL)//temp_pfm==1)//
      {
        

          
                 UC_ISR_Counter = 0;
                 BAKMEM1 = UpperCoverRemovedTamperError; 
                 UC_ISR_Counter = BAKMEM1;
                 BAKMEM2 = BAKMEM0 + UC_ISR_Counter;
                 UC_ISR_Counter = 0;
      #if defined(LOW_POWER_LOG_ENABLED)     
                 uint16_t bm2=BAKMEM2;
          printf("Upper Cover Tampered BAKMEM0=%d,BAKMEM1=%d,BAKMEM2=%d",BAKMEM0,UC_ISR_Counter,bm2);
          _EINT();
          IEC_Tx_Done=0;
          while(IEC_Tx_Done==0);
          _DINT();
      #endif       
              status.UpperCoverRemovedTamperStatus = 1;
           write_to_eeprom(&status,(uint8_t *)0,logMeterStatus);   //comment this        
      }
      else
      {
        if(UCTamperSchaduled == 0)
        {
            UCTamperSchaduled =1;
            ScaduleTask(ProcessTamperUC,100,PROCESS_TAMPER_UC_TASK);//SchaduleTask(&ProcessTamperUC,100);
        }
      }
      
      break;    
  }
}
   

/**
* when power failes disable all functionality and enter low power mode 
* 
*/
void custom_power_fail_handler() {
  
  
        //disable watch dog timer 
        WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
        
        //Disable UART1
        disableIEC_UART();
        
        //Disable UART2
        disablePLC_UART();
        
        //Disable RTC Interrupt 
        RTCCTL0_H = RTCKEY_H;
        RTCCTL0_L &= ~RTCRDYIE_L;
        RTCCTL0_H = 0;   // LOCK RTC
        
        //turn off leds 
        //GreenLedOff();
        RedLedOff();
        EnergyLedOff();
        
        
        //turn off the buzzer 
        BuzzerOff();
        
        //stop the scaduler from executing any scaduled tasks 
        stopSchaduler();
        

        
       
        
                     
        disable_eeprom_port();       
        

        
        //turn off the LCD
        LCDCCTL0 &=~LCDON;
        
        
        //set all IO ports FOR low power mode configuration, according to msp430's datasheet 
        P1DIR = 0;
        P1SEL = 0;
        P1REN = 0xff & ~BIT0 & ~BIT1;
        P1OUT = 0;
        
        P2DIR = 0;
        P2SEL = 0;
        P2REN = 0xff;
        P2OUT = 0;
        
        P3DIR = 0;
        P3SEL = 0;
        P3REN = 0xff;
        P3OUT = 0;
        
        P4DIR = 0;
        P4SEL = 0;
        P4REN = 0xff;
        P4OUT = 0;        
        
        P5DIR = 0;
        P5SEL = 0;
        P5REN = 0xff;
        P5OUT = 0;        
        
        P6DIR = 0;
        P6SEL = 0;
        P6REN = 0xff;
        P6OUT = 0;
        
        P7DIR = 0;
        P7SEL  =0;
        P7REN = 0xff;
        P7OUT = 0;        
        
        P8DIR = 0;
        P8SEL = 0;
        P8REN = 0xff;
        P8OUT = 0;
        
        P9DIR = 0;
        P9SEL = 0;
        P8REN = 0xff;
        P9OUT = 0;
        
        LCDCPCTL0 = 0x0;                     
        LCDCPCTL1 = 0x0;                     
        LCDCPCTL2 = 0x0;   
        
//        IEC_TX_DIR_OUT;
//        IEC_RX_DIR_IN;
        
}


uint8_t prc=0;
extern uint8_t is_time_valid(const rtc_t *time);
/**
* when power is back check the backup memory for any UC and LC tamper events 
* and log the events to the EERPOM and restart
*/
void custom_power_restore_handler()
{
    
      #if defined(LOW_POWER_LOG_ENABLED)
       printf("POR Reset ");
       
       
       IEC_Tx_Done=0;
       while(IEC_Tx_Done==0);
      
       
       #endif
       
        EEPROM2Init();
                                    
      #if defined(LOW_POWER_LOG_ENABLED)
        printf("before reset BAKMEM0 = %d, ",BAKMEM0);
        printf("BAKMEM1 = %d, ",BAKMEM1);
        printf("BAKMEM2 = %d\n ",BAKMEM2);
       
       //_EINT();
       IEC_Tx_Done=0;
       while(IEC_Tx_Done==0);
       //_DINT();
       
       #endif

       //check if tamper happend during low power mode and process them 
        uint16_t test_temp = BAKMEM0;
        test_temp +=BAKMEM1;

        if(BAKMEM2 == test_temp)
        {
           #if defined(LOW_POWER_LOG_ENABLED)
            printf("BAKMEM0 = %d, ",BAKMEM0);
            printf("BAKMEM1 = %d, ",BAKMEM1);
            printf("BAKMEM2 = %d\n ",BAKMEM2);
           
           
           IEC_Tx_Done=0;
           while(IEC_Tx_Done==0);
          
           
           #endif
          //process the tamper 
          if(BAKMEM0  == LowerCoverRemovedTamperError)
          {   
            
             #if defined(LOW_POWER_LOG_ENABLED)
               printf("Tampered Lower Cover");
             #endif
            
              BAKMEM0 = 0;
//              status.LowerCoverRemovedTamperStatus = 1;
              LCTamperDetected();
          }
          if(BAKMEM1 == UpperCoverRemovedTamperError)
          {
             #if defined(LOW_POWER_LOG_ENABLED)
               printf("Tampered Upper Cover");
             #endif     
               
//               status.UpperCoverRemovedTamperStatus =1;
               UCTamperDetected();
              BAKMEM1 = 0;
          }    
          write_to_eeprom(&status,(uint8_t *)0,logMeterStatus);//uncomment this 
        }
               
        BAKMEM2 = 0; 
        BAKMEM3 = 0;            
            
        operating_mode = OPERATING_MODE_NORMAL;
        Reset_System();
  
    
 
}



