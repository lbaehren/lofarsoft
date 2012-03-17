/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ThreadMemory.h"
#include "tostring.h"

#include <stdlib.h> 

using namespace std;

void destructor (void* memory)
{
  if (memory)
    free (memory);
}


ThreadMemory::ThreadMemory ()
{
  pthread_key_create (&key, &destructor);
}

ThreadMemory::~ThreadMemory ()
{
  pthread_key_delete (key);
}

void * ThreadMemory::get ()
{
  
  void * memory= pthread_getspecific (key);
  if (!memory)
    throw Error (InvalidState, "ThreadMemory::get",
		 "key ptr not set for this thread");
  return memory;
}

void ThreadMemory::set (void * memory)
{
  pthread_setspecific (key, memory);
}
