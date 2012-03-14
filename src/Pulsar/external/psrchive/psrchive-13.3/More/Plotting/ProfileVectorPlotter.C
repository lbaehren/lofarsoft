/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ProfileVectorPlotter.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Profile.h"

#include <cpgplot.h>
#include <vector>

using namespace std;


double mod( double dividend, double quotient )
{
  return dividend - double(int(dividend/quotient));
}




Pulsar::ProfileVectorPlotter::ProfileVectorPlotter ()
{
  plot_histogram = false;
}

void Pulsar::ProfileVectorPlotter::minmax (PlotFrame* frame) const
{
  if (!profiles.size())
    throw Error (InvalidState, "Pulsar::ProfileVectorPlotter::minmax",
		 "no Profiles");

  unsigned nbin = profiles[0]->get_nbin();

  unsigned i_min, i_max;

  bool cyclic = true;
  frame->get_x_scale()->get_indeces (nbin, i_min, i_max, cyclic);

  float min = profiles[0]->min(i_min, i_max);
  float max = profiles[0]->max(i_min, i_max);

  for (unsigned iprof=1; iprof < profiles.size(); iprof++) {
    min = std::min( min, profiles[iprof]->min(i_min, i_max) );
    max = std::max( max, profiles[iprof]->max(i_min, i_max) );
  }

  frame->get_y_scale()->set_minmax (min, max);
}

void Pulsar::ProfileVectorPlotter::draw ( float sx, float ex )
{
  for (unsigned iprof=0; iprof < profiles.size(); iprof++) {

    if (plot_sci.size() == profiles.size())
      cpgsci (plot_sci[iprof]);
    else
      cpgsci (iprof+1);

    if (plot_sls.size() == profiles.size())
      cpgsls (plot_sls[iprof]);
    else
      cpgsls (iprof+1);

    draw (profiles[iprof], sx, ex );
  }
}

//! draw the profile in the current viewport and window
void Pulsar::ProfileVectorPlotter::draw( const Profile* profile, 
					 float start_x, float end_x )
{
  const float* amps =  profile->get_amps();
  unsigned nbin = profile->get_nbin();

  if (ordinates.size() != nbin) {
    ordinates.resize (nbin);
    for (unsigned i=0; i<nbin; i++)
      ordinates[i] = (double(i)+0.5) / double(nbin);
  }

  if (start_x > end_x)
    throw Error (InvalidParam, "Pulsar::ProfileVectorPlotter::draw",
		 "start_x=%f greater than end_x=%f", start_x, end_x);

  unsigned total_pts = unsigned( ( end_x - start_x ) * nbin );
  
#ifdef _DEBUG
  cerr << "start_x=" << start_x << " end_x=" << end_x 
       << " total pts=" << total_pts << endl;
#endif

  float xs[total_pts];
  float ys[total_pts];

  // calculate the span of the ordinates
  double span = (ordinates[nbin-1]-ordinates[0]) * double(nbin)/double(nbin-1);
  
  // calculate the offset applied to the ordinates in the first region
  double xoff = floor(start_x) * span;

#ifdef _DEBUG
  cerr << "span=" << span << " xoff=" << xoff << endl;
#endif

  // set 0 <= start_x < 1
  start_x -= floor (start_x);

  unsigned index = unsigned( start_x * nbin );

  // cyclically fill the temporary data arrays
  for (unsigned i = 0; i < total_pts; i ++ )
  {
    ys[i] = amps[index];
    xs[i] = ordinates[index] + xoff;

#ifdef _DEBUG
    cerr << "index=" << index
	 << " x[" << i << "]=" << xs[i] 
	 << " y[" << i << "]=" << ys[i] << endl;
#endif

    index ++;

    if (index == nbin) {
      index = 0;
      xoff += span;
    }
  }

  if (plot_histogram)
    cpgbin (total_pts, xs, ys, true);
  else
    cpgline (total_pts, xs, ys );
}
