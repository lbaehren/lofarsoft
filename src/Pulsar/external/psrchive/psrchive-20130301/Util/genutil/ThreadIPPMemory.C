/***************************************************************************
 *
 *   Copyright (C) 2011 by Andrew Jameson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ThreadIPPMemory.h"
#include "tostring.h"

using namespace std;

void ipp_destructor (void* memory)
{
  if (memory)
    ippFree (memory);
  memory = 0;
}

ThreadIPPMemory::ThreadIPPMemory()
{
  pthread_key_create (&key, &ipp_destructor);
}

ThreadIPPMemory::~ThreadIPPMemory ()
{
  pthread_key_delete (key);
}

Ipp8u * ThreadIPPMemory::get ()
{
  
  void * memory = pthread_getspecific (key);
  if (!memory)
    throw Error (InvalidState, "ThreadMemory::get",
		 "key ptr not set for this thread");
  return (Ipp8u *) memory;
}

void ThreadIPPMemory::set (Ipp8u * memory)
{
  pthread_setspecific (key, (void *) memory);
}
