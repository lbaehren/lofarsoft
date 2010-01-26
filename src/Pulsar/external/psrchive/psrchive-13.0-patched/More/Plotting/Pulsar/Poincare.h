//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Poincare.h,v $
   $Revision: 1.9 $
   $Date: 2009/10/16 17:35:47 $
   $Author: straten $ */

#ifndef __Pulsar_Poincare_h
#define __Pulsar_Poincare_h

#include "Pulsar/FramedPlot.h"
#include "Pulsar/Index.h"
#include "Pulsar/PlotScale.h"

namespace Pulsar {

  //! Plots the Stokes vector in three-dimensional Poincare space
  class Poincare : public FramedPlot {

  public:

    //! Default constructor
    Poincare ();

    //! Plot in the current viewport
    void plot (const Archive*);

    // Text interface to the Poincare class
    class Interface : public TextInterface::To<Poincare> {
    public:
      Interface (Poincare* = 0);
    };

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ()
    { return new Interface (this); }

    //! Set the longitude of the camera
    void set_longitude (float f) { longitude = f; }
    //! Get the longitude of the camera
    float get_longitude () const { return longitude; }

    //! Set the latitude of the camera
    void set_latitude (float f) { latitude = f; }
    //! Get the latitude of the camera
    float get_latitude () const { return latitude; }

    //! Set the sub-integration to plot (where applicable)
    void set_subint (Index _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (Index _ichan) { ichan = _ichan; }
    Index get_chan () const { return ichan; }

    //! Provide access to the phase scale
    PlotScale* get_phase_scale () { return &phase_scale; }

    //! Set the animate of the camera
    void set_nstep (unsigned steps) { nstep = steps; }
    //! Get the animate of the camera
    unsigned get_nstep () const { return nstep; }

    //! Set the longitude of the camera
    void set_longitude_step (float f) { longitude_step = f; }
    //! Get the longitude of the camera
    float get_longitude_step () const { return longitude_step; }

    //! Set the latitude of the camera
    void set_latitude_step (float f) { latitude_step = f; }
    //! Get the latitude of the camera
    float get_latitude_step () const { return latitude_step; }

    //! Set the animate of the camera
    void set_label_bins (const std::string& bins) { label_bins = bins; }
    //! Get the animate of the camera
    std::string get_label_bins () const { return label_bins; }

  protected:

    Index isubint;
    Index ichan;
    PlotScale phase_scale;

    mutable float longitude;
    mutable float latitude;

    unsigned nstep;
    float longitude_step;
    float latitude_step;

    std::string label_bins;
  };

}

#endif
