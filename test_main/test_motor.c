#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "motor_control.h"

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

int main() {
    // Initialize the motor control pins
    initialize_motor();

    while (1) {
        // Move forward
        move_forward();
        _delay_ms(2000);

        // Turn left
        turn_left();
        _delay_ms(2000);

        // Turn right
        turn_right();
        _delay_ms(2000);

        // Stop motors
        PORTB &= ~((1 << motor_control_pinL) | (1 << motor_control_pinR)); // Both LOW
        _delay_ms(2000);
    }

    return 0;
}
