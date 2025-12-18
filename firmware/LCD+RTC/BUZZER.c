/* 
 * File:   BUZZER.c
 * Author: supeng2
 *
 * Created on December 3, 2024, 7:27 PM
 */

#include "buzzer.h"
#include <util/delay.h>

#define F_CPU 16000000UL

// Macro to calculate OCR0A value for a given frequency
#define OCR0A_VALUE(freq) ((F_CPU / (2UL * 64UL * freq)) - 1)

// Initialize Timer0 for buzzer control
void init_timer0(void) {
    DDRD |= (1 << PD6); // Set PD6 (OC0A) as output
    TCCR0A = (1 << WGM01) | (1 << COM0A0); // CTC mode, toggle OC0A on compare match
    TCCR0B = 0; // Stop Timer0 initially
}

// Play a single note with the specified frequency and duration
void play_note(uint16_t frequency, uint16_t duration_ms) {
    if (frequency == 0) {
        TCCR0B = 0;           // Stop Timer0 for rests
        PORTD &= ~(1 << PD6); // Ensure PD6 is low
    } else {
        OCR0A = OCR0A_VALUE(frequency); // Set OCR0A for the given frequency
        TCCR0B = (1 << CS01) | (1 << CS00); // Start Timer0 with prescaler 64
    }

    for (uint16_t i = 0; i < duration_ms; i++) {
        _delay_ms(1);
    }

    TCCR0B = 0;           // Stop Timer0 after the note
    PORTD &= ~(1 << PD6); // Ensure PD6 is low
    _delay_ms(50);        // Short pause between notes
}

// Play "Twinkle Twinkle Little Star"
void play_twinkle_twinkle(void) {
    // First line: C C G G A A G
    play_note(NOTE_C4, QUARTER);
    play_note(NOTE_C4, QUARTER);
    play_note(NOTE_G4, QUARTER);
    play_note(NOTE_G4, QUARTER);
    play_note(NOTE_A4, QUARTER);
    play_note(NOTE_A4, QUARTER);
    play_note(NOTE_G4, HALF);

    // Second line: F F E E D D C
    play_note(NOTE_F4, QUARTER);
    play_note(NOTE_F4, QUARTER);
    play_note(NOTE_E4, QUARTER);
    play_note(NOTE_E4, QUARTER);
    play_note(NOTE_D4, QUARTER);
    play_note(NOTE_D4, QUARTER);
    play_note(NOTE_C4, HALF);

    // Third line: G G F F E E D
    play_note(NOTE_G4, QUARTER);
    play_note(NOTE_G4, QUARTER);
    play_note(NOTE_F4, QUARTER);
    play_note(NOTE_F4, QUARTER);
    play_note(NOTE_E4, QUARTER);
    play_note(NOTE_E4, QUARTER);
    play_note(NOTE_D4, HALF);

    // Fourth line: G G F F E E D
    play_note(NOTE_G4, QUARTER);
    play_note(NOTE_G4, QUARTER);
    play_note(NOTE_F4, QUARTER);
    play_note(NOTE_F4, QUARTER);
    play_note(NOTE_E4, QUARTER);
    play_note(NOTE_E4, QUARTER);
    play_note(NOTE_D4, HALF);

    // Fifth line: C C G G A A G
    play_note(NOTE_C4, QUARTER);
    play_note(NOTE_C4, QUARTER);
    play_note(NOTE_G4, QUARTER);
    play_note(NOTE_G4, QUARTER);
    play_note(NOTE_A4, QUARTER);
    play_note(NOTE_A4, QUARTER);
    play_note(NOTE_G4, HALF);

    // Sixth line: F F E E D D C
    play_note(NOTE_F4, QUARTER);
    play_note(NOTE_F4, QUARTER);
    play_note(NOTE_E4, QUARTER);
    play_note(NOTE_E4, QUARTER);
    play_note(NOTE_D4, QUARTER);
    play_note(NOTE_D4, QUARTER);
    play_note(NOTE_C4, HALF);
}