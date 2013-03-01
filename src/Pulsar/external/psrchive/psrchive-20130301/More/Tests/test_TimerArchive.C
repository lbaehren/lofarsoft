/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <cpgplot.h>

#include "TimerPulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

int main (int argc, char** argv)  {

  try {

    Pulsar::Archive::verbose = true;
    Pulsar::Integration::verbose = true;
    Pulsar::Profile::verbose = true;

    Pulsar::TimerArchive archive;
    
    if (argc > 1)
      archive.load (argv[1]);
    
    archive.pscrunch();
    archive.fscrunch();
    archive.tscrunch();
    archive.bscrunch(4);

    cpgbeg (0, "?", 0, 0);
    archive.get_Profile(0,0,0)->display();
    cpgend ();

  }

  catch (Pulsar::Error& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}

