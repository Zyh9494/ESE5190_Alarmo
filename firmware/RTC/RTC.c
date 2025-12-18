#define __AVR_ATmega32PB__
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define F_CPU 16000000UL    // Define CPU frequency
#define F_SCL 100000UL      // I2C clock speed
#define TWBR_VALUE ((F_CPU / F_SCL) - 16) / 2

#define DS1307_ADDRESS 0x68 // DS1307 I2C address
#define BAUD 9600           // UART baud rate
#define MYUBRR F_CPU/16/BAUD-1

// UART Functions
void UART_Init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, 1 stop bit
}

void UART_Transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = data;
}

void UART_PrintString(const char *str) {
    while (*str) {
        UART_Transmit(*str++);
    }
}

void UART_PrintTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "Time: %02d:%02d:%02d\r\n", hours, minutes, seconds);
    UART_PrintString(buffer);
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

// Main Function
int main(void) {
    // Initialize UART and I2C
    UART_Init(MYUBRR);
    TWI_Init();

    // Set initial time to 14:45:00
//    DS1307_SetTime(19, 03, 0);

    uint8_t hours, minutes, seconds;

    while (1) {
        // Read time from DS1307
        DS1307_ReadTime(&hours, &minutes, &seconds);

        // Print time via UART
        UART_PrintTime(hours, minutes, seconds);

        _delay_ms(1000); // 1-second delay
    }

    return 0;
}
