#include <avr/io.h>
#include <util/delay.h>
#include "motor_control.h"

// Initialize motor control pins
void initialize_motor() {
    DDRC |= (1 << motor_control_pinL) | (1 << motor_control_pinR); // Set pins as outputs
}

// Move both motors forward
void move_forward() {
    PORTC |= (1 << motor_control_pinL);  // IN1 HIGH
    PORTC |= (1 << motor_control_pinR);  // IN1 HIGH
}

// Turn left: stop the left motor, run the right motor
void turn_left() {
    PORTC &= ~(1 << motor_control_pinL); // Stop left motor
    PORTC |= (1 << motor_control_pinR);  // Run right motor forward
    _delay_ms(1000);                     // Delay to complete the turn
    PORTC &= ~(1 << motor_control_pinR); // Stop right motor
}

// Turn right: stop the right motor, run the left motor
void turn_right() {
    PORTC |= (1 << motor_control_pinL);  // Run left motor forward
    PORTC &= ~(1 << motor_control_pinR); // Stop right motor
    _delay_ms(1000);                     // Delay to complete the turn
    PORTC &= ~(1 << motor_control_pinL); // Stop left motor
}
