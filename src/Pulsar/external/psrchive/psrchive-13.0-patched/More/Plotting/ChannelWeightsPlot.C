



#include "Pulsar/ChannelWeightsPlot.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>
#include <Pulsar/Integration.h>
#include <float.h>
#include <iostream>
#include <cpgplot.h>


using namespace Pulsar;
using namespace std;




ChannelWeightsPlot::ChannelWeightsPlot()
{
  
}



ChannelWeightsPlot::~ChannelWeightsPlot()
{
}



TextInterface::Parser *ChannelWeightsPlot::get_interface()
{
  return new ChannelWeightsPlot::Interface( this );
}



void ChannelWeightsPlot::prepare( const Archive *data )
{
  Reference::To<Archive> copy = data->clone();

  int num_chan = copy->get_nchan();

  weights.resize( num_chan );

  Pulsar::Profile* profile = 0;

  weights_minmax.first = FLT_MAX;
  weights_minmax.second = -FLT_MAX;
  
  for (unsigned i = 0; i < copy->get_nchan(); i++)
  {
    profile = copy->get_Integration(0)->get_Profile(0, i);
    weights[i] = profile->get_weight();

    if( weights[i] < weights_minmax.first )
      weights_minmax.first = weights[i];
    if( weights[i] > weights_minmax.second )
      weights_minmax.second = weights[i];
  }
  
  float min_y, max_y;
  
  if( weights_minmax.first == weights_minmax.second )
  {
    min_y = weights_minmax.first - 0.01;
    max_y = weights_minmax.second + 0.01;
  }
  else
  {
    float range = weights_minmax.second - weights_minmax.first;
    min_y = weights_minmax.first - range * 0.1;
    max_y = weights_minmax.second + range * 0.1;
  }
  
  get_frame()->get_y_scale()->set_minmax( min_y, max_y );
  get_frame()->get_y_axis()->set_label( "Weight" );
}



void ChannelWeightsPlot::draw( const Archive *data )
{
  int num_chan = data->get_nchan();

  float xs[num_chan];
  float ys[num_chan];

  float freq = data->get_centre_frequency();
  float bw = data->get_bandwidth();
  float next_x = freq - bw / 2;
  float step_x = bw / num_chan;
  
  for( int i = 0; i < num_chan; i ++ )
  {
    xs[i] = next_x;
    ys[i] = weights[i];
    next_x += step_x;
  }

  float old_ch;
  cpgqch( &old_ch );
  int old_ci;
  cpgqci( &old_ci );
  
  cpgsci(6);
  cpgsch(0.5);
  for( int i = 0; i < num_chan; i ++ )
    cpgpt1(xs[i], ys[i], 4);
  
  cpgsci( old_ci );
  cpgsch( old_ch );
   
//   int old_lw;
//   cpgqlw( &old_lw );
//   cpgslw( 15 );
//   cpgpt( num_chan, xs, ys, -1 ); 
//   cpgslw( old_lw );
}


