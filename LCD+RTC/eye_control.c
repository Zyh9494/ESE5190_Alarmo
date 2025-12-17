#include "eye_control.h"
#include <util/delay.h>
#include "RA8875.h"

// Draw open eyes
void drawEyesOpen() {
    // Left eye
    RA8875_fillEllipse(250, 240, 90, 100, RA8875_WHITE);

    // Right eye
    RA8875_fillEllipse(550, 240, 90, 100, RA8875_WHITE);
}

// Clear open eyes
void clranEyesOpen() {
    // Left eye
    RA8875_fillEllipse(250, 240, 90, 100, RA8875_BLACK);

    // Right eye
    RA8875_fillEllipse(550, 240, 90, 100, RA8875_BLACK);
}

// Draw eyes with optimized state for blinking
void drawEyeStateOptimized(uint16_t shortAxis) {
    static uint16_t previousShortAxis = 100; // Cache for the previous short axis
    uint16_t longAxis = 90;                 // Long axis of the ellipse

    // Skip if the state has not changed
    if (shortAxis == previousShortAxis) {
        return;
    }

    // Clear previous eye state
    fillCrescent(250, 240, longAxis, previousShortAxis, longAxis, shortAxis, RA8875_BLACK, RA8875_BLACK, 0);
    fillCrescent(250, 240, longAxis, previousShortAxis, longAxis, shortAxis, RA8875_BLACK, RA8875_BLACK, 1);

    fillCrescent(550, 240, longAxis, previousShortAxis, longAxis, shortAxis, RA8875_BLACK, RA8875_BLACK, 0);
    fillCrescent(550, 240, longAxis, previousShortAxis, longAxis, shortAxis, RA8875_BLACK, RA8875_BLACK, 1);

    // Draw current eye state
    RA8875_fillEllipse(250, 240, longAxis, shortAxis, RA8875_WHITE); // Left eye
    RA8875_fillEllipse(550, 240, longAxis, shortAxis, RA8875_WHITE); // Right eye

    // Update the cache
    previousShortAxis = shortAxis;
}

// Optimized blinking function
void blinkEyesOptimized() {
    uint16_t shortAxisStates[9] = {100, 90, 80, 70, 60, 50, 40, 20, 10};

    // Close eyes
    for (uint8_t i = 0; i < 9; i++) {
        drawEyeStateOptimized(shortAxisStates[i]); // Update eye state
        _delay_ms(5);                             // Delay 5 ms
    }

    // Open eyes
    for (int8_t i = 8; i >= 0; i--) {
        drawEyeStateOptimized(shortAxisStates[i]); // Update eye state
        _delay_ms(10);                             // Delay 10 ms
    }
}

// Move the eye to a new position
void drawEyeMove(uint16_t xCurrent, uint16_t xPrevious, uint8_t isLeftEye) {
    uint16_t yCenter = 240;          // Y-coordinate of eye center
    uint16_t longAxisOuter = 90;     // Long axis of outer ellipse
    uint16_t shortAxisOuter = 100;   // Short axis of outer ellipse
    uint16_t longAxisInner = 80;     // Long axis of inner ellipse
    uint16_t shortAxisInner = 100;   // Short axis of inner ellipse

    if (xCurrent < xPrevious) {
        // Move left
        fillCrescent(xCurrent, yCenter, longAxisOuter, shortAxisOuter, longAxisInner, shortAxisInner, RA8875_WHITE, RA8875_WHITE, 2);
        fillCrescent(xPrevious, yCenter, longAxisOuter, shortAxisOuter, longAxisInner, shortAxisInner, RA8875_BLACK, RA8875_BLACK, 3);
    } else if (xCurrent > xPrevious) {
        // Move right
        fillCrescent(xCurrent, yCenter, longAxisOuter, shortAxisOuter, longAxisInner - 8, shortAxisInner, RA8875_WHITE, RA8875_WHITE, 3);
        fillCrescent(xPrevious, yCenter, longAxisOuter, shortAxisOuter, longAxisInner, shortAxisInner, RA8875_BLACK, RA8875_BLACK, 2);
    }
}

// Dynamically move the eyes
void moveEyesDynamic(uint8_t direction) {
    drawEyesOpen(); // Initialize eyes open
    uint16_t prevXLeft = 250, prevXRight = 550; // Initial positions
    uint16_t targetXLeft, targetXRight;         // Target positions
    uint16_t step = 5;                          // Step size for movement

    // Determine target positions based on direction
    if (direction == 0) {
        // Move left
        targetXLeft = 150;
        targetXRight = 450;
    } else {
        // Move right
        targetXLeft = 350;
        targetXRight = 650;
    }

    // Move to the target positions
    while ((direction == 0 && (prevXLeft > targetXLeft || prevXRight > targetXRight)) ||
           (direction == 1 && (prevXLeft < targetXLeft || prevXRight < targetXRight))) {
        if ((direction == 0 && prevXLeft > targetXLeft) || (direction == 1 && prevXLeft < targetXLeft)) {
            uint16_t nextXLeft = (direction == 0)
                                     ? (prevXLeft - step > targetXLeft ? prevXLeft - step : targetXLeft)
                                     : (prevXLeft + step < targetXLeft ? prevXLeft + step : targetXLeft);
            drawEyeMove(nextXLeft, prevXLeft, 1); // Update left eye
            prevXLeft = nextXLeft;               // Update previous position
        }
        if ((direction == 0 && prevXRight > targetXRight) || (direction == 1 && prevXRight < targetXRight)) {
            uint16_t nextXRight = (direction == 0)
                                      ? (prevXRight - step > targetXRight ? prevXRight - step : targetXRight)
                                      : (prevXRight + step < targetXRight ? prevXRight + step : targetXRight);
            drawEyeMove(nextXRight, prevXRight, 0); // Update right eye
            prevXRight = nextXRight;               // Update previous position
        }
        _delay_ms(5); // Delay 5 ms
    }

    // Pause for 1 second
    _delay_ms(1000);

    // Return to initial positions
    while (prevXLeft != 250 || prevXRight != 550) {
        if (prevXLeft != 250) {
            uint16_t nextXLeft = (prevXLeft < 250)
                                     ? (prevXLeft + step < 250 ? prevXLeft + step : 250)
                                     : (prevXLeft - step > 250 ? prevXLeft - step : 250);
            drawEyeMove(nextXLeft, prevXLeft, 1); // Update left eye
            prevXLeft = nextXLeft;
        }
        if (prevXRight != 550) {
            uint16_t nextXRight = (prevXRight < 550)
                                      ? (prevXRight + step < 550 ? prevXRight + step : 550)
                                      : (prevXRight - step > 550 ? prevXRight - step : 550);
            drawEyeMove(nextXRight, prevXRight, 0); // Update right eye
            prevXRight = nextXRight;
        }
        _delay_ms(5); // Delay 5 ms
    }
}
