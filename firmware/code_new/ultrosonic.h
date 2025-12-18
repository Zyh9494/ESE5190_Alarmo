#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000UL  // Clock frequency
#define TRIG_PIN PD3      // Ultrasonic sensor trigger pin
#define ECHO_PIN PD2      // Ultrasonic sensor echo pin

// Initialize the ultrasonic sensor pins and timer
void init_ultrasonic();

// Send a trigger pulse to the ultrasonic sensor
void send_trigger_pulse();

// Calculate the distance in centimeters based on echo timing
uint16_t calculate_distance();

#endif // ULTRASONIC_H
