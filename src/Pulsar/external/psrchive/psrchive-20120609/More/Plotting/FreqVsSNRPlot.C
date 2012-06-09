/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FreqVsSNRPlot.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include <cpgplot.h>
#include <algorithm>

using namespace Pulsar;
using namespace std;

FreqVsSNRPlot::FreqVsSNRPlot()
{
  subint = 0;
  pol = 0;

  get_frame()->get_y_scale()->set_buf_norm( 0.05 );
  get_frame()->get_y_axis()->set_label( "Signal-to-Noise Ratio" );
}

void FreqVsSNRPlot::prepare( const Archive *data )
{
  unsigned nchan = data -> get_nchan();
  if (!nchan)
    return;

  snrs.resize (nchan);

  for (unsigned i = 0; i < nchan; i++)
  {
    const Profile *next_profile = data->get_Profile( subint, pol, i );
    if( next_profile )
    {
      snrs[i] = next_profile->snr();
    }
    else
      snrs[i] = 0;
  }
  
  float snr_min = *std::min_element (snrs.begin(), snrs.end());
  float snr_max = *std::max_element (snrs.begin(), snrs.end());
  
  get_frame()->get_y_scale()->set_minmax( snr_min, snr_max );
}



void FreqVsSNRPlot::draw( const Archive *data )
{
  float y_min, y_max;
  get_frame()->get_y_scale(true)->get_range_external (y_min, y_max);
  cpgswin (0, data->get_nchan(), y_min, y_max);

  for (unsigned k = 0; k < data->get_nchan(); k++)
  {
    cpgsci(3);
    cpgpt1(k, snrs[k], 0);
  }
}

TextInterface::Parser *FreqVsSNRPlot::get_interface( void )
{
  return new Interface( this );
}


