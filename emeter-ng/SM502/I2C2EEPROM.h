/*
 * I2C2EEPROM.h
 *
 *  Created on: May 22, 2013
 *      Author: Donayam
 */

#ifndef I2C2EEPROM_H_
#define I2C2EEPROM_H_
#include<stdint.h>
   
#define MaxRetransmissionsCount  10

   



void DisableEEPROM2PORT();
void EEPROM2Init();
uint8_t EEPROM2_WriteInt8(uint8_t c,uint32_t Address, uint8_t end);
uint8_t EEPROM2_WriteNextInt8(uint8_t c, uint8_t end);
uint8_t EEPROM2_WriteLong(unsigned long c,uint32_t Address,uint8_t end);
uint8_t EEPROM2_WriteNextLong(unsigned long c,uint8_t end);


uint8_t EEPROM2_ReadInt8(uint32_t Address,uint8_t end,uint8_t *data);
uint8_t EEPROM2_ReadNextInt8(uint8_t end,uint8_t *data);
uint8_t EEPROM2_ReadLong(uint32_t Address,uint8_t end,unsigned long *data);
uint8_t EEPROM2_ReadNextLong(uint8_t end,unsigned long *data);


#endif /* I2C2EEPROM_H_ */
