#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
// Read/Write 16384 words or 64kB
#define TEST_SIZE 16384

// Pointer to the external memory
volatile unsigned int * memptr = (unsigned int*) XPAR_MIG_7SERIES_0_BASEADDR;
// Thomas Wang's 32-bit mix hash
unsigned int hash(unsigned int key)
{
key += ~(key << 15);
key ^= (key >> 10);
key += (key << 3);
key ^= (key >> 6);
key += ~(key << 11);
key ^= (key >> 16);
return key;
}
int main()
{
  init_platform();

  int i, errors;

  // Write TEST_SIZE words to memory
  print("BEGIN WRITE\n\r");
  for (i = 0; i < TEST_SIZE; i++)
    {
      memptr[i] = hash(i);
    }

  // Read TEST_SIZE words to memory and compare with golden values
  print("BEGIN READ\n\r");
  errors = 0;
  for (i = 0; i < TEST_SIZE; i++)
    {
      if (memptr[i] != hash(i))
	errors++;
    }
  
  // Print Results
  if (errors != 0)
    print("ERROR FOUND\n\r");
  else
    print("ALL GOOD!\n\r");
  return 0;
}
