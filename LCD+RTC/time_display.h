/* 
 * File:   time_display.h
 * Author: supeng2
 *
 * Created on November 30, 2024, 2:14 PM
 */

#ifndef TIME_DISPLAY_H
#define TIME_DISPLAY_H

#include <stdint.h>

// ????
void drawHexSegmentHorizontal(uint16_t x, uint16_t y, uint16_t length, uint16_t height, uint16_t color);
void drawHexSegmentVertical(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void drawDigit(uint8_t digit, uint16_t x, uint16_t y, uint16_t length, uint16_t height, uint16_t color);
void drawColon(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);
void displayTime(uint8_t hours, uint8_t minutes, uint16_t color);
void flashHours(uint8_t x, uint8_t y, uint16_t color);
void flashMinutes(uint8_t x, uint8_t y, uint16_t color);

#endif // TIME_DISPLAY_H
