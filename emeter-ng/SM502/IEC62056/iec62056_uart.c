/*
 * UART.c
 *
 *  Created on: Apr 28, 2013
 *      Author: Donayam
 */


#include "iec62056_uart.h"
#include "Status.h"
#include "Schaduler.h"
#include "Queue.h"
#include "iec62056_46_link.h"
#include "DLMS.h"
#include "uart_comms.h"
extern iec62056_46_link_t link[];
extern async_hdlc_rx_t rx[];
extern uint32_t next_octet_timeout[MAX_USCI_PORTS],next_inactivity_timeout[MAX_USCI_PORTS];

uint8_t IEC_RX_BUF[MaxIECRxBufferLength];

Queue *iec_tx_buff;
Queue *iec_rx_buff;

unsigned int volatile IEC_Tx_Buffer_Index  = 0;
volatile unsigned int IEC_Rx_Buffer_Index = 0;

unsigned int IEC_CurrentTxLength = 0;
unsigned int IEC_CurrentRxLength = 0;

volatile uint8_t IEC_Tx_Done = 1;
uint8_t IEC_Rx_Done = 1;
uint8_t Transmit = 0;
unsigned int IEC_RXCount = 0;
extern int switch_ready;
volatile uint8_t Error_Char_Recived = 0;

extern MeterStatus status;
Queue IEC_Tx_Queue;

void IEC_resetTXBuffer()
{
  IEC_Tx_Done=1;
  IEC_Tx_Buffer_Index = 0;

}
//change the UART MODULE 
void InitIECUART()
{

  iec_tx_buff = &IEC_Tx_Queue;
  iec_tx_buff->head = 0;
  iec_tx_buff->tail = 0;

#if defined OPT_INT
  UCA1CTLW0 |= UCSWRST;      // reset for configuration 
  UCA1CTLW0 |= UCPEN;        // Enable Parity
  UCA1CTLW0 |= UCPAR;        // Even Parity default
  UCA1CTLW0 &=~UCMSB;        // LSB first  default
  UCA1CTLW0 |= UC7BIT;       // 7 -bit    default
  UCA1CTLW0 &=~UCSPB;        // one stop bit default
  UCA1CTLW0 |=UCMODE_0;      // UART Mode default
  UCA1CTLW0 &=~UCSYNC;       // Asynchronous mode default
  UCA1CTL1  |=UCSSEL_3;      // SMCLK is the clock source
  UCA1CTLW1 |= UCGLIT_1;     // 50nsec glitch time
  change_boud_rate(0);       // 300 bits/sec  
 
#else 
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
  
  change_boud_rate(6); //19200 bits/sec //was 6
#endif  

  IEC_TX_DIR_OUT;
  IEC_RX_DIR_IN;
  IEC_SELCT_UARTA1;
}
void enableIEC_UART()
{ 
  UCA1CTLW0 &=~UCSWRST; 
}
void disableIEC_UART()
{
  UCA1CTLW0 |= UCSWRST; 
}

void change_boud_rate(uint8_t x)
{

  
   switch(x)
   {
   case 0://300db
     //UCBRSx table Table 37-4
   //set the baud rate to 300, with clock source of 16777216  SMLCK
  //N=16777216/300  ==> 55924.0533333....
  //UCBRx = Int(N/16) = INT(55924.0533333/16)  = INT(3495.253333...)  = 3495
  //UCBRFx = (3495.253333... - 3495) * 16  = 4.05333 app. 4
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.05333  from table==>0x01
  //UCA0BRW = 3495; // baud rate
  UCA1BRW_L = 3495 & 0xff;
  UCA1BRW_H = 3495>>8;
  UCA1MCTLW=0x0100|UCOS16|UCBRF_4;//oversampling baud-rate generation mode 0x0100 UCBRSx
  UCA1STATW &= ~UCLISTEN ; //disable loop back mode use this for test only
  
     break;
     
     
   case 1://600db
     
  //set the baud rate to 600, with clock source of 16777216  SMLCK
  //N=16777216/600  ==> 27962.026666666666666666666666667....
  //UCBRx = Int(N/16) = INT(27962.026666666666666666666666667/16)  = INT(1747.6266666666666666666666666667)  = 1747
  //UCBRFx = (1747.6266666... - 1747) * 16  = 10.026666666666666666666666667 app. 10
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.02666  from table==>0x00
  //UCA0BRW = 1747; // baud rate
  UCA1BRW_L = 1747 & 0xff;
  UCA1BRW_H = 1747>>8;
  UCA1MCTLW=0x0000|UCOS16|UCBRF_10;//oversampling baud-rate generation mode 0x0000 UCBRSx
  UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only

  
     break;
   case 2://1200db
     
       //set the baud rate to 1200, with clock source of 16777216  SMLCK
  //N=16777216/1200  ==> 13981.013333333333333333333333333....
  //UCBRx = Int(N/16) = INT(13981.013333333333333333333333333/16)  = INT(873.81333333333333333333333333333)  = 873
  //UCBRFx = (873.81333333333333333333333333333... - 873) * 16  = 13.013333333333333333333333333333 app. 13
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.01333  from table==>0x00
  //UCA0BRW = 873; // baud rate
  UCA1BRW_L = 873&0xff;
  UCA1BRW_H = 873>>8;
  UCA1MCTLW=0x0000|UCOS16|UCBRF_13;//oversampling baud-rate generation mode 0x0000 UCBRSx
  UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only
  
     break;
   case 3://2400db
     
   //set the baud rate to 2400, with clock source of 16777216  SMLCK
  //N=16777216/2400  ==> 6990.5066666666666666666666666667....
  //UCBRx = Int(N/16) = INT(6990.5066666666666666666666666667/16)  = INT(436.90666666666666666666666666667)  = 436
  //UCBRFx = (436.90666666666666666666666666667... - 436) * 16  = 14.5066666666666666666666666667 app. 14
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.5066  from table==>0xAA
  //UCA0BRW = 436; // baud rate
  UCA1BRW_L = 436&0xff;
  UCA1BRW_H = 436>>8;
  UCA1MCTLW=0xAA00|UCOS16|UCBRF_14;//oversampling baud-rate generation mode 0x0000 UCBRSx
  UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only
     
     
     break;
   case 4://4800
     
  //set the baud rate to 4800, with clock source of 16777216  SMLCK
  //N=16777216/4800  ==> 3495.2533333333333333333333333333....
  //UCBRx = Int(N/16) = INT(3495.2533333333333333333333333333/16)  = INT(218.45333333333333333333333333333)  = 218
  //UCBRFx = (218.45333333333333... - 218) * 16  = 7.2533333333333333333333333333 app. 7
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.2533  from table==>0xAA
  //UCA0BRW = 218; // baud rate
  UCA1BRW_L = 218;
  UCA1BRW_H = 0;
  UCA1MCTLW=0x4400|UCOS16|UCBRF_7;//oversampling baud-rate generation mode 0x4400 UCBRSx
  UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only
     
     break;
   case 5://9600
     
  //set the baud rate to 9600, with clock source of 16777216  SMLCK
  //N=16777216/9600  ==> 1747.6266666666666666666666666667....
  //UCBRx = Int(N/16) = INT(1747.6266666666666666666666666667/16)  = INT(109.22666666666666666666666666667)  = 109
  //UCBRFx = (109.226666666... - 109) * 16  = 3.62 app. 3
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.62666  from table==>0xAA
  //UCA0BRW = 109; // baud rate
  UCA1BRW_L = 109;
  UCA1BRW_H = 0;
  UCA1MCTLW=0xB500|UCOS16|UCBRF_3;//oversampling baud-rate generation mode 0x4400 UCBRSx
  UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only
  
     break;
   case 6://19200
     
       //set the baud rate to 19200, with clock source of 16777216  SMLCK
  //N=16777216/19200  ==> 873.81333333333333333333333333333....
  //UCBRx = Int(N/16) = INT(873.81333333333333333333333333333/16)  = INT(54.613333333333333333333333333333)  = 54
  //UCBRFx = (54.613333333333333333333333333333... - 54) * 16  = 9.8133333333333333333333333333333 app. 9
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.81333  from table==>0xAA
  //UCA0BRW = 54; // baud rate
  UCA1BRW_L = 54;
  UCA1BRW_H = 0;
  UCA1MCTLW=0xEE00|UCOS16|UCBRF_9;//oversampling baud-rate generation mode 0x4400 UCBRSx
  UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only
          
     break;
   case 7://57600
      //set the baud rate to 57600, with clock source of 16777216  SMLCK
      //N=16777216/57600  ==> 291.271111111111111111111111111111....
      //UCBRx = Int(N/16) = INT(291.271111111111111111111111111111/16)  = INT(18.20444444444444444)  = 18
      //UCBRFx = (18.20444444444444444... - 18) * 16  = 3.2711111111111111111111111111111 app. 3
      //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.271111  from table==>0x44
      //UCA0BRW = 18; // baud rate
      UCA1BRW_L = 18;
      UCA1BRW_H = 0;
      UCA1MCTLW=0x4400|UCOS16|UCBRF_3;//oversampling baud-rate generation mode 0x4400 UCBRSx
      UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only     
     
     break;   
     
   default:
     break;
   }
   
   UCA1CTLW0 &=~UCSWRST;  
   UCA1IE|=UCTXIE|UCRXIE;//UCTXCPTIE|
}
uint8_t IEC_AddToRXBuffer(uint8_t data)
{
	if(IEC_Rx_Buffer_Index >= MaxIECRxBufferLength)
	{
	   IEC_Rx_Buffer_Index = 0;
		return 0;
	}

	IEC_RX_BUF[IEC_Rx_Buffer_Index] =  data;
        IEC_Rx_Buffer_Index++;
        return 1;
}

uint8_t IEC_AddtoTXBuffer(uint8_t data)
{
    return queue_enqueue(iec_tx_buff,&data);
}

void IEC_Start_SendBuffer()
{
    if(IEC_Tx_Done == 1)
    { 
       IEC_Tx_Done = 0; //restart transmission
       Transmit = 1;
       
       uint8_t data=0;
       if(queue_dequeue(iec_tx_buff,&data) == 1) //IEC_TX_BUF[0];
           UCA1TXBUF  = data;
    }
    else
    {
      //do noting 
    }
}

void receive_firmware_image();
extern int local_comm_exchange_mode_flag;
const uint32_t next_octect_timeout_thr = ((32768L*TIMEOUT_3_INTRA_FRAME)/1000L);
const uint32_t inactivity_timeout_thr = ((32768L*TIMEOUT_2_INACTIVITY)/1000L);

/*
 * USCI_A1 interrupt service routine
 */
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	if(local_comm_exchange_mode_flag)
	{
		receive_firmware_image();
	}
	else
	{

		switch (__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
		{
			case USCI_NONE: break;              // No interrupt
			case USCI_UART_UCRXIFG:             // RXIFG
				if((UCA1STATW & UCRXERR) !=0 )
				{
					Error_Char_Recived = 1;
				}
				uint32_t now;
				now = current_time();
				if (now >= next_octet_timeout[IEC_PORT])
				{
					if (rx[IEC_PORT].state > 1)
					{
						iec62056_46_rx_timeout(&link[IEC_PORT], &rx[IEC_PORT]);
					}
				}
				iec62056_46_rx_byte(&link[IEC_PORT], &rx[IEC_PORT], UCA1RXBUF);
				next_octet_timeout[IEC_PORT] = now + next_octect_timeout_thr ;
				next_inactivity_timeout[IEC_PORT] = now + inactivity_timeout_thr;
			case USCI_UART_UCTXIFG:
				 if((queue_isEmpty(iec_tx_buff)!=1) &&(Transmit == 1))//
				 {
					 uint8_t data=0;
					 if(queue_dequeue(iec_tx_buff,&data) == 1)
					 {
						 UCA1TXBUF  = data;
					 }
				 }
				 else
				 {
					 IEC_Tx_Done = 1;
	  		 		 Transmit= 0;
			 	 }
				break;      // TXIFG
			case USCI_UART_UCSTTIFG:
				break;     // TTIFG
			case USCI_UART_UCTXCPTIFG:
				break;   // TXCPTIFG
			default:
				break;
		}
	}
}

