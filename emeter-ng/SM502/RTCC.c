/*
 * RTCC.c
 *
 *  Created on: May 18, 2013
 *      Author: Elec_Lab
 */

#include "RTCC.h"
#include <msp430.h>
#include "Status.h"

extern MeterStatus status;
//RTCtime time;

void RTCCinit()
{
	RTCCTL0_H  = RTCKEY_H;
	RTCCTL0_L |= RTCOFIE|RTCTEVIE|RTCAIE|RTCRDYIE;//RTC fault wake up Enabled
								  //Time event interrupt enabled
								  //Real Time Clock alarm Enabled
								  //Real Time Clock ready interrupt
	RTCCTL1 |= RTCBCD|RTCHOLD|RTCMODE|RTCTEV_0; //BCD Mode
									   	   	   //RTC hold
									   	   	   //in calendar mode
											   //RTC Time Event Every Day At Midnight
	//RTCCTL3 |= RTCCALF_2; 512 Hz Calibration Frequency
						    //pins need to be configured for calibration

	//RTCOCAL  RTC oscillator offset calibration value

    //RTCTCMP  RTC temperature compensation register
	RTCSEC  = 0x20;
	RTCMIN  = 0x12;
	RTCHOUR = 0x5;
	RTCDOW  = 0x2;
	RTCDAY  = 0x13;
	RTCMON  = 0x5;
	RTCYEAR = 0x2011;

        RTCADOWDAY = 0x3;                       // RTC Day of week alarm = 0x2
        RTCADAY    = 0x22;                         // RTC Day Alarm = 0x22
        RTCAHOUR   = 0x23;                        // RTC Hour Alarm
        RTCAMIN    = 0x45;                         // RTC Minute Alarm

        RTCCTL1    &= ~(RTCHOLD);                  // Start RTC calendar mode
        RTCCTL0_H   = 0;                          // Lock RTC_C module
//        time.Minute = 12;
//        time.Second = 20;
//        time.Hour  = 15;

    __no_operation();
}


int x=0;
int read=0;

//#pragma vector  = RTC_VECTOR
__interrupt void RTC_Interrupt()
{
	switch(__even_in_range(RTCIV,16))
	{
    case RTCIV_NONE:                    // No interrupts
          break;
      case RTCIV_RTCOFIFG:                // RTCOFIFG

          break;
      case RTCIV_RTCRDYIFG:               // RTCRDYIFG
          P1OUT ^= 0x01;                  // Toggles P1.0 every second
    	  //if(read==1)
    	  {
//
//             x = RTCMIN;
//
//             time.Minute =  (x & 0xf) + 10 *(x>>4);
//
//             status.UpdateTime = 1;
//
//             x=RTCSEC;
//             time.Second =  (x & 0xf) + 10 * ((x>>4));
//             read= 0;
    	  }
          break;
      case RTCIV_RTCTEVIFG:               // RTCEVIFG
          //read =1;
          break;
      case RTCIV_RTCAIFG:                 // RTCAIFG
          __no_operation();               // Interrupts every alarm event
          break;
      case RTCIV_RT0PSIFG:                // RT0PSIFG
          break;
      case RTCIV_RT1PSIFG:                // RT1PSIFG
          break;
      case 14: break;                     // Reserved
      case 16: break;                     // Reserved
      default: break;
	}
}
