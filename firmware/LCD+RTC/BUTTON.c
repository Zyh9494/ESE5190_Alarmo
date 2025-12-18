/* 
 * File:   BUTTON.c
 * Author: supeng2
 *
 * Created on December 2, 2024, 10:07 PM
 */

#include "button.h"

// Initialize button inputs
void Button_init() {
    // Set PD0-PD3 as input with pull-up resistors enabled
    DDRD &= ~((1 << DDD0) | (1 << DDD1) | (1 << DDD2) | (1 << DDD3));
    PORTD |= (1 << PORTD0) | (1 << PORTD1) | (1 << PORTD2) | (1 << PORTD3);

    // Enable pin change interrupt
    PCICR |= (1 << PCIE2);  // Enable interrupts for PCINT[23:16] (PORTD pins)
    PCMSK2 |= (1 << PCINT16) | (1 << PCINT17) | (1 << PCINT18) | (1 << PCINT19); // Enable interrupts for PD0-PD3

    // Enable global interrupts
    sei();
}

// Interrupt Service Routine for PD0-PD3 button press
ISR(PCINT2_vect) {
    _delay_ms(50); // Debounce delay

    // Check PD0-PD3 button states
    if (!(PIND & (1 << PIND0))) { // PD0 pressed
        last_mode = mode;             // Save the current mode
        mode = (mode + 1) % 3;        // Cycle through modes 0, 1, and 2
        inactivity_counter = 0;       // Reset inactivity counter
    } else if (!(PIND & (1 << PIND1))) { // PD1 pressed
        if (mode == 1) {
            alarmHours = (alarmHours + 1) % 24; // Increment alarm hours
        } else if (mode == 2) {
            alarmMinutes = (alarmMinutes + 1) % 60; // Increment alarm minutes
        }
        inactivity_counter = 0; // Reset inactivity counter
    } else if (!(PIND & (1 << PIND2))) { // PD2 pressed
        if (mode == 1) {
            alarmHours = (alarmHours == 0) ? 23 : alarmHours - 1; // Decrement alarm hours
        } else if (mode == 2) {
            alarmMinutes = (alarmMinutes == 0) ? 59 : alarmMinutes - 1; // Decrement alarm minutes
        }
        inactivity_counter = 0; // Reset inactivity counter
    } else if (!(PIND & (1 << PIND3))) { // PD3 pressed
        alarm_on = !alarm_on; // Toggle alarm on/off state
        if (!alarm_on) {
            buzzer_on = 0; // Turn off buzzer if alarm is off
        }
    }
}

// Check for inactivity and reset to clock mode if needed
void checkInactivity() {
    if (mode != 0 && inactivity_counter >= 8000) { // 8 seconds = 8000 ms
        mode = 0;              // Reset to clock mode
        inactivity_counter = 0; // Reset inactivity counter
    }
}
