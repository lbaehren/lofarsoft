/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include "Pulsar/baseband_header.h"

int main () {

  baseband_header hdr;
  char* ptr1 = (char*) &hdr;
  char* ptr2 = 0;

  ptr2 = (char*) &(hdr.power_normalization);
  fprintf (stderr, "baseband_header.power_normalization = %d\n", ptr2-ptr1);

#if 0

  ptr2 = (char*) &(hdr.);
  fprintf (stderr, "baseband_header. = %d\n", ptr2-ptr1);

  ptr2 = (char*) &(hdr.);
  fprintf (stderr, "baseband_header. = %d\n", ptr2-ptr1);

  ptr2 = (char*) &(hdr.);
  fprintf (stderr, "baseband_header. = %d\n", ptr2-ptr1);

#endif

  int size = sizeof(baseband_header);

  if (size == BASEBAND_HEADER_SIZE) {
    fprintf (stderr, "baseband_header size = %d ... ok\n", size);
    return 0;
  }

  fprintf (stderr, 
	   "****************************************************\n"
	   "\n"
	   "WARNING baseband_header struct has wrong size!\n"
	   "\n"
	   "sizeof(baseband_header) == %d\n"
	   "BASEBAND_HEADER_SIZE    == %d\n"
	   "\n"
	   "****************************************************\n",
	   size, BASEBAND_HEADER_SIZE);

  return -1;
}

