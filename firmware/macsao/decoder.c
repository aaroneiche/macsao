//Command decoder

#include "ch32v003fun.h"
#include <stdio.h>
#include "ssd1306.h"
#include <string.h>
#include "buffer.h"
#include "paintBuffer.c"
// #include "hello.h"
/* 
Command Decoder - takes bytes, turns them into screens

0x01 - Desktop - the standard Mac Desktop with Menus, a startup disk, and Trash Icon
    Disk state **TODO**
        0x00 - nothing
        0x01 - selected
        0x02 - open 

0x02 - MacPaint - the Standard MacPaint with menus, tool palette, window, patterns
    Tool - The highlighted tool in the palette
        0x01:0x0E - tool starting from top left and incrementing to the right and down.
    Pattern **TODO**
        0x00 - black
        0x01 - white
*/


//Some generic variables to track.
uint8_t x = 0;
uint8_t y = 0;
uint8_t curr = 0;
uint8_t paintFrame = 0;

/*
    Decoder steps through items in the command list and decodes them to executing methods.

*/
void decoder(uint8_t **command, uint8_t data[])
{

    /*
        Helpful programming info:
        *(*command)         The value of the passed-in command.
        
        *(++(*command))     Increment the pointer before reading the command's
                            value. This will give you the value of the next command, 
                            And it moves to pointer so you don't have to mess 
                            with tracking it otherwise.
    */

    // printf("Command: %u at: %u\n",*(*command), *command);
    // 0x01, 0x02, 0x06, 0x10, 0x0A, 0x20, 0xFF

    uint8_t c = 0; //color 

        // First read the command given
    switch (*(*command))
    {
    case 0x01: // Desktop
        desktop(*(++(*command)));
        break;
    case 0x02: //MacPaint
        macPaint(*(++(*command))); //Next argument is the tool selected.
        break;
    case 0x03: //MacWrite
        macWrite();    
        break;
    case 0x05: //Window
        window(*(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)));

        break;
    case 0x06: //Draw a menu (does not include a background.)
        // menus, selected, selected Item
        menubar(1, *(++(*command)), *(++(*command)));
    break;
    case 0x07: //Draw an icon

        drawIcon(*(++(*command)),*(++(*command)),*(++(*command)),*(++(*command)));
    break;
    
    // Basic Drawing tools
    case 0x09: //Draw a pixel at coordinates x,y
        ssd1306_drawPixel(*(++(*command)), *(++(*command)), *(++(*command)));
        break;
    case 0x0A: // draw Horizontal line
        //x y length, color  
        ssd1306_drawFastHLine(*(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)));
    break;
    case 0x0B: // draw Horizontal line
        // x y length, color
        ssd1306_drawFastVLine(*(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)));
    break;
    case 0x0C:
        // x, y, width, height, color
        ssd1306_fillRect(*(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)));
    break;
    case 0x0D:
        // x, y, width, height, color
        ssd1306_drawRect(*(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)));
    break;
    case 0x0E:
        ssd1306_drawCircle(*(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)));
    break;
    case 0x0F:
        ssd1306_fillCircle(*(++(*command)), *(++(*command)), *(++(*command)), *(++(*command)));
        data[2] = 1;
    break;

    case 16: //MouseTo

        //Mouse goal
        uint8_t xT = *(++(*command));
        uint8_t yT = *(++(*command));
        // printf("Target: (%u,%u) Actual (%u,%u)\n",xT,yT,data[0],data[1]);
        bool res = mouseTo(data, xT, yT); 

        if(res) {
            data[2] = 1;
        }

        break;
    case 17: //Draw Pixel to the Buffer
        x = *(++(*command));
        y = *(++(*command));
        c = *(++(*command));

        buffer_drawPixel(x, y, c);
        data[2] = 1;
        // printf("drawing %u pixel at %u,%u\n",c,x,y);

        break;
    case 18: //Wait 

        uint8_t target = *(++(*command)); //how much to wait.
        if(data[3] < target) {

            Delay_Ms(100);
            data[3]++;
        }else{
            data[2] = 1; // Set the animation step to "complete"
            data[3] = 0; //Clear the wait counter
        }

    break;
    case 19: //Print out characters 
        // data[3] // Counter to keep track of current character
        x = *(++(*command));
        y = *(++(*command));

        curr = *(++(*command));
        
        while (curr != 0) 
        {
            writeChar(x,y,curr,0);
            curr = *(++(*command));
            x += 5;
        }

        break;

    // 1 3 255 20 4 20 87 101 108 99 111 109 101 32 116 111 254

    case 20: // Type out characters
        // data[3] // Counter to keep track of current character
        x = *(++(*command));
        y = *(++(*command));

        curr = *(++(*command)); //steps the val to the next letter

        // if(curr != 254) {
        c = 0; 

        //Write all the previous characters out 
        while(c < data[3] && curr != 0) {
            uint8_t cx = x;
            uint8_t cy = y;
            
            if(c < 10) { 
                cx = x + (5 * c); 
            } else if(c < 20 && c >= 10) {
                cx = x + (5 * (c-10));
                cy = y + 8;
            }else if(c > 20) {
                cx = x + (5 * (c - 20));
                cy = y + 16;
            }
            
            writeChar(cx, cy, curr, 0);
            curr = *(++(*command));
            c++;
        }

        if(curr != 0) {
            data[3] += 1;
        }else{
            data[3] = 0;
            data[2] = 1;
            // Delay_Ms(2000);
        }
        
            Delay_Ms(100);

        break;

        case 0x15: //Clear buffer
            c = *(++(*command));
            buffer_clear(c);
            data[2] = 1;
        break;

        /* 
        // This feature turned out to be too much for the onboard flash.
        // Maybe resurrect it in the future.

        case 47: //Write 'hello' in Mac Paint
            if(paintFrame < numFrames){
                for (const Point *p = frames[paintFrame]; p->x != 0xFF || p->y != 0xFF; p++) {
                    buffer_drawPixel(p->x, p->y,0);
                }
                Delay_Ms(100); //delay between each frame.
                paintFrame += 1;
            }else{
                //we're done, move along.
                paintFrame = 0;
                buffer_clear(1);
                data[2] = 1;
            }

        break;
        */

        case 22: //Mouse To menu
            // Move the mouse to a menu position, opens menu. Scrolls down to that item. Uses the "wait" value in store to 
            // determine if it's made it to the menu.

            // which menu, which item in the menu.
            // menubar(1, *(++(*command)), *(++(*command)));
            int8_t menu = *(++(*command));
            uint8_t menuItem = *(++(*command));

            if(data[3] == 0) {
                //going to the menu
                
                data[3] = mouseTo(data, menuData[menu][0] + 1, 3);
            }else{
                //we've reached the menu
                menubar(1, menu, (data[1] - 4)  / 4);   //the mouse over this thing is: mouse y %4
                data[2] = mouseTo(data, menuData[menu][0] + 1, menuItem * 3);
                Delay_Ms(40);
                if(data[2]) data[3] = 0;
            }


        break;        

        case 23: //draw a circle to the buffer
            buffer_drawCircle(*(++(*command)),*(++(*command)),*(++(*command)),*(++(*command)));
            data[2] = 1;
        break;

        case 24: //draw a circle to the buffer
            buffer_drawFilledCircle(*(++(*command)),*(++(*command)),*(++(*command)),*(++(*command)));
            data[2] = 1;
        break;

        case 48: // Settings

        int a = *(++(*command));

        // Draw the settings window
        // window(2, 8, 60, 34, 0);
        window(2, 6, 60, 40, 0);


        //Selected Address x position
        int o = (a==10)? 4 : (a==34)? 24 : 43;

        // "I2C Address"
        writeChar(4, 14, 73, 0);
        writeChar(9, 14, 50, 0);
        writeChar(14, 14, 67, 0);

        writeChar(24, 14, 65, 0);
        writeChar(29, 14, 68, 0);
        writeChar(34, 14, 68, 0);
        writeChar(39, 14, 82, 0);

        writeChar(44, 14, 69, 0);
        writeChar(49, 14, 83, 0);
        writeChar(54, 14, 83, 0);

        //Address Options
        ssd1306_fillRect(o, 24, 17, 9, 0);
        ssd1306_drawstr(5,26, "0A", a==10 ? 1 : 0);
        ssd1306_drawstr(25,26, "22", a == 34 ? 1 : 0);
        ssd1306_drawstr(44,26, "56", a == 86 ? 1 : 0);

        //Version placement
        const uint8_t verY = 36;
        const uint8_t verX = 45;

        //v1.1
        writeChar(verX,     verY, 118, 0);
        writeChar(verX + 5, verY,  49, 0);
        writeChar(verX + 8, verY,  46, 0);
        writeChar(verX + 10, verY, 49, 0);

        // ssd1306_drawFastHLine(4, 28, 16, 0);

        break;

       
    }

}
