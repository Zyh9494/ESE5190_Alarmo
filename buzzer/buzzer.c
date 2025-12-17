#include "buzzer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int melody[] = {
    119, 119, 105, 119, 88, 94,   // C6, C6, D6, C6, F6, E6
    119, 119, 105, 119, 79, 88,   // C6, C6, D6, C6, G6, F6
    119, 119, 59, 70, 88, 94, 105, // C6, C6, C7, A6, F6, E6, D6
    62, 62, 70, 88, 79, 88         // B6, B6, A6, F6, G6, F6
};

int birthday_song_durations[] = {
    500, 500, 500, 500, 500, 1000,
    500, 500, 500, 500, 500, 1000,
    500, 500, 500, 500, 500, 500, 1000,
    500, 500, 500, 500, 500, 1000
};

void buzzer_init() {
    // ??????
    cli();

    // ??PD5???
    DDRD |= (1 << DDD5);

    // ??PWM??????
    TCCR0A |= (1 << WGM00) | (1 << WGM01); //PWM
    TCCR0B |= (1 << WGM02); 
    TCCR0B |= (1 << CS02);                 // 256??

    // ??????
    sei();
}

// ??????
void play_note(int ocr0a_value, int duration_ms) {
    OCR0A = ocr0a_value;         // ??????
    OCR0B = OCR0A / 2;          // ?????50%
    TCCR0A |= (1 << COM0B1);    // ??PWM??

    for (int i = 0; i < duration_ms; i++) {
        _delay_ms(1);
    }

    // ????
    TCCR0A &= ~(1 << COM0B1);   // ??PWM??
    _delay_ms(1000);             // ????
}

// ?????
void play_birthday_song() {
    int notes_count = sizeof(melody) / sizeof(melody[0]);
    for (int i = 0; i < notes_count; i++) {
        play_note(melody[i], birthday_song_durations[i]);
    }
}

// ??x????????
void buzzer(uint8_t x) {
    if (x == 0) {
        // ????????
        play_note(88, 1000);
    } else if (x == 1) {
        // ?????
        play_birthday_song();
    }
}
