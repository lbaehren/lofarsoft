/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/





#include "Pulsar/DigitiserStatsPlot.h"
#include <Pulsar/DigitiserStatistics.h>
#include <iostream>
#include <float.h>
#include <cpgplot.h>
#include <tostring.h>




using std::pair;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using Pulsar::DigitiserStatsPlot;





/**
 * Constructor
 *
 *  DOES     - Inititialization
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

DigitiserStatsPlot::DigitiserStatsPlot()
{
  subint = -1;
  srange.first = -1;
  srange.second = -1;
  valid_archive = false;
}



/**
 * AdjustSubRange
 *
 *  DOES     - Looks at subint,srange.first and srange.second to determine the correct subint range.
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void DigitiserStatsPlot::AdjustSubRange( void )
{
  // Make sure srange.first,srange.second have the subint range we want to display
  if( srange.first == -1 && srange.second == -1 )
  {
    if( subint == -1 )
    {
      srange.first = 0;
      srange.second = nsub -1;
    }
    else
    {
      srange.first = subint;
      srange.second = subint;
    }
  }

  if( srange.first < 0 )
    throw Error( InvalidParam,
                 "DigitiserStatsPlot::AdjustSubRange",
                 (string("bad srange.first value: ") + tostring<int>(srange.first)).c_str() );

  if( srange.second >= nsub )
    throw Error( InvalidParam,
                 "DigitiserStatsPlot::AdjustSubRange",
                 (string("bad srange.second value: ") + tostring<int>(srange.second)).c_str() );
}



/**
 * CheckStats
 *
 *  DOES     - Checks that the pointer points to a valid DigitiserStatistics extension
 *  RECEIVES - The extension to examine
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

bool DigitiserStatsPlot::CheckStats( Reference::To<DigitiserStatistics> ext )
{
  if( !ext )
  {
    if( verbose > 1 )
      cerr << "No digitiser Satistics extension" << endl;
    return false;
  }

  if( ext->rows.size() == 0 )
  {
    if( verbose > 1 )
      cerr << "Digitiser Satistics extension has zero rows" << endl;
    return false;
  }

  return true;
}




/**
 * prepare
 *
 *  DOES     - Store all the data into our multidimensional vector (data), determine the x and y ranges
 *  RECEIVES - The archive to examine
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - There are a few ways we could improve the performance of this plot, the first would be to
 *             place it in the new structure when loading it in the extension in the first place,
 *             have to check what other code makes use of DigitiserStatistics first. Or, we could
 *             cache the min and max values while loading from disk, this would mean 1 processing
 *             step before rendering instead of 2. We still have to loop through the data once
 *             to increase y values in order to separate data from different subintegrations.
 **/

void DigitiserStatsPlot::prepare( const Archive *const_arch )
{
  Reference::To<Archive> arch = const_cast<Archive*>( const_arch );

  Reference::To<DigitiserStatistics> ext = arch->get<DigitiserStatistics>();

  if( CheckStats( ext ) )
  {
    ncycsub = ext->get_ncycsub();
    ndigr = ext->get_ndigr();
    npar = ext->get_npar();
    nsub = ext->rows.size();

    AdjustSubRange();

    y_min = FLT_MAX;
    y_max = -FLT_MAX;

    // The data in the file isn't how we would like it. [p1][p2][p3] etc what we want
    // is all the p1 values sequentially, then all the p2 etc. So create an array of
    // profiles [digitiser channel][parameter] then we just pass it to cpgline
    // when we want to draw them. Grab the min and max while we're doing this.

    profiles.resize( ndigr );
    for( int g = 0; g < ndigr; g ++ )
    {
      profiles[g].resize( npar );
      for( int p = 0; p < npar; p ++ )
      {
        profiles[g][p].resize( ncycsub * nsub );
        for( int c = 0; c < ncycsub; c ++ )
        {
          for( int s = srange.first; s <= srange.second; s ++ )
          {
            float next_val = ext->rows[s].data[c*ndigr*npar + g*npar + p];
            if( next_val < y_min )
              y_min = next_val;
            if( next_val > y_max )
              y_max = next_val;
            profiles[g][p][s*ncycsub + c] = next_val;
          }
        }
      }
    }


    if( !(y_min == 0 && y_max == 0) )
    {
      valid_archive = true;

      // adjust the viewport to show all the data, don't overlap the subints, looks crap.

      get_frame()->get_y_scale()->set_minmax( y_min, y_max );
      get_frame()->get_x_scale()->set_minmax( 0, ndigr );
      get_frame()->get_y_scale()->set_buf_norm( .07 );

      get_frame()->hide_axes();
    }
    else
    {
      if( verbose > 1 )
        cerr << "Digitiser Stats values are all zero" << endl;
    }
  }
}



/**
 * draw
 *
 *  DOES     - Draws the vector data as, sets of npar lines, increasing in y with subint
 *             increasing in x with digitiser channel
 *  RECEIVES - The archive to render
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void DigitiserStatsPlot::draw( const Archive *const_arch )
{
  Reference::To<Archive> arch = const_cast<Archive*>( const_arch );
  Reference::To<DigitiserStatistics> ext = arch->get<DigitiserStatistics>();
  if( CheckStats( ext ) && valid_archive )
  {
    vector<float> xs( ncycsub*nsub );

    float x_step = 1.0 / (((ncycsub)*(nsub))-1);

    for( int g = 0; g < ndigr; g ++ )
    {
      // adjust the xs array for this digitiser channel
      float next_x = g;
      for( int i = 0; i < ncycsub*nsub; i ++ )
      {
        xs[i] = next_x;
        next_x += x_step;
      }

      // for each parameter, draw the profile
      for( int p = 0; p < npar; p ++ )
      {
        cpgsci( (p + 1)* 2 );
        cpgline( ncycsub*nsub, &xs[0], &profiles[g][p][0] );
      }
    }

    cpgsci( 1 );

    // get the viewport range including buffer
    float vp_y1, vp_y2;
    get_frame()->get_y_scale()->get_range_external( vp_y1, vp_y2 );

    // get the window range
    float w_x1, w_x2, w_y1, w_y2;
    cpgqwin( &w_x1, &w_x2, &w_y1, &w_y2 );

    // Save the current viewport, so that we can adjust relative to it and restore it later.
    float tx_min, tx_max, ty_min, ty_max;
    cpgqvp( 0, &tx_min, &tx_max, &ty_min, &ty_max );

    // For each digitiser channel
    //   set the viewport to cover that channel
    //   set the window to have x ranging from start time to end time
    //   draw a box around the viewport
    float duration = arch->integration_length();
    float nx = tx_min;
    float xw = (tx_max-tx_min)*(1.0/ndigr);
    for( int c = 0; c < ndigr; c ++ )
    {
      cpgsvp( nx, nx + xw, ty_min, ty_max );
      cpgswin( 0, duration, w_y1, w_y2 );
      if( nx == tx_min )
        cpgbox("bcnt", 0.0, 0, "bcnt", 20.0, 0);
      else
        cpgbox("bcnt", 0.0, 0, "bct", 20.0, 0 );
      string dig_label = string("Dig ") + tostring<int>(c);
      cpgmtxt( "T", -1, 0.01, 0, dig_label.c_str() );
      nx += xw;
    }

    // Restore the original viewport
    cpgsvp( tx_min, tx_max, ty_min, ty_max );
  }
}




/**
 * get_interface
 *
 *  DOES     - Creates a text interfaces to this object
 *  RECEIVES - Nothing
 *  RETURNS  - a DigitiserCountsPlot::Interface pointer
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

TextInterface::Parser *DigitiserStatsPlot::get_interface()
{
  return new Interface( this );
}



/**
 * get_xlabel
 *
 *  DOES     - Returns label for x axis
 *  RECEIVES - The archive
 *  RETURNS  - "subint"
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

std::string DigitiserStatsPlot::get_xlabel( const Archive * data )
{
  return "Time(sec)";
}




/**
 * get_ylabel
 *
 *  DOES     - Returns the label for y axis
 *  RECEIVES - The archive
 *  RETURNS  - "Digitiser Channel"
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

std::string DigitiserStatsPlot::get_ylabel( const Archive *data )
{
  return "Digitiser Counts";
}







