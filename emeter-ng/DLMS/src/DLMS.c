
#include <stdint.h>
#include <isr_compat.h>
#include "iec62056_46_link.h"
#include "iec62056_46_user.h"
#include "uart_comms.h"
#include "msgs.h"
#include "iec62056_uart.h"
#include "PLC_UART.h"
#include "DLMS.h"


iec62056_46_link_t link[MAX_USCI_PORTS];
async_hdlc_rx_t rx[MAX_USCI_PORTS];
async_hdlc_tx_t *txptr[MAX_USCI_PORTS];
async_hdlc_tx_t tx[MAX_USCI_PORTS];
uint16_t rx_data_to_process[MAX_USCI_PORTS];

volatile uint16_t Chunk_Size = CHUNK_SIZE;

uint8_t Frame_Received[MAX_USCI_PORTS];
extern void process_rx_frame(iec62056_46_link_t *, async_hdlc_rx_t *);

int delay_count = 0;

uint32_t timer_epoch = 0;
uint16_t last_timeout = 0;
uint32_t next_octet_timeout[MAX_USCI_PORTS]={0},next_inactivity_timeout[MAX_USCI_PORTS]={0};



extern uint8_t Data_Buffer[];

void send_frame(iec62056_46_link_t *link, async_hdlc_tx_t *tx)
{
  
#ifdef DLMS_IEC_21 
  if(link->port == IEC_PORT)
  {
     IEC_resetTXBuffer();
     txptr[IEC_PORT] = tx;

     while(txptr[IEC_PORT]->msg_ptr < txptr[IEC_PORT]->msg_len)
     {
       IEC_AddtoTXBuffer((txptr[IEC_PORT]->msg[txptr[IEC_PORT]->msg_ptr++]));
       //queue_enqueue(iec_tx_buff,&(txptr[IEC_PORT]->msg[txptr[IEC_PORT]->msg_ptr++]));
     }
     IEC_Start_SendBuffer();
     txptr[IEC_PORT]= NULL;  
  }

#endif 
  

#ifdef DLMS
  
  if(link->port == PLC_PORT)
  {

     txptr[PLC_PORT] = tx;
     
  #ifdef HLP 
     
     PLC_resetTXBuffer();
     while(txptr[PLC_PORT]->msg_ptr < txptr[PLC_PORT]->msg_len)
     {
        AddtoTXBuffer(txptr[PLC_PORT]->msg[txptr[PLC_PORT]->msg_ptr++]);
     }
     
     PLC_Start_SendBuffer();
     txptr[PLC_PORT]= NULL;
     
  #else     
     uint8_t *message = txptr[PLC_PORT]->msg+3;
     
     DataTransferRequest(message, txptr[PLC_PORT]->msg_len-3);//txptr[PLC_PORT]->msg, txptr[PLC_PORT]->msg_len-3);    
     
  #endif
     
  }
#endif   
   
}

void init_dlms()
{
      /* Set up timer A to use as a timeout interrupt function */
    /* TACCR0 determines the next interrupt, clocked from ACLK */
    TA0R = 0;
    TA0CCR0 = 0;
    TA0CCTL0 = CCIE;
    TA0CTL = TACLR | MC_2 | TASSEL_1;
    
//    iec62056_46_link_init(&link[PREFERRED_PORT], PREFERRED_PORT,0x4100);
//    hdlc_async_rx_init(&rx[PREFERRED_PORT]);
//    Frame_Received[PREFERRED_PORT]=0;
    
    
#ifdef DLMS_IEC_21    
    iec62056_46_link_init(&link[IEC_PORT], IEC_PORT,0x4001);//was 4100
    hdlc_async_rx_init(&rx[IEC_PORT]);
     Frame_Received[IEC_PORT]=0;
#endif 

#if  defined(DLMS)
    iec62056_46_link_init(&link[PLC_PORT], PLC_PORT,0x4100);
    hdlc_async_rx_init(&rx[PLC_PORT]);
    Frame_Received[PLC_PORT]=0;
#endif   
    
}



uint8_t  process_dlms_frame()
{
  
#ifdef DLMS_IEC_21
   if(Frame_Received[IEC_PORT]==1)
   {
           rx[IEC_PORT].state=0;
           Frame_Received[IEC_PORT]=0;
           
//           //test
//           if(rx[IEC_PORT].control_field != 0x93){
//             rx[IEC_PORT].msg[59] = 0;
//             rx[IEC_PORT].msg[60] = 0;
//             rx[IEC_PORT].control_field = 0;
//           }
//           //delete this 
           
           process_rx_frame(&link[IEC_PORT], &rx[IEC_PORT]);
           return 1;
   }
#endif 
  
   
#ifdef DLMS
   if(Frame_Received[PLC_PORT]==1)
   {
           rx[PLC_PORT].state=0;
           Frame_Received[PLC_PORT]=0;
           process_rx_frame(&link[PLC_PORT], &rx[PLC_PORT]);
           return 1;
   }   
#endif   
  
  
//   if(Frame_Received[PREFERRED_PORT]==1)
//   {
//           rx[PREFERRED_PORT].state=0;
//           Frame_Received[PREFERRED_PORT]=0;
//           process_rx_frame(&link[PREFERRED_PORT], &rx[PREFERRED_PORT]);
//           return 1;
//   }
   return 0;
}


int dl_connect_indication(iec62056_46_link_t *linkp)
{
  
#ifdef DLMS_IEC_21
     if(linkp->port == IEC_PORT)
     {
#if defined(DLMS) 
       if(link[PLC_PORT].configured == TRUE)
       {
         link[PLC_PORT].disconnected = TRUE;
         link[PLC_PORT].configured = FALSE;
         return DL_CONNECT_RESPONSE_NOK;
       }
#endif     
       return DL_CONNECT_RESPONSE_OK;  
     }
     else 
#endif 
                 
       
#ifdef DLMS
      if(linkp->port == PLC_PORT)
      {
#ifdef DLMS_IEC_21
       if(link[IEC_PORT].configured == TRUE)
       {
         link[IEC_PORT].disconnected = TRUE;
         link[IEC_PORT].configured = FALSE;
         return DL_CONNECT_RESPONSE_NOK;
       }   
#endif 
       return DL_CONNECT_RESPONSE_OK;  
      }     
#endif 
     
#ifdef DLMS_IEC_21
     {}
#endif      
  
  
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
//        }
//      return DL_CONNECT_RESPONSE_OK;  
//    } 
   return DL_CONNECT_RESPONSE_OK;
}

void dl_connect_confirm(iec62056_46_link_t *link, int response)
{
}

int dl_disconnect_indication(iec62056_46_link_t *link, int reason)
{
    return DL_DISCONNECT_RESPONSE_OK;
}

void idle_timeout_callback(void *user_data)
{
    iec62056_46_link_t *link;

    link = (iec62056_46_link_t *) user_data;
    iec62056_46_link_idle_timeout(link);
}

void iec62056_46_link_idle_restart(iec62056_46_link_t *link)
{
    //schedule_timeout(link, TIMEOUT_2_INACTIVITY);
    //if (xyzzy >= 0)
    //    schedule_del(&sched, xyzzy);
    //xyzzy = schedule_event(&sched, TIMEOUT_2_INACTIVITY*1000, idle_timeout_callback, link);
    //TODO: schedule a TIMEOUT_2_INACTIVITY seconds timeout, to call idle_timeout_callback
}

uint32_t current_time(void)
{
    uint16_t snapshot;

    snapshot = TA0R;
    if (snapshot < last_timeout)
        timer_epoch += 0x10000L;
    last_timeout = snapshot;
    return timer_epoch + snapshot;
}
extern int local_comm_exchange_mode_flag;
ISR(TIMER0_A0, TIMER0_A0_isr)
{
    uint32_t now;
    uint16_t i;

    now = current_time();
    TA0CCR0 += 0xc00;
    for (i = 0;  i < MAX_USCI_PORTS;  i++)
    {
      if (next_inactivity_timeout[i]  &&  (now >= next_inactivity_timeout[i]))
      {
          /* Wake up the main code, and let is service anything which has timed out. */
  //        P1OUT ^= BIT1;
          next_inactivity_timeout[i] = 0;
          //iec62056_46_link_timeout(&link[1]);
           if(i==1 && local_comm_exchange_mode_flag==0)
          {
            configure_uart_port(1,0);
          }
          iec62056_46_link_idle_timeout(&link[i]);
      }   
    }
}
