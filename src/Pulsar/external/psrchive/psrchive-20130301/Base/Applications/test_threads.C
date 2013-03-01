/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "BatchQueue.h"
#include "Error.h"

#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void usage()
{
  cout << 
    "Test creation of archives in a multi-threaded program \n"
    "Usage: test_threads [options] <filename> \n"
    "Where the options are as follows: \n"
    "  -h           This help message \n"
    "  -t threads   Number of threads \n"
       << endl;
}

class Test : public Reference::Able
{
public:
  Reference::To<Pulsar::Archive> input;
  unsigned loops;

  void run (unsigned ithread)
  {
    char filename[64];
    sprintf (filename, "/tmp/%d.tmp", ithread);
    cerr << "test_threads: Test::run " << filename << endl;
    for (unsigned i=0; i<loops; i++)
    {
      Reference::To<Pulsar::Archive> copy = input->clone();
      copy->unload(filename);
    }
  }
};

int main (int argc, char **argv) try
{
  bool verbose = false;
  bool quiet = false;
  unsigned nthread = 1;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "ht:qVv")) != -1)
    switch (gotc)
    {
    case 't':
      nthread = atoi(optarg);
      break;

    case 'h':
      usage();
      return 0;

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

  if (optind >= argc)
  {
    usage ();
    return 0;
  }

  string filename = argv[optind];

  if (verbose)
    cerr << "Loading " << filename << endl;

  Test test;
  test.input = Pulsar::Archive::load (filename);
  test.loops = 1000;

  BatchQueue queue (nthread);

  for (unsigned i=0; i<nthread; i++)
    queue.submit( &test, &Test::run, i );

  queue.wait ();

  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}
