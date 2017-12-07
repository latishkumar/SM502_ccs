/*
 * graphics.h
 *
 *  Created on: Apr 25, 2013
 *      Author: Donayam
 */

#ifndef LCD_C_GRAPHICS_H_
#define LCD_C_GRAPHICS_H_

#include <stdint.h>
#include "errors.h"

   
   
#define AM  0
#define EN  1
#define AM_EN 2



   
/**
*   writes a floating number to the lcd's center digits
*   @param number the nuber to write 
*   @param decimalPoint: number of degits after decimal point
*/
void WriteFloat_c(long number, uint8_t decimalPoint);


/**
* initializes the lcd hardware and variables 
*/
void init_GLCD_c();
/**
* updates the display based on the system status 
*/
void UpdateDisplay_c();
/**
* updates the display and shows the next item inline to be displayd
*/
void display_next_item_c();
/**
* disables auto scrolling and swithces the display to manual mode
*/
void switch_to_manual_mode_c();
/**
* disables manual scrolling and switches the display to auto mode
*/
void switch_to_auto_mode_c();

/**
* updates the Consumed energy progress bar
*/
void UpdateDisplayProgressBar_c();
/**
* updates the date display base on the system status
*/
void UpdateDisplayDate_c();
/**
* updates the time display base on the system status
*/
void UpdateDisplayTime_c();
/**
* updates the tamper indicators base on the system status
*/
void UpdateDisplayTamper_c();

/**
* show a tamper indicator 
* @param TamperType:- see the tmaper module for list of tamper types
*/
void ShowTamper_c(int TamperType);
/**
* @see ShowTamper_c
*/
void ClearTamper_c(int TamperType);


#endif /* LCD_C_GRAPHICS_H_ */
