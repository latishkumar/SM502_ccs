//#include <stdint.h>
//#include <isr_compat.h>
//#include "iec62056_46_link.h"
//#include "iec62056_46_user.h"
//#include "uart_comms.h"
//
//#define NULL ((void *) 0)
//
//iec62056_46_link_t link[MAX_USCI_PORTS];
//async_hdlc_rx_t rx[MAX_USCI_PORTS];
//async_hdlc_tx_t *txptr[MAX_USCI_PORTS];
//async_hdlc_tx_t tx[MAX_USCI_PORTS];
//uint16_t rx_data_to_process[MAX_USCI_PORTS];
//uint16_t Chunk_Size=CHUNK_SIZE;
//
//#define PREFERRED_PORT 1
//
//int delay_count = 0;
//
//uint32_t timer_epoch = 0;
//uint16_t last_timeout = 0;
//uint32_t next_octet_timeout[MAX_USCI_PORTS]=0,
//         next_inactivity_timeout[MAX_USCI_PORTS]=0;
//
//
//void send_frame(iec62056_46_link_t *link, async_hdlc_tx_t *tx)
//{
//    txptr[1] = tx; //txptr[link->port] = tx;
//    //UCA1IE |= UCTXIE;
//    
//    UCA1TXBUF = txptr[1]->msg[txptr[1]->msg_ptr++];   
//    if (txptr[1]->msg_ptr >= txptr[1]->msg_len)
//        {
//             UCA1IE &= ~UCTXIE;
//             txptr[1] = NULL;
//        }  
//}
//
////int schedule_timeout(iec62056_46_link_t *link, int32_t timeout)
////{
////    next_timeout = timer_epoch + TA0R + 32L*timeout;
////    return 0;
////}
//
////void cancel_timeout(iec62056_46_link_t *link, int id)
////{
////    next_timeout = 0;
////}
//extern uint8_t Data_Buffer[];
///*extern uint8_t challenge_s2c[16];
//extern uint8_t aes_key;*/
//#include "msgs.h"
//uint8_t Frame_Received[MAX_USCI_PORTS];
//extern void process_rx_frame(iec62056_46_link_t *, async_hdlc_rx_t *);
////int main_old(void)
////{
////    int i;
////
////    // Stop watchdog timer to prevent time out reset
////    WDTCTL = WDTPW + WDTHOLD;
////
////#if defined(ARCH_5XX)  ||  defined(ARCH_6XX) 
////    __bis_SR_register(SCG0);                        /* Disable the FLL control loop */
////    UCSCTL6 = (UCSCTL6 | XT1DRIVE_3);               /* Highest drive setting for XT1 startup */
////    while (SFRIFG1 & OFIFG)
////    {
////        /* Check OFIFG fault flag */
////        UCSCTL7 &= ~(DCOFFG | XT1LFOFFG | XT2OFFG); /* Clear OSC fault flags */
////        SFRIFG1 &= ~OFIFG;                          /* Clear OFIFG fault flag */
////    }
////    UCSCTL6 = (UCSCTL6 & ~(XT1DRIVE_3));            /* Reduce drive to something weaker */
////
////    UCSCTL1 = DCORSEL_5;                            /* Set RSELx for DCO = 16 MHz */
////    UCSCTL2 = FLLD_1 + 255;                         /* Set DCO Multiplier for 8MHz */
////                                                    /* (N + 1) * FLLRef = Fdco */
////                                                    /* (63 + 1) * 32768 = 2MHz */
////                                                    /* Set FLL Div = fDCOCLK/2 */
////    __bic_SR_register(SCG0);                        /* Enable the FLL control loop */
////
////    UCSCTL5 |= DIVS__1;
////    UCSCTL4 = SELM__DCOCLK | SELS__DCOCLK | SELA__XT1CLK;     //16MHz MCLK, 16MHz SMCLK, 32KHz ACLK
////
////#else
////    FLL_CTL0 |= OSCCAP_3;               /* Configure load caps */
////
////    {
////        long int x;
////        for (x = 0;  x < 100000L;  x++)
////            _NOP();
////    }
////    
////    /* Speed up the clock to 8.388608MHz */
////    SCFI0 = FN_3 | FLLD_4;
////    SCFQCTL = 128 - 1;
////    FLL_CTL0 |= DCOPLUS;
////#endif
////
////    /* Set up timer A to use as a timeout interrupt function */
////    /* TACCR0 determines the next interrupt, clocked from ACLK */
////    TA0R = 0;
////    TA0CCR0 = 0;
////    TA0CCTL0 = CCIE;
////    TA0CTL = TACLR | MC_2 | TASSEL_1;
////
////  //  aes_encrypt(challenge_s2c,aes_key);
////  //  Capture_Billing_Profile_Data(Data_Buffer,ATTR_READ);
////    /* Configure all available USCI UART ports as DLMS ports */
////    for (i = 0;  i < MAX_USCI_PORTS;  i++)
////    {
////        configure_uart_port(i, 1);
////
////        iec62056_46_link_init(&link[i], i,0x4100);
//////      iec62056_46_set_far_msap(&link[i], 0x2300);/*don*/
////
////        //iec62056_46_link_init(&link[i], i, 0x100);
////        hdlc_async_rx_init(&rx[i]);
////        //rx_data_to_process[i]=0;
////        Frame_Received[i]=0;
////    }
////
////    _EINT();
////   // P1DIR |= (BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
////    for (;;)
////    {
////
////     // As we need to priortize Meterology Interrupts, we are doing this processing in the main routine.
////      for(i=0;i < MAX_USCI_PORTS;  i++)
////       {
////        if(Frame_Received[i]==1)
////         {
////           rx[i].state=0;
////           Frame_Received[i]=0;
////           process_rx_frame(&link[i], &rx[i]);
////#if defined(__MSP430F47196__)  ||  defined(__MSP430F47197__)  ||  defined(__MSP430F4794__)  ||  defined(__MSP430FG4619__) 
////           if(i==0)
////           {   if (txptr[0]  &&  (UC0IE & UCA0TXIE) == 0)
////             UC0IE |= UCA0TXIE; }
////           else
////           {   if (txptr[1]  &&  (UC1IE & UCA1TXIE) == 0)
////             UC1IE |= UCA1TXIE; }
////#elif defined(ARCH_5XX)  ||  defined(ARCH_6XX)
////           switch(i)
////           {
////           case 0:
////                     if (txptr[0]  &&  (UCA0IE & UCTXIE) == 0)
////            UCA0IE |= UCTXIE;
////            break;
////           case 1:
////                     if (txptr[1]  &&  (UCA1IE & UCTXIE) == 0)
////            UCA1IE |= UCTXIE;
////            break;
////           case 2:
////                     if (txptr[2]  &&  (UCA2IE & UCTXIE) == 0)
////            UCA2IE |= UCTXIE;
////            break;
////#if MAX_USCI_PORTS > 3
////           case 3:
////                     if (txptr[3]  &&  (UCA3IE & UCTXIE) == 0)
////            UCA3IE |= UCTXIE;
////            break;
////#endif            
////           default: break;
////           }
////#endif            
////           
////         }
////       }  
////     
////
////    }
////}
//#define FALSE 0
//#define TRUE (!FALSE)
//
//int dl_connect_indication(iec62056_46_link_t *linkp)
//{
//  uint16_t i;
//   if(linkp->port==PREFERRED_PORT)
//    { 
//      for(i=0;i<MAX_USCI_PORTS;i++)
//       { 
//         if(i==linkp->port)
//            continue;
//         else
//            if(link[i].configured==TRUE)
//            {
//               // send_dm(&link[i]);
//                link[i].disconnected = TRUE;
//                link[i].configured = FALSE;
//              //  link[i].far_msap = 0;
//            }
//       }
//      return DL_CONNECT_RESPONSE_OK;
//    }
//    else
//    {
//       for(i=0;i<MAX_USCI_PORTS;i++)
//       { 
//         if(linkp->port==i)
//            continue;
//         else
//            if(link[i].configured==TRUE)
//            {
//                linkp->disconnected = TRUE;
//                linkp->configured = FALSE;
//               // linkp->far_msap = 0;
//              return DL_CONNECT_RESPONSE_NOK;  
//            }
//      }
//      return DL_CONNECT_RESPONSE_OK;  
//    } 
//   
//}
//
//void dl_connect_confirm(iec62056_46_link_t *link, int response)
//{
//}
//
//int dl_disconnect_indication(iec62056_46_link_t *link, int reason)
//{
//    return DL_DISCONNECT_RESPONSE_OK;
//}
//
//void idle_timeout_callback(void *user_data)
//{
//    iec62056_46_link_t *link;
//
//    link = (iec62056_46_link_t *) user_data;
//    iec62056_46_link_idle_timeout(link);
//}
//
//void iec62056_46_link_idle_restart(iec62056_46_link_t *link)
//{
//    //schedule_timeout(link, TIMEOUT_2_INACTIVITY);
//    //if (xyzzy >= 0)
//    //    schedule_del(&sched, xyzzy);
//    //xyzzy = schedule_event(&sched, TIMEOUT_2_INACTIVITY*1000, idle_timeout_callback, link);
//    //TODO: schedule a TIMEOUT_2_INACTIVITY seconds timeout, to call idle_timeout_callback
//}
//
//uint32_t current_time(void)
//{
//    uint16_t snapshot;
//
//    snapshot = TA0R;
//    if (snapshot < last_timeout)
//        timer_epoch += 0x10000L;
//    last_timeout = snapshot;
//    return timer_epoch + snapshot;
//}
//
////ISR(TIMER0_A0, TIMER0_A0_isr)
////{
////    uint32_t now;
////    uint16_t i;
////
////    now = current_time();
////    TA0CCR0 += 0xc00;
////    for (i = 0;  i < MAX_USCI_PORTS;  i++){
////    if (next_inactivity_timeout[i]  &&  (now >= next_inactivity_timeout[i]))
////    {
////        /* Wake up the main code, and let is service anything which has timed out. */
////        P1OUT ^= BIT1;
////        next_inactivity_timeout[i] = 0;
////        //iec62056_46_link_timeout(&link[1]);
////        
////        iec62056_46_link_idle_timeout(&link[i]);
////    }   
////    }
////}
//
//
