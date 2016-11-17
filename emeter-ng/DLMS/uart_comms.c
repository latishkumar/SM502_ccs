#include <stdint.h>

#include <isr_compat.h>

#include "core/iec62056_46_link.h" /*PATH CORRECTION*/
#include "core/iec62056_46_user.h" /*PATH CORRECTION*/
#include "uart_comms.h"

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
void change_uart_a1_to_7e1_x_bd(int x);
void change_uart_a1_to_8n1_x_bd(int x);
void change_boud_rate(uint8_t x);
int switch_ready =0;
extern int local_comm_exchange_mode_flag;
void configure_uart_port(int port, int mode)
{
  switch(mode)
  {
  case 0: //7E1@300
   // __delay_cycles(10000);
   // change_uart_a1_to_7e1_x_bd(0);
  if(local_comm_exchange_mode_flag==0)
    {
      while(!switch_ready);
      change_uart_a1_to_7e1_x_bd(0);
    }
    break;
  case 1://7E1@9600
    UCA1CTLW0 |= UCSWRST;          
    change_boud_rate(5);           
    break;
  case 2://8N1@9600
   change_uart_a1_to_8n1_x_bd(5);
   break;
  case 3://8N1@19200
    change_uart_a1_to_8n1_x_bd(6);//19200
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
#include "../SM502/IEC62056/IEC62056_UART.H"
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
   
}//void configure_uart_port_old(int port, int mode)
//{
//    uint32_t bit_rate=9600;
//    int32_t bitrate_divider;
//    uint16_t ctl0;
//    uint16_t ctl1;
//#if !defined(__MSP430_HAS_EUSCI_A0__)
//    /* If the device has EUSCI ports the bit rate control register can be accessed as
//       a single 16 bit operation. The other serial modules need to be accessed with two
//       8 bit operations. */
//    uint8_t br0;
//    uint8_t br1;
//#endif
//    uint8_t mctl;
//
//    /* Set some defaults */
//    //serial_configure_timeouts(port, 1, 1);
//
//    /* We only need two modes for the serial characters in the work we have done to date.
//       Just support those for the time being */
//    if (mode == 0)
//    {
//        /* 7-bit character, even parity */
//#if defined(__MSP430_HAS_UART0__)
//        ctl0 = PENA | PEV | SWRST;
//#else
//        ctl0 = UC7BIT | UCPEN;
//#endif
//    }
//    else
//    {
//        /* 8-bit character, no parity */
//#if defined(__MSP430_HAS_UART0__)
//        ctl0 = CHAR | SWRST;
//#else
//        ctl0 = 0;
//#endif
//    }
//    /* Use ACLK for slow bit rates. Use SMCLK for higher bit rates */
//    if (bit_rate <= 4800L)
//    {
//        bitrate_divider = (32768L*16L)/bit_rate;
//#if defined(__MSP430_HAS_UART0__)
//        ctl1 = SSEL0;               /* ACLK */
//#else
//        ctl1 = UCSSEL_1 | UCSWRST;  /* ACLK */
//#endif
//    }
//    else
//    {
//#if defined(__MSP430F47197__)  ||  defined(__MSP430F6736__)  ||  defined(__MSP430F6779__)  ||  defined(__MSP430F67791__)
//        /* 16MHz clock */
//        bitrate_divider = (32768L*512L*16L)/bit_rate;
//#elif defined(__MSP430AFE251__)  ||  defined(__MSP430AFE252__)  ||  defined(__MSP430AFE253__)
//        /* Exactly 8MHz clock */
//        bitrate_divider = (8000000L*16L)/bit_rate;
//#else
//        /* 8MHz clock */
//        bitrate_divider = (32768L*256L*16L)/bit_rate;
//#endif
//#if defined(__MSP430_HAS_UART0__)
//        ctl1 = SSEL1;               /* SMCLK */
//#else
//        ctl1 = UCSSEL_2 | UCSWRST;  /* SMCLK */
//#endif
//    }
//    bitrate_divider++;
//#if defined(__MSP430_HAS_EUSCI_A0__)  ||  defined(__MSP430_HAS_UART0__)
//    mctl = (bitrate_divider & 0x0E) >> 1;
//    mctl = mod_patterns[mctl];
//#else
//    mctl = bitrate_divider & 0x0E;
//#endif
//    bitrate_divider >>= 4;
//#if !defined(__MSP430_HAS_EUSCI_A0__)
//    br1 = bitrate_divider >> 8;
//    br0 = bitrate_divider;
//#endif
//
//    switch (port)
//    {
//#if defined(__MSP430_HAS_USCI_AB0__)  ||  defined(__MSP430_HAS_USCI_A0__)  ||  defined(__MSP430_HAS_EUSCI_A0__)
//    case 0:
//        /* Enable the USCI1 UART pins */
//        /* Enable the USCI0 UART pins */
//      #if defined(__MSP430F47196__)  ||  defined(__MSP430F47197__)  ||  defined(__MSP430F4793__)  ||  defined(__MSP430F4794__)  ||  defined(__MSP430FG4619__)
//              P2SEL |= (BIT4 | BIT5);
//              P2DIR |= (BIT4);
//      #elif defined(ARCH_67SP)
//              P1SEL |= (BIT2 | BIT3);
//              P1DIR |= BIT3;
//      #elif defined(ARCH_67TP)
//              P3SEL0 |= (BIT0 | BIT0);
//              P3DIR |= BIT1;
//      #else
//              P3SEL |= (BIT5 | BIT4);
//              P3DIR |= (BIT4);
//      #endif
//
//        /* Configure the port with the reset bit held high */
//        UCA0CTL1 |= UCSWRST;
//        UCA0CTL0 = ctl0;
//        UCA0CTL1 = ctl1;
//        #if defined(__MSP430_HAS_EUSCI_A0__)
//        UCA0BRW = bitrate_divider;
//        UCA0MCTLW = ((uint16_t) mctl << 8);
//        #else
//        UCA0BR1 = br1;
//        UCA0BR0 = br0;
//        UCA0MCTL = mctl;
//        UCA0STAT = 0;
//        #endif
//        UCA0TXBUF = 0;
//        UCA0CTL1 &= ~UCSWRST;
//        #if defined(UART_0_DMA_SUPPORT)
//        //DMACTL0 = DMA1TSEL_3 | DMA2TSEL_4;
//        #endif
//        #if defined(__MSP430_HAS_USCI_AB0__)
//        UC0IE |= UCA0RXIE;
//        #else
//        UCA0IE |= UCRXIE;
//        #endif
//    #elif defined(__MSP430_HAS_UART0__)
//        UCTL0 = ctl0;
//        UTCTL0 = ctl1;
//        UBR10 = br1;
//        UBR00 = br0;
//        UMCTL0 = mctl;
//        UCTL0 &= ~SWRST;
//        /* Enable USART0 TXD/RXD */
//        U0ME |= (UTXE0 | URXE0);
//        U0IE |= URXIE0;
//        /* If you do not initially kick the Tx port the TXEPT bit is not set. */
//        TXBUF0 = 0;
//    #endif
//
//#if defined(__MSP430_HAS_USCI_AB1__)  ||  defined(__MSP430_HAS_USCI_A1__)  ||  defined(__MSP430_HAS_EUSCI_A1__)        
//    case 1:
//              /* Enable the USCI1 UART pins */
//        #if defined(__MSP430FG4619__)  ||  defined(__MSP430F4793__)  ||  defined(__MSP430F4794__)
//                P4SEL |= (BIT1 | BIT0);
//                P4DIR |= (BIT0);
//        #elif defined(__MSP430F47196__)  ||  defined(__MSP430F47197__)
//                P1SEL |= (BIT7 | BIT6);
//                P1DIR |= (BIT6);
//        #elif defined(ARCH_67SP)
//                P1SEL |= (BIT4 | BIT5);
//                P1DIR |= (BIT5);
//        #elif defined(ARCH_67TP)
//                P3SEL0 |= (BIT4 | BIT5);
//                P3DIR |= (BIT5);
//        #else
//                P5SEL |= (BIT7 | BIT6);
//                P5DIR |= (BIT6);
//        #endif        
//
//        /* Configure the port with the reset bit held high */
//        UCA1CTL1 |= UCSWRST;
//        UCA1CTL0 = ctl0;
//        UCA1CTL1 = ctl1;
//        #if defined(__MSP430_HAS_EUSCI_A1__)
//        UCA1BRW = bitrate_divider;
//        UCA1MCTLW = ((uint16_t) mctl << 8);
//        #else
//        UCA1BR1 = br1;
//        UCA1BR0 = br0;
//        UCA1MCTL = mctl;
//        UCA1STAT = 0;
//        #endif
//        UCA1TXBUF = 0;
//        UCA1CTL1 &= ~UCSWRST;
//        #if defined(UART_1_DMA_SUPPORT)
//        DMACTL0 = DMA1TSEL_9 | DMA2TSEL_10;
//        #endif
//        #if defined(__MSP430_HAS_USCI_AB1__)
//        UC1IE |= UCA1RXIE;
//        #else
//        UCA1IE |= UCRXIE;
//        #endif
//    #elif defined(__MSP430_HAS_UART1__)
//        UCTL1 = ctl0;
//        UTCTL1 = ctl1;
//        UBR11 = br1;
//        UBR01 = br0;
//        UMCTL1 = mctl;
//        UCTL1 &= ~SWRST;
//        /* Enable USART1 TXD/RXD */
//        ME2 |= (UTXE1 | URXE1);
//        IE2 |= URXIE1;
//        /* If you do not initially kick the Tx port, the TXEPT bit is not set. */
//        TXBUF1 = 0;
//    #endif
//
//#if defined(__MSP430_HAS_USCI_AB2__)  ||  defined(__MSP430_HAS_USCI_A2__)  ||  defined(__MSP430_HAS_EUSCI_A2__)
//    case 2:
//      /* Enable the USCI2 UART pins */
//        #if defined(ARCH_67SP)
//                P2SEL |= (BIT2 | BIT3);
//                P2DIR |= (BIT3);
//        #elif defined(ARCH_67TP)
//                P3SEL0 |= (BIT6 | BIT7);
//                P3DIR |= (BIT7);
//        #else       
//                P9SEL |= (BIT5 | BIT4);
//                P9DIR |= (BIT4);
//        #endif        
//       /* Configure the port with the reset bit held high */
//        UCA2CTL1 |= UCSWRST;
//        UCA2CTL0 = ctl0;
//        UCA2CTL1 = ctl1;
//        #if defined(__MSP430_HAS_EUSCI_A2__)
//        UCA2BRW = bitrate_divider;
//        UCA2MCTLW = ((uint16_t) mctl << 8);
//        #else
//        UCA2BR1 = br1;
//        UCA2BR0 = br0;
//        UCA2MCTL = mctl;
//        UCA2STAT = 0;
//        #endif
//        UCA2TXBUF = 0;
//        UCA2CTL1 &= ~UCSWRST;
//        #if defined(__MSP430_HAS_USCI_AB2__)
//        UC2IE |= UCA2RXIE;
//        #else
//        UCA2IE |= UCRXIE;
//        #endif
//    #endif
//
//#if defined(__MSP430_HAS_USCI_AB3__)  ||  defined(__MSP430_HAS_USCI_A3__)  ||  defined(__MSP430_HAS_EUSCI_A3__)
//    case 3:
//              /* Enable the USCI3 UART pins */
//        #if defined(ARCH_67TP)
//                P4SEL0 |= (BIT1 | BIT2);
//                P4DIR |= (BIT2);
//        #else       
//              P10SEL |= (BIT5 | BIT4);
//              P10DIR |= (BIT4);
//        #endif      
//        /* Configure the port with the reset bit held high */
//        UCA3CTL1 |= UCSWRST;
//        UCA3CTL0 = ctl0;
//        UCA3CTL1 = ctl1;
//        #if defined(__MSP430_HAS_EUSCI_A3__)
//        UCA3BRW = bitrate_divider;
//        UCA3MCTLW = ((uint16_t) mctl << 8);
//        #else
//        UCA3BR1 = br1;
//        UCA3BR0 = br0;
//        UCA3MCTL = mctl;
//        UCA3STAT = 0;
//        #endif
//        UCA3TXBUF = 0;
//        UCA3CTL1 &= ~UCSWRST;
//        #if defined(__MSP430_HAS_USCI_AB3__)
//        UC3IE |= UCA3RXIE;
//        #else
//        UCA3IE |= UCRXIE;
//        #endif
//    
//      #endif
//    }
//}

/* We need a separate set of interrupt routines for the 4xx and 5xx devices. Although the body of the
   USCI module is functionally similar in the 4xx and 5xx devices, the interrupt scheme is quite different. */
#if defined(ARCH_5XX)  ||  defined(ARCH_6XX)
//#if MAX_USCI_PORTS >= 1 /* &&  defined(__MSP430_HAS_USCI_AB0__)*/
//ISR(USCI_A0, USCI_A0_isr)
//{
////    static uint32_t next_octet_timeout = 0;
//    uint32_t now;
//        now = current_time();
//    switch(__even_in_range(UCA0IV, 4))
//    {
//    case 0:                                 // Vector 0 - no interrupt
//        break;
//    case 2:                                 // Vector 2 - RXIFG
//
//        if (now >= next_octet_timeout[0])
//        {
//            iec62056_46_rx_timeout(&link[0], &rx[0]);
//        }
//        iec62056_46_rx_byte(&link[0], &rx[0], UCA0RXBUF);
//        next_octet_timeout[0] = now + ((32768L*TIMEOUT_3_INTRA_FRAME)/1000L);
////        if (txptr[0]  &&  (UCA0IE & UCTXIE) == 0)
////            UCA0IE |= UCTXIE;
//        break;
//    case 4:                                 // Vector 4 - TXIFG
//        if (txptr[0])
//        {
//            UCA0TXBUF = txptr[0]->msg[txptr[0]->msg_ptr++];
//            if (txptr[0]->msg_ptr >= txptr[0]->msg_len)
//            {
//                UCA0IE &= ~UCTXIE;
//                P1OUT ^= BIT0;
//                txptr[0] = NULL;
//            }
//        }
//        else
//        {
//            UCA0TXBUF = 0;
//        }
//        break;
//    default:
//        break;
//    }
//        next_inactivity_timeout[0] = now + ((32768L*TIMEOUT_2_INACTIVITY)/1000L);
//}
#endif

