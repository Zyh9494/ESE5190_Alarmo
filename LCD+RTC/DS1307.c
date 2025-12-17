/* 
 * File:   DS1307.c
 * Author: supeng2
 *
 * Created on December 2, 2024, 3:47 PM
 */

#include "DS1307.h"
#include <stdio.h>

void UART_init(void) {

    // Set baud rate
    UBRR0H = (unsigned char) (BAUD_PRESCALER >> 8);
    UBRR0L = (unsigned char) BAUD_PRESCALER;
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Enable as Asynchronous USART
    UCSR0C &= ~((1 << UMSEL01) | (1 << UMSEL00));
    /* Set frame format for 8N1: 8 data bits, no parity, 1 stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);     // 8 data bits
    UCSR0C &= ~((1 << UPM01) & (1 << UPM00));   // Disable Parity
    UCSR0C &= ~(1 << USBS0);                    // 1 stop bit
}

void UART_send(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    // Put data into buffer and send data
    UDR0 = data;
}

void UART_putstring(char *StringPtr) {
    while (*StringPtr != 0x00) {
        UART_send(*StringPtr);
        StringPtr++;
    }
}

void UART_PrintTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "Time: %02d:%02d:%02d\r\n", hours, minutes, seconds);
    UART_putstring(buffer);
}

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

// DS1307 RTC Functions
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


