/*
 * MacSAO Firmware Aaron Eiche 2024
 * 
 * Started from E. Brombaugh's OLED Demo, 
 * Utilizes ADBeta's Software I2C Library
 */

// what type of OLED - uncomment just one
#define SSD1306_64X48
// #define SSD1306_64X32
// #define SSD1306_128X32
// #define SSD1306_128X64

#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// Though this should be on by default we can extra force it on.
#define FUNCONF_USE_DEBUGPRINTF 1
#define FUNCONF_DEBUGPRINTF_TIMEOUT (1 << 31) // Wait for a very very long time.

#define CH32V003 1

#endif

#include "ch32v003fun.h"
#include <stdio.h>
#include "ssd1306.h"
#include <string.h>

// #include "bomb.h"
#include "../extralibs/ch32v003_GPIO_branchless.h"
#include "i2c_slave.h"
#include "lib_swi2c.c"

#include "decoder.c"

// State: Either in settings mode or "Mac" animation playback.
#define MAC 0
#define SETTINGS 1

#define BACKGROUND 0
#define ACTION 1

bool mode = BACKGROUND;

volatile uint32_t overflowCount = 0;

uint8_t buttonLastState = 0;
int start = 0;
int stop = -1;
bool counting = false;
bool buttonShortPress = false;

//Where i2c input goes after being received.
volatile uint8_t i2c_registers[32] = {};

//Where EEPROM data is put to make available externally.
volatile uint8_t eepData[64]; // max data to read in.

//Default display stack
volatile uint8_t displayStack[312] = {
	// 0x01, 0xFF,		  // Desktop
	// 0x10, 0x0A, 0x0A, // Mouse Around
	// 0x01, 0xFF,		  // Desktop
	// 0x10, 0x32, 0x0A, // Mouse Around
	// 0xFF,
	// 0x02, 0x02, 0xFF,		// MacPaint, tool 6
	// 0x10, 0x37, 0x18,		// Mouse around
	// 0x02, 0x06, 0xFF,		// MacPaint, tool
	// 0x10, 0x0A, 0x0F, 0xFF,
	// 0x12, 0x43, 0x03,0xFF
/*
	2, 12, 254, // Right now only 1 command fires after background
	16, 4, 10,
	21, 1,
	255,
	2, 4, 254,
	16, 48, 10,
	16, 40, 40,
	16, 10, 15,

	17, 10, 15, 0,
	17, 10, 14, 0,
	17, 10, 13, 0,
	17, 10, 12, 0,
	17, 10, 11, 0,
	// top

	17, 11, 10, 0,
	17, 12, 10, 0,
	17, 13, 10, 0,
	17, 14, 10, 0,
	17, 15, 10, 0,
	17, 16, 10, 0,
	// right

	17, 17, 11, 0,
	17, 17, 12, 0,
	17, 17, 13, 0,
	17, 17, 14, 0,
	17, 17, 15, 0,
	// bottom

	17, 16, 16, 0,
	17, 16, 17, 0,
	17, 15, 17, 0,
	17, 14, 17, 0,
	17, 13, 17, 0,
	17, 12, 17, 0,
	17, 11, 17, 0,
	17, 11, 16, 0,
	// eyes

	17, 12, 12, 0,
	17, 15, 12, 0,
	
	// smile
	17, 13, 14, 0,
	17, 13, 15, 0,
	17, 14, 15, 0,
	17, 15, 14, 0,

	18, 40,
	255, 255
*/
	3, 254,
	18, 10,
	255, 255,
	/* 	2, 4,
		254,
		16, 40, 40,
		255,
		1,
		254,
		16, 10, 7,
		18, 10,
		255,
		255 */

	/*
	//Writes "Welcome to Supercon"

	3, 255, 16, 48, 40, 3, 255, 16, 9, 2,
	3, 255,
	20, 4, 20, 87, 101, 108, 99, 111, 109, 101, 32, 116, 111,
	115, 117, 112, 101, 114, 99, 111, 110, 254,
	3, 255,
	19, 4, 20, 87, 101, 108, 99, 111, 109, 101, 32, 116, 111,
	115, 117, 112, 101, 114, 99, 111, 110, 254,
	18, 10, 255
	 */
};

//This stack displays the settings menu. 
volatile uint8_t settingsStack[32] = {
	1, 48, 10, 254, 16, 38, 40, 255, 255
};

volatile uint8_t preferences[16];

/* 
These variables keep track of the step in the sequence we're on, 
as well as the item in the sequence we're currently drawing.
 */
uint8_t * stackPtr = displayStack; // 
// uint8_t * stepPtr = displayStack; // each step in the display stack.


uint8_t * bgStartPtr = displayStack;
uint8_t * actionStartPtr;

// Keeps track of persistant info:
// Mouse X,Y, and animation step complete(0:1), wait count
uint8_t store[] = {5, 5, 0, 0};

uint16_t r_addr;
uint8_t len;

uint8_t	ssd1306_soft_init(void);

volatile uint8_t state = MAC;
// volatile uint8_t state = SETTINGS;
bool drawn = false;

void onWrite(uint8_t reg, uint8_t length) {
	// for(int c = 0; c < length; c++) {
	// 	printf("%u: %u\n", c, i2c_registers[c]);
	// }

	//Reset the state of the i2c lines, in case anything else is happening.
	swi2c_init(&device);

	switch (i2c_registers[0])
	{
		case 1:
		//write to display data.
		
		//Clear the store data in case the new command is using it.
		store[3] = 0;

		for (int i = 1; i <= length; i++)
		{
			displayStack[i - 1] = i2c_registers[i];
		};

		displayStack[length-1] = 0xFF; // A final byte at the end of last animation
		
		stackPtr = displayStack;
		// stepPtr = displayStack;

		break;
		case 2: 
		//write to stored memory

		// 16bit address. Here to auto-overflow. 
		uint16_t addr = (i2c_registers[1] << 8) | i2c_registers[2];
		// printf("address: %4x\n", addr);
		// Get distance to end of page.

		uint8_t bytesForPage = 32 - (addr % 32); // How far to end of page.

		uint8_t *sptr = i2c_registers + 3; //A pointer to the first byte we're gonna send.
		uint8_t dataLen = length - 3; // Length of the data to store.

		// printf("data length: %u\n", dataLen);
		// printf("byte value of array: %u\n", sptr);
		while (dataLen > 0) // While there are bytes to write
		{
			// printf("another loop: %u\n", dataLen);
			if (dataLen > bytesForPage) // We have more bytes than can write in this page.
			{

				// printf("sending %u bytes to %4x\n",addr, dataLen);
				//send bytes at pointer, for bytesForPage length
				swi2c_master_transmit_eeprom(&eeprom, addr, sptr, dataLen);
				
				//reduce the number of bytes to write.
				dataLen -= bytesForPage;
				
				//increment the address pointer
				sptr +=  bytesForPage; // Array pointer
				addr += bytesForPage; // address value.

				//update bytesForPage to the next length of bytes to send.
				bytesForPage = 32 - (addr % 32);
			}
			else // We can write these bytes just to this page.
			{
				// printf("one shot: sending %u bytes to %4x\n", dataLen, addr);
				swi2c_master_transmit_eeprom(&eeprom, addr, sptr, dataLen);
				dataLen -= dataLen; // subtract remaining bytes.
			}
		}

		break;

		case 3:
			
			r_addr = (i2c_registers[1] << 8) | i2c_registers[2]; //16-bit address
			len = i2c_registers[3];
			
			if(r_addr == 0xFFFF) { 
				// FFFF is not a valid EEPROM address (this is only 64kbits)
				//Read display stack in, rather than 
				for(int i = 0; i < len; i++){
					eepData[i] = displayStack[i];
				}
			}else{
				swi2c_master_receive_eeprom(&eeprom, r_addr, eepData, len);
			}
						
			
			for (int i = 0; i < len; i++)
			{
				// printf("%u: %u\n", i, eepData[i]);
				i2c_registers[i] = eepData[i];
				// memcpy(i2c_registers, eepData, len); //copy the data over to i2cr
			}
			// memcpy(i2c_registers, eepData, len);
			// for (int r = 0; r < len; r++)
			// {
			// 	printf("reg %u: %u\n",r,i2c_registers[r]);
			// }
		break;
		case 4:
			// Load a sequence from EEPROM into displayStack
			// 4 Args: Address Hi, Address Low, Number of Bytes, offset on displayStack

			uint16_t r_addr = (i2c_registers[1] << 8) | i2c_registers[2]; // 16-bit address
			len = i2c_registers[3];						// Number of bytes to read

			// Where to place the new bytes in the display stack
			// useful if you want to add to existing stack.
			uint8_t stackOffset = i2c_registers[4];		
			swi2c_master_receive_eeprom(&eeprom, r_addr, eepData, len);

			// write eepData vals into displayStack
			for(int i = 0; i < len; i++) {
				displayStack[stackOffset + i] = eepData[i]; 
			}
			
			displayStack[stackOffset + len] = 255; // Extra 255 at end of new data indicates end of Sequence.

			// reset display stack
			stackPtr = displayStack;
			actionStartPtr = displayStack;
			bgStartPtr = displayStack;
			break;
		}
	printf("command finished.\n");

}

void onRead(uint8_t reg) {
	// read data out of eepData
	// SetupI2CSlave
	printf("Read %u at ...\n",i2c_registers[reg], reg);
}


/* 
	Explicitly sets the i2c address of the MacSAO
*/
void setI2CAddress(uint8_t addressVal){
	if (swi2c_start(&eeprom) == I2C_OK)
	{
		swi2c_master_tx_byte(&eeprom, eeprom.address);		  // Transmit Address w/write bit;
		swi2c_master_tx_byte(&eeprom, 0); // Address High Byte
		swi2c_master_tx_byte(&eeprom, 0);		  // Address Low Byte
		swi2c_master_tx_byte(&eeprom, addressVal); // Address Low Byte
		swi2c_stop(&eeprom);
	}
} 

void readData(uint8_t addrH, uint8_t addrL, uint8_t data[], uint8_t length)
{
	if (swi2c_start(&eeprom) == I2C_OK)
	{
		swi2c_master_tx_byte(&eeprom, eeprom.address); // Transmit Address w/write bit;
		swi2c_master_tx_byte(&eeprom, addrH);		   // Page Address High Byte
		swi2c_master_tx_byte(&eeprom, addrL);		   // Page Address Low Byte
		// swi2c_stop(&eeprom);
	}

	if (swi2c_start(&eeprom) == I2C_OK)
	{
		swi2c_master_tx_byte(&eeprom, eeprom.address | 1); // Transmit Address w/read bit;
		for (int i = 0; i < length; i++)
		{
			
			data[i] = swi2c_master_rx_byte(&eeprom, I2C_ACK); //Read bytes into data.
			// printf("read %u %u\n",i, data[i]);
		}
		swi2c_master_rx_byte(&eeprom, I2C_NACK); //End Sequential Read.
		swi2c_stop(&eeprom);
	}
}

void writeData(uint8_t addrH, uint8_t addrL,  uint8_t data[], uint8_t length ) {
	printf("writing data\n");
	if (swi2c_start(&eeprom) == I2C_OK)
	{
		// printf("i2c ok: ");
		swi2c_master_tx_byte(&eeprom, eeprom.address); 	// Transmit Address w/write bit;
		swi2c_master_tx_byte(&eeprom, addrH); 			// Address High Byte
		swi2c_master_tx_byte(&eeprom, addrL);		  	// Address Low Byte
		for(int l = 0; l < length; l++){
			swi2c_master_tx_byte(&eeprom, data[l]);
		} 
		swi2c_stop(&eeprom);
		// printf("\n\n");
	}
}

void getStored(uint16_t address, uint8_t data[], uint8_t length) {
	//Set the read address
	if(swi2c_start(&eeprom) == I2C_OK) {
		swi2c_master_tx_byte(&eeprom, eeprom.address); 			// Transmit Address w/write bit;
		swi2c_master_tx_byte(&eeprom, (address >> 8) & 0xFF);   // Address High Byte
		swi2c_master_tx_byte(&eeprom, address & 0xFF);			// Address Low Byte
		// swi2c_stop(&eeprom);
	}
	
	//Read the EEPROM
	if (swi2c_start(&eeprom) == I2C_OK)
	{
		swi2c_master_tx_byte(&eeprom, eeprom.address | 1); // Transmit Address w/read bit;
		for (int i = 0; i < length; i++)
		{
			data[i] = swi2c_master_rx_byte(&eeprom, I2C_ACK);
		}
		swi2c_master_rx_byte(&eeprom, I2C_NACK);
		swi2c_stop(&eeprom);
	}
	
}

/*  
Button press Interrupt
*/
void EXTI7_0_IRQHandler(void) __attribute__((interrupt));
void EXTI7_0_IRQHandler(void)
{
	int res = funDigitalRead(PC4);
	// Delay_Ms(10); // debounce
	// int resB = funDigitalRead(PC4);
	
	// if(res == resB) {
		//debounced, this is fine.
		if (res)
		{ //Button pulls up.
			stop = SysTick->CNT; //The end of the button press time.

			//Set counting to 
			if (counting && (stop - start) / 4799 > 50) 
			{
				buttonShortPress = true;
			}
			counting = false;
		}
		else
		{ //Button Pressed
			start = SysTick->CNT;
			counting = true;
		}
	// }

	// Acknowledge the interrupt
	EXTI->INTFR = EXTI_Line4;
}

int stateManager(int newState) {
	if (newState == SETTINGS)
	{
		printf("state to SETTINGS\n");
		stackPtr = settingsStack; // each item in the display stack
		// stepPtr = settingsStack;  // each step in the display stack.

		actionStartPtr = settingsStack;
		bgStartPtr = settingsStack;
		return SETTINGS;
	}
	else if(newState == MAC)
	{
		printf("state to MAC\n");
		stackPtr = displayStack; // each item in the display stack
		// stepPtr = displayStack;	 // each step in the display stack.

		actionStartPtr = displayStack;
		bgStartPtr = displayStack;

		return MAC;
	}
	return MAC; // If somehow we don't return properly...
}



int main()
{

	// 48MHz internal clock
	SystemInit();
	funGpioInitAll();

	//Setup the button input.
	funPinMode(PC4, GPIO_CFGLR_IN_PUPD); 
	funDigitalWrite(PC4, FUN_HIGH);	

	//Setup the interrupt handler for the button.
	AFIO->EXTICR = AFIO_EXTICR_EXTI4_PC;
	EXTI->INTENR = EXTI_INTENR_MR4; // Enable EXT4
	EXTI->RTENR =  EXTI_RTENR_TR4; // Rising edge trigger
	EXTI->FTENR = EXTI_FTENR_TR4;  //Falling edge trigger

	NVIC_EnableIRQ(EXTI7_0_IRQn);
/*
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1; //Enable Timer1
	// RCC->APB2PRSTR |= RCC_APB2Periph_TIM1; //Reset Timer?


	//Initialize Timer 1 		
	TIM1->PSC = 4799; 		// 48Mhz = 0.1ms
	TIM1->ATRLR = 0x7530;	//Auto reload register
	
	// TIM1->INTFR &= ~TIM_UIF; //Clear the interrupt before enabling.
	// printf("--%16x\n",TIM1->INTFR);

	// TIM1->INTFR = ~TIM_UIF;
	TIM1->DMAINTENR |= TIM_UIE; //Enable the interrupt enable register.

	// printf("--%16x\n", TIM1->INTFR);
	// printf("DMA --%16x\n", TIM1->DMAINTENR);

	TIM1->CNT = 0; //Initial count to 0
	
	// TIM1->INTFR &= ~TIM_UIF; // clear interrupt flag.
	// TIM1->CTLR1 |= TIM_CEN; // Enable the counter.

	TIM1->INTFR &= ~TIM_UIF; // Clear the interrupt before enabling.

	// NVIC_EnableIRQ(TIM1_UP_IRQn); //Setup Timer 1 interrupt.
*/

	buffer_clear(1); //set the MacPaint buffer to white

	// initialize i2c for Display.
	i2c_err_t a = swi2c_init(&device);

	if (a != I2C_OK){
		// printf("Error in i2c Device Setup: %u\n", a);
	}

	//Initialize I2C as target for talking to host badge.
	funPinMode(PC1, GPIO_CFGLR_OUT_10Mhz_AF_OD); // SDA
	funPinMode(PC2, GPIO_CFGLR_OUT_10Mhz_AF_OD); // SCL

	// Get stored preferences from the EEPROM.
	getStored(0x0000, preferences, 16);

	//Default Address in case something goes wrong.
	if(preferences[0] != 10 && preferences[0] != 34 && preferences[0] != 86) {
		preferences[0] = 10;
	}
	//Make sure the display data for settings is up-to-date.
	settingsStack[2] = preferences[0];

	// Preferences 1 and 2 are address bytes (high and low) load data from.
	uint16_t loadFromAddr = (preferences[1] << 8) | preferences[2];
	printf("loadFrom address: %4x\n", loadFromAddr, preferences[1], preferences[2]);
	//Preferences 3 is the number of bytes to load.

	if(loadFromAddr != 0) {
		printf("Stored address is %4x. Loading...\n", loadFromAddr);
		// Get data from eeprom
		getStored(loadFromAddr, displayStack, preferences[3]);
		displayStack[preferences[3]] = 255;
		
	}else{
		printf("Stored address is 0, playing default\n");
	} // Else display stack remains default, or current or whatever.

	// Setup I2C from the badge.
	SetupI2CSlave(preferences[0], i2c_registers, sizeof(i2c_registers), onWrite, NULL, false);
	
	//Setup display
	ssd1306_soft_init();

	printf("Command Start: %u\n", displayStack);

	while(1) {

		if(counting == true && ((SysTick->CNT - start)/47999) > 350) {
			counting = false; // Stop counting. We're not doing anything else here
			state = stateManager(!state);
		} 
		// printf("post button check. State now: %u\n",state);
		if(buttonShortPress) {
			if(state == SETTINGS){
				//advance I2C Address
				switch(preferences[0]) {
					case 10:
						preferences[0] = 34;
					break;
					case 34:
						preferences[0] = 86;
					break;
					case 86:
						preferences[0] = 10;
					break;
				}
				settingsStack[2] = preferences[0];
				setI2CAddress(preferences[0]);
			}
			printf("button short!\n");
			//Here is where button handling is for advancing the state
			buttonShortPress = false;
		}

		/* 
			Typical command: 

			2 7				MacPaint Tool #7
			254				END background
			16 40 40 		MouseTo (40,40)
			18 10			Wait 1 second (10 * 100ms)
			255				End of Action, move to next background.
			
			Abstraction: 

			//If in Background mode: 
			Run through all commands until you hit a 254.
			Switch to Action mode
			Step Forward one
			
			If current ActionStartPtr is greater than this, step forward (repeat until you get to current)
			If current ActionStartPtr is less than this, set this command to ActionStartPtr 
			
			Run through commands (some may issue repeat) 
				return 0 means go back to backgroundPtr
				return 1 means move ahead to next command
			
			...Until you get to 255
			
			Step Forward one
			Set Background Ptr to this command
		*/

		while (*stackPtr != 254)  // Not 254. In background
		{
			// printf("current bg: %u \n",*stackPtr);
			decoder(&stackPtr, store); // Decode command, include store for data access/updates.
			stackPtr++;
		}
		stackPtr++; 	//Advance to the first item in the Action set

		// printf("StackPtr: %u, ActionStartPtr: %u\n", stackPtr, actionStartPtr);

		//check if this is the current Action 
		while(stackPtr < actionStartPtr) {
			// printf("skip: %u \n", *stackPtr);
			stackPtr++;	//increment the stack pointer until we're at the right action.
		}
		// printf("current action: %u \n", *stackPtr);
		//Now we're at the right action
		decoder(&stackPtr, store);

		//if this action completed, advance the stackptr
		if (store[2] == 1) {
			stackPtr++;
			actionStartPtr = stackPtr;
			store[2] = 0; //clear after processed.
		}else{
			//Done - draw and go back to background to repeat.
			stackPtr = bgStartPtr;
		}

		// printf("displaying...\n");
		ssd1306_refresh(); // Update the display.

		if(*stackPtr == 255) {
			// printf("end of action\n");
			//end of actions for this background. Set background to next address.
			stackPtr++;
						
			// if *this* address is also 255, that's the end of commands, go back to the beginning.
			if(*stackPtr == 255) {
				// printf("double 255, end of command set. Starting over\n");
				stackPtr = displayStack;
				actionStartPtr = displayStack; //Since we don't know where the first action is, it'll always be behind the stack pointer
				bgStartPtr = stackPtr;

				if (state == MAC)
				{
					// printf("Mac!\n");
					stackPtr = displayStack;
					actionStartPtr = displayStack;
					bgStartPtr = stackPtr;
					// printf("%u\n",stackPtr);
				}
				else if (state == SETTINGS)
				{
					stackPtr = settingsStack;
					actionStartPtr = settingsStack;
					bgStartPtr = stackPtr;
				}
			}else{
				bgStartPtr = stackPtr;
			}
		}else{
			stackPtr = bgStartPtr;
		}

	}
}


uint8_t ssd1306_soft_init(void)
{
	// initialize OLED
	uint8_t *cmd_list = (uint8_t *)ssd1306_init_array;
	while (*cmd_list != SSD1306_TERMINATE_CMDS)
	{
		ssd1306_cmd(*cmd_list++);
	}
	// clear display
	ssd1306_setbuf(0);
	ssd1306_refresh();

	return 0;
}