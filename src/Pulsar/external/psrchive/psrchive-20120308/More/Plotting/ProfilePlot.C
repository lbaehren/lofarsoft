/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ProfilePlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include <cpgplot.h>

Pulsar::ProfilePlot::ProfilePlot ()
{
  plot_cal_transitions = true;
}

TextInterface::Parser* Pulsar::ProfilePlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::ProfilePlot::get_profiles (const Archive* data)
{
  plotter.profiles.push_back( get_Profile (data, isubint, ipol, ichan) );
}

//! Derived classes must draw in the current viewport
void Pulsar::ProfilePlot::draw (const Archive* data)
{
  FluxPlot::draw (data);

  if (plot_cal_transitions && data->get_type() != Signal::Pulsar)
    draw_transitions (plotter.profiles[0]);
}

/*!  Plots the calibrator hi/lo levels using the transitions
 determined by Profile::find_transitions.  Plots in the currently open
 pgplot device using the current viewport and window.  */
void Pulsar::ProfilePlot::draw_transitions (const Profile* profile)
{
  int hightolow, lowtohigh, buffer;
  profile->find_transitions (hightolow, lowtohigh, buffer);

  double mean_hi, var_hi;
  profile->stats (&mean_hi, &var_hi, 0,
                  lowtohigh + buffer,
                  hightolow - buffer);

  double mean_lo, var_lo;
  profile->stats (&mean_lo, &var_lo, 0,
                  hightolow + buffer,
                  lowtohigh - buffer);

  int st_low = 0;
  if (lowtohigh < hightolow)
    st_low = 1;

  double sigma_hi = sqrt(var_hi);
  double sigma_lo = sqrt(var_lo);

  float hip[3] = { mean_hi, mean_hi-sigma_hi, mean_hi+sigma_hi };
  float lop[3] = { mean_lo, mean_lo-sigma_lo, mean_lo+sigma_lo };

  int colour=0, line=0;

  cpgqci(&colour);
  cpgqls(&line);

  float nbin = profile->get_nbin();

  float xp[2];
  xp[st_low] = float(hightolow)/nbin;
  xp[!st_low] = float(lowtohigh)/nbin;

  int cp[2];
  cp[st_low] = 3;
  cp[!st_low] = 2;

  float space = float(buffer)/nbin;


  // Added by DS, add xoff(set) to repeat if we are zooming outside 0 to 1

  float sx, ex;
  get_frame()->get_x_scale()->get_range( sx, ex );

  for( int xoff = int(sx)-1; xoff < int(ex)+1; xoff ++ )
  {
    cpgsls(1);
    for (int i=0; i<3; i++)
    {
      float yp[2];
      yp[st_low] = hip[i];
      yp[!st_low] = lop[i];

      cpgsci (cp[0]);
      cpgmove (xp[1]+space-1.0 + xoff,yp[0]);
      cpgdraw (xp[0]-space + xoff,yp[0]);

      cpgsci (cp[1]);
      cpgmove (xp[0]+space + xoff,yp[1]);
      cpgdraw (xp[1]-space + xoff,yp[1]);

      cpgsci (cp[0]);
      cpgmove (xp[1]+space + xoff,yp[0]);
      cpgdraw (xp[0]-space+1.0 + xoff,yp[0]);

      // draw the error bars dotted
      cpgsls(2);
    }
  }

  // restore the colour and line attributes
  cpgsls(line);
  cpgsci(colour);
}


