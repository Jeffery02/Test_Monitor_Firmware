
#include "NHD-160128WG.h"

// Global Variables
	// Look up table for flipByte function
uint8_t flip[] = {0b0000, 0b1000, 0b0100, 0b1100, 0b0010, 0b1010, 0b0110, 0b1110, 0b0001, 0b1001, 0b0101, 0b1101, 0b0011, 0b1011, 0b0111, 0b1111};

	// Used to keep track of current character location
uint8_t charRow = 0;
uint8_t charCol = 0;

#ifdef TEST
	uint16_t testMon[96] = {50, 12, 80, 43, 60, 12, 95, 100, 46, 37, 20, 25, 76, 34, 5, 50, 50, 12, 80, 43, 60, 12, 95, 100, 46, 37, 20, 25, 76, 34, 5, 50, 50, 12, 80, 43, 60, 12, 95, 100, 46, 37, 20, 25, 76, 34, 5, 50, 50, 12, 80, 43, 60, 12, 95, 100, 46, 37, 20, 25, 76, 34, 5, 50, 50, 12, 80, 43, 60, 12, 95, 100, 46, 37, 20, 25, 76, 34, 5, 50, 50, 12, 80, 43, 60, 12, 95, 100, 46, 37, 20, 25, 76, 34, 5, 50};

	uint16_t memMon[128];
	uint16_t memMonTest[96] = {100, 15, 80, 43, 60, 20, 95, 60, 100, 37, 20, 25, 46, 76, 5, 50, 15, 12, 80, 43, 60, 12, 95, 20, 46, 37, 46, 25, 76, 34, 5, 50, 60, 12, 37, 43, 15, 12, 95, 100, 37, 42, 15, 25, 92, 76, 5, 50, 50, 12, 80, 90, 100, 37, 95, 100, 37, 37, 20, 60, 92, 20, 5, 50, 50, 12, 80, 43, 37, 12, 46, 100, 46, 90, 20, 25, 76, 34, 5, 50, 50, 12, 80, 43, 60, 12, 95, 100, 46, 37, 20, 25, 42, 34, 5, 37};

	uint16_t netMon[128];

	uint8_t testGFX[] = {
		0b0000011, 0b1100000,
		0b0000010, 0b0100000,
		0b0011111, 0b1111100,
		0b0010000, 0b0000100,
		0b0010000, 0b0000100,
		0b0011111, 0b1111100,
		0b0010000, 0b0000100,
		0b0010000, 0b0000100,
		0b0011111, 0b1111100,
		0b0010000, 0b0000100,
		0b0010000, 0b0000100,
		0b0011111, 0b1111100,
		0b0010000, 0b0000100,
		0b0010000, 0b0000100,
		0b0011111, 0b1111100,
		0b0000000, 0b0000000
	};
#endif

// Used for numGFX to create any hex digit using a simple font (V was included at the end (0x37) for labeling voltage)
uint8_t numChar[] = {0x77, 0x12, 0x5D, 0x5B, 0x3A, 0x6B, 0x6F, 0x52, 0x7F, 0x7B, 0x7E, 0x2F, 0x65, 0x1F, 0x6D, 0x6C, 0x37};

uint8_t flipByte(uint8_t in) {		// Flip the bits of a byte using a look up table
	return(flip[in&0x0F]<<4 | flip[in>>4]);
}

void test() {
	while(1) {
		_delay_ms(250);
		PORTA ^= 0x01;
	}
}

void datNHD(uint8_t data) {
	nRD;				// Disable read mode
	DB = flipByte(data);			// Set data bus
	Dat;				// Set D/C pin to data
	WR;					// Enable LCD write mode
	CE;					// Enable LCD
	_delay_us(4);		// Wait for LCD to stabalize
	nCE;				// Disable LCD
	nWR;				// Disable LCD write mode
//	_delay_us(10);		// Wait for disabling pins to stabalize just in case
}

void exComNHD(uint8_t command, uint8_t data0, uint8_t data1) {		// Read datasheet for associated data per command
	datNHD(data0);		// Extended command lines
	datNHD(data1);		// Must be sent even if not needed for specific command
	nRD;				// Disable read mode
	DB = flipByte(command);		// Set data bus
	Com;				// Set D/C pin to command
	WR;					// Enable LCD write mode
	CE;					// Enable LCD
	_delay_us(4);		// Wait for LCD to stabalize
	nCE;				// Disable LCD
	nWR;				// Disable LCD write mode
//	_delay_us(10);		// Wait for disabling pins to stabalize just in case
}

void comNHD(uint8_t command) {			// Read datasheet for associated data per command
	nRD;				// Disable read mode
	DB = flipByte(command);		// Set data bus
	Com;				// Set D/C pin to command
	WR;					// Enable LCD write mode
	CE;					// Enable LCD
	_delay_us(4);		// Wait for LCD to stabalize
	nCE;				// Disable LCD
	nWR;				// Disable LCD write mode
}

void clearCharNHD() {
	int i;
	exComNHD(0x24, charAdd & 0xFF, charAdd >> 8);		// Set address pointer
	comNHD(0xB0);			// Data auto-write
	for(i = 0; i < 480; i++) {		// Clear LCD buffer
		datNHD(0x00);				// clear buffer
	}
	comNHD(0xB2);			// Disable auto-write
	charRow = 0;
	charCol = 0;
	exComNHD(0x21, charCol, charRow);		// Reset cursor positition
	exComNHD(0x24, charAdd & 0xFF, charAdd >> 8);		// Set address pointer
}

void clearGFXNHD() {
	int i;
	exComNHD(0x24, GFXAdd & 0xFF, GFXAdd >> 8);		// Set address pointer
	comNHD(0xB0);			// Data auto-write
	for(i = 0; i < resWidth*resHeight; i++) {		// Clear LCD buffer
		datNHD(0x00);				// clear buffer
	}
	comNHD(0xB2);			// Disable auto-write
}

void initNHD() {
	PORTD |= 0x80;
	nRD;				// Disable read mode
	RESET;				// Reset LCD
	_delay_us(500);		// Wait for LCD reset to finish
	nRESET;				// Set LCD for normal function

	exComNHD(0x40, charAdd & 0xFF, charAdd >> 8);		// Set text home address = LCD ram address for upper-most and left-most character
	exComNHD(0x41, charWidth & 0xFF, charWidth >> 8);	// Set text area = number of columns starting from left most positition of the LCD
	exComNHD(0x42, GFXAdd & 0xFF, GFXAdd >> 8);			// Set graphic home address = LCD ram address for upper-most and left-most byte
	exComNHD(0x43, charWidth & 0xFF, charWidth >> 8);	// Set graphic area = number of columns starting from left most positition of the LCD
	exComNHD(0x22, 0x00, 0x00);	// Set offset register
	comNHD(0xA7);		// 8-line cursor
	comNHD(0x81);		// EXOR mode = (text output) EXOR (graphic output)
	comNHD(0x9C);		// Set display mode to text w/ graphics and cursor (0x9F) -- Check data sheet for alternate display modes
	comNHD(0xB2);		// Initialize auto display mode off
	_delay_us(500);
}

void strNHD(char* str) {
	uint16_t i = 0, j = charRow*23 + charCol;
	exComNHD(0x24, j & 0xFF, j >> 8);		// Set address pointer
	comNHD(0xB0);					// Data auto-write
	while (str[i]) {				// Go until NULL is found
		datNHD(str[i] - 0x20);			// Send string to LCD one character at a time
		i++;				// Move to next character
		charCol++;			// Increment column counter
		if(charCol == (charWidth - 1) || str[i] == '\n' || !str[i]) {
			comNHD(0xB2);			// Disable auto-write
			charRow = (charRow + 1) % 32;
			charCol = 0;
			j = charRow*charWidth + charCol;
			exComNHD(0x24, j & 0xFF, j >> 8);		// Set address pointer
			i++;
			comNHD(0xB0);				// Data auto-write
		}
	}
	comNHD(0xB2);			// Disable auto-write
	exComNHD(0x21, charCol, charRow);		// Set cursor positition
}
 #ifdef TEST
void gfxNHD(uint8_t* pic, uint8_t x, uint8_t y, uint8_t picWidth, uint8_t picHeight) {		// Display binary picture
	uint16_t temp = GFXAdd + y*charWidth*8 + x;		// Calculate desired starting address
	uint16_t i = 0, j = 0;
	exComNHD(0x24, temp & 0xFF, temp >> 8);		// Set address pointer
	while (j < picHeight*8) {
		for(i = 0; i < picWidth; i++) {
			temp = GFXAdd + y*charWidth*8 + j*charWidth + i+x;
			exComNHD(0x24, temp & 0xFF, temp >> 8);		// Set address pointer
			datNHD(testGFX[i + j*picWidth]);		// Data
			comNHD(0xC4);				// Command to manually write data to LCD
		}
		j++;
	}
}
#endif

void gfxNHD2(uint8_t* pic, uint8_t x, uint8_t y, uint8_t picWidth, uint8_t picHeight) {		// Display binary picture (alternate method)
	uint8_t i, j, k;
	for(i = 0; i < picWidth; i++) {
		for(j = 0; j < picHeight; j++) {

		}
	}
}

void setPixel(uint8_t x, uint8_t y) {		// Turn pixel on at location (x, y)
	uint16_t temp = GFXAdd + (x/FW) + y*charWidth;		// Calculate RAM address for specific pixel
	exComNHD(0x24, temp & 0xFF, temp >> 8);		// Tell LCD where to set pixel
	comNHD(0xF8 | (FW - x%FW - 1));
}

void clearPixel(uint8_t x, uint8_t y) {		// Turn pixel off at location (x, y)
	uint16_t temp = GFXAdd + (x/FW) + y*charWidth;		// Calculate RAM address for specific pixel
	exComNHD(0x24, temp & 0xFF, temp >> 8);		// Tell LCD where to clear pixel
	comNHD(0xF0 | (FW - x%FW - 1));
}

	// Use setPixel to create a horizontal line where (x, y) is the left position of the line
void createLineLR(uint8_t x, uint8_t y, uint8_t length) {
	uint8_t i;
	uint16_t addr = GFXAdd + (x/FW) + y*charWidth;		// Calculate RAM address for start of line
	exComNHD(0x24, addr & 0xFF, addr >> 8);	// Tell LCD where to start line
	if(!length)		// Make sure a line is even going to be created
		return;
	for(i = x; i < x+length; i++)	// Set 'length' amount of pixels starting from (x,y) coordinate
		setPixel(i, y);
}

	// Use clearPixel to remove a horizontal line where (x, y) is the left position of the line
void deleteLineLR(uint8_t x, uint8_t y, uint8_t length) {
	uint8_t i;
	uint16_t addr = GFXAdd + (x/FW) + y*charWidth;		// Calculate RAM address for start of line
	exComNHD(0x24, addr & 0xFF, addr >> 8);	// Tell LCD where to start line
	if(!length)		// Make sure a line is even going to be created
		return;
	for(i = x; i < x+length; i++)	// Set 'length' amount of pixels starting from (x,y) coordinate
		clearPixel(i, y);
}

	// Use setPixel to create a verticle line where (x, y) is the top position of the line
void createLineUD(uint8_t x, uint8_t y, uint8_t length) {
	uint8_t i;
	uint16_t addr = GFXAdd + (x/FW) + y*charWidth;		// Calculate RAM address for start of line
	exComNHD(0x24, addr & 0xFF, addr >> 8);	// Tell LCD where to start line
	if(!length)		// Make sure a line is even going to be created
		return;
	for(i = y; i < y+length; i++)	// Set 'length' amount of pixels starting from (x,y) coordinate
		setPixel(x, i);
}

	// Use clearPixel to remove a verticle line where (x, y) is the top position of the line
void deleteLineUD(uint8_t x, uint8_t y, uint8_t length) {
	uint8_t i;
	uint16_t addr = GFXAdd + (x/FW) + y*charWidth;		// Calculate RAM address for start of line
	exComNHD(0x24, addr & 0xFF, addr >> 8);	// Tell LCD where to start line
	if(!length)		// Make sure a line is even going to be created
		return;
	for(i = y; i < y+length; i++)	// Set 'length' amount of pixels starting from (x,y) coordinate
		clearPixel(x, i);
}

void deleteBlock( uint8_t x, uint8_t y, uint8_t delWidth, uint8_t delHeight) {		// Clear a section of the screen of any graphics
	uint8_t i, j;
	for(i = 0; i < delWidth; i++) {
		for(j = 0; j < delHeight; j++) {
			clearPixel(x + i, y + j);
		}
	}
}

	// Create user controlled solid blocks on top of each other
void createBars(uint8_t barCount, uint8_t barMax, uint8_t x, uint8_t y, uint8_t barWidth, uint8_t barHeight) {
	uint16_t i;
	if(!barCount) {	// End recursion when at 0 bars
		return;
	}

	for(i = 1; i < barHeight; i++) {		// Create bar of selected height
		createLineLR(x+2, y+i+1, barWidth-4);		// Create each horizintal section of selected width
	}

	createBars(barCount - 1, barMax, x, y + barHeight, barWidth, barHeight);	// Recursively make the next bar
}

void createBox(uint8_t x, uint8_t y, uint8_t boxWidth, uint8_t boxHeight) {
	createLineLR(x, y, boxWidth);						// Create top of the box
	createLineUD(x, y, boxHeight);						// Create left of the box
	createLineUD(x + boxWidth - 1, y, boxHeight);		// Create right of the box
	createLineLR(x, y + boxHeight - 1, boxWidth);		// Create bottom of the box
}

void numGFX(uint8_t numVal, uint8_t numThick, uint8_t x, uint8_t y, uint8_t numWidth, uint8_t numHeight) {		// Create a simple number of variable size
	uint8_t i, numSeg = numChar[numVal];
	deleteBlock(x, y, numWidth + numThick, numHeight + numThick);		// Clear section to be used by number
	if(numSeg & 0x40) {					// Create top section if used
		for(i = 0; i < numThick; i++)			// Control for thickness of section
			createLineLR(x, y + i, numWidth + numThick);
	}
	if(numSeg & 0x20) {					// Create top-left section if used
		for(i = 0; i < numThick; i++)			// Control for thickness of section
			createLineUD(x + i, y, (numHeight>>1) + numThick);
	}
	if(numSeg & 0x10) {					// Create top-right section if used
		for(i = 0; i < numThick; i++)			// Control for thickness of section
			createLineUD(x + i + numWidth, y, (numHeight>>1) + numThick);
	}
	if(numSeg & 0x08) {					// Create middle section if used
		for(i = 0; i < numThick; i++)			// Control for thickness of section
			createLineLR(x, y + i + (numHeight>>1), numWidth + numThick);
	}
	if(numSeg & 0x04) {					// Create bottom-left section if used
		for(i = 0; i < numThick; i++)			// Control for thickness of section
			createLineUD(x + i, y + (numHeight>>1), (numHeight>>1) + numThick);
	}
	if(numSeg & 0x02) {					// Create bottom-right section if used
		for(i = 0; i < numThick; i++)			// Control for thickness of section
			createLineUD(x + i + numWidth, y + (numHeight>>1), (numHeight>>1) + numThick);
	}
	if(numSeg & 0x01) {					// Create bottom section if used
		for(i = 0; i < numThick; i++)			// Control for thickness of section
			createLineLR(x, y + i + numHeight - 1, numWidth + numThick);
	}
}

void updateBar(uint16_t* in, uint8_t monCount, uint8_t x, uint8_t y, uint8_t monWidth, uint8_t monHeight) {
	uint8_t spacer = 1;
	if(monWidth > resWidth) {monWidth = resWidth;}
	uint16_t temp, i, j, barWidth = (monWidth-2)/monCount - spacer;
	monWidth = ((barWidth + spacer)*monCount) + 3;

	// Create monitor section
	createBox(x, y, monWidth, monHeight);
	for(i = 0; i < monCount; i++) {		// Run for each device being monitored
		temp = (in[i]*(monHeight - 3)) / 100;		// Calculate number of rows to output
		for(j = 0; j < temp; j += 2) {			// Output every other row
			createLineLR(2+x+(barWidth+spacer)*i, y+monHeight-j-3, barWidth);	// Create the bar lines
		}
		for(j = temp; j < (monHeight - 3); j+=2) {
			deleteLineLR(2+x+(barWidth+spacer)*i, y+monHeight-j-3, barWidth);	// Delete the bar lines that may still be there
		}
	}
}

void updatePlot(uint16_t* in, uint8_t x, uint8_t y, uint8_t memWidth, uint8_t memHeight) {
	uint16_t i, j, tip, temp;
	createLineLR(x, y, memWidth);		// Create top of window
	createLineUD(x, y, memHeight - 1);		// Create left of window
	createLineUD(x + memWidth - 1, y, memHeight - 1);		// Create right of window
	createLineLR(x, y + memHeight - 1, memWidth);		// Create bottom of window

	for(i = 1; i < (memWidth - 1); i++) {			// Populate plot
		temp = (in[i - 1]*(memHeight - 1)) / 100;	// Calculate height of point on plot (assuming in[] = percentage value)
		if(temp >= memHeight)				// Make sure plot maximum cannot be exceeded
			temp = memHeight - 1;			// Make plot point max value
		tip = y + memHeight - temp - 1;		// Find tip location
		createLineUD(x + memWidth - 1 - i, tip, temp);		// Create line for plot point
		for(j = y + 1; j < tip; j++)			// Make sure pixels above the tip are clear
			clearPixel(x + memWidth - 1 - i, j);
	}
}
