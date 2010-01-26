/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransformBench.h"
#include "tostring.h"

#include "Management/psrchive_install.h"

using namespace std;

int main (int argc, char** argv)
{
  unsigned nfft = 1024;

  if (argc > 1)
    nfft = fromstring<unsigned> (argv[1]);

  FTransform::Bench bench;
  bench.set_path (PSRCHIVE_INSTALL + std::string("/share"));

  FTransform::Bench::Entry entry = bench.get_best ( nfft );

  cout << "Best library for nfft=" << nfft << " is " << entry.library << endl;
}
