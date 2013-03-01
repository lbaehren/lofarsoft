/***************************************************************************
*
*   Copyright (C) 2007 by David Smith
*   Licensed under the Academic Free License version 2.1
*
***************************************************************************/



#include "Pulsar/LinePhasePlot.h"
#include "Pulsar/Profile.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Integration.h>
#include <float.h>
#include <cpgplot.h>



using std::pair;
using std::cerr;
using std::cout;
using std::endl;
using Pulsar::Archive;
using Pulsar::Profile;
using Pulsar::LinePhasePlot;




/**
 * @brief Constructor
 *        Assume the subint, or range of subints are unset until we know how many there are
 *        Assume channel and polarization are zero, leave it up to the user to scrunch the
 *        data in frequency and polarization.
 **/

LinePhasePlot::LinePhasePlot()
{
  ichan = 0;                          // assume channel of 0
  ipol = 0;                           // assume polarization of 0
  srange = pair<int,int>( -1, -1 );   // leave subint range unset, use 0 to nbin-1 later.
  y_buffer_norm = 0.07;               // For this plot we use 7% buffer on y axis.
  line_colour = 7;                    // default line colour (yellow)
}




/**
 * @brief   Figures out some measurements before plotting
 *          We want to setup the y scale with world coordinates to fit all
 *          the subints on the plot. We also want to setup the external world coordinates
 *          that the y axis will be labelled with. Being a PhasePlot, x axis takes care
 *          of itself.
 * @param   data The archive to prepare, hopefully the one that will be draw(n)
 **/

void LinePhasePlot::prepare (const Archive *data )
{
  // if we didn't have a subint range set, use the full range

  if( srange.first == -1 )
  {
    srange.first = 0;
    srange.second = data->get_nsubint() - 1;
  }

  // determine the minimum and maximum amplitudes from
  // all the integrations we are looking at

  float min_amp = FLT_MAX;
  float max_amp = -FLT_MAX;
  for( int s = srange.first; s <= srange.second; s ++ )
  {
    Reference::To<const Profile> next_profile = data->get_Integration(s)->get_Profile( ipol, ichan );
    float profile_min = next_profile->min();
    float profile_max = next_profile->max();

    if( profile_min < min_amp ) min_amp = profile_min;
    if( profile_max > max_amp ) max_amp = profile_max;
  }
  amp_range = max_amp - min_amp;

  // set our y range to be from the min to the max times the number of subints

  get_frame()->get_y_scale()->set_minmax( min_amp + srange.first * amp_range,
                                          max_amp + srange.second * amp_range );

  // give the y axis its buffer

  get_frame()->get_y_scale()->set_buf_norm( y_buffer_norm );

  // set the external range, we have to account for the buffer space
  // TODO see if we want to change the way world_external works to account
  //      for the buffer space automatically

  pair<float,float> ext_range( srange.first + (min_amp / amp_range),
                               srange.second + (max_amp / amp_range) );
  float width = ext_range.second - ext_range.first;
  ext_range.first -= y_buffer_norm * width;
  ext_range.second += y_buffer_norm * width;
  get_frame()->get_y_scale()->set_world_external( ext_range );

  // Setup the y axis to draw integral tick marks, have no markings on opposite axis
  // and be labelled "Sub Integration".

  get_frame()->get_y_axis()->set_alternate( false );
  get_frame()->get_y_axis()->set_tick( 1.0 );
  get_frame()->get_y_axis()->set_label( "Sub Integration" );
}



/**
 * @brief   Draws a stack of profiles, one for each subint
 * @param   data The archive to draw, hopefully the archive that was prepare(d) :)
 **/

void LinePhasePlot::draw( const Archive *data )
{
  int nbin = data->get_nbin();

  // premake the array of x coords

  vector<float> xs( nbin );
  for( int i = 0; i < nbin; i ++ )
    xs[i] = i / float(nbin);

  // Set the line colour to use

  cpgsci( line_colour );

  // for each integration
  //   get the profile given the polarization and channel
  //   increase the amplitudes by the offset
  //   draw the profile as a line

  for( int s = srange.first; s <= srange.second; s ++ )
  {
    vector<float> profile_data = data->get_Integration(s)->get_Profile( ipol, ichan )->get_weighted_amps();

    // TODO see if you want to use transform here instead of the loop, measure for efficiency
    float amp_offset = s * amp_range;
    for( int i = 0; i < nbin; i ++ )
      profile_data[i] += amp_offset;

    cpgline( nbin, &xs[0], &profile_data[0] );
  }
}



/**
 * @brief   Returns a text ineterface that can be used with this object
 * @return  a LinePhasePlot::Interface class pointing at this
 **/

TextInterface::Parser *LinePhasePlot::get_interface()
{
  return new Interface( this );
}




