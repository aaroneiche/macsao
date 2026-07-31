#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define BUFF_H 26
#define BUFF_W 46

//156 + 30

volatile uint8_t buffer[184];


void buffer_drawPixel(uint8_t x, uint8_t y, uint8_t color);
void buffer_drawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);
;
void buffer_drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color);
void buffer_drawFilledCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color);