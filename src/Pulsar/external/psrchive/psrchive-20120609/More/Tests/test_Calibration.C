/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>
#include <algorithm>

#include "Database.h"
#include "strutil.h"

void usage ()
{
  cerr << "psrcal file1 [file2 ...]" << endl;
}

int main (int argc, char** argv) 
{
  char* metafile = NULL;

  bool verbose = false;
  char c;
  while ((c = getopt(argc, argv, "hvV")) != -1)  {

    switch (c)  {

    case 'h':
      usage();
      return 0;

    case 'M':
      metafile = optarg;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      Pulsar::Calibrator::verbose = true;
    case 'v':
      Error::verbose = true;
      verbose = true;
      break;

    } 
  }

  if (!metafile && optind >= argc) {
    cerr << "psrcal requires a list of archive filenames as parameters.\n";
    return -1;
  }

  vector <string> filenames;
  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  // sort the filename (roughly speaking, by date)
  sort (filenames.begin(), filenames.end());

  // the individual archive
  Reference::To<Pulsar::Archive> archive;

  Pulsar::Database dbase;
 
  for (unsigned ifile=0; ifile<filenames.size(); ifile++) {  try {

    archive = Pulsar::Archive::load( filenames[ifile] );

    Pulsar::PolnCalibrator* pcal = dbase.generatePolnCalibrator (archive,
				Pulsar::Database::SingleAxis);
    
    pcal->calibrate (archive);
    
    archive->unload ( filenames[ifile] + ".calib" );

  }
  catch (Error& error) {
    cerr << "psrcal: Error calibrating " << filenames[ifile] << error << endl;
    return -1;
  }
  }

  return 0;
}
