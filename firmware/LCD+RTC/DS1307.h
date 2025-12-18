/* 
 * File:   DS1307.h
 * Author: supeng2
 *
 * Created on December 2, 2024, 3:47 PM
 */

#ifndef DS1307_H
#define DS1307_H

#include <avr/io.h>
#include <stdint.h>

// DS1307 I2C address
#define DS1307_ADDRESS 0x68
#define F_CPU 16000000UL
#define F_SCL 100000UL
#define TWBR_VALUE ((F_CPU / F_SCL) - 16) / 2

#define BAUD_RATE 9600     // ?????
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)  // ????????

// UART Functions
void UART_init(void);        // ??? UART
void UART_send(unsigned char data);  // ????????
void UART_putstring(char* StringPtr);  // ?????

void UART_PrintTime(uint8_t hours, uint8_t minutes, uint8_t seconds);

// I2C (TWI) Functions
void TWI_Init();
void TWI_Start();
void TWI_Stop();
void TWI_Write(uint8_t data);
uint8_t TWI_ReadACK();
uint8_t TWI_ReadNACK();

// DS1307 RTC Functions
void DS1307_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void DS1307_ReadTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

// Helper Functions for BCD Conversion
uint8_t bcdToDec(uint8_t val);
uint8_t decToBcd(uint8_t val);

#endif

