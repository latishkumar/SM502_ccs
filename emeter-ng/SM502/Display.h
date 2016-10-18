/*
 * Display.h
 *
 *  Created on: Apr 17, 2013
 *      Author: Donayam
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <msp430.h>
#include <stdint.h>
//Pin Connections
#define	LCD_dataOut P7OUT
#define LCD_dataIn P7IN
#define LCD_dataDIR P7DIR


#ifdef KIT


#define A0(x) (x==1?(P6OUT|=BIT5):(P6OUT&=~BIT5))
#define A0_Dir(x) (x==1?(P6DIR|=BIT5):(P6DIR&=~BIT5))

#define RD(x) (x==1?(P6OUT|=BIT7):(P6OUT&=~BIT7))
#define RD_Dir(x) (x==1?(P6DIR|=BIT7):(P6DIR&=~BIT7))

#define WR(x) (x==1?(P6OUT|=BIT6):(P6OUT&=~BIT6))
#define WR_Dir(x) (x==1?(P6DIR|=BIT6):(P6DIR&=~BIT6))

#define Reset(x) (x==1?(P6OUT|=BIT4):(P6OUT&=~BIT4))
#define Reset_Dir(x) (x==1?(P6DIR|=BIT4):(P6DIR&=~BIT4))

#define CS1(x) (x==1?(P6OUT|=BIT3):(P6OUT &=~BIT3))
#define CS1_Dir(x)(x==1?(P6DIR|=BIT3):(P6DIR&=~BIT3))


#else


#define A0(x) (x==1?(P6OUT|=BIT3):(P6OUT&=~BIT3))//(x==1?(P6OUT|=BIT5):(P6OUT&=~BIT5))
#define A0_Dir(x) (x==1?(P6DIR|=BIT3):(P6DIR&=~BIT3))//(x==1?(P6DIR|=BIT5):(P6DIR&=~BIT5))

#define RD(x) (x==1?(P6OUT|=BIT5):(P6OUT&=~BIT5))//(x==1?(P6OUT|=BIT7):(P6OUT&=~BIT7))
#define RD_Dir(x) (x==1?(P6DIR|=BIT5):(P6DIR&=~BIT5))//(x==1?(P6DIR|=BIT7):(P6DIR&=~BIT7))

#define WR(x) (x==1?(P6OUT|=BIT4):(P6OUT&=~BIT4))//(x==1?(P6OUT|=BIT6):(P6OUT&=~BIT6))
#define WR_Dir(x) (x==1?(P6DIR|=BIT4):(P6DIR&=~BIT4))//(x==1?(P6DIR|=BIT6):(P6DIR&=~BIT6))

#define Reset(x) (x==1?(P6OUT|=BIT2):(P6OUT&=~BIT2))//(x==1?(P6OUT|=BIT4):(P6OUT&=~BIT4))
#define Reset_Dir(x) (x==1?(P6DIR|=BIT2):(P6DIR&=~BIT2))// (x==1?(P6DIR|=BIT4):(P6DIR&=~BIT4))

#define CS1(x) (x==1?(P6OUT|=BIT1):(P6OUT &=~BIT1))//(x==1?(P6OUT|=BIT3):(P6OUT &=~BIT3))
#define CS1_Dir(x)(x==1?(P6DIR|=BIT1):(P6DIR&=~BIT1))//(x==1?(P6DIR|=BIT3):(P6DIR&=~BIT3))


#endif

///bit1 CS1
//bit2 reset
//bit3 a0
//bit4 WR
//bit5 RD

//Commands
#define DisplayOn_CommandData 0xAF
#define DisplayOff_CommandData 0xAE

#define Start_LineAddressCommandData(LineNumber) ((LineNumber & 0x7F)|0x40)
#define PageAddressSetCommandData(PageNumber) ((0xB0)|(PageNumber & 0x0f))
#define ColomonAdd_LowerbyteCommandData(LowerByte) (LowerByte & 0x0F)
#define ColomonAdd_HigherbyteCommandData(HigherByte) ((HigherByte & 0x0F)|0x10)


#define ADCSegmentDirectionCommandData(Direction) ((0xA0)|(Direction & 0x1)) //column address Direction 0 Normal 1 reversed


#define DisplayAllPointsOnCD 0xA5
#define DisplayAllPointsNormalCD 0xA4

#define DisplayNormalCommandData 0xA6
#define DisplayReverseCommandData 0xA7

//bias 0 1/9 bias 1 1/7 bias
#define DisplayBiasSetCommandData(Bias) ((0xA2)|(Bias&0x1))

#define CommonOutputModeSelectCD(Mode)  ((0xC0)|((Mode&0x01)<<3))


//display status
#define DisplayReset_Status 0x10 // 1 reset in progress 0 operating state
#define DisplayOFF_Status 0x20  //display status 1 off 0 on
#define DisplayADC_Status 0x40    //This shows the relationship between the column address and the segment driver. 1 normal 0 reversed
#define DisplayBusy_Status 0x80 // 0 not busy 1 busy



#define DisplayWidth 128
#define DisplayHeight 64

void InitGraphicsLCD();
void sendDisplayCommand(char Command);
void sendDisplayData(char Data);
void readDisplayStatus();
uint8_t readDisplayData();
void waitBeasy();
void setCursorPosition(int row,int col);
void delay(int i);
void CLS();

#endif /* DISPLAY_H_ */
