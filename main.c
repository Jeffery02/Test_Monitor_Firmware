#define MAIN

// Required Libraries
#include "NHD-160128WG.h"

void initialize() {
	DDRD = 0xFF;		// Initialize data direction of port D
	DDRC = 0xFF;		// Initialize data direction of port C
	DDRA = 0x01;		// Initialize data direction of port A

	// ADC setup
	ADCSRA = (1<<ADEN) | (0<<ADPS2)| (1<<ADPS1)| (1<<ADPS0);	//enable ADC
}

uint16_t getADC(char chan) {		// Equation for current: (I * 8.25) + 100 = (ADC)
	ADMUX = chan;		// Select which ADC MUX channel to read
	_delay_us(100);		// Stabalize ADC MUX output before retrieving value
	uint16_t j = 0;
	ADCSRA |= (1<<ADSC);	// Start ADC conversion
	while(!(ADCSRA & (1<<ADIF)));	// Wait for ADC conversion to finish

	j = ADCL + (ADCH<<8);	// Extract ADC value
	return j;		// Return ADC value
}

void getTemp(uint8_t temp, uint8_t x, uint8_t y, uint8_t tempWidth, uint8_t tempHeight) {
	uint8_t tempThick = 3;											// Chosen line thickness of individual digits
	uint8_t digitWidth = ((tempWidth - 1)/3) - tempThick - 1;		// Calculate width of individual digits
	uint8_t digitHeight = ((tempHeight - tempThick)*2)/3;			// Calculate height of individual digits
	if(temp/100)
		numGFX(temp/100, tempThick, x, y + (tempHeight/3), digitWidth, digitHeight);	// Display hundreds digit
	else
		deleteBlock(x, y + (tempHeight/3), tempThick + digitWidth, tempThick + digitHeight);		// Do not display zero
	numGFX((temp%100)/10, tempThick, x + digitWidth + tempThick + 2, y + (tempHeight/3), digitWidth, digitHeight);			// Display tens digit
	numGFX(temp%10, tempThick, x + (digitWidth<<1) + ((tempThick + 2)<<1), y + (tempHeight/3), digitWidth, digitHeight);	// Display ones digit
	numGFX(0, 1, x + (digitWidth<<1) + ((tempThick + 2)<<1), y, digitWidth>>2, 1 +(digitWidth>>2));								// Display zero as degree symbol
	numGFX(0xF, 2, x + (digitWidth*2.5) + ((tempThick + 2)<<1), y, digitWidth>>1, (digitHeight/3 - 1));						// Display 0xF for Fahrenheit
}

void labels() {
	uint16_t i, j;
	// Voltage labels
	j = 0x26;					// Top of voltage bar label
	for(i = 1; i < 3; i++) {
		numGFX(j%16, 1, 8 - (i<<2), 0, 2, 5); j = j>>4;
	}
	numGFX(16, 1, 8, 0, 2, 5);			// Create 'V' for label
	clearPixel(8, 4); clearPixel(10, 4);		// Make 'V' more apparent
	j = 0x16;					// Bottom of voltage bar label
	for(i = 1; i < 3; i++) {
		numGFX(j%16, 1, 8 - (i<<2), 45, 2, 5); j = j>>4;
	}
	numGFX(16, 1, 8, 45, 2, 5);		// Create 'V' for label
	clearPixel(8, 49); clearPixel(10, 49);		// Make 'V' more apparent
	// Current labels
	j = 0x100A;					// Top of current plot label
	for(i = 0; i < 4; i++) {
		numGFX(j%16, 1, 56 - (i<<2), 0, 2, 5); j = j>>4;
	}
	clearPixel(56, 0); clearPixel(58, 0);		// Make 'A' more apparent
	j = 0x0A;					// Bottom of current plot label
	for(i = 0; i < 2; i++) {
		numGFX(j%16, 1, 56 - (i<<2), 45, 2, 5); j = j>>4;
	}
	clearPixel(56, 45); clearPixel(58, 45);		// Make 'A' more apparent
}

int main(void) {
	uint16_t volt, current[80], fahren, CPU[80], Mem[80], Net[80], Disk, i;
	initialize();
	initNHD();
	clearCharNHD();
	clearGFXNHD();
	labels();
	while(1) {
		_delay_ms(10);
		for(i = 80; i > 0; i--) {
			current[i] = current[i-1];
		}

		// get voltage
		volt = (getADC(7) + 1) * 50 / 512;
		updateBar(&volt, 1, 12, 0, 25, 50);

		// get current
		i = getADC(6);
		if(i < 100)
			i = 100;
		current[0] = (i - 100) * 4 / 33;
		updatePlot(current, 60, 0, 59, 50);

		// get temperature
		i = (getADC(6)>>2);
		getTemp(i, 120, 0, 39, 50);
	}

	while (1) {		// Test loop in case code ends
		_delay_ms(250);
		PORTA ^= 0x01;
	}
}