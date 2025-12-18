#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"
#include "motor_control.h"
#include "ultrosonic.h"

#define OBSTACLE_DISTANCE_THRESHOLD 15  // Distance in cm to trigger turn
#define F_CPU 16000000UL

char String[64];
volatile uint8_t mode = 0;         // Current mode: 0 = idle, 1 = set hours, 2 = set minutes
volatile uint8_t hours = 0, minutes = 0; // Alarm time variables
volatile uint16_t inactivity_counter = 0; // Inactivity timer

void setup() {
    // Configure buttons as inputs with pull-up resistors
    DDRD &= ~((1 << DDD2) | (1 << DDD3)); // PD2 (INT0), PD3 (INT1)
    PORTD |= (1 << PORTD2) | (1 << PORTD3);

    DDRB &= ~((1 << DDB0) | (1 << DDB1)); // PB0 (PCINT0), PB1 (PCINT1)
    PORTB |= (1 << PORTB0) | (1 << PORTB1);

    // Configure external interrupts
    EICRA |= (1 << ISC01) | (1 << ISC00); // Rising edge on INT0
    EICRA |= (1 << ISC11) | (1 << ISC10); // Rising edge on INT1
    EIMSK |= (1 << INT0) | (1 << INT1);   // Enable INT0 and INT1

    // Configure pin change interrupts
    PCICR |= (1 << PCIE0); // Enable pin change interrupt for PCINT[7:0]
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT1); // Enable PCINT0, PCINT1

    // Initialize UART, motors, and ultrasonic sensor
    UART_init(BAUD_PRESCALER);
    initialize_motor();
    init_ultrasonic();

    // Enable global interrupts
    sei();
}

// ISR for INT0 (PD2) - Increment
ISR(INT0_vect) {
    inactivity_counter = 0;
    if (mode == 1) {
        hours = (hours + 1) % 24;
        sprintf(String, "Hours: %02u\r\n", hours);
    } else if (mode == 2) {
        minutes = (minutes + 1) % 60;
        sprintf(String, "Minutes: %02u\r\n", minutes);
    }
    UART_putstring(String);
}

// ISR for INT1 (PD3) - Decrement
ISR(INT1_vect) {
    inactivity_counter = 0;
    if (mode == 1) {
        hours = (hours == 0) ? 23 : (hours - 1);
        sprintf(String, "Hours: %02u\r\n", hours);
    } else if (mode == 2) {
        minutes = (minutes == 0) ? 59 : (minutes - 1);
        sprintf(String, "Minutes: %02u\r\n", minutes);
    }
    UART_putstring(String);
}

// ISR for PCINT0 (PB0, PB1) - Mode and Reset
ISR(PCINT0_vect) {
    inactivity_counter = 0;
    if (!(PINB & (1 << PINB0))) { // Mode button
        mode = (mode + 1) % 3;
        if (mode == 0) {
            sprintf(String, "Set: %02u:%02u\r\n", hours, minutes);
        } else if (mode == 1) {
            sprintf(String, "Setting Hours\r\n");
        } else if (mode == 2) {
            sprintf(String, "Setting Minutes\r\n");
        }
        UART_putstring(String);
    } else if (!(PINB & (1 << PINB1))) { // Reset button
        // add RTC reset time
        sprintf(String, "Reset to 00:00\r\n");
        UART_putstring(String);
    }
}

// Check inactivity timeout
void check_inactivity() {
    if (mode != 0 && inactivity_counter >= 6000) { // 6 seconds (6000 ms)
        sprintf(String, "Timeout. Final: %02u:%02u\r\n", hours, minutes);
        UART_putstring(String);
        mode = 0; // Exit setting mode
    }
}

int main(void) {
    setup();

    while (1) {
        _delay_ms(1); // Increment inactivity timer every millisecond
        inactivity_counter++;
        check_inactivity();
    }

    return 0;
}
