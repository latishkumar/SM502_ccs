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
	                                SYSRSTIV &= ~SYSRSTIV_BOR;
	                                break;
	        case SYSRSTIV_RSTNMI:                     // SYSRSTIV : RST/NMI
	                                log_standard_events(SYSRSTIV_RSTNMI_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_RSTNMI;
	                                break;
	        case SYSRSTIV_DOBOR:                      // SYSRSTIV : Do BOR
	                                log_standard_events(SYSRSTIV_DOBOR_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_DOBOR;
	                                break;
	        case SYSRSTIV_LPM5WU:                     // SYSRSTIV : Port LPM5 Wake Up
	                                log_standard_events(SYSRSTIV_LPM5WU_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_LPM5WU;
	                                break;
	        case SYSRSTIV_SECYV:                      // SYSRSTIV : Security violation
	                                log_standard_events(SYSRSTIV_SECYV_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_SECYV;
	                                break;
	        case SYSRSTIV_SVSL:                       // SYSRSTIV : SVSL
	                                log_standard_events(SYSRSTIV_SVSL_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_SVSL;
	                                break;
	        case SYSRSTIV_SVSH:                       // SYSRSTIV : SVSH
	                                log_standard_events(SYSRSTIV_SVSH_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_SVSH;
	                                break;
	        case SYSRSTIV_SVML_OVP:                   // SYSRSTIV : SVML_OVP
	                                log_standard_events(SYSRSTIV_SVML_OVP_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_SVML_OVP;
	                                break;
	        case SYSRSTIV_SVMH_OVP:                   // SYSRSTIV : SVMH_OVP
	                                log_standard_events(SYSRSTIV_SVMH_OVP_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_SVMH_OVP;
	                                break;
	        case SYSRSTIV_DOPOR:                      // SYSRSTIV : Do POR
	                                log_standard_events(SYSRSTIV_DOPOR_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_DOPOR;
	                                break;
	        case SYSRSTIV_WDTTO:                      // SYSRSTIV : WDT Time out
	                                log_standard_events(SYSRSTIV_WDTTO_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_WDTTO;
	                                break;
	        case SYSRSTIV_WDTKEY:                     // SYSRSTIV : WDTKEY violation
	                                log_standard_events(SYSRSTIV_WDTKEY_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_WDTKEY;
	                                break;
	        case SYSRSTIV_KEYV:                       // SYSRSTIV : Flash Key violation
	                                log_standard_events(SYSRSTIV_KEYV_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_KEYV;
	                                break;
	        case SYSRSTIV_FLLUL:                      // SYSRSTIV : FLL unlock
	                                log_standard_events(SYSRSTIV_FLLUL_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_FLLUL;
	                                break;
	        case SYSRSTIV_PERF:                       // SYSRSTIV : peripheral/config area fetch
	                                log_standard_events(SYSRSTIV_PERF_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_PERF;
	                                break;
	        case SYSRSTIV_PMMKEY:                     // SYSRSTIV : PMMKEY violation
	                                log_standard_events(SYSRSTIV_PMMKEY_RESET_EVENT);
	                                SYSRSTIV &= ~SYSRSTIV_PMMKEY;
	                                break;
	        default: break;
	    }
}
void perform_flash_backup(uint8_t backup_type);
void log_standard_events(uint8_t event_code);
#pragma vector = UNMI_VECTOR,SYSNMI_VECTOR,WDT_VECTOR
__interrupt void UNMI_SYSNMI_ISR(void)
{
    //
    if(SFRIFG1 & WDTIFG) /* WDT Interrupt Flag */
    {
        log_standard_events(WDT_INTERRUPT_FLAG);
        return;
    }
    if(SFRIFG1 & OFIFG) /* Osc Fault Flag */
    {
        perform_flash_backup(0x08);
        log_standard_events(OSCILLATOR_FAULT_FLAG);
        SFRIFG1 &= ~OFIFG;
        PMMCTL0_H = 0xA5;
        PMMCTL0 |= PMMSWBOR;             //   generate BOR
        return;
    }
    if(SFRIFG1 & VMAIFG)//0x000A) /*Vacant memory access violation*/
    {
        log_standard_events(VACANT_MEMORY_ACCESS_VIOLATION);
        SFRIFG1 &= ~VMAIFG;
        return;
    }
    if(SFRIFG1 & NMIIFG) /* NMI Interrupt Flag */
    {
        log_standard_events(NMIPIN_INTERRUPT_FLAG);
        SFRIFG1 &= ~NMIIFG;
        return;
    }
    if(SYSUNIV & 0x0006) /* Flash Access violation flag */
    {
        log_standard_events(FLASH_ACCESS_VIOLATION);
        SYSUNIV &=~0x0006;
        return;
    }

}

#pragma vector=LCD_C_VECTOR,TIMER3_A1_VECTOR,TIMER3_A0_VECTOR,PORT2_VECTOR,TIMER2_A0_VECTOR, \
		       TIMER1_A1_VECTOR,TIMER1_A0_VECTOR,DMA_VECTOR,USCI_A2_VECTOR,					 \
			   ADC10_VECTOR,USCI_B0_VECTOR,RESET_VECTOR
__interrupt void ISR_trap(void)
{
    log_standard_events(OTHER_UNHANDLED_VIOLATION);
}

