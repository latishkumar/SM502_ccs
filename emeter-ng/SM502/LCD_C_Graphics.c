/*
 * graphics.c
 * graphics Library for SM502/Arada Project
 *  Created on: Apr 25, 2013
 *      Author: Donayam
 */

#include "LCD_C_Graphics.h"

#include "EthioCalender.h" 
   
#include "LCD_C.h"
#include "Status.h"
#include "Tariff.h"
#include "Tamper.h" // for tamper types
#include <string.h>
#include <Scheduler.h>
#include "utilities.h"  

extern  struct phase_parms_s chan1;
//extern RTCtime time;
extern rtc_t rtcc; //emeter-toolkit.h
extern uint32_t total_consumed_reactive_energy;

extern CurrentBalance Current_balance;
extern uint32_t MAXActivePowerTripPoint ; //in 10m watt steps 
//extern HardwareStatus hardware_status;





/*
 *  s the string to write
 *  column is the start column
 *  row the initial row
 *
 *
 *The method supports column wind up
 */




#define ErrorDisplayStart 13


uint8_t MaxDisplayState = 13; //=ErrorDisplayStart;
uint8_t MaxDisplayStateMode1 = 2 ;
uint8_t current_display_Error = 0;
uint8_t temp_disp;
int vrms;

extern MeterStatus status;
extern uint8_t active_quadrant;

uint32_t scroll_display_time=15;
uint32_t AutoDisplayTime = 15000;//msec 
uint16_t BackToAutoModeTime = 65000;//msec 
uint8_t Language = AM_EN;
uint8_t SelectedCalender = AM;

/**
* initializes the lcd hardware and variables 
*/
void InitGLCD_c() {

    init_lcd_c();
    
//    hardware_status.LCD_Status = 1;
  
    status.UpdateDate = 1;
    status.UpdateTime = 1;
    //status.DisplayState = 0;
    status.DisplayUpdateRequired = 1;
    swtch_to_auto_mode_c();
    //ScaduleTask(display_next_item_c,AutoDisplayTime,AUTO_DISPLAY_TASK);
}
/**
* disables manual scrolling and switches the display to auto mode
*/
void swtch_to_auto_mode_c()
{
  
  status.DisplayMode  = 0;
  status.DisplayState = 0;
  schedule_task(display_next_item_c,AutoDisplayTime,AUTO_DISPLAY_TASK);
  
}
/**
* disables auto scrolling and swithces the display to manual mode
*/
void switch_to_manual_mode_c()
{
  status.DisplayMode = 1;
//  ScaduleTask(swtch_to_auto_mode_c,BackToAutoModeTime,AUTO_MANUAL_DISPLAY_END_TASK);
}
/**
* updates the display and shows the next item inline to be displayd
*/
void display_next_item_c()
{
  
  
      status.DisplayState++;
      if(status.DisplayState >= ErrorDisplayStart)//start of error number 
      {

          temp_disp = getNextError(&current_display_Error);
          if( temp_disp == 1)
          {
            status.DisplayState=ErrorDisplayStart;
          }
          else
          {
//             if(getCurrentErrorNumber()<CurrentSystemErrorsCount)
//             {
//                        status.DisplayState = 11;
//             }
             status.DisplayState = 0;
          }
      }
      else if(status.DisplayState > MaxDisplayState)
      {                    
          status.DisplayState = 0;   
      } 
      
      if(status.DisplayMode == 0) //auto mode
      {
          schedule_task(display_next_item_c,AutoDisplayTime,AUTO_DISPLAY_TASK);
      }
      else if(status.DisplayMode == 1) //manual Mode 
      {
          cancel_task(swtch_to_auto_mode_c);
          schedule_task(swtch_to_auto_mode_c,BackToAutoModeTime,AUTO_MANUAL_DISPLAY_END_TASK);
      }     
}


/*
 * writes a long number to the LCD
 * @params
 * 			number :- the number to write to the LCD
 * 			column :- the column where the number is to be written
 * 			row :- the row where the number will be displayed
 * 			font:- font size used to display the number. Possible values are font5x8 and font12x16
 */
void WriteFloat_c(long number,uint8_t dp) {
		
    //clear
        clear_numbers();
	//number=1200;
//        long remainder = 0;
	int i = -1;
	uint8_t num[]={0,0,0,0,0,0,0,0,0 }; //Assuming that we are going to write a number with maximum of 8 digits
        longToString_c(number,num,9,&i);
        
        
        if(i==-1) //handel number that is zero
        {
          i=0;
        }
        if(dp>0)
        {
          switch(dp)
           {
             case 1:
               Point4(1);
               break;
             case 2:        
               Point3(1);
               break;
             case 3:
               Point2(1);
               break;
             case 4:
               Point1(1);
               break;
           }
          uint8_t z=0;
          for(;z<=dp;z++)
          {
             switch(z)
              {
                case 0:
                  D1(0);
                  break;
                case 1:
                  D2(0);
                  break;
                case 2:
                  D3(0);
                  break;
                case 3:
                  D4(0);
                  break;
                case 4:
                  D5(0);
                  break;
                case 5:
                  D6(0);
                  break;
                case 6:
                  D7(0);
                  break;
                case 7:
                  D8(0);
                  break;
              }          
          }
        }
        
	//handle one digit number as 0x
	if (i == 0) {
		D2(0);
                D1(0);
	} else {
//          uint8_t z=i;
               i-=1;
		for (; i >= 0; i--) {
                    switch(i)
                    {
                    case 0:
                      D1(num[i]);
                      break;
                    case 1:
                      D2(num[i]);
                      break;
                    case 2:
                      D3(num[i]);
                      break;
                    case 3:
                      D4(num[i]);
                      break;
                    case 4:
                      D5(num[i]);
                      break;
                    case 5:
                      D6(num[i]);
                      break;
                    case 6:
                      D7(num[i]);
                      break;
                    case 7:
                      D8(num[i]);
                      break;
                    }                       
		}                
	}    
}

/**
* show a tamper indicator 
* @param TamperType:- see the tamper module for list of tamper types
*/
void ShowTamper_c(int TamperType)
{
    switch(TamperType)
    {
    case MagneticTamperType:
    	MTamper(1);
    	break;
    case  UC_TamperType:
    	UTamper(1);
    	break;
    case LC_TamperType:
    	LTamper(1);
    	break;
    case NeutralTamperType: 
        NTamper(1);
    	break;
    case EarthTamperType:
    	break;
    case ConfigrationPortTamperType:
    	break;
    case ReverseConnactionTamperType: //reverse tamper
    	//WriteString("R~", ReverseTamperStartX, 7, font5x8);
    	break;
    case MeterBypassTamperType:
    	break;
    default:
    	break;
    }
}
/**
* @see ShowTamper_c
*/
void ClearTamper_c(int TamperType)
{
	    switch(TamperType)
	    {
	    case MagneticTamperType:
	    	MTamper(0);
	    	break;
	    case  UC_TamperType:
	    	UTamper(0);
	    	break;
	    case LC_TamperType:
	    	LTamper(0);
	    	break;
	    case NeutralTamperType:
            NTamper(0);
	    	break;
	    case EarthTamperType:
	    	break;
	    case ReverseConnactionTamperType:
	    	break;
	    case MeterBypassTamperType:
	    	break;
	    default:
	    	break;
	    }
}
uint8_t tariffslot = 0;
extern uint32_t power_brakes[];//current ratting 
extern uint32_t ConsumptionSinceLastBilling;
extern uint32_t rates[];
/**
* updates the date display base on the system status
*/
void UpdateDisplay_c() {
  
	if (status.DisplayUpdateRequired == 1) {
                 int w;
                 unsigned long z;
                 uint8_t rateIndex;
                 int32_t charge;
                      
//            //clear the display      
//            if(status.DisplayMode ==0 )//normal mode 
//              {
                
                clear_all_amharic_indicators();
                clear_all_english_indicatros();
                clear_all_non_uint_indicators();
                clear_all_units();
                
		switch (status.DisplayState) {
		case 0: // display the active energy import of the meter
                  
                      //show the current tariff rate
                      Tariff(1);
                      
                      tariffslot = GetCurrentMinuteTariffTimeSlot();
                      TH(0);
                      TL(tariffslot);
                      tariffslot = 0;
                  
			if(Language == EN)
                        {
                          EN_Active(1);
                          EN_Energy(1);
                        }
                        else if(Language == AM)
                        {
                           //AM_Active(1);
                           AM_Energy(1);
                        }
                        else if(Language == AM_EN)
                        {
                           EN_Active(1);
                           EN_Energy(1);
                          
                           //AM_Active(1);
                           AM_Energy(1);
                        }
                        
                        WriteFloat_c(chan1.import_active_energy_QI_QIV,3);
			UINT_KWH;
                        
			break;

		case 1: // display the reactive energy consumption of the meter
                        if(Language == EN)
                        {
                          EN_Reactive(1);
                          EN_Energy(1);
                        }
                        else if(Language == AM)
                        {
//                           AM_Reactive(1);
                           AM_Energy(1);
                        }
                        else if(Language == AM_EN)
                        {
                           EN_Reactive(1);
                           EN_Energy(1);
                          
//                           AM_Reactive(1);
                           AM_Energy(1);
                        }
                        
                        WriteFloat_c(phase->consumed_reactive_energy_QI+phase->consumed_reactive_energy_QIV,3);
			UNIT_KVARH;

			break;
                case 2://display Active Power
                        if(Language == EN)
                        {
                          EN_Active(1);
                          EN_Power(1);
                        }
                        else if(Language == AM)
                        {
//                           AM_Active(1);
                           AM_Power(1);
                        }
                        else if(Language == AM_EN)
                        {
                          EN_Active(1);
                          EN_Power(1);
                          
//                          AM_Active(1);
                          AM_Power(1);
                        }
                        
                        WriteFloat_c(phase->readings.active_power,2);
			UNIT_W;

			break;
                  
                case 3: //display the reactive Power
                        if(Language == EN)
                        {
                          EN_Reactive(1);
                          EN_Power(1);
                        }
                        else if(Language == AM)
                        {
//                           AM_Reactive(1);
                           AM_Power(1);
                        }
                        else if(Language == AM_EN)
                        {
                           EN_Reactive(1);
                           EN_Power(1);
                          
//                           AM_Reactive(1);
                           AM_Power(1);
                        }
                        
                        WriteFloat_c(phase->readings.reactive_power,2);
			UINT_VAr;

			break;

		case 4: // display the power factor

                        PF(1);					
                        w = chan1.readings.power_factor;
                        
//                        if(w<0)
//                          w=-w;
                        
                        WriteFloat_c((w),4);

			break;

		case 5: // display the RMS Current
                  
                        Current(1);

                        WriteFloat_c(chan1.readings.I_rms,3);
			UINT_Amp;
                        
			break;

		case 6: // display the RMS voltage
			Voltage(1);
                        vrms = chan1.readings.V_rms;
                        WriteFloat_c(vrms,2);
                        UINT_Volt;
			break;
                        
                 case 7: // display frequency
                   
                        Frequency(1);
                        WriteFloat_c(chan1.readings.frequency,2);
			UINT_HZ;
                        
			break;

                case 8:  //display Current balance

                        if(Language == EN)
                        {
                           EN_Birr(1);
                           EN_Balance(1);
                           
                        }
                        else if(Language == AM)
                        {
                           AM_Birr(1);
                           AM_Balance(1);
                        }
                        else if(Language == AM_EN)
                        {
                           AM_Birr(1);
                           AM_Balance(1);
                           EN_Birr(1);
                           EN_Balance(1);
                        }
                        z= Current_balance.balance;

                        WriteFloat_c(z,4);

                  break;
                  
                case 9:  //display todays current tariff rate                   
                case 10:
                case 11:
                case 12:
                  
                       if(Language == EN)
                        {
                           EN_Birr(1);
                           //EN_DAY(1);
                        }
                        else if(Language == AM)
                        {
                           AM_Birr(1);
                           //AM_DAY(1);
                        }
                        else if(Language == AM_EN)
                        {
                           AM_Birr(1);
                           //AM_DAY(1);
                           EN_Birr(1);
                           //EN_DAY(1);
                        } 
                       
                       
                        
                       
                       if(status.DisplayState == 9)
                       {
                         tariffslot = 0;
                       }
                       else if(status.DisplayState == 10)
                       {
                         tariffslot = 1;
                       }
                       else if(status.DisplayState == 11)
                       {
                         tariffslot = 2;
                       }
                       else if(status.DisplayState == 12)
                       {
                         tariffslot = 3; 
                       }                        
//                        tariffslot++;
//                        if(tariffslot>3)
//                          tariffslot = 0;
                       
                        rateIndex = getRateStartIndex(power_brakes,ConsumptionSinceLastBilling);
                        charge    = rates[rateIndex+(tariffslot*10)];
                        
                        Tariff(1);
                        TH(0);
                        TL((tariffslot+1));
                        
                        WriteFloat_c(charge,4);
                        AM_Birr(1);
                         
                  break;
                  
		default:
//                 temp_disp = getNextError(&current_display_Error);
//                 if( temp_disp == 1)
//                 {
                   if(Language == EN)
                    {
                       EN_Error(1);
                    }
                    else if(Language == AM)
                    {
                       AM_Error(1);
                    }
                    else if(Language == AM_EN)
                    {
                       AM_Error(1);
                       EN_Error(1);
                    }
		    WriteFloat_c(current_display_Error,0);
//#ifndef LCD_REV_2
                    if(current_display_Error == HighVoltageError) 
                    {
                           if(Language == EN)
                            {
                               EN_Over(1);
                            }
                            else if(Language == AM)
                            {
                               AM_Over(1);
                            }
                            else if(Language == AM_EN)
                            {
                               EN_Over(1);
                               //AM_Over(1);
                            }
                      
                             Voltage(1);
                      
                    }
                    else if(current_display_Error == LowVoltageError)
                    {
                      if(current_display_Error == HighVoltageError) 
                        {
                           if(Language == EN)
                            {
                               EN_Under(1);
                            }
                            else if(Language == AM)
                            {
                               AM_Under(1);
                            }
                            else if(Language == AM_EN)
                            {
                               EN_Under(1);
                               AM_Under(1);
                            }
                             Voltage(1);                      
                        }
                    }                  
			break;
		}
		status.DisplayUpdateRequired = 0;
	}
        
        

	if (status.UpdateDate == 1) {
		/*
		 update the date
		 */
                clear_date();
		uint8_t temp_buff[4] = {0,0,0,0};
                rtc_t temp_time;
                int length;
                if(SelectedCalender == EN || SelectedCalender == AM_EN)
                {
                  temp_time.day = rtcc.day;
                  temp_time.year = rtcc.year;
                  temp_time.month = rtcc.month;
//                  temp_time.hour = rtcc.hour;
                  GC;
                  
                  
                   //month first  
                  longToString_c(temp_time.month,temp_buff,4,&length);
                  YH(temp_buff[1]);
                  YL(temp_buff[0]);                
                  YM_Bar(1);
                  
                  temp_buff[0] = 0;
                  temp_buff[1] = 0;
                  //day next
                  longToString_c(temp_time.day,temp_buff,4,&length);
                  MH(temp_buff[1]);
                  ML(temp_buff[0]);
                  MD_Bar(1);
                  
                  temp_buff[0] = 0;
                  temp_buff[1] = 0;
                  //year last 
                  longToString_c(temp_time.year,temp_buff,4,&length);
                  DH(temp_buff[1]);               
                  DL(temp_buff[0]);
                }
                else if(SelectedCalender == AM)
                {
                  toEthioCalender(&rtcc,&temp_time);
                  EC;
                  
                 //day first 
                temp_buff[0] = 0;
                temp_buff[1] = 0;
                longToString_c(temp_time.day,temp_buff,4,&length);
                YH(temp_buff[1]);
                YL(temp_buff[0]);                
                YM_Bar(1);
                
                //month next
                temp_buff[0] = 0;
                temp_buff[1] = 0;
                longToString_c(temp_time.month,temp_buff,4,&length);
                MH(temp_buff[1]);
                ML(temp_buff[0]);
                MD_Bar(1);
                
                //year last 
                temp_buff[0] = 0;
                temp_buff[1] = 0;
                longToString_c(temp_time.year,temp_buff,4,&length);
                DH(temp_buff[1]);               
                DL(temp_buff[0]);
                  

                }
                

                
		status.UpdateDate = 0;
	}

	if (status.UpdateTime == 1) {
          
                clear_hourminute();
                uint8_t temp_buff[2]={0,0};
                int length;
                longToString_c(rtcc.hour,temp_buff,2,&length);
                
                HH(temp_buff[1]);               
                HL(temp_buff[0]);
                
                temp_buff[0] = 0;
                temp_buff[1] = 0;
                longToString_c(rtcc.minute,temp_buff,2,&length);
                MinH(temp_buff[1]);
                MinL(temp_buff[0]);
                Sec(1);
                               		          
		status.UpdateTime = 0;
                
	}
        
          if(status.RelayStatus == 1) //connected
          {
             Relay(1);
          }
          else
          {
             Relay(0);
          }
        
         if(phase->status & PHASE_REVERSED)//active_quadrant == 1 || active_quadrant == 4)
         {
             showExportIndicator;
         }
         else
         {
             showImportIndicator;
         }    
}
/**
* updates the Consumed energy progress bar
*/
void UpdateDisplayProgressBar_c() {  
        uint8_t percent= (chan1.readings.active_power)/MAXActivePowerTripPoint;        
	int per = percent ;//((100 - percent) >> 1); // divided by 2
        
	PB(per);
}
