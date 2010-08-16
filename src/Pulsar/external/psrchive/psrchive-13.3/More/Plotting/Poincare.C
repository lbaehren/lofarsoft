/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Poincare.h"
#include "Pulsar/PolnProfile.h"

#include "Plot3D.h"
#include "pgutil.h"
#include "templates.h"

#include <cpgplot.h>

#include <algorithm>
#include <math.h>

using namespace std;

Pulsar::Poincare::Poincare ()
{
  longitude = 45;
  latitude = 25;

  longitude_step = 4;
  latitude_step = 1;

  nstep = 1;
}

//! Plot in the current viewport
void Pulsar::Poincare::plot (const Archive* data)
{
  Reference::To<const PolnProfile> profile = get_Stokes (data, isubint, ichan);

  vector<unsigned> bins;

  if (!label_bins.empty())
    TextInterface::parse_indeces (bins, label_bins, profile->get_nbin());

  unsigned i_min, i_max, nbin = profile->get_nbin();

  get_phase_scale()->get_indeces (nbin, i_min, i_max);

  float max = 0;

  for (unsigned ipol=1; ipol < 4; ipol++)
  {
    const float* amps = profile->get_Profile(ipol)->get_amps();

    float max_amp = *max_element (amps+i_min, amps+i_max);
    float min_amp = *min_element (amps+i_min, amps+i_max);

    float abs_min = fabs( min_amp );
    max_amp = std::max( abs_min, max_amp );
    max = std::max (max_amp, max);
  }

  max *= 1.1;

  float aspect_ratio = pgplot::get_viewport_aspect_ratio ();

  float xfactor = 1.0;
  float yfactor = 1.0;

  if (aspect_ratio > 1)
    yfactor = aspect_ratio;
  else
    xfactor = 1.0/aspect_ratio;
  
  cpgswin (-max*xfactor, max*xfactor,
	   -max*yfactor, max*yfactor);

  pgplot::Plot3D volume;

  Cartesian origin (0,0,0);
  Cartesian x0 (max,0,0);
  Cartesian y0 (0,max,0);
  Cartesian z0 (0,0,max);

  float textsep = 0.05;

  for (unsigned istep=0; istep < nstep; istep++)
  {
    volume.set_camera (longitude, latitude);
    
    // draw and label the axis
    cpgsci (2);
    volume.arrow (origin, x0);
    volume.text (x0*(1+textsep), "Q");
    
    cpgsci (3);
    volume.arrow (origin, y0);
    volume.text (y0*(1+textsep), "U");
    
    cpgsci (4);
    volume.arrow (origin, z0);
    volume.text (z0*(1+textsep), "V");
    
    cpgsci (1);
    for (unsigned ibin=i_min; ibin < i_max; ibin++)
    {
      Cartesian p ( profile->get_Profile(1)->get_amps()[ibin],
		    profile->get_Profile(2)->get_amps()[ibin],
		    profile->get_Profile(3)->get_amps()[ibin] );
      
      if (ibin == i_min)
	volume.move (p);
      else
	volume.draw (p);

      if (found (ibin, bins))
      {
	string text = tostring (ibin);
	volume.text (p, text.c_str(), 0.5);
	volume.move (p);
      }
    }

    get_frame()->decorate (data);

    if (nstep > 1)
    {
      cpgpage ();
      longitude += longitude_step;
      latitude += latitude_step;
    }
  }
}

