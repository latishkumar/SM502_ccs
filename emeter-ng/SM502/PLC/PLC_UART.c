/*
 * UART.c
 *
 *  Created on: Apr 28, 2013
 *      Author: Donayam
 */


#include "PLC_UART.h"
#include "PLC.h"
#include "../Status.h"
#if defined(LOG_PLC_MESSAGE) || defined(LOG_PLC_RECIVED_MESSAGE)
#include "../IEC62056/IEC62056_UART.H"
#endif 
#include <stdio.h>
#include "../Schaduler.h"
Queue plc_tx_buff;
//uint8_t TX_BUF[MaxTxBufferLength];
uint8_t RX_BUF[MaxRxBufferLength];

unsigned int Tx_Buffer_Index  = 0;
unsigned int CurrentTX_Index = 0;
unsigned int Rx_Bufffer_Index = 0;

unsigned int CurrentTxLength = 0;
unsigned int CurrentRxLength = 0;

int Tx_Done = 1;
int Rx_Done = 1;

unsigned int RXCount = 0;

PLC_Message RecivedMessage;

//define PLC RX message status 
int MessageStatus;
#define RecevingFrame 1
#define RecevingHeader 2
#define RecevingBody 3

#define WaittingNewFrame 4

extern  MeterStatus status;



uint8_t header_array[4]; //Temporary storage for frame header CRC calculation[M.G]
uint16_t CalcHeaderCRC;  //Varable to hold calculated header CRC [M.G]


uint8_t readBlobStatus=0;
uint8_t blobDataLength=0;
uint8_t readBlobAddress=0;

extern volatile uint8_t read_blob_flag;
typedef struct
{
  
   uint8_t message_type;//0x97, 0x98
   uint8_t message_flags;//MSG_ORG and MSG_RPY
   /*
     Blob_id  Name
      0  Configuration file
      1  FW upgrade status info
      2  DMEM_data_blob
      3  PMEM_data_blob
      4  Addressed Data blob
   */
   uint16_t blob_id;
   uint16_t blob_total_len;//4 byte max
   uint8_t blob_data[6];//which is 4 byte
   
   uint16_t readAddress; //read blob address 
   
}read_write_blob;
read_write_blob read_write_blob_msg;


#ifdef MONITOR_HP_COM
  uint8_t log_now=0;
#endif 

void ResetPLC_StateFlow()
{
  MessageStatus = WaittingNewFrame;
  RXCount =0;
  RecivedMessage.header.Message_HeaderCRC  = 0;
  RecivedMessage.header.Message_PayloadCRC = 0;
  Rx_Bufffer_Index = 0;
}
  
  
void PLC_Byte_Recived(uint8_t data)
{
 
  
        switch(MessageStatus)
        {

         case WaittingNewFrame:
   #ifdef MONITOR_HP_COM
      
     if(log_now==1)
        printf("_%d",data);
   #endif           
            switch(RXCount)
            {
              case 0:   
                ScaduleTask(ResetPLC_StateFlow,PLC_Frame_Timeout,IEC_REST_TASK);
 
                 if ( data > 0x14 && ( data != 0x97 && data != 0x96 )) //Valid range is 0x00-0x14
                 {
                   //discard byte
                   return;
                 }
                 
                 Rx_Done = 0;
                 RecivedMessage.header.Message_Type = data;
                 header_array[0] = data; //Message_Type [M.G]
                 RXCount++;
//               }
              break;
              case 1:
                 RecivedMessage.header.Message_flags = data;
                 header_array[1] = data; //Message_Flages [M.G]
                 RXCount++;
              break;
              case 2:
                 RecivedMessage.header.Payload_Length = data;
                 RXCount++;
              break;
              case 3:
                 RecivedMessage.header.Payload_Length |= data<<8;
                 
                 

                 
                 
                if(RecivedMessage.header.Message_Type == 0x97 || RecivedMessage.header.Message_Type == 0x96) //read or write blob message, scape CRC 
                {
                   RecivedMessage.header.Message_HeaderCRC  = 0;
                   RecivedMessage.header.Message_PayloadCRC = 0;
                   MessageStatus = RecevingBody;
                   //RXCount = 0;
                   Rx_Bufffer_Index = 0;
                   return;
                }
                 RXCount++;
                
                 RecivedMessage.header.Payload_Length -= 4; // remove Header CRC and Payload CRC from the message length                
                 header_array[2] = (RecivedMessage.header.Payload_Length+4);       //Payload_Length. Why add 4?? [M.G]
                 header_array[3] = ( (RecivedMessage.header.Payload_Length+4)>>8);

              break; 
              case 4:
                 RecivedMessage.header.Message_HeaderCRC =data;
                 RXCount++;
              break;
              case 5:
                 RecivedMessage.header.Message_HeaderCRC |=data<<8;
                 RXCount++;
              break;
              case 6:
                 RecivedMessage.header.Message_PayloadCRC =data;
                 RXCount++;
              break;
              case 7:
                 RXCount++;
                 RecivedMessage.header.Message_PayloadCRC |=data<<8;
                 
                 //Calculate and verify Header CRC [M.G]
                 CalcHeaderCRC = CRC16_BlockChecksum(header_array, 4);
                 if(RecivedMessage.header.Message_HeaderCRC != CalcHeaderCRC )
                 {
                   //CRC verification failed. Discard frame
                   MessageStatus = WaittingNewFrame;
                   RXCount = 0;
                   return;
                 }
                 else
                 {
                   MessageStatus = RecevingBody;
                   //RXCount = 0;
                   Rx_Bufffer_Index = 0;
                 }
              break;
            }
             //RXCount++;
           break;
        case RecevingBody:
 #ifdef MONITOR_HP_COM
      
     if(log_now==1)
        printf("!%d",data);
   #endif
             RX_BUF[Rx_Bufffer_Index] = data;
             Rx_Bufffer_Index ++;
           if(Rx_Bufffer_Index>=(2000-1))//check this 
           {
              Rx_Bufffer_Index = 0;
           }
                                        
	   if(Rx_Bufffer_Index < RecivedMessage.header.Payload_Length)
	   {		
		return ;
	   }
           else if (Rx_Bufffer_Index == RecivedMessage.header.Payload_Length)
           {
               if(RecivedMessage.header.Payload_Length % 2 == 0)
               {
		 RecivedMessage.Payload = RX_BUF;        
                 //done receving 
                 MessageStatus = WaittingNewFrame;
                 RXCount = 0;
                 Rx_Bufffer_Index = 0;
                      #ifdef MONITOR_HP_COM
                           printf("Recived Full Frame %d __\n",MessageStatus);
                      #endif

                   status.PLCCommunicationDetected = 1;

                 return ;
               }
           }
           else
            {
                 RecivedMessage.Payload = RX_BUF;
                 
                //done receving     
                 MessageStatus = WaittingNewFrame;
                      #ifdef MONITOR_HP_COM
                           printf("Recived Full Frame %d __\n",MessageStatus);
                      #endif

                 RXCount = 0;
                 Rx_Bufffer_Index = 0;                
                 status.PLCCommunicationDetected = 1;                 
            }             
          break;
        }
	return ;
}


#if defined(KIT) || defined(PLC_USCIA0)

void InitPLCUART()
{  
  UCA1CTLW0 |= UCSWRST; //reset for configuration 
  UCA1CTLW0 &= ~UCPEN; //Disable Parity
  //UCA0CTLW0 &=~UCPAR;//Odd Parity default
  UCA1CTLW0 &=~UCMSB;//LSB first  default
  UCA1CTLW0 &=~UC7BIT;//8 -bit    default
  UCA1CTLW0 &=~UCSPB;// one stop bit default
  UCA1CTLW0 |=UCMODE_0;//UART Mode default
  UCA1CTLW0 &=~UCSYNC;//Asynchronous mode default
  UCA1CTL1 |=UCSSEL_3;//SMCLK is the clock source

  UCA1CTLW1 |= UCGLIT_1;//50nsec glitch time
  
  //set the baud rate to 57600, with clock source of 16777216  SMLCK
  //N=16777216/57600  ==> 291.27111
  //UCBRx = Int(N/16) = INT(291.27111/16)  = INT(18.2044)  = 18
  //UCBRFx = (18.2044.. - 18) * 16  = 3.2711 app. 3
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.27111  from table==>0x44
  //UCA0BRW = 18; // baud rate
  UCA1BRW_L = 18;
  UCA1BRW_H = 0;
  UCA1MCTLW=0x4400|UCOS16|UCBRF_3;//oversampling baud-rate generation mode 0x4400 UCBRSx
  UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only
  
//         //set the baud rate to 19200, with clock source of 16777216  SMLCK
//  //N=16777216/19200  ==> 873.81333333333333333333333333333....
//  //UCBRx = Int(N/16) = INT(873.81333333333333333333333333333/16)  = INT(54.613333333333333333333333333333)  = 54
//  //UCBRFx = (54.613333333333333333333333333333... - 54) * 16  = 9.8133333333333333333333333333333 app. 9
//  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.81333  from table==>0xAA
//  //UCA0BRW = 54; // baud rate
//  UCA1BRW_L = 54;
//  UCA1BRW_H = 0;
//  UCA1MCTLW=0xEE00|UCOS16|UCBRF_9;//oversampling baud-rate generation mode 0x4400 UCBRSx
//  UCA1STATW &= ~UCLISTEN ; //disable loop back mode for test only
  
    
  
  UCA1CTLW0 &=~UCSWRST;
  UCA1IE|=UCTXCPTIE|UCTXIE|UCRXIE;


  //configure ports
  TX_DIR_OUT;
  RX_DIR_IN;
  PLC_SELCT_UARTA1; 
    
  MessageStatus =  WaittingNewFrame;
  queue_reinit(&plc_tx_buff);

}



//// USCI_A1 interrupt service routine
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    switch (__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;              // No interrupt
        case USCI_UART_UCRXIFG:             // RXIFG


        	PLC_Byte_Recived(UCA1RXBUF);
        case USCI_UART_UCTXIFG:
                //UCA1TXBUF = 67;
        	//if(Tx_Buffer_Index < CurrentTxLength)
               if(queue_isEmpty(&plc_tx_buff)!=1)//if(CurrentTX_Index < Tx_Buffer_Index)
        	{
                    uint8_t data;
                    queue_dequeue(&plc_tx_buff,&data);
                    UCA1TXBUF = data;//TX_BUF[CurrentTX_Index];//TX_BUF[Tx_Buffer_Index];
        	}
        	else
        	{
        		Tx_Done = 1;
        	}
        	break;      // TXIFG
        case USCI_UART_UCSTTIFG:
        	break;     // TTIFG
        case USCI_UART_UCTXCPTIFG:
                //CurrentTxLength=5;
        	break;   // TXCPTIFG
        default:
        	break;
    }
}

void PLC_Start_SendBuffer()
{
    
    if(Tx_Done == 1)
    { 
       //CurrentTX_Index=0;
       Tx_Done = 0; //restart transmission
       uint8_t data;
       if(queue_dequeue(&plc_tx_buff,&data)!=-1)
       UCA1TXBUF = data;//UCA0TXBUF = TX_BUF[0];
    }
    else
    {
      //do noting 
    }
}


#else 

void PLC_Start_SendBuffer()
{
    
    if(Tx_Done == 1)
    { 
       //CurrentTX_Index=0;
       Tx_Done = 0; //restart transmission
       uint8_t data;
       if(queue_dequeue(&plc_tx_buff,&data)!=-1)
       UCA0TXBUF = data;//UCA0TXBUF = TX_BUF[0];
    }
    else
    {
      //do noting 
    }
    
    #if defined(LOG_PLC_MESSAGE) 
      IEC_Start_SendBuffer();
    #endif    
    
}

void InitPLCUART()
{
  
  UCA0CTLW0 |= UCSWRST; //reset for configuration 
  UCA0CTLW0 &= ~UCPEN; //Disable Parity
  //UCA0CTLW0 &=~UCPAR;//Odd Parity default
  UCA0CTLW0 &=~UCMSB;//LSB first  default
  UCA0CTLW0 &=~UC7BIT;//8 -bit    default
  UCA0CTLW0 &=~UCSPB;// one stop bit default
  UCA0CTLW0 |=UCMODE_0;//UART Mode default
  UCA0CTLW0 &=~UCSYNC;//Asynchronous mode default
  UCA0CTL1 |=UCSSEL_3;//SMCLK is the clock source

  UCA0CTLW1 |= UCGLIT_1;//50nsec glitch time
  
  //set the baud rate to 57600, with clock source of 16777216  SMLCK
  //N=16777216/57600  ==> 291.27111
  //UCBRx = Int(N/16) = INT(291.27111/16)  = INT(18.2044)  = 18
  //UCBRFx = (18.2044.. - 18) * 16  = 3.2711 app. 3
  //UCBRSx  =  from table on the datasheet using frac(N) as index  frac(N) = 0.27111  from table==>0x44
  //UCA0BRW = 18; // baud rate
  UCA0BRW_L = 18;
  UCA0BRW_H = 0;
  UCA0MCTLW=0x4400|UCOS16|UCBRF_3;//oversampling baud-rate generation mode 0x4400 UCBRSx
  UCA0STATW &= ~UCLISTEN ; //disable loop back mode for test only


  
  UCA0CTLW0 &=~UCSWRST;
  UCA0IE|=UCTXCPTIE|UCTXIE|UCRXIE;
  
  //configure ports
  TX_DIR_OUT;
  RX_DIR_IN;
  PLC_SELCT_UARTA1; 
    
  MessageStatus =  WaittingNewFrame;
  queue_reinit(&plc_tx_buff);
}

void enablePLC_UART()
{
  UCA0CTLW0 |= UCSWRST;
}

void disablePLC_UART()
{
  UCA0CTLW0 &=~UCSWRST;
}

#ifdef LOG_PLC_RECIVED_MESSAGE
uint8_t uart_temp_buff=0;
#endif 

//// USCI_A1 interrupt service routine
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch (__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;              // No interrupt
        case USCI_UART_UCRXIFG:             // RXIFG

          
          #ifdef LOG_PLC_RECIVED_MESSAGE
               uart_temp_buff = UCA0RXBUF;
               IEC_AddtoTXBuffer(uart_temp_buff);
               IEC_Start_SendBuffer();
               PLC_Byte_Recived(uart_temp_buff);
          #else 
               PLC_Byte_Recived(UCA0RXBUF);
          #endif
          
        	
        case USCI_UART_UCTXIFG:
                if(queue_isEmpty(&plc_tx_buff)!=1)//if(CurrentTX_Index < Tx_Buffer_Index)
        	{
                        uint8_t data;
                        queue_dequeue(&plc_tx_buff,&data);
        		UCA0TXBUF = data;//TX_BUF[CurrentTX_Index];//TX_BUF[Tx_Buffer_Index];

        	}
        	else
        	{
        		Tx_Done = 1;
        	}
        	break;      // TXIFG
        case USCI_UART_UCSTTIFG:
        	break;     // TTIFG
        case USCI_UART_UCTXCPTIFG:
                //CurrentTxLength=5;
        	break;   // TXCPTIFG
        default:
        	break;
    }
}


#endif


__monitor uint8_t AddtoTXBuffer(uint8_t data)
{
   #if defined(LOG_PLC_MESSAGE) 
      IEC_AddtoTXBuffer(data);
      
   #endif 
	return queue_enqueue(&plc_tx_buff,&data);
}


