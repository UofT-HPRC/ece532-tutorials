/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"

volatile unsigned int * led = (unsigned int *)0x40000000;
volatile unsigned int * swt = (unsigned int *)0x40010000;

#define BRAMSIZE 8
unsigned int * brambase = (unsigned int *)0xc0000000;

#define PRINT xil_printf

unsigned int index;
unsigned int step = 0;

int main(void)
{


    init_platform();

    //PRINT("Hello global World\n\r");

    PRINT("Writing to memory\n\r");

    step = 1;

    for(index=0; index<BRAMSIZE; index++)
      {
	*(brambase+index) = index+1;
	PRINT("Write %d at location %d = %X\n\r",index+1,index,brambase+index);
	step = step + 1;
      }

    PRINT("\n\rReading from memory and checking\n\r");

    step++;

    for(index=0; index<BRAMSIZE; index++)
    {
      PRINT("Read %d at location %d = %X\n\r",*(brambase+index),index,brambase+index);
      step = step + 1;
    }

    PRINT("Done reading BRAM, start polling switches\n\r");
    step++;

    while(1)
      *led = *swt;

    return 0;
}
