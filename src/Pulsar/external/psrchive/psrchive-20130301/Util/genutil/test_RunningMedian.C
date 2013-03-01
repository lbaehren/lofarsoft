/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "RunningMedian.h"
#include "BoxMuller.h"
#include "RealTimer.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>

using namespace std;

//! Benchmark the running median algorithm
void benchmark (int argc, char** argv);

int main (int argc, char** argv) 
{
  if (argc > 1)
    benchmark (argc-1, argv+1);

  BoxMuller noise (time(NULL));

  unsigned nth_min = 2;
  unsigned nth_max = 100;

  unsigned nsamp = 10000;
  unsigned errors = 0;

  for (unsigned nth=nth_min; nth < nth_max; nth++)
  {
    vector<double> samples (nsamp);
    generate (samples.begin(), samples.end(), noise);

    // for the old method
    vector<double> window (2*nth+1);

    // new method
    RunningMedian<double> rmedian (nth);
    for (unsigned ipt=0; ipt < window.size(); ipt++)
      rmedian.insert( samples[ipt] );
    
    for (unsigned isamp = 0; isamp < nsamp-window.size(); isamp++)
    {
      // new method
      double new_median = rmedian.get_median();
      rmedian.erase( samples[isamp] );
      rmedian.insert( samples[isamp+window.size()] );

      // the old brute force method
      for (unsigned ipt=0; ipt < window.size(); ipt++)
	window[ipt] = samples[ipt+isamp];

      std::nth_element (window.begin(), window.begin()+nth, window.end());
      double old_median = window[nth];

      if (new_median != old_median)
      {
	cerr << "nth=" << nth << " isamp=" << isamp 
	     << " new=" << new_median << " old=" << old_median << endl;
	errors ++;
      }
    }
  }

  if (errors)
    return -1;

  cerr << "RunningMedian<double> passes all tests" << endl;
  return 0;
}

void benchmark (int argc, char** argv) 
{
  BoxMuller noise (time(NULL));

  unsigned nth = fromstring<unsigned> (argv[0]);
  unsigned nsamp = 100000;

  if (argc > 1)
    nsamp = fromstring<unsigned> (argv[1]);

  cerr << "RunningMedian benchmark nth=" << nth << " nsamp=" << nsamp << endl;

  vector<double> samples (nsamp);
  generate (samples.begin(), samples.end(), noise);

  double median = 0;

  //
  // time the old method
  //

  cerr << "Running the old method" << endl;

  RealTimer timer;

  // for the old method
  vector<double> window (2*nth+1);
    
  for (unsigned isamp = 0; isamp < nsamp-window.size(); isamp++)
  {
    // the old brute force method
    for (unsigned ipt=0; ipt < window.size(); ipt++)
      window[ipt] = samples[ipt+isamp];

    std::nth_element (window.begin(), window.begin()+nth, window.end());
    median = window[nth];
  }

  timer.stop ();

  double old_method = timer.get_elapsed();

  //
  // time the new method
  //

  cerr << "Running the new method" << endl;

  timer.start ();

  // new method
  RunningMedian<double> rmedian (nth);
  for (unsigned ipt=0; ipt < window.size(); ipt++)
    rmedian.insert( samples[ipt] );

  for (unsigned isamp = 0; isamp < nsamp-window.size(); isamp++)
  {
    median = rmedian.get_median();

    rmedian.erase( samples[isamp] );
    rmedian.insert( samples[isamp+window.size()] );
  }

  timer.stop ();

  double new_method = timer.get_elapsed();

  cerr << "RunningMedian benchmark runtime old/new=" << old_method/new_method 
       << endl;
}
