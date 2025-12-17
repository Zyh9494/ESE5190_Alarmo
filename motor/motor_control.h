#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <avr/io.h>

// Define motor control pins
#define motor_control_pinL PC1  // Left motor control pin
#define motor_control_pinR PC2  // Right motor control pin

// Function prototypes
void initialize_motor();
void move_forward();
void turn_left();
void turn_right();

#endif // MOTOR_CONTROL_H
