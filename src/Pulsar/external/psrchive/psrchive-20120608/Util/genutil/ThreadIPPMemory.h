//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten & Andrew Jameson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __ThreadIPPMemory_h
#define __ThreadIPPMemory_h

#include <ippcore.h>

#include "ThreadMemory.h"
#include "Warning.h"

//! Manages a unique void * storage for each thread
class ThreadIPPMemory : public ThreadMemory
{
  public:

    //! Default constructor
    ThreadIPPMemory ();

    //! Destructor
    ~ThreadIPPMemory ();

    //! Interface to get pointer
    Ipp8u * get ();

    //! Set the raw void * for the current thread
    void set (Ipp8u *);

};

#endif
