/*
 * unused_interrupts_handler.c
 *
 *  Created on: Jan 13, 2017
 *      Author: eliab
 */

#include <msp430.h>
#include <stdint.h>
#include "EventTypes.h"
uint8_t BOR_flag;
void log_standard_events(uint8_t event_type);
#pragma vector = RESET_VECTOR,SYSNMI_VECTOR,UNMI_VECTOR
__interrupt void RESET_ISR(void)
{
	 switch (__even_in_range(SYSRSTIV, SYSRSTIV_PMMKEY))
	    {
	        case SYSRSTIV_NONE:                       // No Interrupt pending
	                                __no_operation();
	                                break;
	        case SYSRSTIV_BOR:                        // SYSRSTIV : BOR
	                                log_standard_events(BOR_RESET_EVENT);
	                                __no_operation();
	                                break;
	        case SYSRSTIV_RSTNMI:                     // SYSRSTIV : RST/NMI
	                                __no_operation();
	                                break;
	        case SYSRSTIV_DOBOR:                      // SYSRSTIV : Do BOR
	                                __no_operation();
	                                break;
	        case SYSRSTIV_LPM5WU:    BOR_flag=1;      // SYSRSTIV : Port LPM5 Wake Up
	                                __no_operation();
	                                break;
	        case SYSRSTIV_SECYV:                      // SYSRSTIV : Security violation
	                                __no_operation();
	                                break;
	        case SYSRSTIV_SVSL:                       // SYSRSTIV : SVSL
	                                __no_operation();
	                                break;
	        case SYSRSTIV_SVSH:                       // SYSRSTIV : SVSH
	                                __no_operation();
	                                break;
	        case SYSRSTIV_SVML_OVP:                   // SYSRSTIV : SVML_OVP
	                                __no_operation();
	                                break;
	        case SYSRSTIV_SVMH_OVP:                   // SYSRSTIV : SVMH_OVP
	                                __no_operation();
	                                break;
	        case SYSRSTIV_DOPOR:                      // SYSRSTIV : Do POR
	                                __no_operation();
	                                break;
	        case SYSRSTIV_WDTTO:                      // SYSRSTIV : WDT Time out
	                                __no_operation();
	                                break;
	        case SYSRSTIV_WDTKEY:                     // SYSRSTIV : WDTKEY violation
	                                __no_operation();
	                                break;
	        case SYSRSTIV_KEYV:                       // SYSRSTIV : Flash Key violation
	                                __no_operation();
	                                break;
	        case SYSRSTIV_FLLUL:                      // SYSRSTIV : FLL unlock
	                                __no_operation();
	                                break;
	        case SYSRSTIV_PERF:                       // SYSRSTIV : peripheral/config area fetch
	                                __no_operation();
	                                break;
	        case SYSRSTIV_PMMKEY:                     // SYSRSTIV : PMMKEY violation
	                                __no_operation();
	                                break;
	        default: break;
	    }
}
 void Reset_System();
 void one_second_ticker(void);//RTC_VECTOR
#pragma vector=LCD_C_VECTOR,TIMER3_A1_VECTOR,TIMER3_A0_VECTOR,PORT2_VECTOR,TIMER2_A0_VECTOR, \
		       TIMER1_A1_VECTOR,TIMER1_A0_VECTOR,DMA_VECTOR,USCI_A2_VECTOR,					 \
			   ADC10_VECTOR,USCI_B0_VECTOR,WDT_VECTOR//,UNMI_VECTOR//,SYSNMI_VECTOR,//RESET_VECTOR
__interrupt void ISR_trap(void)
{
//	one_second_ticker();
	/*SYSRSTIV=0;
	ADC10IV=0;
	P2IV=0;
	SYSUNIV=0;
	SYSSNIV=0;
	SYSRSTIV=0;
	//TA0IV=0;
	TA1IV=0;
	TA3IV=0;
	UCB0IV=0;
	UCA2IV=0;
	DMAIV=0;
	LCDCIV=0;
	SYSBERRIV=0;*/

  // the following will cause an access violation which results in a PUC reset
 // WDTCTL = 0;
	/*int i;

	P8DIR |= BIT4 | BIT6;
	P8OUT |= BIT4;
	P8OUT |= BIT6;
	 for(i=0;i<20;i++)
	 {

	  __delay_cycles(1000000);
	  P8OUT ^= BIT4;
	  P8OUT ^= BIT6;
	 }*/
	// WDTCTL = 0;
//   PMMCTL0_H = PMMPW_H;
//   PMMCTL0|=PMMSWBOR; //PMMSWPOR;//
//   PMMCTL0_H = 0;
	// Reset_System();
}

