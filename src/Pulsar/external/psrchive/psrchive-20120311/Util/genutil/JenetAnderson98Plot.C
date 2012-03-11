#include "JenetAnderson98Plot.h"
#include "templates.h"

#include <cpgplot.h>

#include <iostream>
#include <algorithm>

using namespace std;

JenetAnderson98::Plot::Plot ()
{
  show_min = 0.01;

  show_cutoff_sigma = true;
  plot_only_cut = false;
  plot_entire_range = false;
}

string JenetAnderson98::Plot::get_xlabel () const
{
  return "Fraction in Low State, \\gF";
}

string JenetAnderson98::Plot::get_ylabel () const
{
  return "Probability, P(\\gF)";
}

void JenetAnderson98::Plot::calculate_theory ()
{
  unsigned nsample = theory.get_measured_prob_Phi()->get_nsample();

  if (nsample < 4)
    throw Error (InvalidState, "JenetAnderson98::Plot::calculate_theory",
		 "invalid nsample=%u", nsample);

  float threshold = theory.get_measured_prob_Phi()->get_threshold();

  if (theory_dist.size() == nsample && theory.get_threshold() == threshold)
    return;

  theory.set_threshold( threshold );
  theory.get_prob_Phi( nsample, theory_dist );
}

template<class Iterator, class T>
unsigned first_greater (Iterator start, Iterator end, T val)
{
  for (Iterator i = start; i != end; i++)
    if (*i > val)
      return i - start;

  return end - start;
}

void JenetAnderson98::Plot::adjust_limits (const vector<float>& hist)
{
  ymax = max( ymax, *max_element( hist.begin(), hist.end() ) );

  if (plot_entire_range)
    return;

  float cut = ymax * show_min;

  imin = min ( imin, first_greater (hist.begin(), hist.end(), cut) );
  imax = min ( imax, first_greater (hist.rbegin(), hist.rend(), cut) );
}

void JenetAnderson98::Plot::init_limits ()
{
  imin = theory.get_measured_prob_Phi()->get_nsample();
  imax = imin;
  ymax = 0;
}

//! Set the interface to the data
void JenetAnderson98::Plot::set_interface (const Probability* data)
{ 
  if (!data)
    return;

  theory.set_measured_prob_Phi (data);

  init_limits ();
  calculate_theory ();
  adjust_limits (theory_dist);

  unsigned ndig = theory.get_measured_prob_Phi()->get_ndig();

  vector<float> hist;

  for (unsigned idig=0; idig < ndig; idig++) {
    theory.get_measured_prob_Phi()->get_histogram (hist, idig);
    normalize (hist);
    adjust_limits (hist);
  }

  unsigned nsamp = theory.get_measured_prob_Phi()->get_nsample();

  // imax current counts the number of samples in from the end
  imax = nsamp - imax;

  ymax *= 1.05;
}

float JenetAnderson98::Plot::get_xmin() const
{
  return float(imin) / theory.get_measured_prob_Phi()->get_nsample();
}

float JenetAnderson98::Plot::get_xmax() const
{
  return float(imax) / theory.get_measured_prob_Phi()->get_nsample();
}

float JenetAnderson98::Plot::get_ymin() const
{
  return ymax * show_min;
}

float JenetAnderson98::Plot::get_ymax() const
{
  return ymax;
}

void cpgpt (std::vector<float>& vals, int type)
{
  for (unsigned i=0; i<vals.size(); i++) {
    float ind = i;
    float val = vals[i];
    cpgpt (1, &ind, &val, type);
  }
}

void cpgline (std::vector<float>& y)
{
  std::vector<float> x (y.size());
  for (unsigned i=0; i<y.size(); i++)
    x[i] = i;
  cpgline (y.size(), &x[0], &y[0]);
}

void JenetAnderson98::Plot::plot ()
{
  cpgswin (imin, imax, get_ymin(), get_ymax());

  unsigned ndig = theory.get_measured_prob_Phi()->get_ndig();
  vector<float> hist;

  for (unsigned idig=0; idig < ndig; idig++) {

    theory.get_measured_prob_Phi()->get_histogram (hist, idig);
    normalize (hist);

    cpgsci (idig + 2);
    cpgpt (hist, 2);

#if 0    
    float fractone = data->get_histogram_mean (idig[idig]);
    cpgsls (4);
    cpgmove (fractone*float(nsample), 0.0);
    cpgdraw (fractone*float(nsample), midheight);
#endif

  }

  cpgsci (1);
  cpgline (theory_dist);


#if 0
  float hp_min, hp_max;
  unsigned n_min = theory.get_measured_prob_Phi()->get_nmin ();
  unsigned n_max = theory.get_measured_prob_Phi()->get_nmax ();

  if (plot_only_cut)  {
    hp_min = n_min - 10;
    hp_max = n_max + 10;
  }

  // adjust to keep theoretical values in plot
  if (hp_min < imin)
    imin = (unsigned) hp_min;
  if (hp_max > imax)
    imax = (unsigned) hp_max;


  // draw the cut-off sigma lines
  if (show_cutoff_sigma) {
    cpgmove (n_min, 0.0);
    cpgdraw (n_min, theory_max * nweights);
    cpgmove (n_max, 0.0);
    cpgdraw (n_max, theory_max * nweights);
  }
#endif

}
