/* 
 * File:   time_display.c
 * Author: supeng2
 *
 * Created on November 30, 2024, 2:14 PM
 */

#define F_CPU 16000000UL
#include "time_display.h"
#include "RA8875.h"
#include <util/delay.h>

// Draw a horizontal hex segment (used in digit display)
void drawHexSegmentHorizontal(uint16_t x, uint16_t y, uint16_t length, uint16_t height, uint16_t color) {
    // Draw rectangle for the segment
    RA8875_fillRect(x, y, length, height, color);

    // Draw left triangle
    uint16_t triangleLeftX[3] = {x, x, x - height / 2};
    uint16_t triangleLeftY[3] = {y, y + height, y + height / 2};
    RA8875_fillTriangle(triangleLeftX[0], triangleLeftY[0], triangleLeftX[1], triangleLeftY[1], triangleLeftX[2], triangleLeftY[2], color);

    // Draw right triangle
    uint16_t triangleRightX[3] = {x + length, x + length, x + length + height / 2};
    uint16_t triangleRightY[3] = {y, y + height, y + height / 2};
    RA8875_fillTriangle(triangleRightX[0], triangleRightY[0], triangleRightX[1], triangleRightY[1], triangleRightX[2], triangleRightY[2], color);
}

// Draw a vertical hex segment (used in digit display)
void drawHexSegmentVertical(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    // Draw rectangle for the segment
    RA8875_fillRect(x, y, width, height, color);

    // Draw top triangle
    uint16_t triangleTopX[3] = {x, x + width, x + width / 2};
    uint16_t triangleTopY[3] = {y, y, y - width / 2};
    RA8875_fillTriangle(triangleTopX[0], triangleTopY[0], triangleTopX[1], triangleTopY[1], triangleTopX[2], triangleTopY[2], color);

    // Draw bottom triangle
    uint16_t triangleBottomX[3] = {x, x + width, x + width / 2};
    uint16_t triangleBottomY[3] = {y + height, y + height, y + height + width / 2};
    RA8875_fillTriangle(triangleBottomX[0], triangleBottomY[0], triangleBottomX[1], triangleBottomY[1], triangleBottomX[2], triangleBottomY[2], color);
}

// Draw a single digit using hex segments
void drawDigit(uint8_t digit, uint16_t x, uint16_t y, uint16_t length, uint16_t height, uint16_t color) {
    uint16_t segmentWidth = height;                // Width of a segment
    uint16_t segmentHeight = length - 2 * height;  // Height of a segment

    switch (digit) {
        case 0: // Draw "0"
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y, length - segmentWidth - 4, segmentWidth, color); // A
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // B
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + 2 * segmentHeight + 2 * segmentWidth + 20, length - segmentWidth - 4, segmentWidth, color); // D
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // E
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // F
            break;
            
        case 1: //  "1"
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // B
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            break;

        case 2: // "2"
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y, length - segmentWidth - 4, segmentWidth, color); // A
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // B
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + segmentHeight + 10 + segmentWidth, length - segmentWidth - 4, segmentWidth, color); // G
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // E
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + 2 * segmentHeight + 2 * segmentWidth + 20, length - segmentWidth - 4, segmentWidth, color); // D
            break;

        case 3: //  "3"
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y, length - segmentWidth - 4, segmentWidth, color); // A
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // B
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + segmentHeight + 10 + segmentWidth, length - segmentWidth - 4, segmentWidth, color); // G
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + 2 * segmentHeight + 2 * segmentWidth + 20, length - segmentWidth - 4, segmentWidth, color); // D
            break;

        case 4: // "4"
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // B
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + segmentHeight + 10 + segmentWidth, length - segmentWidth - 4, segmentWidth, color); // G
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // F
            break;

        case 5: //  "5"
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y, length - segmentWidth - 4, segmentWidth, color); // A
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + segmentHeight + 10 + segmentWidth, length - segmentWidth - 4, segmentWidth, color); // G
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // F
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + 2 * segmentHeight + 2 * segmentWidth + 20, length - segmentWidth - 4, segmentWidth, color); // D
            break;

        case 6: // "6"
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y, length - segmentWidth - 4, segmentWidth, color); // A
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // E
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // F
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + 2 * segmentHeight + 2 * segmentWidth + 20, length - segmentWidth - 4, segmentWidth, color); // D
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + segmentHeight + 10 + segmentWidth, length - segmentWidth - 4, segmentWidth, color); // G
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            break;

        case 7: // "7"
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y, length - segmentWidth - 4, segmentWidth, color); // A
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // B
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            break;

        case 8: // "8"
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y, length - segmentWidth - 4, segmentWidth, color); // A
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // B
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + 2 * segmentHeight + 2 * segmentWidth + 20, length - segmentWidth - 4, segmentWidth, color); // D
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // E
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // F
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + segmentHeight + 10 + segmentWidth, length - segmentWidth - 4, segmentWidth, color); // G
            break;

        case 9: // "9"
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y, length - segmentWidth - 4, segmentWidth, color); // A
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // B
            drawHexSegmentVertical(x + length - segmentWidth / 2, y + segmentWidth + 5 + segmentHeight + segmentWidth + 10, segmentWidth, segmentHeight, color); // C
            drawHexSegmentHorizontal(x + segmentWidth / 2 + 2, y + segmentHeight + 10 + segmentWidth, length - segmentWidth - 4, segmentWidth, color); // G
            drawHexSegmentVertical(x - segmentWidth / 2, y + segmentWidth + 5, segmentWidth, segmentHeight, color); // F
            break;
        default:
            break;
    }
}

// Draw colon symbol
void drawColon(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) {
    // Draw top circle of colon
    RA8875_fillCircle(x, y - radius * 2, radius, color);
    // Draw bottom circle of colon
    RA8875_fillCircle(x, y + radius * 2, radius, color);
}

// Display time in HH:MM format
void displayTime(uint8_t x, uint8_t y, uint16_t color) {
    uint8_t hourTens = x / 10;     // Tens place of hours
    uint8_t hourUnits = x % 10;    // Units place of hours
    uint8_t minuteTens = y / 10;   // Tens place of minutes
    uint8_t minuteUnits = y % 10;  // Units place of minutes

    // Draw each digit
    drawDigit(hourTens, 70, 100, 110, 20, color);    // Hour tens
    drawDigit(hourUnits, 240, 100, 110, 20, color);  // Hour units
    drawDigit(minuteTens, 450, 100, 110, 20, color); // Minute tens
    drawDigit(minuteUnits, 620, 100, 110, 20, color); // Minute units
    
    drawColon(400, 240, 10, color); // Draw colon
}

// Clear the time display
void clearTime() {
    uint16_t black = 0x0000; // Black color for clearing

    // Clear digits
    drawDigit(8, 70, 100, 110, 20, black);   // Hour tens
    drawDigit(8, 240, 100, 110, 20, black);  // Hour units
    drawDigit(8, 450, 100, 110, 20, black);  // Minute tens
    drawDigit(8, 620, 100, 110, 20, black);  // Minute units

    // Clear colon
    drawColon(400, 240, 10, black);
}

// Clear only the hour part of the time display
void clearHourTime() {
    uint16_t black = 0x0000; // Black color for clearing

    // Clear hour digits
    drawDigit(8, 70, 100, 110, 20, black);   // Hour tens
    drawDigit(8, 240, 100, 110, 20, black);  // Hour units
}

// Clear only the minute part of the time display
void clearMinuteTime() {
    uint16_t black = 0x0000; // Black color for clearing

    // Clear minute digits
    drawDigit(8, 450, 100, 110, 20, black);  // Minute tens
    drawDigit(8, 620, 100, 110, 20, black);  // Minute units
}

// Flash the hour part of the time display
void flashHours(uint8_t x, uint8_t y, uint16_t color) {
    uint8_t hourTens = x / 10;     // Tens place of hours
    uint8_t hourUnits = x % 10;    // Units place of hours

    // Turn off hour digits
    drawDigit(8, 70, 100, 110, 20, 0x0000);  // Black
    drawDigit(8, 240, 100, 110, 20, 0x0000); // Black
    _delay_ms(500); // Delay for 500 ms

    // Turn on hour digits
    drawDigit(hourTens, 70, 100, 110, 20, color);    // Hour tens
    drawDigit(hourUnits, 240, 100, 110, 20, color);  // Hour units
    _delay_ms(500); // Delay for 500 ms
}

// Flash the minute part of the time display
void flashMinutes(uint8_t x, uint8_t y, uint16_t color) {
    uint8_t minuteTens = y / 10;   // Tens place of minutes
    uint8_t minuteUnits = y % 10;  // Units place of minutes

    // Turn off minute digits
    drawDigit(8, 450, 100, 110, 20, 0x0000);  // Black
    drawDigit(8, 620, 100, 110, 20, 0x0000);  // Black
    _delay_ms(500); // Delay for 500 ms

    // Turn on minute digits
    drawDigit(minuteTens, 450, 100, 110, 20, color); // Minute tens
    drawDigit(minuteUnits, 620, 100, 110, 20, color); // Minute units
    _delay_ms(500); // Delay for 500 ms
}
