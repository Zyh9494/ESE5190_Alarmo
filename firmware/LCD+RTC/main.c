#define F_CPU 16000000UL
#define UART_BAUD_RATE      9600
#define UART_BAUD_PRESCALER (((F_CPU / (UART_BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <util/delay.h>
#include "RA8875.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <string.h>
#include "DS1307.h"
#include "BUTTON.h"
#include "BUZZER.h"
#include "uart.h"
#include "motor_control.h"
#include "ultrosonic.h"


#define LCD_WIDTH 800
#define LCD_HEIGHT 480

volatile uint8_t mode = 0;          // 0 = clock mode, 1 = alarm hours mode, 2 = alarm minutes mode
volatile uint8_t last_mode = 0;     // Tracks the last mode for mode switching logic
volatile uint8_t alarmHours = 22;   // Alarm hour
volatile uint8_t alarmMinutes = 30; // Alarm minute
volatile uint16_t inactivity_counter = 0; // Counter for inactivity
volatile uint8_t alarm_on = 0;      // Alarm on/off state
volatile uint8_t buzzer_on = 0;     // Buzzer on/off state

uint8_t hours, minutes, seconds;

// Initialize LCD and RTC
void setup() {
    UART_init();       // Initialize UART
    TWI_Init();        // Initialize I2C
    RA8875_init();     // Initialize LCD
    RA8875_backlight(1,255); // Set LCD backlight
    Button_init();
    init_timer0();
    
    // Set PB0 (LED) and PD6 (Buzzer) as outputs
    DDRB |= (1 << PB0); // LED
    DDRD |= (1 << PD6); // Buzzer
    PORTB &= ~(1 << PB0); // Initialize LED off
    PORTD &= ~(1 << PD6); // Initialize buzzer off
    // 配置 PC3（Mode button 和 LED），PB4（Reset button），PD6（其他用途）

    // 配置超声波控制引脚（PD4, PB0）
    DDRD |= (1 << PD4); // 配置 PD4 为输出（超声波触发信号）
    DDRB |= (1 << PB0); // 配置 PB0 为输出（超声波回波信号）
    PORTD &= ~(1 << PD4); // 初始关闭超声波触发信号
    PORTB &= ~(1 << PB0); // 初始关闭超声波回波信号

    // 配置按钮引脚（PD0, PD1, PD2, PD3）
    DDRD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3)); // 配置为输入
    PORTD |= (1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3);  // 启用上拉电阻

    // 配置 LCD 控制引脚（PB1, PB2, PB3, PB4, PB5）
    DDRB |= (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5); // 配置为输出
    PORTB &= ~((1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5)); // 初始设置为低电平

    // 配置 RTC 控制引脚（PC4, PC5）
    DDRC |= (1 << PC4) | (1 << PC5); // 配置为输出
    PORTC &= ~((1 << PC4) | (1 << PC5)); // 初始设置为低电平

}

uint8_t animation_type;          // Animation type
uint8_t animation_direction;     // Animation direction: 0 = left, 1 = right
uint16_t time_counter = 0;       // Counter for animation timing (1 ms steps)

// Current action and step counters
uint8_t current_action = 0;  // Current action: 0 = display time, 1 = animation
uint8_t current_step = 0;    // Current step in the action sequence

#define TIME_DISPLAY_COUNT 3 // Number of time display iterations
#define ANIMATION_COUNT 3    // Number of animations per sequence

void handleActions() {
    if (current_action == 0) {
        // Display current time
        clearTime();                                // Clear previous display
        DS1307_ReadTime(&hours, &minutes, &seconds); // Read current time
        displayTime(hours, minutes, RA8875_RED);    // Display the time
        _delay_ms(1500);                            // Delay for visibility
        clearTime();                                // Clear time display
        _delay_ms(300);                             // Short delay for transition

        // Move to the next step
        current_step++;
        if (current_step >= TIME_DISPLAY_COUNT) {
            // Transition to animation mode
            RA8875_fillScreen(RA8875_BLACK);
            current_step = 0;
            current_action = 1;
        }
    } else if (current_action == 1) {
        // Play animation
        uint8_t animation_type = rand() % 2;
        uint8_t animation_direction = rand() % 2;

        if (animation_type == 0) {
            blinkEyesOptimized(); // Blink eyes animation
        } else {
            moveEyesDynamic(animation_direction); // Move eyes animation
        }

        _delay_ms(2000); // Delay for animation

        // Move to the next step
        current_step++;
        
        if (mode != 0 || current_step == ANIMATION_COUNT) {
            RA8875_fillScreen(RA8875_BLACK);
        }
        
        if (current_step >= ANIMATION_COUNT) {
            // Transition back to time display
            current_step = 0;
            current_action = 0;
        }
    }
}

int main(void) {
    setup(); // Initialize UART, RTC, and LCD

    while (1) {
        if (mode != last_mode) {
            // If switching from mode 1 or mode 2 to mode 0
            if ((last_mode == 1 || last_mode == 2) && mode == 0) {
                clearTime(); // Clear any residual alarm settings
            }
            last_mode = mode; // Update last_mode
        }
        
        if (mode == 0) {
            handleActions();             
        } else if (mode == 1) {
            // Alarm hours adjustment mode
            displayTime(alarmHours, alarmMinutes, 0xFFFF);
            flashHours(alarmHours, alarmMinutes, 0xFFFF); // Flash hour adjustment
        } else if (mode == 2) {
            // Alarm minutes adjustment mode
            displayTime(alarmHours, alarmMinutes, 0xFFFF);
            flashMinutes(alarmHours, alarmMinutes, 0xFFFF); // Flash minute adjustment
        }

        // Check if the current time matches the alarm time
        DS1307_ReadTime(&hours, &minutes, &seconds);
        if (alarm_on && hours == alarmHours && minutes == alarmMinutes) {
            buzzer_on = 1; // Turn on the buzzer
        }

        // Control LED and buzzer states
        if (alarm_on) {
            PORTB |= (1 << PB0); // Turn on LED
        } else {
            PORTB &= ~(1 << PB0); // Turn off LED
        }

        if (buzzer_on) {
            PORTD |= (1 << PD6); // Turn on buzzer
            play_twinkle_twinkle();
            send_trigger_pulse();
            _delay_ms(60); // Wait for echo to return

            uint16_t distance = calculate_distance(); // Get distance measurement
            sprintf(String, "Distance: %u cm\r\n", distance);
            UART_putstring(String);

            if (distance > 0 && distance < OBSTACLE_DISTANCE_THRESHOLD) {
                // Turn to avoid obstacle
                turn_left(); 
            } else {
                // Continue moving forward
                move_forward();
        } else {
            PORTD &= ~(1 << PD6); // Turn off buzzer
        }

        // Delay for 1 ms and increment inactivity counter
        _delay_ms(1);
        inactivity_counter++;
        checkInactivity(); // Check for user inactivity
    }

    return 0;
}
