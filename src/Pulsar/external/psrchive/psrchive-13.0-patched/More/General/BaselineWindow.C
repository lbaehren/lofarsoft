/***************************************************************************
 *
 *   Copyright (C) 2005-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BaselineWindow.h"

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/SmoothMean.h"
#include "Pulsar/SmoothMedian.h"

#include "Pulsar/Config.h"
#include "Pulsar/Profile.h"

using namespace std;

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Default constructor
Pulsar::BaselineWindow::BaselineWindow ()
{
  bin_start = bin_end = 0;

  range_specified = false;

  find_max = false;
  find_mean = false;

  mean = 0;

  /* The default smoothing algorithm is not set in the constructor
     because the Smooth constructor uses a Pulsar::Config attribute
     that may not be initialized at the time of construction of this
     instance.
  */
}

Pulsar::BaselineWindow* Pulsar::BaselineWindow::clone () const
{
  return new BaselineWindow (*this);
}

//! Set the smoothing function
void Pulsar::BaselineWindow::set_smooth (Smooth* function)
{
  smooth = function;
}

static Pulsar::Option<std::string> default_smooth
(
 "BaselineWindow::smooth", "mean",

 "Baseline window smoothing function",

 "The BaselineWindow algorithm defines the off-pulse baseline by the\n"
 "the minimum phase of a smoothed version of the profile.  The smoothing \n"
 "function can be either 'mean' or 'median'."
);

//! Get the smoothing function
Pulsar::Smooth* Pulsar::BaselineWindow::get_smooth ()
{
  if (!smooth)
  {
    string smooth_name = default_smooth;

    if (smooth_name == "median")
      smooth = new Pulsar::SmoothMedian;

    else if (smooth_name == "mean")
      smooth = new Pulsar::SmoothMean;

    else
      throw Error (InvalidState, 
		   "Pulsar::BaslineWindow::get_smooth",
		   "no smoothing function named '" + smooth_name + "'");
  }

  return smooth;
}

//! Retrieve the PhaseWeight
void Pulsar::BaselineWindow::calculate (PhaseWeight* weight)
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::BaselineWindow::calculate",
		 "Profile not set");

  unsigned nbin = profile->get_nbin();

  float centre = find_phase (nbin, profile->get_amps());
  float bins = get_smooth()->get_last_bins();

  unsigned ibin1 = nbin + unsigned (nbin * centre - 0.5 * bins);
  unsigned ibin2 = nbin + unsigned (nbin * centre + 0.5 * bins);

#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::calculate centre=" << centre
       << " bins=" << bins << " ibin1=" << ibin1 << " ibin2=" << ibin2 << endl;
#endif

  weight->resize( nbin );
  weight->set_all( 0.0 );

  for (unsigned ibin=ibin1; ibin<ibin2; ibin++)
    (*weight)[ibin%nbin] = 1.0;
}

//! Set to find the minimum mean
void Pulsar::BaselineWindow::set_find_minimum ()
{
  find_max = false;
}
  
//! Set to find the maximum mean
void Pulsar::BaselineWindow::set_find_maximum ()
{
  find_max = true;
}

void Pulsar::BaselineWindow::set_find_mean (float _mean)
{
  find_mean = true;
  mean = _mean;
}

//! Set the start and end bins of the search
void Pulsar::BaselineWindow::set_range (int start, int end)
{
  bin_start = start;
  bin_end = end;
  range_specified = true;
}

float Pulsar::BaselineWindow::find_phase (const std::vector<float>& amps)
{
  return find_phase (amps.size(), &amps[0]);
}

//! Return the phase at which minimum or maximum mean is found
float Pulsar::BaselineWindow::find_phase (unsigned nbin, const float* amps)
try {

  Profile temp (nbin);
  temp.set_amps (amps);

#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::find_phase" << endl;
#endif

  get_smooth()->transform( &temp );

  unsigned start = 0;
  unsigned stop = nbin;
  
  if (range_specified)
  {
    nbinify (bin_start, bin_end, nbin);
    start = bin_start;
    stop = bin_end;
  }
  
#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::find_phase init"
    " start=" << start << " stop=" << stop << endl;
#endif

  bool first = true;
  float found_val = 0;
  unsigned found_bin = 0;

  for (unsigned ibin=start; ibin < stop; ibin++)
  {
    float value = temp.get_amps()[ibin%nbin];

    if ( find_mean )
    {
      double diff = fabs( value - mean );
      if ( first || diff < found_val )
      {
	found_val = diff;
	found_bin = ibin;
      }
      continue;
    }


    if ( first ||
	 (find_max && value>found_val) || (!find_max && value<found_val) )
    {
      found_val = value;
      found_bin = ibin;
    }

    first = false;

  }

  float phase = float(found_bin%nbin) / float(nbin);

#ifdef _DEBUG
  cerr << "phase=" << phase << endl;
#endif

  return phase;
}
catch (Error& error)
{
  throw error += "Pulsar::BaselineWindow::find_phase";
}

class Pulsar::BaselineWindow::Interface 
  : public TextInterface::To<BaselineWindow>
{
public:
  Interface (BaselineWindow* instance)
  {
    if (instance)
      set_instance (instance);

    // set the smoothing function someday
  }

  std::string get_interface_name () const { return "minimum"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::BaselineWindow::get_interface ()
{
  return new Interface (this);
}
