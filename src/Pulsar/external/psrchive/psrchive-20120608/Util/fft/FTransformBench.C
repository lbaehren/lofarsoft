/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransformBench.h"
#include "debug.h"

#include <fstream>
#include <math.h>

using namespace std;

bool FTransform::Bench::verbose = false;

FTransform::Bench::Bench ()
{
  nthread = FTransform::nthread;
  path = ".";
  loaded = false;
  max_nfft = 0;
}

//! Set the patch from which benchmarks will be loaded
void FTransform::Bench::set_path (const std::string& _path)
{
  path = _path;
  reset ();
}

//! Set the number of threads for which benchmarks should be loaded
void FTransform::Bench::set_nthread (unsigned _nthread)
{
  nthread = _nthread;
  reset ();
}

void FTransform::Bench::reset ()
{
  entries.resize (0);
  max_nfft = 0;
  loaded = false;
}

//! Get the maximum FFT length measured
unsigned FTransform::Bench::get_max_nfft () const
{
  if (!loaded)
    load ();

  return max_nfft;
}

void FTransform::Bench::load () const
{
  unsigned use_nthread = 1;
  while (use_nthread < nthread)
    use_nthread *= 2;

  max_nfft = 0;

  unsigned nlib = FTransform::get_num_libraries ();

  for (unsigned ilib=0; ilib < nlib; ilib++) try
  {
    string library = FTransform::get_library_name (ilib);
    string nt = tostring(use_nthread);

    string filename = path + "/fft_bench_" + library + "_" + nt + ".dat";

    if (verbose)
      cerr << "FTransform::Bench::load filename=" << filename << endl;

    load (library, filename);
  }
  catch (Error& error)
  {
    if (verbose)
      cerr << "FTransform::Bench::load " << error.get_message() << endl;
  }

  loaded = true;
}

void FTransform::Bench::load (const std::string& library,
			      const std::string& filename) const
{
  ifstream in (filename.c_str());
  if (!in)
    throw Error (FailedSys, "FTransform::Bench::load",
                 "std::ifstream (" + filename + ")");

  while (!in.eof())
  {
    Entry entry;
    double log2nfft, mflops;

    in >> entry.nfft >> entry.cost >> log2nfft >> mflops;

    if (in.eof())
      continue;

    entry.library = library;
    
    DEBUG(library << " " << entry.nfft << " " << entry.cost);

    entries.push_back (entry);

    if (entry.nfft > max_nfft)
      max_nfft = entry.nfft;
  }
}

double theoretical (unsigned nfft)
{
  return nfft * log2 (nfft);
}

//! Get the best FFT cost for the transform length
FTransform::Bench::Entry FTransform::Bench::get_best (unsigned nfft) const
{
  if (!loaded)
    load ();

  unsigned use_nfft = nfft;

  if (nfft > max_nfft)
    use_nfft = max_nfft;

  Entry entry;

  for (unsigned i=0; i<entries.size(); i++)
    if ( entries[i].nfft == use_nfft &&
	 (entry.nfft == 0 || entries[i].cost < entry.cost) )
      entry = entries[i];

  if (entry.nfft == 0)
  {
    entry.library = "theoretical";
    entry.nfft = nfft;
    entry.cost = theoretical (nfft);
  }
  else if (nfft > max_nfft)
  {
    entry.library += "+theory";
    entry.cost *= theoretical (nfft) / theoretical (max_nfft);
  }

  if (verbose)
    cerr << "FTransform::Bench::get_best lib=" << entry.library
	 << " nfft=" << entry.nfft << " cost=" << entry.cost << endl;

  return entry;
}

