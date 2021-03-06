


/* In this tutorial our compiler invocation becomes more complicated. On macOS, 
we compile with...
    clang 040mainTexturing.c 000pixel.o -lglfw -framework OpenGL
This invocation causes our compiled code to be 'linked' with three other pieces 
of software, that have been compiled elsewhere: OpenGL, GLFW, and pixel.o. 
OpenGL is a cross-platform, standardized library for interacting with graphics 
hardware. It comes pre-installed on macOS and Linux, and is easily installed on 
Windows. GLFW is a cross-platform, minimalist user interface toolkit, that we 
can use to build simple OpenGL applications. Often it is not pre-installed, but 
it is easy to install. pixel.o is a library that I have written, to provide a 
simple foundation for this graphics course. You must have pixel.o and pixel.h 
in the same directory as linking.c. */

/* Notice that we include pixel.h, enclosed in quotation marks rather than 
angle brackets. Feel free to read pixel.h. It documents all of the public 
functionality of the library pixel.o. */
#include <stdio.h>
#include <math.h>
#include "000pixel.h"

char gMouseDown = 0;
char gColor = 'W';

/* This function is a user interface 'callback'. Once we register it with the 
user interface infrastructure, it is called whenever the user releases a 
keyboard key. For details, see the pixSetKeyUpHandler documentation. */
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {
	if (!shiftIsDown && !controlIsDown && !altOptionIsDown && !superCommandIsDown) {
	    gColor = key;
	}
}

/* Similarly, the following callbacks handle some mouse interactions. */
void handleMouseUp(int button, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {
	if (!shiftIsDown && !controlIsDown && !altOptionIsDown && !superCommandIsDown) {
	    gMouseDown = 0;
	}
}

void handleMouseDown(int button, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {
	if (!shiftIsDown && !controlIsDown && !altOptionIsDown && !superCommandIsDown) {
	    gMouseDown = 1;
	}
}

void handleMouseMove(double x, double y) {
    if (gMouseDown) {
        switch (gColor) {
            case 'R':
                pixSetRGB(x, y, 1.0, 0.0, 0.0);
                break;
            case 'G':
                pixSetRGB(x, y, 0.0, 1.0, 0.0);
                break;
            case 'B':
                pixSetRGB(x, y, 0.0, 0.0, 1.0);
                break;
            case 'C':
                pixSetRGB(x, y, 0.0, 1.0, 1.0);
                break;
            case 'M':
                pixSetRGB(x, y, 1.0, 0.0, 1.0);
                break;
            case 'Y':
                pixSetRGB(x, y, 1.0, 1.0, 0.0);
                break;
            case 'K':
                pixSetRGB(x, y, 0.0, 0.0, 0.0);
                break;
            case 'W':
                pixSetRGB(x, y, 1.0, 1.0, 1.0);
                break;
        }
    }
}

void handleMouseScroll(double xOffset, double yOffset) {
	printf("mouse scroll xOffset %f yOffset %f\n", xOffset, yOffset);
}

/* This callback is called once per animation frame. As parameters it receives 
the time for the current frame and the time for the previous frame. Both times 
are measured in seconds since some distant past time. */ 
void handleTimeStep(double oldTime, double newTime) {
	if (floor(newTime) - floor(oldTime) >= 1.0)
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
}

/* You can also set callbacks for key down, key repeat, and mouse down. See 
000pixel.h for details. */

int main(void) {
	/* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
	returns 0 if no error occurred. */
	if (pixInitialize(512, 512, "Pixel Graphics") != 0) {
		return 1;
	} else {
		/* Register the callbacks (defined above) with the user interface, so 
		that they are called as needed during pixRun (invoked below). */
		pixSetKeyUpHandler(handleKeyUp);
		pixSetMouseUpHandler(handleMouseUp);
		pixSetMouseDownHandler(handleMouseDown);
		pixSetMouseMoveHandler(handleMouseMove);
		// pixSetMouseScrollHandler(handleMouseScroll);
		// pixSetTimeStepHandler(handleTimeStep);
		
		/* Clear the window to black. */
		pixClearRGB(0.0, 0.0, 0.0);
		
		/* Run the event loop. The callbacks that were registered above are 
		invoked as needed. At the end, the resources supporting the window are 
		deallocated. */
		pixRun();
		return 0;
	}
}


