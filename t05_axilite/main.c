#include "xparameters.h"
#include "xil_io.h"
#include "myip.h"

volatile unsigned int *myip_ptr = (unsigned int*) XPAR_MYIP_0_S00_AXI_BASEADDR;

int main()
{
	// Low Level Driver
	MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR, 0, 0xDEADBEEF);

	// Direct Access
	*(myip_ptr + 1) = 0xBAADF00D;

	return 0;
}
