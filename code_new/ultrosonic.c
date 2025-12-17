#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000UL  // Clock frequency
#define TRIG_PIN PD3      // Ultrasonic sensor trigger pin
#define ECHO_PIN PB0    // Ultrasonic sensor echo pin

volatile uint16_t echo_start = 0;
volatile uint16_t echo_end = 0;

// Initialize the ultrasonic sensor pins and timer
void init_ultrasonic() {
    DDRD |= (1 << TRIG_PIN);  // TRIG is an output
    DDRD &= ~(1 << ECHO_PIN); // ECHO is an input

    // Configure Timer1 for input capture
    TCCR1B |= (1 << ICES1) | (1 << CS11); // Rising edge detection, prescaler of 8
    TIMSK1 |= (1 << ICIE1);               // Enable input capture interrupt
}

// Send a trigger pulse to the ultrasonic sensor
void send_trigger_pulse() {
    PORTD |= (1 << TRIG_PIN);  // Set TRIG high
    _delay_us(10);             // Wait for 10 microseconds
    PORTD &= ~(1 << TRIG_PIN); // Set TRIG low
}

// Interrupt Service Routine for Timer1 Capture Event
ISR(TIMER1_CAPT_vect) {
    if (TCCR1B & (1 << ICES1)) {  // If currently detecting the rising edge
        echo_start = ICR1;        // Record the start time
        TCCR1B &= ~(1 << ICES1);  // Switch to falling edge detection
    } else {                      // If currently detecting the falling edge
        echo_end = ICR1;          // Record the end time
        TCCR1B |= (1 << ICES1);   // Switch back to rising edge detection
    }
}

// Calculate the distance in centimeters based on echo timing
uint16_t calculate_distance() {
    uint32_t pulse_length = echo_end - echo_start; // Duration of the echo pulse
    return (uint16_t)(pulse_length / 58);         // Convert to centimeters
}
