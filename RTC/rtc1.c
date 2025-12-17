#include <avr/io.h>
#include "rtc.h"

#define F_CPU 16000000UL
#define F_SCL 100000UL      // I2C clock speed
#define TWBR_VALUE ((F_CPU / F_SCL) - 16) / 2

// TWI (I2C) Functions
void TWI_Init() {
    TWBR0 = (uint8_t)TWBR_VALUE; // Set bit rate
    TWSR0 = 0x00;                // Prescaler set to 1
}

void TWI_Start() {
    TWCR0 = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Send START condition
    while (!(TWCR0 & (1 << TWINT)));                  // Wait for START to complete
}

void TWI_Stop() {
    TWCR0 = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); // Send STOP condition
}

void TWI_Write(uint8_t data) {
    TWDR0 = data;                                    // Load data into data register
    TWCR0 = (1 << TWEN) | (1 << TWINT);              // Start transmission
    while (!(TWCR0 & (1 << TWINT)));                 // Wait for transmission to complete
}

uint8_t TWI_ReadACK() {
    TWCR0 = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); // Enable ACK after reception
    while (!(TWCR0 & (1 << TWINT)));                  // Wait for reception to complete
    return TWDR0;
}

uint8_t TWI_ReadNACK() {
    TWCR0 = (1 << TWEN) | (1 << TWINT);               // No ACK after reception
    while (!(TWCR0 & (1 << TWINT)));                  // Wait for reception to complete
    return TWDR0;
}

// Helper functions for BCD conversion
uint8_t bcdToDec(uint8_t val) {
    return (val / 16 * 10) + (val % 16);
}

uint8_t decToBcd(uint8_t val) {
    return (val / 10 * 16) + (val % 10);
}

// DS1307 Functions
void DS1307_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    TWI_Start();
    TWI_Write(DS1307_ADDRESS << 1); // DS1307 write address
    TWI_Write(0x00);                // Start at register 0x00
    TWI_Write(decToBcd(seconds));
    TWI_Write(decToBcd(minutes));
    TWI_Write(decToBcd(hours));
    TWI_Stop();
}

void DS1307_ReadTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    TWI_Start();
    TWI_Write(DS1307_ADDRESS << 1); // DS1307 write address
    TWI_Write(0x00);                // Start reading from register 0x00
    TWI_Stop();

    TWI_Start();
    TWI_Write((DS1307_ADDRESS << 1) | 0x01); // DS1307 read address

    *seconds = bcdToDec(TWI_ReadACK() & 0x7F);
    *minutes = bcdToDec(TWI_ReadACK());
    *hours = bcdToDec(TWI_ReadNACK());

    TWI_Stop();
}
