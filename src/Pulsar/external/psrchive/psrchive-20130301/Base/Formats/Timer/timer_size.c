/***************************************************************************
 *
 *   Copyright (C) 2001 by wvanstra Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include "timer.h"

int main () {

  struct timer hdr;
  char* ptr1 = (char*) &hdr;
  char* ptr2 = 0;

  int size = sizeof(struct timer);

  ptr2 = (char*) &(hdr.rcvr_id);
  fprintf (stderr, "timer.rcvr_id = %d\n", ptr2-ptr1);

  if (ptr2-ptr1 != 832) {
    fprintf (stderr, 
	     "****************************************************\n"
	     "\n"
	     "WARNING timer header rcvr_id has wrong address!\n"
	     "\n"
	     "****************************************************\n");
    return -1;
  }

  if (size != TIMER_SIZE) {
    fprintf (stderr, 
	     "****************************************************\n"
	     "\n"
	     "WARNING timer header struct has wrong size!\n"
	     "\n"
	     "sizeof(timer) == %d\n"
	     "TIMER_SIZE    == %d\n"
	     "\n"
	     "****************************************************\n",
	     size, TIMER_SIZE);
    
    return -1;
  }

  fprintf (stderr, "timer size ok\n");
  return 0;
}
