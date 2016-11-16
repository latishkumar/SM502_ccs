

#ifndef IEC62056_UART_H
#define IEC62056_UART_H
#include <msp430.h>
#include <stdint.h>
#include "../Queue.h"


#define MaxIECTxBufferLength 200
#define MaxIECRxBufferLength 200
  
#ifdef KIT
#define IEC_TX_DIR_OUT (P1DIR|=BIT5) 
#define IEC_SELCT_UARTA1 (P1SEL |=BIT4 | BIT5)                  
#define IEC_TX_DIR_IN  (P1DIR&=~BIT5)

#define IEC_RX_DIR_IN  (P1DIR&=~BIT4 )
#define IEC_RX_DIR_OUT (P1DIR|=BIT4)
#else

#define IEC_TX_DIR_OUT (P1DIR|=BIT5) 
#define IEC_SELCT_UARTA1 (P1SEL |=BIT4 | BIT5)
#define IEC_DE_SELCT_UARTA1 (P1SEL &=~BIT4 & ~BIT5)
#define ENABLE_UART_PULL_DOWN (P1REN=0xff)
#define IEC_TX_DIR_IN  (P1DIR&=~BIT5)

#define IEC_RX_DIR_IN  (P1DIR&=~BIT4 )
#define IEC_RX_DIR_OUT (P1DIR|=BIT4)

#endif 


extern uint8_t IEC_RX_BUF[MaxIECRxBufferLength];

extern Queue *iec_tx_buff;
extern Queue *iec_rx_buff;
/**
* initializes the UART module 
*/
void InitIECUART();
/**
* add data to tx buffer 
* @param data the data to add to the TX buffer 
*/
uint8_t IEC_AddtoTXBuffer(uint8_t data);
/**
* add data to rx buffer
* @param the data to add to the rx buffer
*/
uint8_t IEC_AddToRXBuffer(uint8_t data);
/**
* start sending data through the uart module 
*/
void IEC_Start_SendBuffer();
/**
* change the uart bouderate 
* @param x fron 0-7 for Boouderate of 300 to 56700
*/
void change_boud_rate(uint8_t x);
/**
* enable the UART module 
*/
void enableIEC_UART();
/**
* disable the uart module 
*/
void disableIEC_UART();
/**
* reset the tx buffer pointer to begining of the buffer
*/
void IEC_resetTXBuffer();
#endif 
