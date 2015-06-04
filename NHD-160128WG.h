/*
Meant for NHD-160128WG-BTMI-VZ#-1
White and blue Newhaven Graphic LCD resolution 160 x 128
*/

// Clock Frequency
#define F_CPU 	16000000

// Required Libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

// LCD uC pinout definitions
#define DB 		PORTC					// All of Port C is directly connected to the data lines of the LCD
#define RESET 	PORTB &= ~(0x02)		// Reset LCD with reset pin connected to pin B1
#define nRESET 	PORTB |= 0x02			// Disable Reset
#define C_D 	0x40					// Defined pin 6 for use with Com and Dat
#define Dat		PORTD &= ~(C_D)			// Set LCD to data mode
#define Com		PORTD |= C_D			// Set LCD to command mode
#define CE		PORTD &= ~(0x20)		// Use pin D5 to enable LCD chip
#define nCE		PORTD |= 0x20			// Disable LCD chip
#define WR		PORTB &= ~(0x01)		// Enable write mode to LCD using pin B0
#define nWR		PORTB |= 0x01			// Disable write mode
#define RD 		PORTD &= ~(0x10)		// Enable read mode from LCD using pin D4
#define nRD		PORTD |= 0x10			// Disable read mode

#define resWidth 	160 				// Max width of LCD (pixel count)
#define resHeight	128 				// Max height of LCD (pixel count)

#define FW			7					// Font Width = number of actual bits per byte that directly control a pixel
#define FWb			0xFF>>(8-FW)		// Font Width bits that are used for actual pixel control

#define charAdd		0x0000				// LCD RAM address starting point for character output
#define charWidth	23					// Max character count per row
#define charHeight	16					// Max character row count

#define GFXAdd		0x0200				// LCD RAM address starting point for GFX output

// Global Variables
extern uint8_t numChar[];
extern uint8_t charRow;
extern uint8_t charCol;

#ifdef TEST
	extern uint16_t testMon[];
	extern uint16_t memMon[];
	extern uint16_t memMonTest[];
	extern uint16_t netMon[];
	extern uint8_t testGFX[];
#endif

uint8_t flipByte(uint8_t);

void datNHD(uint8_t);
void exComNHD(uint8_t, uint8_t, uint8_t);
void comNHD(uint8_t);
void clearCharNHD();
void clearGFXNHD();
void initNHD();
void strNHD(char*);
void gfxNHD(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t);
void setPixel(uint8_t, uint8_t);
void clearPixel(uint8_t, uint8_t);
void createLineLR(uint8_t, uint8_t, uint8_t);
void deleteLineLR(uint8_t, uint8_t, uint8_t);
void createLineUD(uint8_t, uint8_t, uint8_t);
void deleteLineUD(uint8_t, uint8_t, uint8_t);
void deleteBlock(uint8_t, uint8_t, uint8_t, uint8_t);
void createBars(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void createBox(uint8_t, uint8_t, uint8_t, uint8_t);
void numGFX(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void updateBar(uint16_t*, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void updatePlot(uint16_t*, uint8_t, uint8_t, uint8_t, uint8_t);

#ifndef DB
	#error "Please define 'DB' for the data bus"
#endif

#ifndef RESET
	#error "Please define 'RESET' for the reset pin"
#endif

#ifndef C_D
	#error "Please define 'C_D' for the command or data control pin"
#endif

#ifndef CE
	#error "Please define 'CE' for the chip enable pin"
#endif

#ifndef RD
	#error "Please define 'RD' for the read control pin"
#endif

#ifndef WR
	#error "Please define 'WR' for the write control pin"
#endif
