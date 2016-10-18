/*
 * Dispaly.c
 *
 *  Created on: Apr 17, 2013
 *      Author: Donayam
 */

#include "Display.h"

char displaystatus;

//void delay1us(unsigned int x)
//{
//	//const int y=x*16;
//	for(;x>=1;x--)
//   __delay_cycles(16);
//
//}


//void delay1ms(unsigned int x)
//{
//	unsigned int y=1000;
//	for(;y>=1;y--)
//		delay1us(x);
//}

void delay(int i)
{
	for(;i>=1;i--)
   __delay_cycles(16);
}

void setCursorPosition(int col,int row)
{
   int z = ((2 + row) % 8);

   if(col>=DisplayWidth)
	   col= DisplayWidth-1;
   if(z>=DisplayHeight)
	   z= DisplayHeight-1;

   //row =row%8;
   sendDisplayCommand(PageAddressSetCommandData(z));
   sendDisplayCommand(ColomonAdd_LowerbyteCommandData(col));
   sendDisplayCommand(ColomonAdd_HigherbyteCommandData(col>>4));

}

void sendDisplayData(char Data)
{

  CS1(0);
  A0(1);

  delay(1);
  //delay(50);
  WR(0);
  //RD(1);

  LCD_dataOut = Data;
  LCD_dataDIR = 0xff;


  delay(1);
  //delete this
  WR(0);
  delay(2);
  WR(1);
  //end of delete this
  //WR(1);
  //delay(2);
  //WR(0);
  CS1(1);
  delay(50);

}


void InitGraphicsLCD()
{
	  Reset(1);
	   LCD_dataDIR = 0xff;

	   A0_Dir(1);
	   RD_Dir(1);
	   WR_Dir(1);
	   CS1_Dir(1);
	   Reset_Dir(1);

	   CS1(0);
	   RD(1);
	   WR(1);

	   Reset(1);

	   Reset(0);
	  //delay some time
	   delay(1000);

	   Reset(1);
	   delay(200);
	   //sendDisplayCommand(0xE2);
	   //delay(100);
	   sendDisplayCommand(DisplayBiasSetCommandData(1)); //bas command
	  // waitBeasy();
	   sendDisplayCommand(ADCSegmentDirectionCommandData(0));  //ADC segment driver direction (A0=Normal) x direction reverse
	   //waitBeasy();
	   sendDisplayCommand(0xC8);   //added 
	   //waitBeasy();
	   sendDisplayCommand(CommonOutputModeSelectCD(1));	 //COM output scan direction (C0= Normal) y direction reverse
	   //waitBeasy();
	   sendDisplayCommand(0x40);	 //Operating Mode 
	  // waitBeasy();
	   //delay(0); 
	   delay(5);
	   sendDisplayCommand(0x50);	//resistor ratio 50
	   //waitBeasy();
	   //delay(0); 
	   delay(5);
	   sendDisplayCommand(0x81);	 //electronic volume mode set 
	   //waitBeasy();
	   // delay(0); 
	   delay(5);
	   sendDisplayCommand(0x19);	//electronic volume register set 
	   //waitBeasy();
	   // delay(0); 
	   delay(5);

	   sendDisplayCommand(0x2F);//2F	 //power control set 
	   //waitBeasy();
	   //delay(0); 
	   delay(5);
	   sendDisplayCommand(DisplayOn_CommandData);	 //display ON/OFF ‐ set to ON 

	  // waitBeasy();
	   //sed_cls(0xff);;
	   sendDisplayCommand(DisplayAllPointsNormalCD);
	   //sendDisplayCommand(DisplayReverseCommandData);
	    //sendDisplayCommand(DisplayOff_CommandData);
	   CLS();
}

//	CS1(0);
//    A0(0);
//
//    //delay(1)
//    delay(500);
//
//
//    RD(1);
//    WR(0);
//    LCD_dataDIR = 0xff;
//    LCD_dataOut = Command;
//
//    //delete this
//    RD(0);
//    delay(2);
//    RD(1);
//    //end of delete this
//
//    delay(500);
//    WR(1);
//    CS1(1);
//    //delay
//    delay(500);

void sendDisplayCommand(char Command)
{

	CS1(0);
	A0(0);

	//delay(1)
	delay(1);
	// RD(1);
	WR(0);
	LCD_dataDIR = 0xff;
	LCD_dataOut = Command;

	//    //delete this
	//    RD(0);
	//    delay(2);
	//    RD(1);
	//    //end of delete this

	delay(1);
	WR(1);
	CS1(1);
	//delay
	delay(5);
}




void readDisplayStatus()
{
  CS1(0);
  A0(0);
  RD(0);
  WR(1);

  delay(50);
  LCD_dataDIR = 0x00;
  displaystatus = LCD_dataIn;

}

uint8_t readDisplayData()
{  
  CS1(0);
  delay(2);
  A0(1);
  delay(2);
  LCD_dataOut = 0;
  LCD_dataDIR = 0x00;//input 
  
  WR(1);
  RD(0);
  delay(1000);
  uint8_t z = LCD_dataIn;
  delay(10);
  z= LCD_dataIn;
  LCD_dataDIR = 0xff;

  
  RD(1);
  WR(1);
  CS1(0);

  return z;
}


void waitBeasy()
{
   readDisplayStatus();
  //while((displaystatus &=DisplayBusy_Status)==DisplayBusy_Status)
  //{
  //	  displaystatus = LCD_dataIn;
  //}
}
//
void CLS()
{
  int i=7;
  int j=127;

  for(;i>=0;i--)
  {
	  setCursorPosition(0,i);
	  j=127;
	  for(;j>=0;j--)
		  sendDisplayData(0x00);
  }
}
