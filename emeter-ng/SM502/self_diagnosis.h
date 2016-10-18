
#ifndef self_diagnosis_h
#define self_diagnosis_h
#include <stdint.h>

#define MAX_EXPECTED_METER_OFF_TIME_MINUTES 43200L

typedef struct{
   
   uint8_t RTC_Status;
   uint8_t RTCResetToDefaultStatus;
   uint8_t PLC_Status;
   uint8_t EEPROM1_Status;
   uint8_t EEPROM2_Status;
//   uint8_t LCD_Status;
   uint8_t Battry_Status;
   
}HardwareStatus;


/**
* this method checks if EEPROM 1 and EEPROM 2 are fine 
*/
uint8_t DiagnosisMeter();


#endif 