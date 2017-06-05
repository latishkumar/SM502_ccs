
#include "iec62056_uart.h"
#include <msp430.h>
#include "UI.h"
#include "EventTypes.h"
#include "Relay.h"
//#include "graphics.h"
#include "LCD_C.h"
#include "Status.h"
#include "Tamper.h"
#include "PLC/MAC_PIB.h"
#include "PLC/Data_Transfer.h"
#include "Schaduler.h"
#include "LCD_C_Graphics.h"

extern MeterStatus status;

//initialized in graphics.c file
extern uint8_t MaxDisplayState;
extern uint8_t MaxDisplayStateMode1;

extern uint8_t Current_ErrorNo;
extern uint8_t Error_counts;
//extern volatile uint8_t switch_to_power_fail_mode;
//uint8_t waitup=0;
//uint8_t keyProcessed = 1;
uint16_t  keyPressCount = 0;
uint16_t keyPressDebounce =0 ;
uint8_t keyReleasedDebounce = 0;
//uint8_t tempPressDebounce = 0;

const uint16_t keydebounce = 2;//120;
uint8_t KeyPadState = 0;

extern volatile uint8_t CurrentSystemErrorsCount ;

uint8_t buzzer_status = 0;//off


#define Current_Balance_Warning_Freq
#define Contract_Power_Warning_Freq

#define Buzzer_beep_interval 10
int buzeer_time_counter=0;

#define NumberOfBeep 5
int numberOfBeepCounter=0;
/**
* initilizes ports for key pad and buzzor
*/
void InitUI()
{

   KeyDir &= ~KeyBit; // make the key pin an input

   LedRedDir |= LedRedBit;
   LedEnergyDir |= LedEnergyBit;
  
   RedLedOff();
   EnergyLedOff();
  
  
   configurationRS232Dir   &= ~configurationRS232Bit;
   
#ifdef LOW_POWER_TEST
      configurationUSBDir  |= configurationUSBBit;
#else
      configurationUSBDir  &= ~configurationUSBBit;
#endif    

    
   
   BuzzerDir |= BuzzerBit;
   BuzzerSEL |= BuzzerBit;
   
   /*
     1 coount  ~~ 1/16MHz 
      x        ~~ 1/4KHz
   
      x ~ 4000
   */
   //initialize Buzzer 
         //5                            // PWM Period/2 //app 5khz 
    TA1CTL = TASSEL_2 | MC_0;//| ID_8;               // ACLK, up-down mode   MC_1 up mode 
    TA1CCTL1 = OUTMOD_7;                             // CCR0 reset/set
    TA1CCR1 = 2000; 
    TA1CCR0 = 4000;
    //TA1CCR1 = 100;                                                                     // MC_2 Continious mode 
                                                                          // MC_3 up/down mode 
    
   //Delete this Lines only for power consumption test 
//          BuzzerOn();
   //end of Delete this Lines only for power consumption test 
   ScaduleTask(processKey,5000,PROCESS_KEY_TASK);       
}


/*
 checks if key is pressed and raises key pressed flag
 this method reads the keypad without holding the program in a loop
 the main program is expected to execute 244usec so this method will be called
 at least at his rate. 

  the method keeps the key press count, so that this count could be used to update 
  the display or System based on timed key events 
@returns 1 if key is pressed 
         0 otherwise 
*/

uint8_t testKey()
{
#ifdef KIT
  if((KeyPort & KeyBit) ==0 )//!= 0
#else
  if((KeyPort & KeyBit) !=0 )
#endif     
  {
     if(++keyPressDebounce >= keydebounce) //key pad debounce 
       {           
           status.KeyPressed = 1; 
           keyPressDebounce = 0;
           return 1;
       }
  }
#ifdef KIT
  else if((KeyPort & KeyBit) !=0 && status.KeyPressed == 1)  //== 0//key pad debounce 
#else
  else if((KeyPort & KeyBit) ==0 && status.KeyPressed == 1)  //== 0//key pad debounce   
#endif
  {
     if(++keyReleasedDebounce >= keydebounce)
     {
       status.KeyPressed = 0;
       keyReleasedDebounce= 0;
       return 1;
     }
    
  }
  return 0;
}
void KeyRealeased_org();

volatile uint8_t re = 0;
/**
*  Call back for keyReleased event 
*  creates beep sound
*/
void KeyRealeased()
{  
     BeepBuzzer();
     KeyRealeased_org();
}
/**
* key released callback for manual key presses 
* to update the display state
*/
void KeyRealeased_org()
{   

  if(status.DisplayMode == 0) //if auto mode switch to manual mode
    switch_to_manual_mode_c();
  display_next_item_c();
}

/**
* if key is perssed, process the result
*/
void processKey()
{
  
   uint8_t keyState = testKey();
   if(keyState == 1) //key changed, pressed or released with debounce 
   {
     //te++;
     //WriteLong(te,80,2,font5x8);
      switch(KeyPadState)
      {
      case 0: // key is not prossed 
          if(status.KeyPressed == 0)
          {
            KeyPadState = 0;
          }
          else{
            KeyPadState = 1;           
          }   
          keyPressCount = 0;
        break;
      case 1: // key is pressed
          if(status.KeyPressed == 0)
          {
            KeyPadState = 0;
            KeyRealeased();
          }
          else{
            KeyPadState = 2;
            keyPressCount = 1;           
          }
          
        break;  
      case 2: // key is down 
         if(status.KeyPressed == 0)
          {
            KeyPadState = 0;
            KeyRealeased();
          }
         else{            
            keyPressCount++;           
             if( keyPressCount >= 50)//0x400
               {   
                     keyPressCount = 0;
                  
                     if(status.DisplayMode == 0 || status.DisplayMode == 2)
                     {
                       status.DisplayMode = 1;                       
                     }
                     else 
                     {
                       status.DisplayMode = 0;
                     }
                     status.DisplayState = 0;                                     
                }
          }
        break;
      }
   }
   else if(keyState == 0)
   {
   }
   ScaduleTask(processKey,10,PROCESS_KEY_TASK);
}
/**
* alarms a single beep sound on the buzzor
*/
void BeepBuzzer()
{
  BuzzerOn();
  ScaduleTask(BuzzerOff,30,KEEPAD_BEEP_TASK);
  
}
/**
* turns the buzzor on
*/
void BuzzerOn()
{
  TA1CTL |= (TASSEL_2 | (MC_1));
  buzzer_status = 1;
}

/**
* turns the buzzor off 
*/
void BuzzerOff()
{
  TA1CTL =  MC_0;
  buzzer_status = 0;  
}


/**
* toggles the buzzors status. if it is on it turns it off otherwise it will turn it on
*/
void ToggleBuzzer2()
{
  buzeer_time_counter++;
  if(buzeer_time_counter==Buzzer_beep_interval)
  {
     SecondBeeps();
     buzeer_time_counter=0;
  }
  ScaduleTask(ToggleBuzzer2,MINUTE,TOOGLE_BUZZER_TASK); 
}


/**
* creates a beep sound on the bezzor for specified amount of time at an interval of second
*/
void SecondBeeps()
{
  numberOfBeepCounter++;
  if(numberOfBeepCounter<(NumberOfBeep<<1))
  {    
    ToggleBuzzer();
    ScaduleTask(SecondBeeps,1000,SECCOND_BEEP_TASK);
  }
  else
  {
    CancelTask2(SecondBeeps);
    BuzzerOff();
    numberOfBeepCounter=0;
  }
}
/**
* toggles the buzzors status. if it is on it turns it off otherwise it will turn it on
*/
void ToggleBuzzer()
{
  
    if(buzzer_status == 1)
    {
      BuzzerOff();
    }
    else
    {
      BuzzerOn();
    }
}

/**
* turns on the Energy pulse LED
*/
void EnergyLedOn()
{
    LedEnergyPort &= ~LedEnergyBit;
}
/**
* turns off the Energy pulse LED
*/
void EnergyLedOff()
{
    LedEnergyPort |= LedEnergyBit;
}

/**
* turns on the red led on the bord
*/
void RedLedOn()
{
     LedRedPort &= ~LedRedBit;
}
/**
* turns of the red led on the bord
*/
void RedLedOff()
{
     LedRedPort |= LedRedBit;
}



/**
* turns the red led on or off based on the systems status
*/
void UpadateErrorIndicators()
{  
  
  if(CurrentSystemErrorsCount > 0)
  {
     RedLedOn();
  }
  else
  {
    RedLedOff();
  }
  
}


/*[M.G] Restored this function from previous version */
void checkConfigrationPortsStatus()
{
  
  if((configurationRS232Pin&configurationRS232Bit) == configurationRS232Bit)
  {
    RS232(1);
  }
  else
  {
    RS232(0);
  }
  if((configurationUSBPin&configurationUSBBit) == configurationUSBBit)
  {
    USB(1);
  }
  else
  {
    USB(0);
  }
  
}
