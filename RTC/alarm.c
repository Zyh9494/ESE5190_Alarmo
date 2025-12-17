#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"
#include "rtc.h"
#include "motor_control.h" // Assuming buzzer control logic is part of motor control

#define F_CPU 16000000UL
#define BAUD_PRESCALER (((F_CPU / (9600 * 16UL))) - 1)

volatile uint8_t alarm_hours = 0;   // User-set alarm hours
volatile uint8_t alarm_minutes = 0; // User-set alarm minutes
volatile uint8_t alarm_flag = 0;   // Flag for 10 minutes before alarm

// Buzzer control functions
void buzzer_on() {
    PORTD |= (1 << PD6); // Assume PD6 is connected to the buzzer
}

void buzzer_off() {
    PORTD &= ~(1 << PD6);
}

void check_alarm(uint8_t rtc_hours, uint8_t rtc_minutes, uint8_t rtc_seconds) {
    if (rtc_hours == alarm_hours) { // Check if hours match
        if (rtc_minutes == alarm_minutes) { // Check if minutes match
            buzzer_on(); // Trigger alarm
        } else if ((alarm_minutes - rtc_minutes <= 10) || 
                   (rtc_minutes - alarm_minutes >= 50)) { // Within 10 minutes
            if (!alarm_flag) {
                alarm_flag = 1; // Set flag
                UART_putstring("Alarm is within 10 minutes\r\n");
            }
        }
    } else {
        // If hours don't match, ensure the buzzer is off and reset flag
        buzzer_off();
        alarm_flag = 0;
    }
}

int main(void) {
    UART_init(BAUD_PRESCALER); // Initialize UART
    TWI_Init();                // Initialize TWI (I2C)

    // Set initial time to 14:45:00 (comment out after first run)
    // DS1307_SetTime(14, 45, 0);

    uint8_t rtc_hours, rtc_minutes, rtc_seconds;

    // Configure buzzer pin as output
    DDRD |= (1 << PD6); // Assume PD6 is the buzzer pin

    while (1) {
        // Read current time from RTC
        DS1307_ReadTime(&rtc_hours, &rtc_minutes, &rtc_seconds);

        // Print current time via UART
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Current Time: %02u:%02u:%02u\r\n", rtc_hours, rtc_minutes, rtc_seconds);
        UART_putstring(buffer);

        // Check for alarm
        check_alarm(rtc_hours, rtc_minutes, rtc_seconds);

        _delay_ms(1000); // 1-second delay
    }

    return 0;
}
