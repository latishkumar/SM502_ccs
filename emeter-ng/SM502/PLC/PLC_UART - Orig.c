/*
 * UART.c
 *
 *  Created on: Apr 28, 2013
 *      Author: Donayam
 */


#include "PLC_UART.h"
#include "PLC.h"
#include "../Status.h"




uint8_t TX_BUF[MaxTxBufferLength];
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

extern MeterStatus status;


#ifdef KIT
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


  UCA1CTLW0 &=~UCSWRST;
  UCA1IE|=UCTXCPTIE|UCTXIE|UCRXIE;


  //configure ports
  TX_DIR_OUT;
  RX_DIR_IN;
  PLC_SELCT_UARTA1; 
    
  MessageStatus =  WaittingNewFrame;
//  UCA1TXBUF=25;
//  UCA1TXBUF=26;
//  UCA1TXBUF=27;
}

void PLC_Byte_Recived(uint8_t data)
{
        switch(MessageStatus)
        {
         case WaittingNewFrame:
            switch(RXCount)
            {
              case 0:
                 Rx_Done = 0;                 
                 RecivedMessage.header.Message_Type = data;
              break;
              case 1:
                 RecivedMessage.header.Message_flags = data;
              break;
              case 2:
                 RecivedMessage.header.Payload_Length = data;
              break;
              case 3:
                 RecivedMessage.header.Payload_Length |= data<<8;
                 RecivedMessage.header.Payload_Length -= 4;
              break; 
              case 4:
                 RecivedMessage.header.Message_HeaderCRC =data;
              break;
              case 5:
                 RecivedMessage.header.Message_HeaderCRC |=data<<8;
              break;
              case 6:
                 RecivedMessage.header.Message_PayloadCRC =data;
              break;
              case 7:
                 RecivedMessage.header.Message_PayloadCRC |=data<<8;
                 MessageStatus = RecevingBody;
                 //RXCount = 0;
                 Rx_Bufffer_Index = 0;
              break;
            }
             RXCount++;
           break;
        case RecevingBody:
             RX_BUF[Rx_Bufffer_Index] = data;
             Rx_Bufffer_Index ++;
           if(Rx_Bufffer_Index>=200)
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
                 
                 if(plc_state == INITIALIZING) // if the plc is initializing we need to process for the status hear
                 {
                   ProcessRecivedPLCMessage();     //call the done receving method               
                 }
                 else 
                 {
                   status.PLCCommunicationDetected = 1;
                 }
                 //status.PLCCommunicationDetected = 1;
                 
                 return ;
               }
           }
           else
            {
                 RecivedMessage.Payload = RX_BUF;
                 
                //done receving     
                 MessageStatus = WaittingNewFrame;
                 RXCount = 0;
                 Rx_Bufffer_Index = 0;
                 
                 if(plc_state == INITIALIZING) // if the plc is initializing we need to process for the status hear
                 {
                   ProcessRecivedPLCMessage();     //call the done receving method               
                 }
                 else 
                 {
                   status.PLCCommunicationDetected = 1;
                 }
                  
            }             
          break;
        }
	return ;
}

uint8_t AddtoTXBuffer(uint8_t data)
{
	

	if(Tx_Buffer_Index >= MaxTxBufferLength)
	{
	   Tx_Buffer_Index = 0;
		return 0;
	}

	TX_BUF[Tx_Buffer_Index] =  data;
        Tx_Buffer_Index++;
   return 1;
}
void PLC_Start_SendBuffer()
{
    //CurrentTxLength = Tx_Buffer_Index;
    //Tx_Buffer_Index = 0;
   
    
    if(Tx_Done == 1)
    { 
       CurrentTX_Index=0;
       Tx_Done = 0; //restart transmission 
       UCA1TXBUF = TX_BUF[0];
       //Tx_Buffer_Index++;
       CurrentTX_Index++;
    }
    else
    {
      //do noting 
    }
}


//// USCI_A1 interrupt service routine
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    switch (__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;              // No interrupt
        case USCI_UART_UCRXIFG:             // RXIFG

//            while (!(UCA0IFG & UCTXIFG)) ;  // USCI_A0 TX buffer ready?
//            UCA0TXBUF = UCA0RXBUF;          // TX -> RXed character
//            break;
        	PLC_Byte_Recived(UCA1RXBUF);
        case USCI_UART_UCTXIFG:
                //UCA1TXBUF = 67;
        	//if(Tx_Buffer_Index < CurrentTxLength)
                if(CurrentTX_Index < Tx_Buffer_Index)
        	{
        		UCA1TXBUF = TX_BUF[CurrentTX_Index];//TX_BUF[Tx_Buffer_Index];
        		//Tx_Buffer_Index ++;
                        CurrentTX_Index++;
        	}
        	else
        	{
        		Tx_Done = 1;
        		CurrentTX_Index = 0;
                        Tx_Buffer_Index = 0;//reset the buffer to 0
        		CurrentTxLength =  0;
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


#else 











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
//  UCA1TXBUF=25;
//  UCA1TXBUF=26;
//  UCA1TXBUF=27;
}

void PLC_Byte_Recived(uint8_t data)
{

        switch(MessageStatus)
        {
         case WaittingNewFrame:
            switch(RXCount)
            {
              case 0:
                 Rx_Done = 0;                 
                 RecivedMessage.header.Message_Type = data;
              break;
              case 1:
                 RecivedMessage.header.Message_flags = data;
              break;
              case 2:
                 RecivedMessage.header.Payload_Length = data;
              break;
              case 3:
                 RecivedMessage.header.Payload_Length |= data<<8;
                 RecivedMessage.header.Payload_Length -= 4;
              break; 
              case 4:
                 RecivedMessage.header.Message_HeaderCRC =data;
              break;
              case 5:
                 RecivedMessage.header.Message_HeaderCRC |=data<<8;
              break;
              case 6:
                 RecivedMessage.header.Message_PayloadCRC =data;
              break;
              case 7:
                 RecivedMessage.header.Message_PayloadCRC |=data<<8;
                 MessageStatus = RecevingBody;
                 //RXCount = 0;
                 Rx_Bufffer_Index = 0;
              break;
            }
             RXCount++;
           break;
        case RecevingBody:
             RX_BUF[Rx_Bufffer_Index] = data;
             Rx_Bufffer_Index ++;
             
           if(Rx_Bufffer_Index>=200)
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
                 
                 if(plc_state == INITIALIZING) // if the plc is initializing we need to process for the status hear
                 {
                   ProcessRecivedPLCMessage();     //call the done receving method               
                 }
                 else 
                 {
                   status.PLCCommunicationDetected = 1;
                 }
                 //status.PLCCommunicationDetected = 1;
                 
                 return ;
               }
           }
           else
           {
                 RecivedMessage.Payload = RX_BUF;
                 
                //done receving     
                 MessageStatus = WaittingNewFrame;
                 RXCount = 0;
                 Rx_Bufffer_Index = 0;
                 
                 if(plc_state == INITIALIZING) // if the plc is initializing we need to process for the status hear
                 {
                   ProcessRecivedPLCMessage();     //call the done receving method               
                 }
                 else 
                 {
                   status.PLCCommunicationDetected = 1;
                 }  
           }             
          break;
        }
        
}

__monitor uint8_t AddtoTXBuffer(uint8_t data)
{
	

	if(Tx_Buffer_Index >= MaxTxBufferLength)
	{
	   Tx_Buffer_Index = 0;
		return 0;
	}

	TX_BUF[Tx_Buffer_Index] =  data;
        Tx_Buffer_Index++;
   return 1;
}
void PLC_Start_SendBuffer()
{
    //CurrentTxLength = Tx_Buffer_Index;
    //Tx_Buffer_Index = 0;
   
    
    if(Tx_Done == 1)
    { 
       CurrentTX_Index=0;
       Tx_Done = 0; //restart transmission 
       UCA0TXBUF = TX_BUF[0];
       //Tx_Buffer_Index++;
       CurrentTX_Index++;
    }
    else
    {
      //do noting 
    }
}


//// USCI_A1 interrupt service routine
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch (__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;              // No interrupt
        case USCI_UART_UCRXIFG:             // RXIFG

//            while (!(UCA0IFG & UCTXIFG)) ;  // USCI_A0 TX buffer ready?
//            UCA0TXBUF = UCA0RXBUF;          // TX -> RXed character
//            break;
        	PLC_Byte_Recived(UCA0RXBUF);
        case USCI_UART_UCTXIFG:
                //UCA0TXBUF = 67;
        	//if(Tx_Buffer_Index < CurrentTxLength)
                if(CurrentTX_Index < Tx_Buffer_Index)
        	{
        		UCA0TXBUF = TX_BUF[CurrentTX_Index];//TX_BUF[Tx_Buffer_Index];
        		//Tx_Buffer_Index ++;
                        CurrentTX_Index++;
        	}
        	else
        	{
        		Tx_Done = 1;
        		CurrentTX_Index = 0;
                        Tx_Buffer_Index = 0;//reset the buffer to 0
        		CurrentTxLength =  0;
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
