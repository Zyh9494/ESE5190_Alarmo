#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <avr/io.h>
#include <util/delay.h>

// Pin Definitions
#define TRIG_PIN PD5  // TRIG connected to PD5
#define ECHO_PIN PD6  // ECHO connected to PD6
#define TIMEOUT 60000 // Timeout for echo response

// Function Prototypes
void init_ultrasonic(void);
void send_trigger_pulse(void);
uint16_t measure_echo_pulse(void);

#endif // ULTRASONIC_H
