
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "buffer.h"

void buffer_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    uint16_t addr;

    /* clip */
    if (x >= BUFF_W)
        return;
    if (y >= BUFF_H)
        return;

    /* compute buffer address */
    // printf("%u + %u * (%u / 8) [%u]\n",x,BUFF_W,y,y/8);

    addr = x + BUFF_W * (y / 8);

    // /* set/clear bit in buffer */
    if (color) {
        buffer[addr] |= (1 << (y & 7));
    } else {
        buffer[addr] &= ~(1 << (y & 7));
    }
}

void buffer_drawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color)
{
	// clipping
	if((x >= BUFF_W) || (y >= BUFF_H)) return;
	if((x+w-1) >= BUFF_H)  w = BUFF_W-x;

	while (w--)
	{
        buffer_drawPixel(x++, y, color);
	}
}


void buffer_drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color) {

    int16_t x_pos = -radius;
    int16_t y_pos = 0;
    int16_t err = 2 - 2 * radius;
    int16_t e2;

    do {
        buffer_drawPixel(x - x_pos, y + y_pos, color);
        buffer_drawPixel(x + x_pos, y + y_pos, color);
        buffer_drawPixel(x + x_pos, y - y_pos, color);
        buffer_drawPixel(x - x_pos, y - y_pos, color);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if(-x_pos == y_pos && e2 <= x_pos) {
              e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

void buffer_drawFilledCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color) {

    int16_t x_pos = -radius;
    int16_t y_pos = 0;
    int16_t err = 2 - 2 * radius;
    int16_t e2;

    do {
        buffer_drawPixel(x - x_pos, y + y_pos, color);
        buffer_drawPixel(x + x_pos, y + y_pos, color);
        buffer_drawPixel(x + x_pos, y - y_pos, color);
        buffer_drawPixel(x - x_pos, y - y_pos, color);
        buffer_drawFastHLine(x + x_pos, y + y_pos, 2 * (-x_pos) + 1, color);
        buffer_drawFastHLine(x + x_pos, y - y_pos, 2 * (-x_pos) + 1, color);

        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if(-x_pos == y_pos && e2 <= x_pos) {
              e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

void buffer_clear(uint8_t color){
    memset(buffer, color ? 0xFF : 0x00, sizeof(buffer));
}