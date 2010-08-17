/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

// #define _DEBUG 1

/*! Starts an observation on the next UTC second, returned as time_t

    \param start_routine the function that does the starting
    \param arguments the arguments passed to start_routine
*/

time_t start_observation( int(*start_routine)(void*), void* arg )
{

#ifdef _DEBUG
  fprintf (stderr, "busy sleep till next second tick\n");
#endif

  /* busy sleep until a second has ticked over */
  time_t current = time(0);
  time_t prev = current;
  while (current == prev)
    current = time(0);
                                                                                                                                        
#ifdef _DEBUG
  fprintf (stderr, "0.5 second sleep\n");
#endif

  /* sleep for precisely 0.5 seconds */
  struct timeval timeout;
  timeout.tv_sec=0;
  timeout.tv_usec=500000;

  /* sleep for 0.5 seconds */
  select(0,NULL,NULL,NULL,&timeout);

#ifdef _DEBUG
  fprintf (stderr, "call start routine\n");
#endif
  start_routine (arg);

#ifdef _DEBUG
  fprintf (stderr, "return utc %d\n", (int) current + 1);
#endif
  /* return utc of anticpated start */
  return (current + 1);

}
