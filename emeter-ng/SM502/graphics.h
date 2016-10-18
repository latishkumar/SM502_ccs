/*
 * graphics.h
 *
 *  Created on: Apr 25, 2013
 *      Author: Donayam
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdint.h>
#include "Errors.h"



#define __MAIN_PROGRAM__

#include "self_diagnosis.h"






#define Display_Width 128
#define Display_Height 64

#define font12x16 0
#define font5x8 1



/*
   Location of each display data on the GLCD
*/
#define dateLabelStartX  5
#define dateLabelStartY 0

#define dateStartX  15
#define dateStartY  1

#define timeLabelStartX  90
#define timeLabelStartY  0

#define timeStartX  80
#define timeStartY  1

#define MessageTypeStartX  30
#define MessageTypeStartY  3

#define energyStartX  25
#define energyStartY  4

#define UnitStartX  100
#define UnitStartY  5

#define ProgressBarStartX 0
#define ProgressBarStartY 7

#define ProgressBarEndX 50
#define ProgressBarEndY 7

#define MagneticTamperStartX 110
#define UpperCoverTamperStartX 100
#define LowerCoverTamperStartx 90
#define NeutralTamperStartX 80

   
void WriteString(char *so,int column,int row,int font);
void WriteLong(long number,int column,int row,int font);
void WriteFloat(long number, int column, int row, int font,int decimalPoint);
void WriteChar(char c,int column,int row,int font);


//void drawLine(int x1,int y1, int x2,int y2, int lineTickness);
//void drawRectangle(int x,int y,int width,int height,int fill);
//void fillRectangle(int x,int y,int width,int height);

void ClearArea(int row, int length);

void InitGLCD();
void UpdateDisplay();

void UpdateDisplayProgressBar();
void UpdateDisplayDate();
void UpdateDisplayTime();
void UpdateDisplayTamper();


void ShowTamper(int TamperType);
void ClearTamper(int TamperType);

uint8_t checkLCD();

#endif /* GRAPHICS_H_ */
