/*
 * graphics.c
 * graphics Library for SM502/Arada Project
 *  Created on: Apr 25, 2013
 *      Author: Donayam
 */

#include "graphics.h"
#include <emeter-toolkit.h>
#include "../emeter-structs.h"

#include "fonts.h"
#include "Display.h"
#include "Status.h"
#include "RTCC.h"
#include "Tariff.h"
#include "Tamper.h" // for tamper types



extern  phase_parms_s chan1;
//extern RTCtime time;
extern rtc_t rtcc; //emeter-toolkit.h
extern uint32_t total_consumed_reactive_energy;
int vrms=230;
extern CurrentBalance Current_balance;
extern uint32_t MAXActivePowerTripPoint ; //in 10m watt steps 
extern HardwareStatus hardware_status;
/*
 *  s the string to write
 *  column is the start column
 *  row the initial row
 *
 *
 *The method supports column wind up
 */


unsigned int index;


uint8_t MaxDisplayState = 8;
uint8_t MaxDisplayStateMode1 = 2;

char mychar;
unsigned int myoffset;
unsigned int offset2;
unsigned int index2;
uint8_t current_display_Error = 0;

uint8_t temp_disp;

extern MeterStatus status;

void InitGLCD() {

    InitGraphicsLCD();
    uint8_t z = checkLCD();
    
    hardware_status.LCD_Status = z;
  
    status.UpdateDate = 1;
    status.UpdateTime = 1;
    status.DisplayState = 0;
    status.DisplayUpdateRequired = 1;

}

/*
 * writes a string to the display
 * @params
 * 			so:- pointer to the string. The string should be terminated with '~' character to mark the end
 * 			column:- the column where the string is to be written
 * 			row:- the row where the string is to be written. Possible values are from 0 to 7
 * 			font:- the font size to display the string. Possible values are font5x8 and font12x16, defined in graphics.h
 */
void WriteString(char *so, int column, int row, int font) {

	char *s = so;

	index = 0;
	int col = 0;
	int ro = 0;

	if (row >= 8)
		row = 0;
	if (column >= 128)
		column = 0;

	col = column;
	ro = row;	//row * 8

	setCursorPosition(col, ro);	//ks0108GotoXY(x,y);

	if (font == font5x8)
	{
		while (*s != '~') {
			WriteChar(*s, col, ro, font);
			col += 5;
			s++;
		}
	} else if (font == font12x16)
	{
		while (*s != '~') {
			WriteChar(*s, col, ro, font);
			col += 12;
		}
		s++;
	}
}

void WriteFloat(long number, int column, int row, int font,int decimalPoint)
{
        long remainder = 0;
    	char c;
	//number=1200;
	int i = -1;
	int col = column;
	int ro = row;
	char num[]={'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'}; //Assuming that we are going to write a number with maximum of 8 digits
      
         
        if(number == 0)
          i++;
	while (number != 0) {
		i++;
		remainder = number % 10;
		number = (long) number / 10;

		c = (remainder + 48);
		num[i] = c;
	}
        if(i<decimalPoint)
        {
        while(i <= decimalPoint)
          i++;
        }
        int b=i;
         for(;b>=0;b--)
         {
           num[b+1]= num[b];
           if(b==decimalPoint)
           {
             num[b]= '.';
             i++;
             break;
             
           }
         }
         
                   
        for (; i >= 0; i--) {
                
                        if(i==decimalPoint)
                        {
                          if (font == font12x16)
                          WriteChar(num[i], col, ro+1, font5x8);
                          else
                          {
                          WriteChar(num[i], col, ro, font5x8);  
                          col+=2;
                          }
                          col+=2;
                          continue;
                        }
			WriteChar(num[i], col, ro, font);

			if (font == font5x8)
				col += 5;

			else if (font == font12x16)
				col += 12;

			if (col >= 128) {
				ro++;
				col = 0;
			}
		}
}




/*
 * writes a long number to the LCD
 * @params
 * 			number :- the number to write to the LCD
 * 			column :- the column where the number is to be written
 * 			row :- the row where the number will be displayed
 * 			font:- font size used to display the number. Possible values are font5x8 and font12x16
 */
void WriteLong(long number, int column, int row, int font) {
	char c;
	//number=1200;
        long remainder = 0;
	int i = -1;
	int col = column;
	int ro = row;
	char num[]={'0','0','0','0','0','0','0','0','0' }; //Assuming that we are going to write a number with maximum of 8 digits

	while (number != 0) {
		i++;
		remainder = number % 10;
		number = (long) number / 10;

		c = (remainder + 48);
		num[i] = c;
	}
        if(i==-1) //handel number that is zero
        {
          i=0;
        }
	//handle on digit number as 0x
	if (i == 0) {
		WriteChar('0', col, ro, font);
		if (font == font5x8)
			col += 5;

		else if (font == font12x16)
			col += 12;

		if (col >= 128) {
			ro++;
			col = 0;
		}
		WriteChar(num[0], col, ro, font);
	} else {
		for (; i >= 0; i--) {

			WriteChar(num[i], col, ro, font);

			if (font == font5x8)
				col += 5;

			else if (font == font12x16)
				col += 12;

			if (col >= 128) {
				ro++;
				col = 0;
			}
		}
	}

}




/*
 * the method clears the specified rectangular area on the LCD. The implementation is enough for our current requirement.
 *
 * @params
 * 			row :-    the row to be cleared on the LCD
 * 			length :- the number of columns to be cleared starting from column 0
 */
void ClearArea(int row, int length) {
	int i = 0;
	setCursorPosition(0, row); //ks0108GotoXY(0,row*8);
	for (i = 0; i < length; i++) {
		sendDisplayData(0x00); //ks0108WriteData(0x00);
	}
}

/*
 * the method writes character to the LCD
 * @params
 * 			c:- the character to write to the LCD
 * 			column:- the column where the character is to be written
 * 			row: - the row where the character is to be written. Possible values are from 0 to 7
 * 			font:- the font size to be write the character with. Possible values are font5x8 and font12x16
 */
void WriteChar(char c, int column, int row, int font) {
	mychar = c;
	unsigned int offset = 0;
	index = 0;

	unsigned int i = 0;
	unsigned int col = 0;
	unsigned int ro = 0;
	i = 0;


	int x = 0;
	int y = 0;

	if (row >= 8)
		row = 0;
	if (column >= 128)
		column = 0;

	x = column;
	y = row;

	ro = y;
	col = x;

	setCursorPosition(x, y);	//ks0108GotoXY(x,y);
	if (font == font12x16) {
		offset2 = ((c) - 48) * 24;
		myoffset = offset2;
		index2 = offset2;

		setCursorPosition(col, ro);	//ks0108GotoXY(col,ro);
		for (i = 12; i >= 1; i--) {
			sendDisplayData(fontTable12x162[index2]);//ks0108WriteData(fontTable12x162[index2]);
			setCursorPosition(col, ro + 1);	//ks0108GotoXY(col,ro+8);
			sendDisplayData(fontTable12x162[index2 + 1]);//ks0108WriteData(fontTable12x162[index2+1]);
			col++;
			index2 += 2;
			setCursorPosition(col, ro);	//ks0108GotoXY(col,ro);
		}
		return;
	}
	if (font == font5x8) {
		offset = (((c) - 32) * 5);
		index = offset;
		if (col + 5 >= DisplayWidth) {
			col = 0;
			ro += 1;
			setCursorPosition(col, ro);	//ks0108GotoXY(col,ro);
		}
		for (i = 6; i > 1; i--) {
			sendDisplayData(fontTable5x8[index]);//ks0108WriteData(fontTable5x8[index]);
			index++;
			col++;
		}
		return;
	}
}

void ShowTamper(int TamperType)
{
    switch(TamperType)
    {
    case MagneticTamperType:
    	WriteString("M~", MagneticTamperStartX, 7, font5x8);
    	break;
    case  UC_TamperType:
    	WriteString("U~", UpperCoverTamperStartX, 7, font5x8);
    	break;
    case LC_TamperType:
    	WriteString("L~", LowerCoverTamperStartx, 7, font5x8);
    	break;
    case NeutralTamperType: 
        WriteString("N~", NeutralTamperStartX, 7, font5x8);
    	break;
    case EarthTamperType:
    	break;
    case ConfigrationPortTamperType:
    	break;
    case ReverseConnactionTamperType: //reverse tamper
    	//WriteString("R~", ReverseTamperStartX, 7, font5x8);
    	break;
    case MeterBypassTamperType:
    	break;
    default:
    	break;
    }
}

void ClearTamper(int TamperType)
{
	    switch(TamperType)
	    {
	    case MagneticTamperType:
	    	WriteString(" ~", MagneticTamperStartX, 7, font5x8);
	    	break;
	    case  UC_TamperType:
	    	WriteString(" ~", UpperCoverTamperStartX, 7, font5x8);
	    	break;
	    case LC_TamperType:
	    	WriteString(" ~", LowerCoverTamperStartx, 7, font5x8);
	    	break;
	    case NeutralTamperType:
                WriteString(" ~", NeutralTamperStartX, 7, font5x8);
	    	break;
	    case EarthTamperType:
	    	break;
	    case ReverseConnactionTamperType:
	    	break;
	    case MeterBypassTamperType:
	    	break;
	    default:
	    	break;
	    }
}

void UpdateDisplay() {
	if (status.DisplayUpdateRequired == 1) {
                 int w;
                 unsigned long z;
                 
            if(status.DisplayMode ==0 )//normal mode 
              {
		switch (status.DisplayState) {
		case 0: // display the active energy consumption of the meter
			ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Active Energy~", MessageTypeStartX, MessageTypeStartY,
					font5x8);
                        z =neutral.readings.I_rms; //chan1.active_energy_import;//chan1.consumed_active_energy/10000;//z=phase->readings.active_power;//
                        
                        WriteFloat(chan1.active_energy_import,energyStartX ,energyStartY+2,font5x8,3);
			//WriteLong(chan1.active_energy_import, energyStartX, energyStartY+2, font5x8);  
                        WriteFloat(chan1.readings.I_rms,energyStartX ,energyStartY,font5x8,3);
                        WriteFloat(z,energyStartX ,energyStartY+1,font5x8,2);
			WriteString("kWh~", UnitStartX, UnitStartY, font5x8);
			break;

		case 1: // display the reactive energy consumption of the meter
			ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Reactive Energy~", MessageTypeStartX,
					MessageTypeStartY, font5x8);//phase->consumed_reactive_energy_QI
			//WriteFloat(phase->readings.reactive_power, energyStartX, energyStartY, font12x16,3);
                        WriteLong(phase->consumed_reactive_energy_QI, energyStartX, energyStartY, font12x16);
			WriteString("kvar~", UnitStartX, UnitStartY, font5x8);
			break;

		case 2: // display the power factor
			ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Power Factor ~", MessageTypeStartX, MessageTypeStartY,
					font5x8);
			//WriteLong(90, energyStartX + 20, energyStartY, font12x16);
                        w = chan1.readings.power_factor;
                        if(w<0)
                          w=-w;
                        WriteFloat((w),energyStartX ,energyStartY,font12x16,4);
			//WriteChar('%', UnitStartX, UnitStartY, font5x8);
			break;

		case 3: // display the RMS Current
			ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("RMS Current~", MessageTypeStartX, MessageTypeStartY,
					font5x8);
			//WriteLong(chan1.readings.I_rms, energyStartX + 20, energyStartY, font12x16);
                        //vrms=(long) (chan1.readings.I_rms/1000);
                        WriteFloat(chan1.readings.I_rms,energyStartX ,energyStartY,font12x16,3);
                        //WriteLong(vrms, energyStartX + 20, energyStartY, font12x16);
			WriteChar('A', UnitStartX, UnitStartY, font5x8);
			break;

		case 4: // display the RMS voltage
			ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("RMS Voltage  ~", MessageTypeStartX, MessageTypeStartY,
					font5x8);
                        vrms = chan1.readings.V_rms;
                        //vrms  /= 100;
                        WriteFloat(vrms,energyStartX ,energyStartY,font12x16,2);
			//WriteLong(vrms, energyStartX + 20, energyStartY, font12x16);
			WriteChar('V', UnitStartX, UnitStartY, font5x8);
			break;
                        
                 case 5: // display frequency
                   	ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Frequency  ~", MessageTypeStartX, MessageTypeStartY,
					font5x8);
                        //vrms = chan1.readings.frequency;
                        //vrms  /= 100;
                        WriteFloat(chan1.readings.frequency,energyStartX ,energyStartY,font12x16,2);
			//WriteLong(vrms, energyStartX + 20, energyStartY, font12x16);
                        WriteString("Hz  ~", UnitStartX, UnitStartY,font5x8);
					
			break;
                 break;
                case 6:  //display Current balance
                        ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Current Balance  ~", MessageTypeStartX, MessageTypeStartY,
					font5x8);
                        z= Current_balance.balance/1000;
                        //vrms  /= 100;
                        WriteFloat(z,energyStartX,energyStartY,font12x16,1);
			//WriteLong(vrms, energyStartX + 20, energyStartY, font12x16);
                        WriteString("Birr ~", UnitStartX, UnitStartY,font5x8);
                  
                  break;
                  
                case 7:  //display current tariff rate 
                        ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Current Tariff ~", MessageTypeStartX, MessageTypeStartY,font5x8);
					
                        //vrms = chan1.readings.frequency;
                        //vrms  /= 100;
                        WriteFloat(5764,energyStartX  ,energyStartY,font12x16,4);
			//WriteLong(vrms, energyStartX + 20, energyStartY, font12x16);
                         WriteString("Birr ~", UnitStartX, UnitStartY,font5x8);
                  break;
                  
               
                  
//		case 8: // display error
//			ClearArea(MessageTypeStartY, 127);
//			ClearArea(energyStartY, 127);
//			ClearArea(energyStartY + 1, 127);
//			WriteString("Error No      ~", MessageTypeStartX, MessageTypeStartY,
//					font5x8);
//			WriteLong(121, energyStartX, energyStartY, font12x16);
//                        
//			break;
               
		default:
			break;
		}
             }
            else if(status.DisplayMode == 2)
            {
                 temp_disp = getNextError(&current_display_Error);
                 if( temp_disp == 1)
                 {
                   ClearArea(MessageTypeStartY, 127);
		   ClearArea(energyStartY, 127);
		   ClearArea(energyStartY + 1, 127);
		   WriteString("Error No      ~", MessageTypeStartX, MessageTypeStartY,
					font5x8);
		   WriteLong(current_display_Error, energyStartX, energyStartY, font12x16);
                 }
                 else
                 {
                   status.DisplayMode = 0;
                   status.DisplayState = 0;
                 }
            }
            else if(status.DisplayMode ==1)
            {
              switch (status.DisplayState)
              {
               case 0: //display configration menu 1 
                        ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Configration Port set  ~", MessageTypeStartX-20, MessageTypeStartY,font5x8);
					
                        //vrms = chan1.readings.frequency;
                        //vrms  /= 100;
                        //WriteFloat(Current_balance.balance,energyStartX ,energyStartY+1,font5x8,4);
			//WriteLong(vrms, energyStartX + 20, energyStartY, font12x16);
                         WriteString("1. Optical Port ~", energyStartX, energyStartY,font5x8);
                        
                  break;
                  
                case 1: //display configration menu 2
                        ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Configration Port set  ~", MessageTypeStartX-20, MessageTypeStartY,font5x8);
					
                        //vrms = chan1.readings.frequency;
                        //vrms  /= 100;
                        //WriteFloat(Current_balance.balance,energyStartX ,energyStartY+1,font5x8,4);
			//WriteLong(vrms, energyStartX + 20, energyStartY, font12x16);
                        WriteString("2. USB Port ~", energyStartX, energyStartY,font5x8);
                  break;
                  
                case 2://display configration menu 3
                        ClearArea(MessageTypeStartY, 127);
			ClearArea(energyStartY, 127);
			ClearArea(energyStartY + 1, 127);
			WriteString("Configration Port set  ~", MessageTypeStartX-20, MessageTypeStartY,font5x8);
					
                        //vrms = chan1.readings.frequency;
                        //vrms  /= 100;
                        //WriteFloat(Current_balance.balance,energyStartX ,energyStartY+1,font5x8,4);
			//WriteLong(vrms, energyStartX + 20, energyStartY, font12x16);
                        WriteString("3. RS232 ~", energyStartX, energyStartY,font5x8);
                  break;
              }
            }

		status.DisplayUpdateRequired = 0;
	}

	if (status.UpdateDate == 1) {
		/*
		 update the date
		 */
		WriteString("Date~", 5, 0, font5x8);
		//WriteString("5/13/2013~", 15, 1, font5x8);                
                WriteLong(rtcc.month, 15, 1, font5x8);
		WriteChar('/', 25, 1, font5x8);
		//WriteString("11:~",80,1,font5x8);
		WriteLong(rtcc.day, 31, 1, font5x8);
		WriteChar('/', 41, 1, font5x8);
		WriteLong(rtcc.year, 48, 1, font5x8);
		//status.UpdateTime = 0;
		status.UpdateDate = 0;
	}

	if (status.UpdateTime == 1) {

		//remove this
		//this is test for progress bar
		//UpdateDisplayProgressBar(rtcc.second);
		//end of remove this

		/*
		 update the Time
		 */
		WriteString("time~", 90, 0, font5x8);
		WriteLong(rtcc.hour, 80, 1, font5x8);
		WriteChar(':', 91, 1, font5x8);
		//WriteString("11:~",80,1,font5x8);
		WriteLong(rtcc.minute, 96, 1, font5x8);
		WriteChar(':', 106, 1, font5x8);
		WriteLong(rtcc.second, 111, 1, font5x8);
		status.UpdateTime = 0;
	}
}
/*
percent the fill percent of the progress bar:-
b/n 0 and 100
*/
void UpdateDisplayProgressBar() {
  
        unsigned int percent= (chan1.readings.active_power)/(MAXActivePowerTripPoint/100);
        
	//clear ProgressBararea
	ClearArea(ProgressBarStartY, 50);

	//draw progress Bar Border
	//fill progress bar
        
	int i = 50; // the progress bar is of length 50 columns
	int col = ProgressBarStartX;
	int ro  = ProgressBarStartY;
	int per = ((100 - percent) >> 1); // divided by 2

	for (; i > 0; i--) {

		setCursorPosition(col, ro);
		if (i < per)
			sendDisplayData(0x81);
		else
			sendDisplayData(0xff);
		col++;
	}
	sendDisplayData(0xff);
}

uint8_t checkLCD() 
{
  setCursorPosition(5, 0);
  uint8_t data[]= {'A','B','C','D','E','F','G','H','I'};
  uint8_t dataR[9];
  uint8_t z=0;
  uint8_t pc=0,fc=0;
  for(;z<9;z++)
    sendDisplayData(data[z]);
  
  setCursorPosition(5, 0);
  readDisplayData();
  for(z=0;z<9;z++)
    dataR[z] = readDisplayData();
  //setCursorPosition(5,20);
  for(z=0;z<9;z++)
  {
    if(data[z] == dataR[z])
    {
      pc++;
    }
    else
    {
      fc++;
    }
  }
  
  if(pc == 9)
    return 1;
               
  return 0;
}
