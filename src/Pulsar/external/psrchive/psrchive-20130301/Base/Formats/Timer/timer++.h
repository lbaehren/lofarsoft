/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/timer++.h,v $
   $Revision: 1.17 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __TIMER_PLUSPLUS_H
#define __TIMER_PLUSPLUS_H

// ////////////////////////////////////////////////////////////////////
// C++ methods that perform operations on the timer struct
// or calculations with the timer struct
//
// call these functions like:  Timer::load, etc.
//
// ////////////////////////////////////////////////////////////////////
 
#include <stdio.h>
#include "timer.h"
#include "MJD.h"

namespace Timer {

  extern bool verbose;

  const int nbackends = 2;

  // returns the size of the additional backend information, or 0 if none
  unsigned long backend_data_size(const struct timer& hdr);

  // returns index of code in 'backends' or -1 if not recognized
  int backend_recognized (const char* backend);

  // sets the name of the backend in 'hdr' to 'backend'
  // returns 0 if successful, -1 otherwise
  int set_backend (struct timer* hdr, const char* backend);

  // returns a string containing the backend code, or "un-recognized" if so
  std::string get_backend (const struct timer& hdr);

  // may be used to receive soft error messages
  extern std::string reason;

  // loads a timer struct, givent a file name
  int fload (const char* fname, struct timer* hdr, bool big_endian=true);

  // loads a timer struct from a file pointer
  int load (FILE* fptr, struct timer* hdr, bool big_endian=true);

  // unloads a timer struct to a file (always big endian)
  int unload (FILE* fptr, const struct timer& hdr);

  // returns the MJD from a timer struct
  MJD get_MJD (const struct timer& hdr);

  // sets the MJD in a mini struct
  void set_MJD (struct timer& hdr, const MJD& mjd);

  // returns the number of polarizations
  int get_npol (const struct timer& hdr);
  void set_npol (struct timer& hdr, int npol);

  // returns true if two archives can be mixed (tadded, for instance)
  // set max_freq_sep if you wish to alter the action of this function
  bool mixable (const timer& hdr1, const timer& arch2,
		double max_freq_sep = 0.10 /* 100kHz */,
		bool allow_opposite_sideband = false);

  bool cal_mixable (const timer& hdr1, const timer& arch2,
		    double max_freq_sep = 0.10 /* 100kHz */,
		bool allow_opposite_sideband = false);

  // a function to set most of timer struct to NULL values.
  void init (struct timer * hdr);

  // return true if timer struct seems valid enough
  bool is_timer (const struct timer& hdr, std::string* reason = 0);

}

#endif // __TIMER_PLUSPLUS_H
