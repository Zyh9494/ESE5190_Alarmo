#include "uart.h"  // UART library for serial communication
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

char String[64];
volatile uint8_t button_state = 0; // Variable to store which button was pressed

// Initialize UART for debugging
void UART_init() {
    UBRR0H = (uint8_t)(BAUD_PRESCALER >> 8);
    UBRR0L = (uint8_t)BAUD_PRESCALER;
    UCSR0B = (1 << TXEN0);  // Enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, 1 stop bit
}

void UART_putstring(char* str) {
    while (*str) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = *str++;
    }
}

void setup() {
    // Configure button pins as inputs with pull-up resistors
    DDRD &= ~((1 << DDD2) | (1 << DDD3)); // PD2 (INT0), PD3 (INT1)
    PORTD |= (1 << PORTD2) | (1 << PORTD3);

    DDRB &= ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2)); // PB0 (PCINT0), PB1 (PCINT1), PB2 (PCINT2)
    PORTB |= (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2);

    // Configure external interrupts for INT0 and INT1
    EICRA |= (1 << ISC01) | (1 << ISC00); // Rising edge on INT0
    EICRA |= (1 << ISC11) | (1 << ISC10); // Rising edge on INT1
    EIMSK |= (1 << INT0) | (1 << INT1);   // Enable INT0 and INT1

    // Configure pin change interrupts for PB0, PB1, and PB2
    PCICR |= (1 << PCIE0); // Enable pin change interrupt for PCINT[7:0]
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2); // Enable PCINT0, PCINT1, and PCINT2

    // Initialize UART
    UART_init();
    // Enable global interrupts
    sei();
}

// ISR for INT0 (PD2)
ISR(INT0_vect) {
    button_state = 1; // Button 1 pressed
    sprintf(String, "Button 1 pressed\r\n");
    UART_putstring(String);
}

// ISR for INT1 (PD3)
ISR(INT1_vect) {
    button_state = 2; // Button 2 pressed
    sprintf(String, "Button 2 pressed\r\n");
    UART_putstring(String);
}

// ISR for PCINT0 (PB0, PB1, and PB2)
ISR(PCINT0_vect) {
    if (!(PINB & (1 << PINB0))) { // Check if PB0 is low
        button_state = 3; // Button 3 pressed
        sprintf(String, "Button 3 pressed\r\n");
        UART_putstring(String);
    } else if (!(PINB & (1 << PINB1))) { // Check if PB1 is low
        button_state = 4; // Button 4 pressed
        sprintf(String, "Button 4 pressed\r\n");
        UART_putstring(String);
    } else if (!(PINB & (1 << PINB2))) { // Check if PB2 is low
        button_state = 5; // Button 5 pressed
        sprintf(String, "Button 5 pressed\r\n");
        UART_putstring(String);
    }
}

int main(void) {
    setup();

    while (1) {
        // Main loop does nothing; all work is done in ISRs
    }

    return 0;
}
