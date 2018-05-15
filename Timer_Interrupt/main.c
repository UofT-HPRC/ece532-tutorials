#include "xtmrctr.h"
#include "xintc.h"
#include "xparameters.h"
#include "xil_exception.h"

/* constants */
#define X_MAX 640
#define Y_MAX 480

#define CLOCK XPAR_CPU_M_AXI_DP_FREQ_HZ // Clock frequency
#define FPS 30 // 30 Frames per Second
#define TIMER_INTR_ID 2
#define TIMER_CTR_NUM 0

/* global variable */
XTmrCtr TimerInstance;
XIntc IntCtlInstance;
volatile int delay_loop_on;

/* a struct that holds a color. The R, G, B value should not be larger than 0xF (16) */
typedef struct color_struct {
	char R;
	char G;
	char B;
} color;

/* function declarations */
void drawFilledRect(int* DDR_addr, int top_left_x, int top_left_y, int length,
		int width, color c);
void drawFilledCircle(int* DDR_addr, int cent_x, int cent_y, int radius,
		color c);
void clearScreen(int* DDR_addr);
void writeRGB(int* DDR_addr, int x, int y, color c);
void setVideoMemAddr(volatile int* TFT_addr, int* DDR_addr);
void disableVGA(volatile int* TFT_addr);
void enableVGA(volatile int* TFT_addr);

void init();

//void timer_init();
//void xintc_init();
//void xilexception_init();

int main() {
	// addresses
	int* DDR_addr = (int*) 0x80000000; // must be aligned to 2M
	volatile int* TFT_addr = (int *) 0x44a00000;

	// variables that relate to the screen drawing
	int x_pos, y_pos, old_x_pos = 0, old_y_pos = 0;
	int radius = 20;
	int speed = 8;

	// some predefined colors
	color black = { .R = 0x0, .G = 0x0, .B = 0x0 };
	color lettercolor = { .R = 0x0, .G = 0xa, .B = 0xa };
	color circlecolor = { .R = 0xd, .G = 0x4, .B = 0x9 };

	// screen setup
	disableVGA(TFT_addr);
	setVideoMemAddr(TFT_addr, DDR_addr);
	clearScreen(DDR_addr);
	enableVGA(TFT_addr);

	// draws the screen
	// "E"
	drawFilledRect(DDR_addr, 50, 95, 160, 60, lettercolor);
	drawFilledRect(DDR_addr, 50, 215, 160, 60, lettercolor);
	drawFilledRect(DDR_addr, 50, 335, 160, 60, lettercolor);
	drawFilledRect(DDR_addr, 50, 95, 45, 300, lettercolor);

	// "C"
	drawFilledRect(DDR_addr, 240, 95, 60, 300, lettercolor);
	drawFilledRect(DDR_addr, 240, 95, 160, 80, lettercolor);
	drawFilledRect(DDR_addr, 240, 315, 160, 80, lettercolor);

	// "E"
	drawFilledRect(DDR_addr, 430, 95, 160, 60, lettercolor);
	drawFilledRect(DDR_addr, 430, 215, 160, 60, lettercolor);
	drawFilledRect(DDR_addr, 430, 335, 160, 60, lettercolor);
	drawFilledRect(DDR_addr, 430, 95, 45, 300, lettercolor);

	// set up the timer, interrupt controller and exception
	init();

	// animation that uses timer as delay
	while (1) {
		for (y_pos = radius; y_pos < 95 - radius; y_pos += speed * 2) {
			for (x_pos = radius; x_pos < X_MAX - radius; x_pos += speed) {

				// delay
				//Start Timer
				delay_loop_on = 1;
				XTmrCtr_Start(&TimerInstance, TIMER_CTR_NUM);

				while (delay_loop_on) {
				}

				// erase the old moving circle
				drawFilledCircle(DDR_addr, old_x_pos, old_y_pos, radius, black);
				old_x_pos = x_pos;
				old_y_pos = y_pos;

				// a moving circle
				drawFilledCircle(DDR_addr, x_pos, y_pos, radius, circlecolor);
			}

			// randomly changes the color for the moving circle
			circlecolor.R = (circlecolor.R < 16) ? (circlecolor.R + 3) : 0;
			circlecolor.G = (circlecolor.G < 16) ? (circlecolor.G + 2) : 0;
			circlecolor.B = (circlecolor.B < 16) ? (circlecolor.B + 1) : 0;
		}
	}

	return 0;
}

/* draws a filled rectangle */
void drawFilledRect(int* DDR_addr, int top_left_x, int top_left_y, int length,
		int width, color c) {
	int i, j;

	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++) {
			writeRGB(DDR_addr, i + top_left_x, j + top_left_y, c);
		}
	}

}

/* draws a filled circle */
void drawFilledCircle(int* DDR_addr, int cent_x, int cent_y, int radius,
		color c) {

	int x, y;
	for (x = cent_x - radius; x < cent_x + radius; x++) {
		for (y = cent_y - radius; y < cent_y + radius; y++) {
			if ((cent_x - x) * (cent_x - x) + (cent_y - y) * (cent_y - y)
					<= radius * radius) {
				writeRGB(DDR_addr, x, y, c);
			}
		}
	}
}

/* empties the video buffer. DDR_addr must be aligned to 2M */
void clearScreen(int* DDR_addr) {
	int* DDR_addr_MAX = DDR_addr + 0x200000;
	for (; DDR_addr < DDR_addr_MAX; DDR_addr++) {
		*DDR_addr = 0x00000000;
	}
}

/* sets the argument pixel in the buffer to the argument color */
void writeRGB(int* DDR_addr, int x, int y, color c) {
	// take the least significant nybble
	char R = c.R & 0b00001111;
	char G = c.G & 0b00001111;
	char B = c.B & 0b00001111;

	// pixel info encoding
	int pixel_value = R * 0x100000 + G * 0x1000 + B * 0x10;

	DDR_addr[1024 * y + x] = pixel_value;
}

/* sets the video buffer address for the VGA display */
void setVideoMemAddr(volatile int* TFT_addr, int* DDR_addr) {
	TFT_addr[0] = (int) DDR_addr;
}

/* turns off the VGA display */
void disableVGA(volatile int* TFT_addr) {
	TFT_addr[1] = 0x00;
}

/* turns on the VGA display */
void enableVGA(volatile int* TFT_addr) {
	TFT_addr[1] = 0x01;
}

/* interrupt handling routine for the timer */
void Timer_InterruptHandler(void *TimerInstancePtr, u8 TmrCtrNumber) {
	delay_loop_on = 0;
}

/* sets up the timer, interrupt controller and exception */
void init() {
	// Initialize the timer counter. The ID can be found from xparameters.h
	XTmrCtr_Initialize(&TimerInstance, XPAR_AXI_TIMER_0_DEVICE_ID);

	// Initialize the interrupt controller driver. The ID can be found from xparameters.h
	XIntc_Initialize(&IntCtlInstance,
	XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);

	// Connect the device driver handler (timer) to the interrupt controller, so that
	// it can perform its own interrupt processing
	XIntc_Connect(&IntCtlInstance, TIMER_INTR_ID,
			(XInterruptHandler) XTmrCtr_InterruptHandler,
			(void *) &TimerInstance);

	// Start the interrupt controller so that it can receive interrupts from devices. Use XIN_REAL_MODE as mode
	XIntc_Start(&IntCtlInstance, XIN_REAL_MODE);

	// Enable the interrupt for the timer (on the interrupt controller)
	XIntc_Enable(&IntCtlInstance, TIMER_INTR_ID);

	// Initialize the exception table
	Xil_ExceptionInit();

	// Register the interrupt controller handler with the exception table
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XIntc_InterruptHandler, &IntCtlInstance);

	// Enable exceptions
	Xil_ExceptionEnable();

	// Set the interrupt handler (callback functions) for the timer.
	// This interrupt handler will be called when the timer interrupt is generated
	XTmrCtr_SetHandler(&TimerInstance, Timer_InterruptHandler, &TimerInstance);

	// Set the mode of the timer. Enables the interrupt, auto reload, and makes the timer count down
	XTmrCtr_SetOptions(&TimerInstance,
	TIMER_CTR_NUM,
	XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION | XTC_DOWN_COUNT_OPTION);

	// The timer will count from CLOCK/FPS to 0
	XTmrCtr_SetResetValue(&TimerInstance, TIMER_CTR_NUM,
	CLOCK / FPS);
}
