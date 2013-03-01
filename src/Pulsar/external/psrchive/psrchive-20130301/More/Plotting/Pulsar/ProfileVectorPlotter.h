//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/ProfileVectorPlotter.h,v $
   $Revision: 1.6 $
   $Date: 2010/07/01 04:59:23 $
   $Author: jonathan_khoo $ */

#ifndef __Pulsar_ProfileVectorPlotter_h
#define __Pulsar_ProfileVectorPlotter_h

#include "Reference.h"

namespace Pulsar {

  class Profile;
  class PlotFrame;

  //! Plots flux profiles
  class ProfileVectorPlotter : public Reference::Able {

  public:

    //! Default constructor
    ProfileVectorPlotter ();

    //! set the y_scale min and max based on the x_scale min and max
    void minmax (PlotFrame* frame) const;

    //! draw all profiles
    void draw ( float sx, float ex );

    //! draw the profile in the current viewport and window
    void draw (const Profile*, float sx, float ex );

    //! the vector of profiles to be drawn
    std::vector< Reference::To<const Profile> > profiles;

    //! the shared ordinates
    std::vector<float> ordinates;

    //! the PGPLOT colour for each profile
    std::vector< int > plot_sci;

    //! the PGPLOT line style for each profile
    std::vector< int > plot_sls;

    //! the PGPLOT line width for each profile
    std::vector< int > plot_slw;

    //! Plot histogram style
    bool plot_histogram;

    //! Transpose the x and y axes
    bool transpose;
  };

}

#endif
