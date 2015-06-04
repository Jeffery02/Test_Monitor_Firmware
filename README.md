Test_Monitor_Firmware_Scipio (2015)

This code is meant for the pinout of the test monitor board in the backplane of Scipio. Using an AVR USB ASP programmer, the board can be programmed directly.

All functions assume that the (x, y) coordinates are of the top left corner of the desired output.
The 'x' address determines how many pixels away from the left edge of the screen that is being addressed.
The 'y' address determines how many pixels away from the top edge of the screen that is being addressed.


x=		 0 1 2 3 4 5 6 7 8 9 . . . 158 159 160
		_____________________________________
y=	0	|		    						|
	1	|		   x = 5 					|
	2	|   y = 3 >v________                |
	3	|		   |		|				|
	4	|		   |		|				|
	5	|		   |________|				|
	6	|									|
	7	|									|
	8	|									|
	9	|									|
	.	|									|
	.	|									|
	.	|									|
	127	|									|
	128	|___________________________________|
