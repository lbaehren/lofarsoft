/***************************************************************************
 *
 *   Copyright (C) 2004 by Stephen Ord
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"
#include "Reference.h"
#include "dirutil.h"

#include <unistd.h>


int main (int argc, char** argv)
{ try {

  // Pulsar::Archive::verbose = true;
  // Error::verbose = true;
  // Pulsar::Integration::verbose = true;

  int c = 0;
  const char* args = "avVF:";
  int denoise_fraction = 8;
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {
      
    case 'a':
      Pulsar::Archive::agent_report ();
      break;
    case 'F':
      denoise_fraction = atoi(optarg);
      break;

    case 'V':
      Pulsar::Integration::verbose = true;
    case 'v':
      Pulsar::Archive::verbose = true;
      break;

    }
 
  vector <string> filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) {
 
  cerr << "load archive from " << filenames[ifile] << endl;
  Reference::To<Pulsar::Archive> archive;
  archive = Pulsar::Archive::load (filenames[ifile]);

  cerr << "clone archive for denoise" << endl;
  Reference::To<Pulsar::Archive> clone = archive -> clone();

  cerr << "tscrunch" << endl;
  clone -> tscrunch ();

  cerr << "tscrunch from " << archive->get_nsubint() 
       << " to "  << clone->get_nsubint() << " sub-integrations" << endl;

  cerr << "fscrunch" << endl;
  clone -> fscrunch ();

  cerr << "fscrunch from " << archive->get_nchan() 
       << " to "  << clone->get_nchan() << " sub-channels" << endl;

  cerr << "Denoise " << endl;
  clone -> denoise (denoise_fraction);

  cerr << "unload denoise.ar" << endl;
  clone -> unload ("denoise.ar");

  }

}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
catch (string& error) {
  cerr << error << endl;
  return -1;
}
catch (...) {
  cerr << "An exception was thrown" << endl;
  return -1;
} 

 return 0;
}
