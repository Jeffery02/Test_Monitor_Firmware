
// Required Libraries
#include "NHD-160128WG.h"

uint8_t flip[] = {0b0000, 0b1000, 0b0100, 0b1100, 0b0010, 0b1010, 0b0110, 0b1110, 0b0001, 0b1001, 0b0101, 0b1101, 0b0011, 0b1011, 0b0111, 0b1111};

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

uint8_t flipByte(uint8_t in) {		// Flip the bits of a byte
	return(flip[in&0x0F]<<4 | flip[in>>4]);
}

int main(void) {
	uint16_t i = 0, j, k;
	uint8_t x, y;
	char test[6];
	initialize();
	initNHD();
	clearCharNHD();
	clearGFXNHD();
	while(0) {
		_delay_ms(10);
		clearCharNHD();
		j = getADC(5);
		if (j > 100)
			j = (j - 100) * 40 / 33;
		else
			j = 0;
		test[5] = 0x00;
		test[4] = 0x30 + j%10; j /= 10;
		test[3] = '.';
		test[2] = 0x30 + j%10; j /= 10;
		test[1] = 0x30 + j%10; j /= 10;
		test[0] = 0x30 + j%10; j /= 10;
		strNHD(test);
	}
	testMon[0] = (getADC(6) + 1) * 50 / 512;
	testMon[1] = (getADC(7) + 1) * 50 / 512;
	updateMonitor(16, 20);
	updatePlotMem(0, 20, resWidth/2 - 1, 30);
	updatePlotNet(resWidth/2 + 1, 20, resWidth/2 - 1, 30);
	while(1) {
		_delay_ms(25);
		for(j = 128; j > 0; j--) {
			memMon[j] = memMon[j-1];
			netMon[j] = netMon[j-1];
		}
		memMon[0] = (getADC(6) + 1) * 50 / 512;
		netMon[0] = (getADC(7) + 1) * 50 / 512;
		updatePlotMem(0, 20, resWidth/2 - 1, 30);
		updatePlotNet(resWidth/2 + 1, 20, resWidth/2 - 1, 30);
		testMon[0] = (getADC(6) + 1) * 50 / 512;
		testMon[1] = (getADC(7) + 1) * 50 / 512;
		updateMonitor(16, 20);
	}
	while(1);
	_delay_ms(1000);
	strNHD("This is a test.\nMy name is Jeffery.\nHello World!\nHow's this John?\0");
	strNHD("This seems to work for now.\0");
	for (i = 4; i > 0; i--) {
		for (j = 0; j < i; j++) {
			for (k = 0; k < i; k++) {
				x = 0 + (4 - i) + (k*2);
				y = 6 + (4 - i) + (j*2);
				gfxNHD(testGFX, 2, 2, x, y);
			}
		}
	}
	gfxNHD(testGFX, 2, 2, x + 8, y);
	createLineUD(60, 50, 60);
	updateMonitor(16, 20);
	while (1) {
		_delay_ms(250);
		PORTA ^= 0x01;
	}
}