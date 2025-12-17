#ifndef EYE_CONTROL_H
#define EYE_CONTROL_H

#include <stdint.h>

// Function prototypes for eye control operations

// Draw eyes in an open state
void drawEyesOpen();

// Draw eyes in a specific state optimized for blinking
void drawEyeStateOptimized(uint16_t shortAxis);

// Perform a blink animation with optimized transitions
void blinkEyesOptimized();

// Move a single eye to a new position
void drawEyeMove(uint16_t xCurrent, uint16_t xPrevious, uint8_t isLeftEye);

// Dynamically move both eyes in a given direction
void moveEyesDynamic(uint8_t direction);

#endif // EYE_CONTROL_H
