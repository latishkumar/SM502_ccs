/*
 * UART.h
 *
 *  Created on: Apr 28, 2013
 *      Author: Donayam
 */

#ifndef UART_H_
#define UART_H_

#include <msp430.h>
#include "PLC.h"
#include "../Queue.h"

/**
* Read TI host message specification
*/
   
#ifdef KIT 
   
#define TX_DIR_OUT (P1DIR|=BIT5)
#define TX_DIR_IN  (P1DIR&=~BIT5)

#define RX_DIR_IN  (P1DIR&=~BIT4)
#define RX_DIR_OUT (P1DIR|=BIT4)
#define PLC_SELCT_UARTA1 (P1SEL |=BIT4 | BIT5) 

   
#else 

#define TX_DIR_OUT (P1DIR|=BIT3)
#define TX_DIR_IN  (P1DIR&=~BIT3)

#define RX_DIR_IN  (P1DIR&=~BIT2)
#define RX_DIR_OUT (P1DIR|=BIT2)
#define PLC_SELCT_UARTA1 (P1SEL |=BIT2 | BIT3) 

#endif 
   
#define MaxTxBufferLength 200
#define MaxRxBufferLength 2000


#define PLC_Frame_Timeout 86 //msec for max of 50 byte data
#define __monitor


void enablePLC_UART();
void disablePLC_UART();


extern unsigned int Tx_Buffer_Index;
extern unsigned int CurrentTX_Index ;
extern unsigned int Rx_Bufffer_Index ;

extern unsigned int CurrentTxLength ;
extern unsigned int CurrentRxLength ;


extern unsigned int RXCount;



//extern uint8_t TX_BUF[MaxTxBufferLength];
extern uint8_t RX_BUF[MaxRxBufferLength];
extern PLC_Message RecivedMessage;


extern int Tx_Done ;
extern int Rx_Done;

extern void InitPLCUART();
/*
 The method removes data from the TX buffer by properly by handling the buffer index properly 
 
  data : The data removed from the buffer 
*/
int removeFromBuffer(int *data);
void ResetPLC_StateFlow();
/*
* put a data to the TX bufer this method should be used to put data to the tx buffer 
* because the method properrly adds data to the buffer by incrimenting the buffer index
* the external method should not worry about the buffer index.
  
data: the data to put into the buffer 

 The method returns 0 if the buffer is full
*/
__monitor uint8_t AddtoTXBuffer(uint8_t data);

/*
* After the frame to be transmitted is filled in TX buffer 
* we call this method to start the Transmission, then the 
* uart module will take care of the remaining transmission process.
* 
*/
// void PLC_Start_SendBuffer();

#endif /* UART_H_ */
