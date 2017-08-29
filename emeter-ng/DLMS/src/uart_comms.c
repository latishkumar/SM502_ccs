#include <stdint.h>
#include <isr_compat.h>
#include "iec62056_46_link.h"
#include "iec62056_46_user.h"
#include "uart_comms.h"
#include "iec62056_uart.h"
#define NULL ((void *) 0)

extern iec62056_46_link_t link[MAX_USCI_PORTS];
extern async_hdlc_rx_t rx[MAX_USCI_PORTS];
extern async_hdlc_tx_t *txptr[MAX_USCI_PORTS];
extern async_hdlc_tx_t tx[MAX_USCI_PORTS];

extern uint32_t next_octet_timeout[MAX_USCI_PORTS],next_inactivity_timeout[MAX_USCI_PORTS];

extern void push_rx_data(uint8_t datachar, int port);

//#if defined(__MSP430_HAS_UART0__)  ||  defined(__MSP430_HAS_EUSCI_A0__)
///* Dither/modulation patterns to get close to an exact bit rate */
//static const uint8_t mod_patterns[] =
//{
//    0x00, 0x01, 0x11, 0x51, 0x55, 0x5D, 0xDD, 0xDF
//};
//#endif

void configure_uart_port(int port, int mode)
{
  switch(mode)
  {
  case 0: //7E1@300
    __delay_cycles(10000);
    change_uart_a1_to_7e1_x_bd(0);
    break;
  case 1://7E1@9600
    UCA1CTLW0 |= UCSWRST;          
    change_boud_rate(optical_comm_speed);
    break;
  case 2://8N1@9600
   change_uart_a1_to_8n1_x_bd(optical_comm_speed);//19200
   break;
  case 3://8N1@19200
    change_uart_a1_to_8n1_x_bd(usb_comm_speed);//57600
    UCA1IE &= ~UCTXIE;//|UCRXIE;
    break;
  default:
    break;
  }
}
void change_uart_a1_to_8n1_x_bd(int x)
{
  //UCA1IE=0;
  UCA1CTLW0 |= UCSWRST; //reset for configuration 
  UCA1CTLW0 &= ~UCPEN; //Disable Parity
  //UCA0CTLW0 &=~UCPAR;//Odd Parity default
  UCA1CTLW0 &= ~UCMSB;//LSB first  default
  UCA1CTLW0 &= ~UC7BIT;//8 -bit    default
  UCA1CTLW0 &= ~UCSPB;// one stop bit default
  UCA1CTLW0 |= UCMODE_0;//UART Mode default
  UCA1CTLW0 &= ~UCSYNC;//Asynchronous mode default
  UCA1CTL1  |= UCSSEL_3;//SMCLK is the clock source
  UCA1CTLW1 |= UCGLIT_1;//50nsec glitch time
  change_boud_rate(x); //x bits
  
}

void change_uart_a1_to_7e1_x_bd(int x)
{
  //UCA1IE=0;
  UCA1CTLW0 |= UCSWRST; //reset for configuration 
  UCA1CTLW0 |= UCPEN;   //Enable Parity  
  UCA1CTLW0 |= UCPAR;   //even parity
  UCA1CTLW0 &=~UCMSB;   //LSB first  default
  UCA1CTLW0 |= UC7BIT;  //7 - bit    default 
  UCA1CTLW0 &=~UCSPB;   //one stop bit default
  UCA1CTLW0 |=UCMODE_0; //UART Mode default
  UCA1CTLW0 &=~UCSYNC;  //Asynchronous mode default
  UCA1CTL1 |=UCSSEL_3;  //SMCLK is the clock source

  UCA1CTLW1 |= UCGLIT_1;//50nsec glitch time
  change_boud_rate(x); //x db
}
