/* 
 * File:   BUTTON.h
 * Author: supeng2
 *
 * Created on December 2, 2024, 10:07 PM
 */

#ifndef BUTTON_H
#define	BUTTON_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Global variables for button and system states
extern volatile uint8_t mode;             // Current mode: 0 = clock, 1 = alarm hour setting, 2 = alarm minute setting
extern volatile uint8_t last_mode;        // Previous mode, used for mode transition logic
extern volatile uint8_t alarmHours;       // Alarm hour
extern volatile uint8_t alarmMinutes;     // Alarm minute
extern volatile uint16_t inactivity_counter; // Counter to track user inactivity
extern volatile uint8_t alarm_on;         // Alarm on/off state
extern volatile uint8_t reset_flag;       // Flag for resetting system state
extern volatile uint8_t buzzer_on;        // Buzzer on/off state

// Function prototypes
void Button_init(void);       // Initialize buttons
void checkInactivity(void);   // Check for user inactivity

#endif /* BUTTON_H */
