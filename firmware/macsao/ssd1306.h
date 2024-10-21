/*
 * Single-File-Header for using SPI OLED
 * 05-05-2023 E. Brombaugh
 * 
 * Updated By Aaron Eiche for MacSAO
 * 09-27-2024
 */

#ifndef _SSD1306_H
#define _SSD1306_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "font_8x8.h"
#include "tiles.h"
// #include "font.c"
#include "narrowFont.c"
#include "buffer.h"

// comfortable packet size for this OLED
#define SSD1306_PSZ 32

// characteristics of each type
#if !defined(SSD1306_64X32) && !defined(SSD1306_64X48) && !defined(SSD1306_128X32) && !defined(SSD1306_128X64)
#error "Please define the SSD1306_WXH resolution used in your application"
#endif

#ifdef SSD1306_64X48
#define SSD1306_W 64
#define SSD1306_H 48
#define SSD1306_FULLUSE
// #define SSD1306_OFFSET 32
#define SSD1306_OFFSET 32
#endif

#ifdef SSD1306_64X32
#define SSD1306_W 64
#define SSD1306_H 32
#define SSD1306_FULLUSE
#define SSD1306_OFFSET 32
#endif

#ifdef SSD1306_128X32
#define SSD1306_W 128
#define SSD1306_H 32
#define SSD1306_OFFSET 0
#endif

#ifdef SSD1306_128X64
#define SSD1306_W 128
#define SSD1306_H 64
#define SSD1306_FULLUSE
#define SSD1306_OFFSET 0
#endif

/*
 * send OLED command byte
 */
uint8_t ssd1306_cmd(uint8_t cmd)
{
	ssd1306_pkt_send(&cmd, 1, 1);
	return 0;
}

/*
 * send OLED data packet (up to 32 bytes)
 */
uint8_t ssd1306_data(uint8_t *data, uint8_t sz)
{
	ssd1306_pkt_send(data, sz, 0);
	return 0;
}

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_TERMINATE_CMDS 0xFF

#define SSD1306_XFLIP_OFF 0xA1
#define SSD1306_XFLIP 0xA0

#define SSD1306_YFLIP 0xC0
#define SSD1306_YFLIP_OFF 0xC8

/* choose VCC mode */
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2
//#define vccstate SSD1306_EXTERNALVCC
#define vccstate SSD1306_SWITCHCAPVCC

// OLED initialization commands for 128x32
const uint8_t ssd1306_init_array[] =
	{
		SSD1306_DISPLAYOFF,			// 0xAE
		SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
		0x80,						// the suggested ratio 0x80
		SSD1306_SETMULTIPLEX,		// 0xA8
// #if defined(SSD1306_64X32) || defined(SSD1306_64x48)
		0x2F, // 47 = the height of the 64x48 display.
// #else
// 		0x3F, // for 128-wide displays
// #endif
		SSD1306_SETDISPLAYOFFSET,	// 0xD3
		0x00,						// no offset
		SSD1306_SETSTARTLINE | 0x00, // 0x40 | line
		SSD1306_CHARGEPUMP,			// 0x8D
		0x14,						// enable?
		SSD1306_MEMORYMODE,			// 0x20
		0x00,						// 0x0 act like ks0108
		SSD1306_SEGREMAP | 0x1,		// 0xA0 | bit
		SSD1306_COMSCANDEC,
		SSD1306_SETCOMPINS,	 // 0xDA
		0x12,				 //
		SSD1306_SETCONTRAST, // 0x81
		0x8F,
		SSD1306_SETPRECHARGE, // 0xd9
		0xF1,
		SSD1306_SETVCOMDETECT, // 0xDB
		0x40,
		SSD1306_DISPLAYALLON_RESUME, // 0xA4
		// SSD1306_YFLIP,
		// SSD1306_XFLIP,
		SSD1306_NORMALDISPLAY, // 0xA6
		SSD1306_DISPLAYON,	   // 0xAF --turn on oled panel
		SSD1306_TERMINATE_CMDS // 0xFF --fake command to mark end
};

// the display buffer
uint8_t ssd1306_buffer[SSD1306_W*SSD1306_H/8];

/*
 * set the buffer to a color
 */
void ssd1306_setbuf(uint8_t color)
{
	memset(ssd1306_buffer, color ? 0xFF : 0x00, sizeof(ssd1306_buffer));
}

#ifndef SSD1306_FULLUSE
/*
 * expansion array for OLED with every other row unused
 */
const uint8_t expand[16] =
{
	0x00,0x02,0x08,0x0a,
	0x20,0x22,0x28,0x2a,
	0x80,0x82,0x88,0x8a,
	0xa0,0xa2,0xa8,0xaa,
};
#endif

/*
 * Send the frame buffer
 */
void ssd1306_refresh(void)
{
	uint16_t i;
	
	ssd1306_cmd(SSD1306_COLUMNADDR);
	ssd1306_cmd(SSD1306_OFFSET);   // Column start address (0 = reset)
	ssd1306_cmd(SSD1306_OFFSET+SSD1306_W-1); // Column end address (127 = reset)
	

	// #if defined SSD1306_64X48

	ssd1306_cmd(SSD1306_PAGEADDR);
	ssd1306_cmd(0); // Page start address (0 = reset) (2 = offset for 48px display)
	ssd1306_cmd(5); // Page end address

#ifdef SSD1306_FULLUSE
	/* for fully used rows just plow thru everything */
	for(i=0;i<sizeof(ssd1306_buffer);i+=SSD1306_PSZ)
	{
		/* send PSZ block of data */
		ssd1306_data(&ssd1306_buffer[i], SSD1306_PSZ);
	}
#else
	/* for displays with odd rows unused expand bytes */
	uint8_t tbuf[SSD1306_PSZ], j, k;
    for(i=0;i<sizeof(ssd1306_buffer);i+=128)
	{
		/* low nybble */
		for(j=0;j<128;j+=SSD1306_PSZ)
		{
			for(k=0;k<SSD1306_PSZ;k++)
				tbuf[k] = expand[ssd1306_buffer[i+j+k]&0xf];
			
			/* send PSZ block of data */
			ssd1306_data(tbuf, SSD1306_PSZ);
		}
		
		/* high nybble */
		for(j=0;j<128;j+=SSD1306_PSZ)
		{
			for(k=0;k<SSD1306_PSZ;k++)
				tbuf[k] = expand[(ssd1306_buffer[i+j+k]>>4)&0xf];
			
			/* send PSZ block of data */
			ssd1306_data(tbuf, SSD1306_PSZ);
		}
	}
#endif
}

/*
 * plot a pixel in the buffer
 */
void ssd1306_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
	uint16_t addr;
	
	/* clip */
	if(x >= SSD1306_W)
		return;
	if(y >= SSD1306_H)
		return;
	
	/* compute buffer address */
	addr = x + SSD1306_W*(y/8);
	
	/* set/clear bit in buffer */
	if(color)
		ssd1306_buffer[addr] |= (1<<(y&7));
	else
		ssd1306_buffer[addr] &= ~(1<<(y&7));
}

/*
 * plot a pixel in the buffer
 */
void ssd1306_xorPixel(uint8_t x, uint8_t y)
{
	uint16_t addr;
	
	/* clip */
	if(x >= SSD1306_W)
		return;
	if(y >= SSD1306_H)
		return;
	
	/* compute buffer address */
	addr = x + SSD1306_W*(y/8);
	
	ssd1306_buffer[addr] ^= (1<<(y&7));
}

/*
 * draw a an image from an array, directly into to the display buffer
 * the color modes allow for overwriting and even layering (sprites!)
 */
void ssd1306_drawImage(uint8_t x, uint8_t y, const unsigned char* input, uint8_t width, uint8_t height, uint8_t color_mode) {
	uint8_t x_absolute;
	uint8_t y_absolute;
	uint8_t pixel;
	uint8_t bytes_to_draw = width / 8;
	uint16_t buffer_addr;

	for (uint8_t line = 0; line < height; line++) {
		y_absolute = y + line;
		if (y_absolute >= SSD1306_H) {
			break;
		}

		// SSD1306 is in vertical mode, yet we want to draw horizontally, which necessitates assembling the output bytes from the input data
		// bitmask for current pixel in vertical (output) byte
		uint8_t v_mask = 1 << (y_absolute & 7);

		for (uint8_t byte = 0; byte < bytes_to_draw; byte++) {
			uint8_t input_byte = input[byte + line * bytes_to_draw];

			for (pixel = 0; pixel < 8; pixel++) {
				x_absolute = x + 8 * (bytes_to_draw - 1 - byte) + pixel;
				if (x_absolute >= SSD1306_W) {
					break;
				}
				// looking at the horizontal display, we're drawing bytes bottom to top, not left to right, hence y / 8
				buffer_addr = x_absolute + SSD1306_W * (y_absolute / 8);
				// state of current pixel
				uint8_t input_pixel = input_byte & (1 << pixel);

				switch (color_mode) {
					case 0:
						// write pixels as they are
						ssd1306_buffer[buffer_addr] = (ssd1306_buffer[buffer_addr] & ~v_mask) | (input_pixel ? v_mask : 0);
						break;
					case 1:
						// write pixels after inversion
						ssd1306_buffer[buffer_addr] = (ssd1306_buffer[buffer_addr] & ~v_mask) | (!input_pixel ? v_mask : 0);
						break;
					case 2:
						// 0 clears pixel
						ssd1306_buffer[buffer_addr] &= input_pixel ? 0xFF : ~v_mask;
						break;
					case 3:
						// 1 sets pixel
						ssd1306_buffer[buffer_addr] |= input_pixel ? v_mask : 0;
						break;
					case 4:
						// 0 sets pixel
						ssd1306_buffer[buffer_addr] |= !input_pixel ? v_mask : 0;
						break;
					case 5:
						// 1 clears pixel
						ssd1306_buffer[buffer_addr] &= input_pixel ? ~v_mask : 0xFF;
						break;
				}
			}
			#if SSD1306_LOG_IMAGE == 1
			printf("%02x ", input_byte);
			#endif
		}
		#if SSD1306_LOG_IMAGE == 1
		printf("\n\r");
		#endif
	}
}

/*
 *  fast vert line
 */
void ssd1306_drawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color)
{
	// clipping
	if((x >= SSD1306_W) || (y >= SSD1306_H)) return;
	if((y+h-1) >= SSD1306_H) h = SSD1306_H-y;
	while(h--)
	{
        ssd1306_drawPixel(x, y++, color);
	}
}

/*
 *  fast horiz line
 */
void ssd1306_drawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color)
{
	// clipping
	if((x >= SSD1306_W) || (y >= SSD1306_H)) return;
	if((x+w-1) >= SSD1306_W)  w = SSD1306_W-x;

	while (w--)
	{
        ssd1306_drawPixel(x++, y, color);
	}
}

/*
 * abs() helper function for line drawing
 */
int16_t gfx_abs(int16_t x)
{
	return (x<0) ? -x : x;
}

/*
 * swap() helper function for line drawing
 */
void gfx_swap(uint16_t *z0, uint16_t *z1)
{
	uint16_t temp = *z0;
	*z0 = *z1;
	*z1 = temp;
}

/*
 * Bresenham line draw routine swiped from Wikipedia
 */
void ssd1306_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color)
{
	int16_t steep;
	int16_t deltax, deltay, error, ystep, x, y;

	/* flip sense 45deg to keep error calc in range */
	steep = (gfx_abs(y1 - y0) > gfx_abs(x1 - x0));

	if(steep)
	{
		gfx_swap(&x0, &y0);
		gfx_swap(&x1, &y1);
	}

	/* run low->high */
	if(x0 > x1)
	{
		gfx_swap(&x0, &x1);
		gfx_swap(&y0, &y1);
	}

	/* set up loop initial conditions */
	deltax = x1 - x0;
	deltay = gfx_abs(y1 - y0);
	error = deltax/2;
	y = y0;
	if(y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	/* loop x */
	for(x=x0;x<=x1;x++)
	{
		/* plot point */
		if(steep)
			/* flip point & plot */
			ssd1306_drawPixel(y, x, color);
		else
			/* just plot */
			ssd1306_drawPixel(x, y, color);

		/* update error */
		error = error - deltay;

		/* update y */
		if(error < 0)
		{
			y = y + ystep;
			error = error + deltax;
		}
	}
}

/*
 *  draws a circle
 */
void ssd1306_drawCircle(int16_t x, int16_t y, int16_t radius, int8_t color)
{
    /* Bresenham algorithm */
    int16_t x_pos = -radius;
    int16_t y_pos = 0;
    int16_t err = 2 - 2 * radius;
    int16_t e2;

    do {
        ssd1306_drawPixel(x - x_pos, y + y_pos, color);
        ssd1306_drawPixel(x + x_pos, y + y_pos, color);
        ssd1306_drawPixel(x + x_pos, y - y_pos, color);
        ssd1306_drawPixel(x - x_pos, y - y_pos, color);
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

/*
 *  draws a filled circle
 */
void ssd1306_fillCircle(int16_t x, int16_t y, int16_t radius, int8_t color)
{
    /* Bresenham algorithm */
    int16_t x_pos = -radius;
    int16_t y_pos = 0;
    int16_t err = 2 - 2 * radius;
    int16_t e2;

    do {
        ssd1306_drawPixel(x - x_pos, y + y_pos, color);
        ssd1306_drawPixel(x + x_pos, y + y_pos, color);
        ssd1306_drawPixel(x + x_pos, y - y_pos, color);
        ssd1306_drawPixel(x - x_pos, y - y_pos, color);
        ssd1306_drawFastHLine(x + x_pos, y + y_pos, 2 * (-x_pos) + 1, color);
        ssd1306_drawFastHLine(x + x_pos, y - y_pos, 2 * (-x_pos) + 1, color);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if(-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if(e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while(x_pos <= 0);
}

/*
 *  draw a rectangle
 */
void ssd1306_drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
	ssd1306_drawFastVLine(x, y, h, color);
	ssd1306_drawFastVLine(x+w-1, y, h, color);
	ssd1306_drawFastHLine(x, y, w, color);
	ssd1306_drawFastHLine(x, y+h-1, w, color);
}

/*
 * fill a rectangle
 */
void ssd1306_fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
	uint8_t m, n=y, iw = w;
	
	/* scan vertical */
	while(h--)
	{
		m=x;
		w=iw;
		/* scan horizontal */
		while(w--)
		{
			/* invert pixels */
			ssd1306_drawPixel(m++, n, color);
		}
		n++;
	}
}

/*
 * invert a rectangle in the buffer
 */
void ssd1306_xorrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	uint8_t m, n=y, iw = w;
	
	/* scan vertical */
	while(h--)
	{
		m=x;
		w=iw;
		/* scan horizontal */
		while(w--)
		{
			/* invert pixels */
			ssd1306_xorPixel(m++, n);
		}
		n++;
	}
}

/*
 * Draw character to the display buffer
 */
void ssd1306_drawchar(uint8_t x, uint8_t y, uint8_t chr, uint8_t color)
{
	uint16_t i, j, col;
	uint8_t d;
	
	for(i=0;i<8;i++)
	{
		d = fontdata[(chr<<3)+i];
		for(j=0;j<8;j++)
		{
			if(d&0x80)
				col = color;
			else
				col = (~color)&1;
			
			ssd1306_drawPixel(x+j, y+i, col);
			
			// next bit
			d <<= 1;
		}
	}
}

/*
 * draw a string to the display
 */
void ssd1306_drawstr(uint8_t x, uint8_t y, char *str, uint8_t color)
{
	uint8_t c;
	
	while((c=*str++))
	{
		ssd1306_drawchar(x, y, c, color);
		x += 8;
		if(x>120)
			break;
	}
}

/*
 * enum for font size
 */
typedef enum {
    fontsize_8x8 = 1,
    fontsize_16x16 = 2,
    fontsize_32x32 = 4,
	fontsize_64x64 = 8,
} font_size_t;

/*
 * Draw character to the display buffer, scaled to size
 */
void ssd1306_drawchar_sz(uint8_t x, uint8_t y, uint8_t chr, uint8_t color, font_size_t font_size)
{
    uint16_t i, j, col;
    uint8_t d;

    // Determine the font scale factor based on the font_size parameter
    uint8_t font_scale = (uint8_t)font_size;

    // Loop through each row of the font data
    for (i = 0; i < 8; i++)
    {
        // Retrieve the font data for the current row
        d = fontdata[(chr << 3) + i];

        // Loop through each column of the font data
        for (j = 0; j < 8; j++)
        {
            // Determine the color to draw based on the current bit in the font data
            if (d & 0x80)
                col = color;
            else
                col = (~color) & 1;

            // Draw the pixel at the original size and scaled size using nested for-loops
            for (uint8_t k = 0; k < font_scale; k++) {
                for (uint8_t l = 0; l < font_scale; l++) {
                    ssd1306_drawPixel(x + (j * font_scale) + k, y + (i * font_scale) + l, col);
                }
            }

            // Move to the next bit in the font data
            d <<= 1;
        }
    }
}

/*
 * draw a string to the display buffer, scaled to size
 */
void ssd1306_drawstr_sz(uint8_t x, uint8_t y, char *str, uint8_t color, font_size_t font_size)
{
	uint8_t c;
	
	while((c=*str++))
	{
		ssd1306_drawchar_sz(x, y, c, color, font_size);
		x += 8 * font_size;
		if(x>128 - 8 * font_size)
			break;
	}
}

/*
 * initialize I2C and OLED
 */
uint8_t ssd1306_init(void)
{
	// pulse reset
	ssd1306_rst();
	
	// initialize OLED
	uint8_t *cmd_list = (uint8_t *)ssd1306_init_array;
	while(*cmd_list != SSD1306_TERMINATE_CMDS)
	{
		if(ssd1306_cmd(*cmd_list++))
			return 1;
	}
	
	// clear display
	ssd1306_setbuf(0);
	ssd1306_refresh();
	
	return 0;
}

// Constants

// =============== Custom Macintosh Screens =======================

//  Disk Icon position.
#define dx 54
#define dy 6

// Trash Icon position and size
#define tx 54
#define ty 34
#define th 9
#define tw 7

int8_t menuData[][2] = {
	{2, 2},	 // Apple menu
	{6, 4},	 // File menu
	{12, 4}, // Edit menu
	{18, 4}, // View menu
	{24, 6}, // Special menu
};

/* 
Draw the background for the Mac Desktop
 */
void background(void)
{
	for (int w = 0; w < 8; w++)
	{
		for (int h = 0; h < 6; h++)
		ssd1306_drawImage(w*8, h*8, grey, 8, 8, 0);
	}
}

/* 
Displays the menubar. 
@menus: displays the basic menus on the bar
@selected: "opens" the given menu (inverts, displays items)
@menuItem: highlights the given item in the "open menu"
*/
void menubar(int menus, int selected, int menuItem ) {
	// display.setDrawColor(1);
	// display.drawBox(0, 0, 64, 4);

	ssd1306_fillRect(0, 0, 64, 4, 1);
	
	ssd1306_drawPixel(0, 0, 0);
	ssd1306_drawPixel(63, 0, 0);

	//Generate the menus.
	if(menus) {
		for(uint8_t m = 0; m <= sizeof(menus); m++) {
			uint8_t c = (m == selected) ? 1 : 0;
			if(c == 1) {
				//Selected at menu
				ssd1306_fillRect(menuData[m][0]-1,0, menuData[m][1] + 2, 4, 0);

				//open menu
				ssd1306_drawRect(menuData[m][0] - 1, 4, 14, (m%2==0? 21:25), 0);
 
				// Draw 5 or 6 menu items. If menuItem is set, invert colors
				for(int i = 0; i < (m%2==0 ? 5:6); i++) {
					int oc = (menuItem == i) ? 1 : 0; 

					ssd1306_fillRect(menuData[m][0], i * 4 + 4, 12, 4, !oc); //option
					ssd1306_fillRect(menuData[m][0] +1 , i * 4 + 5, 6, 2, oc); //option text
				}
			}
			ssd1306_fillRect(menuData[m][0], 1, menuData[m][1], 2, c);
		}
	}
}

void window(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int scrollBars) {
	//Draw window space
	ssd1306_drawRect(x, y, w, h, 0);
	ssd1306_fillRect(x + 1, y + 1, w - 2, h - 2, 1);

	//Closebox
	ssd1306_drawRect(x + 2, y + 2, 3, 3, 0);
	ssd1306_drawFastHLine(x + 6, y + 2, w - 8, 0);
	ssd1306_drawFastHLine(x + 6, y + 4, w - 8, 0);

	uint8_t rstart = x + (w / 2) - 7;

	ssd1306_fillRect(rstart, y + 2, 16, 3, 1);
	ssd1306_drawFastHLine(x, y + 6, w, 0);

	ssd1306_drawFastVLine(rstart + 1, y + 2, 3, 0);
	ssd1306_drawFastVLine(rstart + 3, y + 2, 3, 0);
	ssd1306_drawPixel(rstart + 2, y + 3, 0);
	ssd1306_drawFastVLine(rstart + 5, y + 3, 2, 0);
	ssd1306_drawFastVLine(rstart + 7, y + 3, 2, 0);

	ssd1306_drawFastVLine(rstart + 9,  y + 2, 3, 0);
	ssd1306_drawPixel(rstart + 10, y + 3, 0);
	ssd1306_drawFastVLine(rstart + 11, y + 3, 2, 0);
	ssd1306_drawFastVLine(rstart + 13, y + 2, 3, 0);
	ssd1306_drawPixel(rstart + 14, y + 3, 0);

	//Horizontal Scrollbars
	if(scrollBars & 0x01) {
		ssd1306_drawFastHLine(x, y + h - 5, w, 0);
		ssd1306_drawFastVLine(x + 4, y + h - 5, 5, 0);
	}

	// Vertical Scrollbars
	if(scrollBars & 0x02) {
		ssd1306_drawFastVLine(x + w - 5, y + 6, h - 6, 0);
		ssd1306_drawFastHLine(x + w - 5, y + 10, 4, 0);
	}
}

void desktop()
{
	background();
	Delay_Ms(1); //For some reason, we get hung up if this isn't here.
	
	drawIcon(54, 6, 1, 0);

		// Trash Can
	ssd1306_drawRect(tx, ty, tw, th, 0);
	ssd1306_fillRect(tx + 1, ty + 1, tw - 2, th - 2, 1);

	ssd1306_drawFastVLine(tx + 2, ty + 2, 5, 0);
	ssd1306_drawFastVLine(tx + 4, ty + 2, 5, 0);

	ssd1306_drawFastHLine(tx + 2, ty - 1, 3, 0);
	ssd1306_fillRect(tx - 2, ty + th, tw + 4, 4, 1);

	//Menubar
	menubar(1, -1, -1);

	// window(5, 7, 44, 35, 3); // Scrollbars: 1 = horizontal, 2 = vertical, 3 = both.
};
/* 
icon:
1 = disk
2 = folder
3 = file
4 = trash

states:
1 unselected
2 selected
3 opened
*/
void drawIcon(uint8_t x, uint8_t y, uint8_t ic, uint8_t state)
{

	switch(ic) {
		case 1:
			ssd1306_drawImage(x, y, disk, 8, 8, 0);
			ssd1306_fillRect(x - 2, y + 9, 12, 4, 1);
			ssd1306_fillRect(x -1, y + 9, 10, 3, 0);

			ssd1306_drawPixel(x + 1, y + 9, 1);
			ssd1306_drawPixel(x + 4, y + 9, 1);
			ssd1306_drawPixel(x + 7, y + 9, 1);

			ssd1306_drawPixel(x + 1, y + 11, 1);
			ssd1306_drawPixel(x + 5, y + 9, 1);
			ssd1306_drawPixel(x + 6, y + 11, 1);

			break;
	}



	// uint8_t *icon;
	// switch(ic) {
	// 	case 1:
	// 		icon = (state) ? disk : diskOpen;
	// 	break;
	// 	case 2:
	// 		icon = (state) ? folder : folderOpen;
	// 	break;
	// 	case 3:
	// 		icon = file;
	// 	break;
	// 	case 4: 
	// 		icon = trash;
	// 	break;
	// }



	// Disk image
	// ssd1306_drawImage(dx, dy, disk, 8, 8, 0);
	// ssd1306_fillRect(dx - 2, dy + th, tw + 4, 4, 1);

	// ssd1306_fillRect(dx - 1, dy + th + 1, tw + 2, 2, 0);

	// ssd1306_drawPixel(dx, dy + th + 1, 0);
	// ssd1306_drawFastVLine(dx - 1, dy + th + 1, 2, 0);
	// ssd1306_drawPixel(dx + 2, dy + th + 1, 0);
	// ssd1306_drawFastVLine(dx + 1, dy + th + 1, 2, 0);

	// ssd1306_drawPixel(dx + 4, dy + th + 2, 0);
	// ssd1306_drawFastVLine(dx + 4, dy + th + 1, 2, 0);

	// ssd1306_drawPixel(dx + 6, dy + th + 1, 0);
	// ssd1306_drawFastVLine(dx + 7, dy + th + 1, 2, 0);
}

void mainWindow() {

	// desktop();
	window(5, 7, 44, 35, 3);
	ssd1306_drawImage(8, 16, appIcon, 8, 8, 0);

	ssd1306_drawImage(20, 16, appIcon2, 8, 8, 0);
}


void openDisk() {
	ssd1306_drawImage(dx, dy, diskOpen, 8, 8, 0);
	ssd1306_fillRect(dx - 2, dy + th, tw + 4, 3, 0);
}

void macPaint(uint8_t selectedTool) {
	background();
	menubar(1,-1,0);
	window(14, 6, 48, 34, 0);

	//Tools Pallette
	ssd1306_fillRect(2,6,10,30,0);
	for (int i = 0; i < 14; i++)
	{
		int boxC = (i + 1 == selectedTool) ? 0 : 1;
		int pix = (i + 1 == selectedTool) ? 1 : 0;

		int bLeft = (i % 2 == 0) ? 3 : 7;
		int bTop = (i % 2 == 0) ? (i / 2 * 4) : ((i - 1) / 2) * 4;

		ssd1306_fillRect(bLeft, bTop + 7, 3, 3, boxC);
		ssd1306_drawPixel(bLeft + 1, bTop + 8, pix);
	}

	ssd1306_fillRect(2, 37, 10, 9, 0);
	ssd1306_fillRect(3, 38, 7, 6, 1);
	ssd1306_drawFastHLine(4, 40, 4, 0);
	ssd1306_drawFastHLine(4, 42, 4, 0);

	for(int i = 0; i < sizeof(buffer); i++) {		
		int by = (i/46) * 8; //width of the buffer
		int bx = i - ((i/46) * 46);

		for(int b = 0; b <= 7; b++) {
			if (by == 24 && b > 1) break;			
			ssd1306_drawPixel(15 + bx, 13 + by + b, 1 & (buffer[i] >> b));
		}
	}

}

void macWrite() {
	
	background();
	menubar(1, -1, 0);
	// 5 1 6 62 41
	window(1, 6, 62, 41, 2);
	// 10 1 18 61 0 // tool bar bottom
	ssd1306_drawFastHLine(1, 18, 57, 0);

	// Caret
	ssd1306_fillRect(3,14,4,3,0);
	ssd1306_drawRect(8,14,4,3,0);

	// Spacing buttons
	ssd1306_fillRect(18, 14, 4, 3, 0);
	ssd1306_drawRect(23, 14, 4, 3, 0);
	ssd1306_drawRect(28, 14, 4, 3, 0);


	//justification buttons
	ssd1306_fillRect(41, 14, 4, 3, 0);
	ssd1306_drawRect(46, 14, 4, 3, 0);
	ssd1306_drawRect(51, 14, 4, 3, 0);
};

	// These characters are 4-bits wide, bytes are read vertically.
void ssd1306_narrowChar(uint8_t x, uint8_t y, uint8_t *ch, uint8_t color) {
		
		uint8_t col; 

		//need to adjust for 5-bit-wide chars
		for(uint8_t h = 0; h < 4; h++) {
			for (uint8_t v = 0; v < 8; v++)
			{

				if (ch[h] >> v & 1) //this pixel is white
				{
					col = color;
				}
				else
				{
					// black pixel here if ch[h] >> v is a 1
					col = (~color) & 1;
				}

				ssd1306_drawPixel(x + h, y + v, col);
			}
		}
	}
/*  
Takes an x, y, character code, and color rule (0-5) and renders 
character to the display.
*/
void writeChar(uint8_t x, uint8_t y, uint8_t ch[], uint8_t color) {

	char* c = getChar(ch);
	// ssd1306_drawImage(x, y, c, 8, 8, color);
	ssd1306_narrowChar(x, y, c, color);
}

bool mouseTo(uint8_t mp[], uint8_t xT, uint8_t yT) {

	if (mp[0] < xT) {
		mp[0]++;
	} else if (mp[0] > xT){
		mp[0]--;
	}

	if (mp[1] < yT)
	{
		mp[1]++;
	}
	else if (mp[1] > yT)
	{
		mp[1]--;
	}
	// printf("draw mouse at %u,%u\n",x,y);
	ssd1306_drawImage(mp[0], mp[1], pointerOutline, 8, 8, 4);
	ssd1306_drawImage(mp[0]+1, mp[1], pointer, 8, 8, 5);
	
	
	if(mp[0] == xT && mp[1] == yT) {
		return true;
	}else{
		return false;
	}
}

#endif



