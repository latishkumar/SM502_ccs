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

uint8_t read_mem(uint8_t* buff, uint32_t start_addres, uint32_t size);
uint8_t write_mem(uint8_t* buff, uint32_t start_addres, uint32_t size);
void eeprom_read(void* res,uint32_t start_addres,uint32_t byte_length);
int eeprom_write(const void* data,uint32_t start_addres,uint32_t byte_length);

#endif /* I2C2EEPROM_H_ */
