
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

void buffer_clear(uint8_t color){
    memset(buffer, color ? 0xFF : 0x00, sizeof(buffer));
}