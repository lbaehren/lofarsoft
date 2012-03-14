/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DynamicBaselineSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>
#include <Pulsar/PhaseWeight.h>

Pulsar::DynamicBaselineSpectrumPlot::DynamicBaselineSpectrumPlot()
{
  use_variance = false;
  reuse_baseline = false;
  base = NULL;
}

Pulsar::DynamicBaselineSpectrumPlot::~DynamicBaselineSpectrumPlot()
{
}

TextInterface::Parser* Pulsar::DynamicBaselineSpectrumPlot::get_interface ()
{
  return new Interface (this);
}

// Calculate values to fill in to plot array
void Pulsar::DynamicBaselineSpectrumPlot::get_plot_array( const Archive *data, 
    float *array )
{

  // Make a copy to dedisperse, need to do this for the 
  // PhaseWeight stuff to work correctly.
  Reference::To<Archive> data_copy;
  if (data->get_dedispersed()) 
    data_copy = const_cast<Archive *>(data);
  else {
    data_copy = data->clone();
    data_copy->dedisperse();
  }

  // Only recalc baseline if needed 
  if (base==NULL || !reuse_baseline) { 
    base = data_copy->total()->get_Profile(0,0,0)->baseline(); 
  }

  int nsub = srange.second - srange.first + 1;
  int nchan = crange.second - crange.first + 1;
  int ii = 0;

  for( int ichan = crange.first; ichan <= crange.second; ichan++) 
  {
    for( int isub = srange.first; isub <= srange.second; isub++ )
    {
      Reference::To<const Profile> prof = 
        data_copy->get_Profile(isub, pol, ichan);

      base->set_Profile(prof);
      float value = 0.0;

      if (prof->get_weight()!=0.0) {
        if (use_variance) {
          value = base->get_variance().get_value();
          value = sqrt(value);
        } else {
          value = base->get_mean().get_value();
        }
      }

      array[ii] = value;
      ii++;
    }
  }

}

