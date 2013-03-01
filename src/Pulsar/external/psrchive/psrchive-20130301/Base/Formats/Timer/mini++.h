/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/mini++.h,v $
   $Revision: 1.8 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __MINI_PLUSPLUS_H
#define __MINI_PLUSPLUS_H

// ////////////////////////////////////////////////////////////////////
// C++ methods that perform operations on the mini struct
// or calculations with the mini struct
//
// call these functions like:  Mini::load (), etc.
//
// ////////////////////////////////////////////////////////////////////
 
#include <stdio.h>
#include <string>

#include "mini.h"
#include "MJD.h"

namespace Mini {

  // may be used to receive soft error messages
  extern std::string reason;

  // a function to set most of mini struct to default values.
  void init (struct mini& hdr);

  // loads a mini struct, givent a file name
  int fload (const char* fname, struct mini* hdr, bool big_endian=true);

  // loads a mini struct from a file pointer
  int load (FILE* fptr, struct mini* hdr, bool big_endian=true);

  // unloads a mini struct to a file (always big endian)
  int unload (FILE* fptr, struct mini& hdr);

  // returns the MJD from a mini struct
  MJD get_MJD (const struct mini& hdr);

  // sets the MJD in a mini struct
  void set_MJD (struct mini& hdr, const MJD& mjd);

}

#endif // __MINI_PLUSPLUS_H
