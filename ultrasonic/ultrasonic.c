#include "ultrasonic.h"

// Initialize the ultrasonic sensor pins
void init_ultrasonic() {
    // Set TRIG_PIN as output, ECHO_PIN as input
    DDRD |= (1 << TRIG_PIN);   // TRIG is an output
    DDRD &= ~(1 << ECHO_PIN);  // ECHO is an input
}

// Send a trigger pulse to the ultrasonic sensor
void send_trigger_pulse() {
    PORTD |= (1 << TRIG_PIN);  // Set TRIG high
    _delay_us(10);             // Wait for 10 microseconds
    PORTD &= ~(1 << TRIG_PIN); // Set TRIG low
}

// Measure the width of the echo pulse in timer ticks
uint16_t measure_echo_pulse() {
    uint16_t pulse_width = 0;
    init_ultrasonic();
    // Send a trigger pulse
    send_trigger_pulse();

    // Wait for the rising edge of echo (ECHO_PIN goes high)
    TCNT1 = 0;
    while (!(PIND & (1 << ECHO_PIN))) {
        if (TCNT1 > 65500) return -1;  // Timeout if no echo is received
    }

    // Start Timer1
    TCNT1 = 0;  // Reset timer 

    // Wait for the falling edge of echo (ECHO_PIN goes low)
    while (PIND & (1 << ECHO_PIN));

    pulse_width = TCNT1;  // Keep reading the counter

    return pulse_width;  // Return the pulse width (in timer ticks)
}
