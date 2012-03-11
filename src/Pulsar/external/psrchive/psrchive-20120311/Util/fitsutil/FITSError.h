//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __FITSError_h
#define __FITSError_h

#include "Error.h"

class FITSError : public Error {

  public:

  //! Error with optional printf-style message
  FITSError (int status, const char* func, const char* msg=0, ...);

  //! Error with string message
  FITSError (int status, const char* func, const std::string& msg);

  //! Destructor
  ~FITSError () {}

};

#endif
