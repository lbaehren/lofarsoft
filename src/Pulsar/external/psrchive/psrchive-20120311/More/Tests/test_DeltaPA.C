/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Estimate.h"
#include "random.h"
#include "spectra.h"

#include <unistd.h>
#include <time.h>
#include <assert.h>

#include <vector>

using namespace std;

void usage ()
{
  cerr << 
    "test_delta_PA - tests the propagation of error in DeltaRM\n"
    "\n"
    "test_delta_PA [options] filename[s]\n"
    "options:\n"
    " -h               Help page \n"
    " -v               Verbose mode \n"
    " -n states        Number of measurements per trial \n"
    " -N trials        Number of trials \n"
    " -s s/n           Signal-to-noise ratio of each measurement \n"
    "\n"
       << endl;
}

double cyclic_diff (double a, double b, double scale)
{
  // rescale
  double diff = fmod(b-a, scale);
  double wrap = 0.5 * scale;

  if (diff > wrap)
    diff -= scale;
  if (diff < -wrap)
    diff += scale;

  return diff;
}

template<typename T>
T sqr(T x) { return x*x; }

double equal (const Estimate<double>& a, const Estimate<double>& b)
{
  return sqr(a.get_value()-b.get_value()) / (a.get_variance()+b.get_variance());
}

double
cyclic_equal (const Estimate<double>& a, const Estimate<double>& b, double s)
{
  return sqr(cyclic_diff (a.get_value(), b.get_value(), s))
    / (a.get_variance() + b.get_variance());
}


int main (int argc, char** argv)
{
  // number of trials
  unsigned trials = 1000;

  // number of measurements per trial
  unsigned measurements = 100;

  // noise added to template clones
  float signal_to_noise = 10.0;

  // verbosity flag
  bool verbose = false;

  char c;
  while ((c = getopt(argc, argv, "hvn:N:s:")) != -1) 

    switch (c)  {

    case 'h':
      usage();
      return 0;

    case 'v':
      verbose = true;
      break;

    case 'n':
      measurements = atoi(optarg);
      break;

    case 'N':
      trials = atoi(optarg);
      break;

    case 's':
      signal_to_noise = atof(optarg);
      break;

    } 

  static long idum = -time(0);

  double old_total_PA_diff = 0.0;
  unsigned old_PA_errors = 0;

  double new_total_PA_diff = 0.0;
  unsigned new_PA_errors = 0;

  for (unsigned itrial=0; itrial < trials; itrial++) {

    double delta_PA;
    random_value (delta_PA, M_PI);

    double cos_delta_PA = cos(delta_PA);
    double sin_delta_PA = sin(delta_PA);

    MeanRadian<double> mean_delta_PA;
    
    vector<double> Q0 (measurements);
    vector<double> U0 (measurements);

    vector<double> Q1 (measurements);
    vector<double> U1 (measurements);

    double C_delta_PA = 0;
    double S_delta_PA = 0;

    for (unsigned istate=0; istate < measurements; istate++) {

      double PA;
      random_value (PA, M_PI);

      Q0[istate] = signal_to_noise * cos(PA);
      U0[istate] = signal_to_noise * sin(PA);
      
      Q1[istate] = cos_delta_PA * Q0[istate] - sin_delta_PA * U0[istate];
      U1[istate] = sin_delta_PA * Q0[istate] + cos_delta_PA * U0[istate];

      // bring the noise
      Q0[istate] += gasdev(&idum);
      U0[istate] += gasdev(&idum);
      Q1[istate] += gasdev(&idum);
      U1[istate] += gasdev(&idum);

      // create the noisy estimates of delta_PA
      Estimate<double> q0 (Q0[istate], 1.0);
      Estimate<double> u0 (U0[istate], 1.0);
      Estimate<double> PA0 = atan2 (u0,q0);

      //cerr << "q0=" << q0 << " u0=" << u0 << endl;
      assert (PA0.var > 0);

      Estimate<double> q1 (Q1[istate], 1.0);
      Estimate<double> u1 (U1[istate], 1.0);
      Estimate<double> PA1 = atan2 (u1,q1);

      //cerr << "q1=" << q1 << " u1=" << u1 << endl;
      assert (PA1.var > 0);

      // cerr << PA0 << " " << PA1 << " " << PA0 - PA1 << endl;
      Estimate<double> dPA = PA1 - PA0;
      assert (dPA.var > 0);

      dPA.set_value( cyclic_diff(PA0.get_value(), PA1.get_value(), M_PI*2) );

      mean_delta_PA += dPA;

      C_delta_PA += Q0[istate]*Q1[istate] + U0[istate]*U1[istate];
      S_delta_PA += Q0[istate]*U1[istate] - Q1[istate]*U0[istate];

    }
 
    double one = C_delta_PA*C_delta_PA + S_delta_PA*S_delta_PA;
    C_delta_PA /= one;
    S_delta_PA /= one;

    double var_delta_PA = 0.0;

    for (unsigned istate=0; istate < measurements; istate++) {

      double del = 0;

      // del delta_PA del Q0
      del = C_delta_PA * U1[istate] - S_delta_PA * Q1[istate];
      var_delta_PA += del * del;

      // del delta_PA del U0
      del = -C_delta_PA * Q1[istate] - S_delta_PA * U1[istate];
      var_delta_PA += del * del;

      // del delta_PA del Q1
      del = -C_delta_PA * U0[istate] - S_delta_PA * Q0[istate];
      var_delta_PA += del * del;

      // del delta_PA del U1
      del = C_delta_PA * Q0[istate] - S_delta_PA * U0[istate];
      var_delta_PA += del * del;

    }

    Estimate<double> radians( atan2(S_delta_PA,C_delta_PA), var_delta_PA );

    double diff = 0;

    Estimate<double> old_delta_PA = mean_delta_PA.get_Estimate();
    assert(old_delta_PA.var > 0);

    // test the estimated PA shift
    diff = cyclic_equal (old_delta_PA, delta_PA, M_PI*2);
    assert(diff > 0);

    old_total_PA_diff += diff;

    if (diff > 1.0) {
      if (verbose)
	cerr << "unequal old PA: " << sqrt(diff) << " "
	     << delta_PA << " " << mean_delta_PA.get_Estimate() << endl;
      old_PA_errors ++;
    }

    // test the estimated PA shift
    diff = cyclic_equal (radians, delta_PA, M_PI*2);
    assert(diff > 0);

    new_total_PA_diff += diff;

    if (diff > 1.0) {
      if (verbose)
	cerr << "unequal new PA: " << sqrt(diff) << " "
	     << delta_PA << " " << radians << endl;
      new_PA_errors ++;
    }

  }

  cerr << 
    "Average diff/sigma: (% error)\n"
    "  old PA: " << sqrt(old_total_PA_diff / trials) << 
    " (" << 100.0 * old_PA_errors / trials << ")\n"
    "  new PA: " << sqrt(new_total_PA_diff / trials) << 
    " (" << 100.0 * new_PA_errors / trials << ")\n"
       << endl;

  return 0;

}
