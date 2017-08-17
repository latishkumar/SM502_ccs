/*
 * unused_interrupts_handler.c
 *
 *  Created on: Jan 13, 2017
 *      Author: eliab
 */

#include <msp430.h>
#include <stdint.h>
#include "EventTypes.h"
#include "Status.h"
extern METER_STATES meter_state;
void log_standard_events(uint8_t event_type);

void RESET_ISR(void)
{
	 switch (__even_in_range(SYSRSTIV, SYSRSTIV_PMMKEY))
	    {
	        case SYSRSTIV_NONE:                       // No Interrupt pending
	                                __no_operation();
	                                break;
	        case SYSRSTIV_BOR:                        // SYSRSTIV : BOR
	                                if(meter_state == RESTARTING){
	                                log_standard_events(SYSRSTIV_BOR_RESET_EVENT);
	                                }
	                                else
                                    log_standard_events(SYSRSTIV_BOR_RESET_EVENT_SW);
	                                break;
	        case SYSRSTIV_RSTNMI:                     // SYSRSTIV : RST/NMI
	                                log_standard_events(SYSRSTIV_RSTNMI_RESET_EVENT);
	                                break;
	        case SYSRSTIV_DOBOR:                      // SYSRSTIV : Do BOR
	                                log_standard_events(SYSRSTIV_DOBOR_RESET_EVENT);
	                                break;
	        case SYSRSTIV_LPM5WU:          // SYSRSTIV : Port LPM5 Wake Up
	                                log_standard_events(SYSRSTIV_LPM5WU_RESET_EVENT);
	                                break;
	        case SYSRSTIV_SECYV:                      // SYSRSTIV : Security violation
	                                log_standard_events(SYSRSTIV_SECYV_RESET_EVENT);
	                                break;
	        case SYSRSTIV_SVSL:                       // SYSRSTIV : SVSL
	                                log_standard_events(SYSRSTIV_SVSL_RESET_EVENT);
	                                break;
	        case SYSRSTIV_SVSH:                       // SYSRSTIV : SVSH
	                                log_standard_events(SYSRSTIV_SVSH_RESET_EVENT);
	                                break;
	        case SYSRSTIV_SVML_OVP:                   // SYSRSTIV : SVML_OVP
	                                log_standard_events(SYSRSTIV_SVML_OVP_RESET_EVENT);
	                                break;
	        case SYSRSTIV_SVMH_OVP:                   // SYSRSTIV : SVMH_OVP
	                                log_standard_events(SYSRSTIV_SVMH_OVP_RESET_EVENT);
	                                break;
	        case SYSRSTIV_DOPOR:                      // SYSRSTIV : Do POR
	                                log_standard_events(SYSRSTIV_DOPOR_RESET_EVENT);
	                                break;
	        case SYSRSTIV_WDTTO:                      // SYSRSTIV : WDT Time out
	                                log_standard_events(SYSRSTIV_WDTTO_RESET_EVENT);
	                                break;
	        case SYSRSTIV_WDTKEY:                     // SYSRSTIV : WDTKEY violation
	                                log_standard_events(SYSRSTIV_WDTKEY_RESET_EVENT);
	                                break;
	        case SYSRSTIV_KEYV:                       // SYSRSTIV : Flash Key violation
	                                log_standard_events(SYSRSTIV_KEYV_RESET_EVENT);
	                                break;
	        case SYSRSTIV_FLLUL:                      // SYSRSTIV : FLL unlock
	                                log_standard_events(SYSRSTIV_FLLUL_RESET_EVENT);
	                                break;
	        case SYSRSTIV_PERF:                       // SYSRSTIV : peripheral/config area fetch
	                                log_standard_events(SYSRSTIV_PERF_RESET_EVENT);
	                                break;
	        case SYSRSTIV_PMMKEY:                     // SYSRSTIV : PMMKEY violation
	                                log_standard_events(SYSRSTIV_PMMKEY_RESET_EVENT);
	                                break;
	        default: break;
	    }
}

#pragma vector=LCD_C_VECTOR,TIMER3_A1_VECTOR,TIMER3_A0_VECTOR,PORT2_VECTOR,TIMER2_A0_VECTOR, \
		       TIMER1_A1_VECTOR,TIMER1_A0_VECTOR,DMA_VECTOR,USCI_A2_VECTOR,					 \
			   ADC10_VECTOR,USCI_B0_VECTOR,WDT_VECTOR,UNMI_VECTOR,SYSNMI_VECTOR,RESET_VECTOR
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

