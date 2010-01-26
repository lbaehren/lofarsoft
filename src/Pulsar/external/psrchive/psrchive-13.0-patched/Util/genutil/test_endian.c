/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>

#include <inttypes.h>
#include "machine_endian.h"

#define BITSPERBYTE 8

int main ()
{
  unsigned int bit, byte;

  int i;
  unsigned char* x1 = NULL;
 
  uint32_t data = 0x8f04c014;

  unsigned char equiv[4] = { 0x8f, 0x04, 0xc0, 0x14 };

  printf ("\nThe test starts with the following number:\n\n");
  printf ("uint64_t: %08x\n\n", data);

  printf ("The following bits are printed from highest to lowest order.\n");

  printf ("bits shifted right    : ");
  for (i=sizeof(uint32_t)*BITSPERBYTE-1; i>=0; i--) {
    if ((i+1)%8 == 0) printf ("*");
    bit = (data >> i) & 0x0001;
    printf ("%d", bit);
  }
  printf ("\n\n");

  printf ("When the address is cast to an unsigned char*, the bytes will\n");
#if MACHINE_LITTLE_ENDIAN
  printf ("have the opposite ordering on a lower endian machine.\n");
#else
  printf ("have the same ordering on an uppper endian machine.\n");
#endif
  x1 = (unsigned char*) &data;
  printf ("\nunsigned char : ");
  for (byte=0; byte<4; byte++)
    printf ("%02x ", x1[byte]);
  printf ("\n");

#if MACHINE_LITTLE_ENDIAN

  for (byte=0; byte<4; byte++)
    if (x1[byte] != equiv[3-byte]) {
      printf ("Unexpected byte ordering for supposedly Ltl Endian machine!\n");
      return -1;
    }

#else

  for (byte=0; byte<4; byte++)
    if (x1[byte] != equiv[byte]) {
      printf ("Unexpected byte ordering for supposedly Big Endian machine!\n");
      return -1;
    }

#endif


  printf ("\nbits inspected by byte: ");
  for (byte=0; byte<4; byte++) {
    printf ("*");
    for (i=BITSPERBYTE-1; i>-1; i--) {
      bit = (*x1 >> i) & 0x0001;
      printf ("%d", bit);
    }
    x1 ++;
  }

  printf ("\n\n");
  return 0;
}

