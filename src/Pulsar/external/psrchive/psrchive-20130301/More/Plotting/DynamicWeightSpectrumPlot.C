/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DynamicWeightSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>

Pulsar::DynamicWeightSpectrumPlot::DynamicWeightSpectrumPlot()
{
  zero_check = false;
}

Pulsar::DynamicWeightSpectrumPlot::~DynamicWeightSpectrumPlot()
{
}

TextInterface::Parser* Pulsar::DynamicWeightSpectrumPlot::get_interface ()
{
  return new Interface (this);
}

// Calculate SNR to fill in to plot array
void Pulsar::DynamicWeightSpectrumPlot::get_plot_array( const Archive *data, 
    float *array )
{

  int nsub = srange.second - srange.first + 1;
  int nchan = crange.second - crange.first + 1;
  int ii = 0;

  for( int ichan = crange.first; ichan <= crange.second; ichan++) 
  {
    for( int isub = srange.first; isub <= srange.second; isub++ )
    {
      Reference::To<const Profile> prof = 
        data->get_Profile ( isub, pol, ichan);
      array[ii] = prof->get_weight();
      ii++;
    }
  }

}

