/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TimerArchive.h"
#include "timer++.h"

using namespace std;

string Pulsar::TimerArchive::Agent::get_description ()
{
  return "TIMER Archive version 12.3";
}

//! return true if filename refers to a timer archive
bool Pulsar::TimerArchive::Agent::advocate (const char* filename)
{
  struct timer hdr;

  Timer::verbose = verbose > 2;
  
  if ( Timer::fload (filename, &hdr, TimerArchive::big_endian) < 0 )  {
    if (verbose > 2)
      cerr << "Pulsar::TimerArchive::Agent::advocate fail Timer::fload" << endl;
    return false;
  }

  /* If backend-specific extensions exist, do not advocate use of this class
     and allow the backend-specific plugin to deal with it */
  if ( Timer::backend_recognized (hdr.backend) != -1)  {
    if (verbose > 2)
      cerr << "Pulsar::TimerArchive::Agent::advocate unrecognized backend '"
           << string(hdr.backend,0,BACKEND_STRLEN) << "'" << endl;
    return false;
  }

  return true;
}


