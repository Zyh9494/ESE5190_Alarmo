#ifndef BUZZER_H
#define BUZZER_H

#include <avr/io.h>

// Note frequencies (Hz)
#define NOTE_C4  261
#define NOTE_D4  274
#define NOTE_E4  287
#define NOTE_F4  293
#define NOTE_G4  306
#define NOTE_A4  319
#define NOTE_B4  332
#define NOTE_C5  335

// Note durations (ms)
#define WHOLE     1600
#define HALF      800
#define QUARTER   400
#define EIGHTH    200

// Function prototypes
void init_timer0(void);
void play_note(uint16_t frequency, uint16_t duration_ms);
void play_twinkle_twinkle(void);

#endif
