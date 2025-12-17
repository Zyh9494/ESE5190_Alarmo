#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "motor_control.h"
#include "ultrosonic.h"
#include "uart.h"

#define OBSTACLE_DISTANCE_THRESHOLD 40  // Distance in cm to trigger turn
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

int main(void) {
    // Initialize motors, ultrasonic sensor, and UART
    initialize_motor();
    init_ultrasonic();
    UART_init(BAUD_PRESCALER);

    sei(); // Enable global interrupts

    char output[50]; // Buffer for UART output

    while (1) {
        // Start ultrasonic measurement
        send_trigger_pulse();
        _delay_ms(60);  // Allow time for echo to return

        // Get the distance from the ultrasonic sensor
        uint16_t distance = calculate_distance();

        // Print the distance to the UART terminal
        sprintf(output, "Distance: %u cm\r\n", distance);
        UART_putstring(output);

        if (distance > 0 && distance < OBSTACLE_DISTANCE_THRESHOLD) {
            // If an obstacle is detected, turn left
            // sprintf(output, "Obstacle detected! Turning left.\r\n");
            // UART_putstring(output);
            turn_left();  // Let turn_left handle timing
        } else {
            // If no obstacle is detected, move forward
            // sprintf(output, "Path is clear. Moving forward.\r\n");
            // UART_putstring(output);
            move_forward();
        }
    }

    return 0;
}
