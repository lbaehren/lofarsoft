/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "strutil.h"
#include "dirutil.h"

#include <unistd.h>

void usage ()
{
  cerr << 
    "psr_template - template PSRCHIVE program \n"
    "\n"
    "psr_template [options] filename[s]\n"
    "options:\n"
    " -h               Help page \n"
    " -M metafile      Specify list of archive filenames in metafile \n"
    " -q               Quiet mode \n"
    " -v               Verbose mode \n"
    " -V               Very verbose mode \n"
    "\n"
       << endl;
}

int main (int argc, char** argv)
{
  // name of file containing list of Archive filenames
  char* metafile = NULL;

  char c;
  while ((c = getopt(argc, argv, "hM:qvV")) != -1) 

    switch (c)  {

    case 'h':
      usage();
      return 0;

    case 'M':
      metafile = optarg;
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'v':
      Pulsar::Archive::set_verbosity (2);
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;


    } 


  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty()) {
    cout << "psr_template: please specify filename[s]" << endl;
    return -1;
  } 

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load( filenames[ifile] );

    unsigned nsub = archive->get_nsubint();
    unsigned nchan = archive->get_nchan();
    unsigned npol = archive->get_npol();
    unsigned nbin = archive->get_nbin();

    for (unsigned isub=0; isub < nsub; isub++) {

      Pulsar::Integration* integration = archive->get_Integration(isub);

      for (unsigned ipol=0; ipol < npol; ipol++)
	for (unsigned ichan=0; ichan < nchan; ichan++)
	  Pulsar::Profile* profile = integration->get_Profile (ipol, ichan);

    }

  }
  catch (Error& error) {
    cerr << "Error while handling '" << filenames[ifile] << "'" << endl
	 << error.get_message() << endl;
  }

  return 0;

}

