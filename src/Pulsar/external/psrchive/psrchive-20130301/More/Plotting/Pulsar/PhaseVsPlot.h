//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsPlot.h,v $
   $Revision: 1.20 $
   $Date: 2009/06/11 05:05:33 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseVsPlot_h
#define __Pulsar_PhaseVsPlot_h

#include "Pulsar/PhasePlot.h"
#include "ColourMap.h"

namespace Pulsar {

  class Profile;

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsPlot : public PhasePlot {

  public:

    //! Default constructor
    PhaseVsPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsPlot class
    class Interface : public TextInterface::To<PhaseVsPlot> {
    public:
      Interface (PhaseVsPlot* = 0);
    };

    //! Derived classes must provide the number of rows
    virtual unsigned get_nrow (const Archive* data) = 0;

    //! Derived classes must provide the Profile for the specified row
    virtual const Profile* get_Profile (const Archive* data, unsigned row) = 0;

    //! Remove the baseline from each sub-integration and channel independently
    void preprocess (Archive*);

    //! Set the minimum and maximum values on the y-axis
    void prepare (const Archive* data);

    //! Draw in the current viewport
    void draw (const Archive*);

    //! Provide access to the z-scale
    PlotScale* get_z_scale () { return &z_scale; }

    //! Provide access to the colour map
    pgplot::ColourMap* get_colour_map () { return &colour_map; }

    //! Set the style
    void set_style (const std::string&);
    //! Get the style
    std::string get_style () const { return style; }
    
    //! Set the line colour
    void set_line_colour( int new_colour ) { line_colour = new_colour; }
    //! Get the line colour
    int get_line_colour( void ) { return line_colour; }
    
    //! Set the crop value
    void set_crop( float s_crop_value ) { crop_value = s_crop_value; }
    //! Get the crop value
    float get_crop() const { return crop_value; }
   
    //! Set the first and last row to plot
    void set_rows( const std::pair<unsigned,unsigned>& rows );
    //! Get the crop value
    std::pair<unsigned,unsigned> get_rows() const { return rows; }

  protected:

    PlotScale z_scale;

    std::pair<unsigned, unsigned> rows;

    pgplot::ColourMap colour_map;

    //! The style in which the data will be plotted
    std::string style;
    
    //! The colour to draw the lines
    int line_colour;
    
    //! The percentage of max to crop at
    float crop_value;

    //! The number of rows in the plot
    unsigned nrow;

    //! The minimum and (half interval) maximum row to be plotted
    unsigned min_row, max_row;

    //! Set when set_rows is accepted
    bool rows_set;

  };

}

#endif
