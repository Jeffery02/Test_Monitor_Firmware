micro = atmega32

makeCompile: main.c
	avr-gcc -g -O3 -mmcu=$(micro) -c main.c NHD-160128WG.c
	avr-gcc -g -O3 -mmcu=$(micro) -o main.elf main.o NHD-160128WG.o
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

program: main.hex 
	sudo avrdude -c USBasp -p $(micro) -U flash:w:main.hex:i

all: main.c
	avr-gcc -g -O3 -mmcu=$(micro) -c main.c NHD-160128WG.c
	avr-gcc -g -O3 -mmcu=$(micro) -o main.elf main.o NHD-160128WG.o
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	sudo avrdude -c USBasp -p $(micro) -U flash:w:main.hex:i
	
test: test.c
	avr-gcc -g -O3 -mmcu=$(micro) -c test.c 
	avr-gcc -g -O3 -mmcu=$(micro) -o test.elf test.o
	avr-objcopy -j .text -j .data -O ihex test.elf test.hex
	sudo avrdude -c USBasp -p $(micro) -U flash:w:test.hex:i

setfuseExt:
	avrdude -c USBasp -p $(micro) -U lfuse:w:0xce:m -U hfuse:w:0xd9:m 

setfuseInt:
	avrdude -c USBasp -p $(micro) -U lfuse:w:0xe4:m -U hfuse:w:0xd9:m 

setfuseDefault:
	avrdude -c USBasp -p $(micro) -U lfuse:w:0xe1:m -U hfuse:w:0x99:m 

