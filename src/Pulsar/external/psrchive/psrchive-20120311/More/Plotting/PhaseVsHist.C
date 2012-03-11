/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include "Pulsar/PhaseVsHist.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PolnProfile.h"

#if HAVE_GSL
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_errno.h>
#endif

Pulsar::PhaseVsHist::PhaseVsHist ()
{
  ichan = 0;
  histarray = NULL;
  weight_scheme = "none";

#if HAVE_GSL
  hist_kernel = "mises";
  gsl_set_error_handler_off();
#else
  hist_kernel = "delta";
#endif
}

TextInterface::Parser* Pulsar::PhaseVsHist::get_interface ()
{
  return new Interface (this);
}

unsigned Pulsar::PhaseVsHist::get_nrow (const Archive* data)
{
  return 256; // XXX temp!
}

void Pulsar::PhaseVsHist::prepare (const Archive* data)
{
  PhaseVsPlot::prepare (data);

  // Not sure if these classes get re-used, delete old results just in case
  if (histarray!=NULL) delete [] histarray;

  // Array to hold histogram
  unsigned nbin = data->get_nbin();
  unsigned nrow = get_nrow(data);
  histarray = new double[nbin*nrow];
  for (unsigned i=0; i<nbin*nrow; i++) histarray[i] = 0.0;

  // Find requested bin range
  bool cyclic = true;
  unsigned min_bin, max_bin;
  get_frame()->get_x_scale()->get_indeces (nbin, min_bin, max_bin, cyclic);

  // XXX temp for PA
  set_yrange(-90.0, 90.0);

  // Pre-calculate sin/cos table for making PA hist
  double *spa_row, *cpa_row;
  spa_row = new double [nrow];
  cpa_row = new double [nrow];
  for (unsigned irow=0; irow<nrow; irow++) {
      double pa_row = M_PI * ((double)irow / (double)nrow - 0.5);
      spa_row[irow] = sin(2.0*pa_row);
      cpa_row[irow] = cos(2.0*pa_row);
  }

  // Fill in histogram array
  // TODO make this call a virtual func to allow different kinds of
  // histograms.
  // TODO Also move all the actual histogram calculation out into
  // its own class once it seems to be working well.
  for (unsigned isub=0; isub<data->get_nsubint(); isub++) {

    // Skip zero-weighted profiles
    if (data->get_Integration(isub)->get_weight(ichan.get_value())==0.0) 
      continue;

    Reference::To<const PolnProfile> stokesprof = get_Stokes(data,isub,ichan);
    std::vector< std::complex< Estimate<double> > > lin;
    stokesprof->get_linear(lin,0.0);

    for (unsigned ibin=min_bin; ibin<max_bin; ibin++) {

      // Pull out Q, U
      double q = lin[ibin].real().get_value();
      double u = lin[ibin].imag().get_value();

      // Combine Q, U variance into a single value somehow.. 
      double var = sqrt( 
          lin[ibin].real().get_variance() * lin[ibin].imag().get_variance());
      if (var==0.0) continue; 

      double k = (q*q + u*u) / var;
      double pa = 0.5*atan2(u,q);

      // Alternate weighting methods
      double wt = 1.0;
      if (weight_scheme == "flux") 
        wt = sqrt(q*q+u*u);
      else if (weight_scheme == "snr")
        wt = sqrt(k);

      // Delta-fn kernel
      if (hist_kernel == "delta") {
        unsigned idx = unsigned( (pa+M_PI/2.0)*(double)nrow/M_PI );
        histarray[idx*nbin + ibin] += wt;
      }

#if HAVE_GSL

      // Von Mises kernel
      else if (hist_kernel == "mises") {

        double spa = sin(2.0*pa);
        double cpa = cos(2.0*pa);
        double norm=2.0*M_PI*gsl_sf_bessel_I0_scaled(k);
        for (unsigned irow=0; irow<nrow; irow++) {

          //double pa_row = M_PI * ((double)irow / (double)nrow - 0.5);
          //histarray[irow*nbin + ibin] += 
          //  wt*exp(k*(cos(2.0*(pa_row-pa))-1.0))/norm;
          
          // Use precalculated sin/cos to speed this up..
          histarray[irow*nbin + ibin] += 
            wt*exp(k*(cpa_row[irow]*cpa + spa_row[irow]*spa - 1.0))/norm;

        }

      }

#endif

      else
        throw Error (InvalidParam, "Pulsar::PhaseVsHist::prepare", 
                     "'"+ hist_kernel +"' is not a valid histogram kernel");

    }
  }

  delete [] spa_row;
  delete [] cpa_row;
}

std::string Pulsar::PhaseVsHist::get_ylabel (const Archive* data)
{
  return "Position Angle (deg)";
}

const Pulsar::Profile*
Pulsar::PhaseVsHist::get_Profile (const Archive* data, unsigned row)
{
  // This is a hack to return a row of the histogram as a Profile
  // object.  Could be cleaned up if needed.
  unsigned nbin = data->get_nbin();
  Profile *p = new Profile(nbin);
  p->set_weight(1.0);
  p->set_amps(&histarray[row*nbin]);
  return p;
}
