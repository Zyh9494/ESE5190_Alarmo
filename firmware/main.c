#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"
#include "motor_control.h"
#include "ultrosonic.h"
#include "RA8875.h"
#include "DS1307.h"
#include "BUTTON.h"
#include "buzzer.h"

#define F_CPU 16000000UL
#define UART_BAUD_RATE 9600
#define UART_BAUD_PRESCALER (((F_CPU / (UART_BAUD_RATE * 16UL))) - 1)
#define OBSTACLE_DISTANCE_THRESHOLD 40  // Distance in cm to trigger turn

volatile uint8_t mode = 0;           // Mode: 0 = idle, 1 = set hours, 2 = set minutes
volatile uint8_t hours = 0, minutes = 0, seconds = 0; // Current time variables
volatile uint8_t alarmHours = 22, alarmMinutes = 30;  // Alarm time
volatile uint16_t inactivity_counter = 0;            // Inactivity timer
volatile uint8_t alarm_on = 0;                       // Alarm status

char String[64]; // UART message buffer

// Unified setup function
void setup() {
    // Initialize UART
    UART_init(UART_BAUD_PRESCALER);

    // Initialize hardware components
    TWI_Init();           // I2C for RTC
    RA8875_init();        // LCD Display
    RA8875_backlight(1, 255); // LCD Backlight
    Button_init();        // Buttons
    initialize_motor();   // Motor control
    init_ultrasonic();    // Ultrasonic sensor

    // Configure buttons for interrupts
    PCICR |= (1 << PCIE0); // Enable PCINT for PCINT[7:0]
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT4); // PCINT0 and PCINT4 for buttons
    sei(); // Enable global interrupts
}

// Pin change ISR for buttons
ISR(PCINT0_vect) {
    inactivity_counter = 0;
    if (!(PINB & (1 << PINB0))) { // Mode button
        mode = (mode + 1) % 3; // Cycle modes: 0 -> 1 -> 2 -> 0
        if (mode == 0) {
            sprintf(String, "Idle mode.\r\n");
        } else if (mode == 1) {
            sprintf(String, "Setting Hours.\r\n");
        } else if (mode == 2) {
            sprintf(String, "Setting Minutes.\r\n");
        }
        UART_putstring(String);
    } else if (!(PINB & (1 << PINB4))) { // Reset button
        hours = minutes = 0;
        mode = 0;
        alarm_on = 0;
        sprintf(String, "Reset to 00:00.\r\n");
        UART_putstring(String);
    }
}

// Check inactivity and handle timeout
void check_inactivity() {
    if (mode != 0 && inactivity_counter >= 6000) { // 6 seconds of inactivity
        sprintf(String, "Timeout. Final: %02u:%02u.\r\n", hours, minutes);
        UART_putstring(String);
        mode = 0;
    }
}

// Display RTC time and animations
void display_rtc_and_animations() {
    DS1307_ReadTime(&hours, &minutes, &seconds); // Read RTC time
    displayTime(hours, minutes, RA8875_RED);    // Display on LCD
    _delay_ms(1500);                            // Pause
    clearTime();                                // Clear display
    _delay_ms(300);                             // Short delay
}

// Main function
int main(void) {
    setup();

    while (1) {
        _delay_ms(1); // Increment inactivity timer
        inactivity_counter++;
        check_inactivity();

        if (mode == 0) { // Idle mode
            display_rtc_and_animations();

            uint16_t distance = calculate_distance(); // Ultrasonic measurement
            sprintf(String, "Distance: %u cm\r\n", distance);
            UART_putstring(String);

            if (distance > 0 && distance < OBSTACLE_DISTANCE_THRESHOLD) {
                turn_left(); // Avoid obstacle
            } else {
                move_forward(); // Move forward
            }
        } else if (mode == 1) { // Set hours
            displayTime(alarmHours, alarmMinutes, 0xFFFF);
            flashHours(alarmHours, alarmMinutes, 0xFFFF);
        } else if (mode == 2) { // Set minutes
            displayTime(alarmHours, alarmMinutes, 0xFFFF);
            flashMinutes(alarmHours, alarmMinutes, 0xFFFF);
        }
    }

    return 0;
}
