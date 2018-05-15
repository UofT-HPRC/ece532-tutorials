#include <stdio.h>
#include <stdlib.h>
#include "fsl.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"


#define DDR_ADDR			0x80000000
#define SAMPLE_SIZE			122000			// Size per note


void output_audio(unsigned int address) {

	unsigned int byte1, byte2, byte3, byte4; // per read, 4 bytes to be sent to audio out
	unsigned int i, value;
	for (i = 0; i < SAMPLE_SIZE; i += 4) { // 4 bytes per read (32 bits)
		// Get sound data
		byte1 = byte2 = byte3 = byte4 = 0;

		value = Xil_In32(address + i);
		byte1 = (value >> 24) & 0xFF;
		byte2 = (value >> 16) & 0xFF;
		byte3 = (value >> 8) & 0xFF;
		byte4 = value & 0xFF;

		// Output audio
		putfslx(byte1, 0, FSL_DEFAULT);
		putfslx(byte2, 0, FSL_DEFAULT);
		putfslx(byte3, 0, FSL_DEFAULT);
		putfslx(byte4, 0, FSL_DEFAULT);
	}
}


int main() {

	// Write a square wave in DDR memory
	int* data_ptr = (int *) DDR_ADDR;
	int counter;
	int end = SAMPLE_SIZE;

	for (counter = 0; counter < end; counter++) {
		if ((counter / 12) % 2 == 0)
			data_ptr[counter] = 0x70707070;
		else
			data_ptr[counter] = 0x00000000;
	}

	// Output the audio
	while (1) {
		output_audio(DDR_ADDR);
	}

	return 0;
}
