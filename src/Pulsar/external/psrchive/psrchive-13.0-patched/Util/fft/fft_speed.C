/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransformAgent.h"
#include "CommandLine.h"
#include "BatchQueue.h"
#include "RealTimer.h"
#include "malloc16.h"

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>

using namespace FTransform;
using namespace std;

class Speed : public Reference::Able
{
public:

  Speed ();

  // parse command line options
  void parseOptions (int argc, char** argv);

  // run the test
  void runTest ();

  // list the available FFT libraries
  void listLibraries();

protected:

  string library;
  unsigned nthread;
  unsigned nloop;
  unsigned nfft;
  bool real_to_complex;
  unsigned streaming_cache;

  class Thread;
};

class Speed::Thread : public Reference::Able
{
 protected:

  friend class Speed;

  unsigned nloop;
  unsigned nfft;
  bool real_to_complex;
  unsigned streaming_cache;

  FTransform::Plan* plan;

  void run ();

  double time;
};


Speed::Speed ()
{
  nloop = 0;
  nfft = 4;
  nthread = 1;
  real_to_complex = false;
  streaming_cache = 0;
}

int main(int argc, char** argv) try
{
  Speed speed;
  speed.parseOptions (argc, argv);
  speed.runTest ();
  return 0;
}
 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }

void Speed::parseOptions (int argc, char** argv)
{
  CommandLine::Menu menu;
  CommandLine::Argument* arg;

  menu.set_help_header ("fft_speed - measure FFT speed");
  menu.set_version ("fft_speed version 1.0");

  arg = menu.add (this, &Speed::listLibraries, 'L');
  arg->set_help ("list available FFT libraries");

  arg = menu.add (library, 'l', "lib");
  arg->set_help ("set FFT library name");

  arg = menu.add (real_to_complex, 'r');
  arg->set_help ("real-to-complex FFT");

  arg = menu.add (nfft, 'n', "nfft");
  arg->set_help ("FFT length");

  arg = menu.add (library, 'i', "niter");
  arg->set_help ("number of iterations");

  arg = menu.add (nthread, 't', "nthread");
  arg->set_help ("number of threads");

  arg = menu.add (streaming_cache, 'c', "bytes");
  arg->set_help ("streaming cache size in bytes");

  menu.parse (argc, argv);
}

void Speed::listLibraries ()
{
  unsigned nlib = FTransform::get_num_libraries ();

  for (unsigned ilib=0; ilib < nlib; ilib++)
    cout << FTransform::get_library_name (ilib) << endl;

  exit (0);
}

double order (unsigned nfft)
{
  return nfft * log2 (nfft);
}

void Speed::runTest ()
{
  if (!library.empty())
    FTransform::set_library (library);

  FTransform::nthread = nthread;
  FTransform::simd = true;

  FTransform::Plan* plan;
  if (real_to_complex)
    plan = Agent::current->get_plan (nfft, FTransform::frc);
  else
    plan = Agent::current->get_plan (nfft, FTransform::fcc);

  BatchQueue queue (nthread);
  vector<Thread*> thread (nthread);

  if (!nloop)
    nloop = unsigned (40000 * order(8192)/order(nfft));

  cerr << "nloop=" << nloop << " library=" << get_library() << endl;

  for (unsigned ithread=0; ithread < nthread; ithread++)
  {
    thread[ithread] = new Thread;
    thread[ithread]->nloop = nloop;
    thread[ithread]->nfft = nfft;
    thread[ithread]->real_to_complex = real_to_complex;
    thread[ithread]->streaming_cache = streaming_cache;

    thread[ithread]->plan = plan;

    queue.submit (thread[ithread], &Thread::run);
  }

  queue.wait ();

  delete plan;

  double total_time = 0.0;
  for (unsigned ithread=0; ithread < nthread; ithread++)
    total_time += thread[ithread]->time;

  double time_us = total_time * 1e6 / nthread;
  double log2_nfft = log2(nfft);

  double mflops = 5.0 * nfft * log2_nfft / time_us;

  cerr << "nfft=" << nfft << " time=" << time_us << "us"
    " log2(nfft)=" << log2_nfft << " mflops=" << mflops << endl;

  cout << nfft << " " << time_us << " " << log2_nfft << " " << mflops << endl;
}

void Speed::Thread::run ()
{
  unsigned nfloat = nfft;
  if (!real_to_complex)
    nfloat *= 2;

  unsigned size = sizeof(float) * nfloat;

  unsigned npart = 1;
  unsigned in_size = size;

  if (streaming_cache && size < streaming_cache)
  {
    in_size = streaming_cache;
    npart = streaming_cache / size;
    cerr << "Speed::Thread::run cache=" << streaming_cache
	 << " parts=" << npart << endl;
  }

  float* in = (float*) malloc16 (in_size);
  memset (in, 0, in_size);
  float* out = (float*) malloc16 (size + 2*sizeof(float));

  RealTimer timer;
  timer.start ();

  for (unsigned i=0; i<nloop; i++)
  {
    unsigned ipart = i % npart;
    float* inptr = in + ipart * nfloat;
    if (real_to_complex)
      plan->frc1d (nfft, out, inptr);
    else
      plan->fcc1d (nfft, out, inptr);
  }

  timer.stop ();

  time = timer.get_elapsed()/nloop;

  // cerr << "time=" << time << endl;

  free16 (in);
  free16 (out);
}
