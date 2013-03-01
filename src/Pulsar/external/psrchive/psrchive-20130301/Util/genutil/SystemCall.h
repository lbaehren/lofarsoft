//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* Util/genutil/SystemCall.h */

#ifndef __SystemCall_h
#define __SystemCall_h

#include <string>

//! Executes a system call with multiple retried
class SystemCall
{
  unsigned retries;

 public:

  //! Create a temporary directory
  SystemCall () { retries = 3; }

  //! Set the number of retries
  void set_retries (unsigned n) { retries = n; }

  //! Run the given command
  void run (const std::string&);

};

#endif

