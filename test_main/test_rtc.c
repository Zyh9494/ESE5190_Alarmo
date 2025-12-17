#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"
#include "rtc.h"

#define F_CPU 16000000UL
#define BAUD_PRESCALER (((F_CPU / (9600 * 16UL))) - 1)

// Main Function
int main(void) {
    UART_init(BAUD_PRESCALER); // Initialize UART
    TWI_Init();                // Initialize TWI (I2C)

    // Set initial time to 14:45:00 (comment out after first run)
    // DS1307_SetTime(14, 45, 0);

    uint8_t hours, minutes, seconds;

    while (1) {
        // Read time from DS1307
        DS1307_ReadTime(&hours, &minutes, &seconds);

        // Print time via UART
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Time: %02u:%02u:%02u\r\n", hours, minutes, seconds);
        UART_putstring(buffer);

        _delay_ms(1000); // 1-second delay
    }

    return 0;
}
