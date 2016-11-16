
/*

*/
//#pragma message("compiling0")
#include <msp430.h>
#include <stdint.h>
//#pragma message("compiling1")
/*
  Level At witch the device can no more opearate on 
*/
#define BATTRY_CRITICAL_LEVEL 3
/**
  Checks the battery level of the meter based on the hardware battery voltage monitor's readings.
  the algorithm stars from the maximum battery level and goes down until the current battery level is found.
  in case the battery is recharched, the algorithm always checks for the next higher level to see if the battery has chared.
  @return from 0 upto 7 indicating the curent battery levels as follows 
                Min  Typ  Max
          0 ==> 1.67 1.74 1.80
          1 ==> 1.87 1.95 2.01
          2 ==> 2.06 2.14 2.21
          3 ==> 2.19 2.27 2.33
          4 ==> 2.33 2.41 2.48
          5 ==> 2.63 2.72 2.79
          6 ==> 2.91 3.02 3.10
          7 ==> 2.91 3.02 3.10
*/
//#pragma message("compiling 2")
uint8_t CheckBattery();
