#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#define DS1307_ADDRESS 0x68 // DS1307 I2C address

// DS1307 RTC Functions
void DS1307_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void DS1307_ReadTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

// Helper functions for BCD conversion
uint8_t bcdToDec(uint8_t val);
uint8_t decToBcd(uint8_t val);

// I2C/TWI Functions
void TWI_Init();
void TWI_Start();
void TWI_Stop();
void TWI_Write(uint8_t data);
uint8_t TWI_ReadACK();
uint8_t TWI_ReadNACK();

#endif // RTC_H
