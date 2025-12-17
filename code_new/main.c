#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"
#include "motor_control.h"
#include "ultrosonic.h"
#include "buzzer.h"
#define OBSTACLE_DISTANCE_THRESHOLD 40  // Distance in cm to trigger turn
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

char String[64];
volatile uint8_t mode = 0;         // Current mode: 0 = idle, 1 = set hours, 2 = set minutes
volatile uint8_t hours = 0, minutes = 0; // Alarm time variables
volatile uint16_t inactivity_counter = 0; // Inactivity timer

void setup() {
    // Configure buttons as inputs with pull-up resistors
    DDRB &= ~((1 << DDB0) | (1 << DDB4)); // PB0 (Mode button), PB4 (Reset button)
    PORTB |= (1 << PORTB0) | (1 << PORTB4);

    // Initialize UART, motors, and ultrasonic sensor
    UART_init(BAUD_PRESCALER);
    initialize_motor();
    init_ultrasonic();

    // Enable pin change interrupts for buttons
    PCICR |= (1 << PCIE0); // Enable pin change interrupt for PCINT[7:0]
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT4); // Enable PCINT0 (PB0) and PCINT4 (PB4)

    // Enable global interrupts
    sei();
}

// ISR for PCINT0 (PB0, PB4) - Mode and Reset
ISR(PCINT0_vect) {
    inactivity_counter = 0;
    if (!(PINB & (1 << PINB0))) { // Mode button
        mode = (mode + 1) % 3; // Cycle through modes: 0 -> 1 -> 2 -> 0
        if (mode == 0) {
            sprintf(String, "Set: %02u:%02u\r\n", hours, minutes);
        } else if (mode == 1) {
            sprintf(String, "Setting Hours\r\n");
        } else if (mode == 2) {
            sprintf(String, "Setting Minutes\r\n");
        }
        UART_putstring(String);
    } else if (!(PINB & (1 << PINB4))) { // Reset button
        hours = 0;
        minutes = 0;
        mode = 0;
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
        _delay_ms(1); // Increment inactivity timer
        inactivity_counter++;
        check_inactivity();

        // Trigger ultrasonic measurement
        send_trigger_pulse();
        _delay_ms(60); // Wait for echo to return

        uint16_t distance = calculate_distance(); // Get distance measurement
        sprintf(String, "Distance: %u cm\r\n", distance);
        UART_putstring(String);

        if (distance > 0 && distance < OBSTACLE_DISTANCE_THRESHOLD) {
            // Turn to avoid obstacle
            turn_left(); 
        } else {
            // Continue moving forward
            move_forward();
        }
    }
}