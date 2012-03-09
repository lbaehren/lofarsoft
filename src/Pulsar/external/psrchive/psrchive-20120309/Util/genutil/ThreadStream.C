/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ThreadStream.h"
#include "tostring.h"

#include <fstream>

using namespace std;

void destructor (void* value)
{
  ostream* stream = reinterpret_cast<ostream*> (value);
  delete stream;
}


ThreadStream::ThreadStream ()
{
  pthread_key_create (&key, &destructor);
}

ThreadStream::~ThreadStream ()
{
  pthread_key_delete (key);
}

ostream& ThreadStream::get ()
{
  ostream* value = reinterpret_cast<ostream*>( pthread_getspecific (key) );

  if (!value)
    throw Error (InvalidState, "ThreadStream::get",
		 "std::ostream not set for this thread");

  return *value;
}


void ThreadStream::set (std::ostream* stream)
{
  pthread_setspecific (key, stream);
}
