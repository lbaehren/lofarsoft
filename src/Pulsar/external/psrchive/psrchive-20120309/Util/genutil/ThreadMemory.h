//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten & Andrew Jameson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __ThreadMemory_h
#define __ThreadMemory_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

#include "Warning.h"

//! Manages a unique void * storage for each thread
class ThreadMemory
{
public:

  //! Default constructor
  ThreadMemory ();

  //! Destructor
  ~ThreadMemory ();

  //! Interface to get pointer
  void * get ();

  //! Set the raw void * for the current thread
  void set (void *);

protected:

#ifdef HAVE_PTHREAD
  pthread_key_t key;
#endif

};

#endif
