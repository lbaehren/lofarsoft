/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "strutil.h"
#include "dirutil.h"
#include "Error.h"

#include <iostream>
#include <unistd.h>

using namespace std;

void usage() {

  cout << 
    "A simple program to convert between pulsar archive formats \n"
    "Usage: psrconv (options) <filenames> \n"
    "Where the options are as follows: \n"
    "     -o fmt The output format [default PSRFITS] \n"
    "     -h     This help message \n"
    "     -q     Quiet mode \n"
    "     -v     Verbose mode \n"
    "     -V     Very verbose mode \n"
    "\n"
    "Available output formats:" << endl;

  Pulsar::Archive::agent_list ();

}

int main (int argc, char** argv)
{
  string output_format = "PSRFITS";
  string unload_cal_ext = ".cf";
  string unload_psr_ext = ".rf";

  char* metafile = 0;

  bool verbose = false;
  bool quiet = false;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "hM:o:qVv")) != -1)
  {
    switch (gotc)
    {
    case 'o':
      output_format = optarg;
      break;

    case 'h':
      usage();
      return 0;

    case 'M':
      metafile = optarg;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      verbose = true;
      break;

    case 'v':
      Pulsar::Archive::set_verbosity (2);
      verbose = true;
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      quiet = true;
      break;
    }
  }

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.size() == 0)
  {
    usage ();
    return 0;
  }
  
  Reference::To<Pulsar::Archive> input;
  Reference::To<Pulsar::Archive> output;

  // ensure that the selected output format can be written
  output = Pulsar::Archive::new_Archive (output_format);  
  if (!output->can_unload())
  {
    cerr << "psrconv: " << output_format << " unload method not implemented"
	 << endl;
    return -1;
  }

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try
  {
    if (!quiet)
      cerr << "Loading " << filenames[ifile] << endl;

    input = Pulsar::Archive::load(filenames[ifile]);
    
    output = Pulsar::Archive::new_Archive (output_format);
    output-> copy (*input);

    if (!quiet)
      cerr << "Conversion complete" << endl;
    
    if (verbose)
    {
      cerr << "Source: " << output -> get_source() << endl;
      cerr << "Frequency: " << output -> get_centre_frequency() << endl;
      cerr << "Bandwidth: " << output -> get_bandwidth() << endl;
      cerr << "# of subints: " << output -> get_nsubint() << endl;
      cerr << "# of polns: " << output -> get_npol() << endl;
      cerr << "# of channels: " << output -> get_nchan() << endl;
      cerr << "# of bins: " << output -> get_nbin() << endl;
    }

    string ext = unload_psr_ext;
    if (input->type_is_cal())
      ext = unload_cal_ext;

    string newname = replace_extension( input->get_filename(), ext );

    if (!quiet)
      cerr << "Unloading " << newname << endl;

    output->unload (newname);

  }
  catch (Error& error)
  {
    cerr << error << endl;
  }
}
