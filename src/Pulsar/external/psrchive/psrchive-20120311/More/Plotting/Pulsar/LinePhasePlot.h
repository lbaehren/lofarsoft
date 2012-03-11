//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/LinePhasePlot.h,v $
   $Revision: 1.6 $
   $Date: 2008/03/10 23:12:43 $
   $Author: nopeer $ */



#ifndef LINE_PHASE_PLOT_H_
#define LINE_PHASE_PLOT_H_



#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>
#include "Pulsar/PhasePlot.h"



using std::pair;
using std::vector;
using Pulsar::Archive;
using Pulsar::Profile;
using Pulsar::PhasePlot;



namespace Pulsar
{

  //! Draw a set of profiles for subints stacked ontop of eachother
  class LinePhasePlot : public PhasePlot
  {
  public:
    LinePhasePlot();

    //! prepare the archive for rendering
    void prepare (const Archive *data );

    //! draw the archive
    void draw( const Archive *data );

    // Text interface for LinePhasePlot
  class Interface : public TextInterface::To<LinePhasePlot>
    {
    public:
      Interface( LinePhasePlot *s_instance = NULL );
    };

    //! Get a LinePhasePlot::Interface for this object
    TextInterface::Parser *get_interface();

    //! Get the first index of the subint range
    int get_isub( void ) const { return srange.first; }

    //! Set the subint range to (x,x)
    void set_isub( int s_isub ) { srange.first = srange.second = s_isub; }

    //! Get the range of subints to plot
    pair<int,int> get_srange() const { return srange; }

    //! Set the range of subints to plot
    void set_srange( const pair<int,int> &s_srange ) { srange = s_srange; }

    //! Get the polarization to plot
    int get_ipol( void ) const { return ipol; }

    //! Set the polarization to plot
    void set_ipol( int s_ipol ) { ipol = s_ipol; }

    //! Get the frequency channel to plot
    int get_ichan() const { return ichan; }

    //! Set the frequency channel to plot
    void set_ichan( const int &s_ichan ) { ichan = s_ichan; }

    //! Get the line colour
    int get_line_colour() const { return line_colour; }

    //! Set the line colour
    void set_line_colour( const int &s_line_colour ) { line_colour = s_line_colour; }

  private:
    // polarization to plot
    int ipol;

    // channel to plot
    int ichan;

    // range of subints to plot
    pair<int,int> srange;

    // The maximum different maxamp - minamp of all relevant subints
    float amp_range;

    // how much space(%) to add above and below the plot
    float y_buffer_norm;

    // The line colour
    int line_colour;
  };
}


#endif

