//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/AnglePlot.h,v $
   $Revision: 1.19 $
   $Date: 2009/06/12 09:10:31 $
   $Author: straten $ */

#ifndef __Pulsar_AnglePlot_h
#define __Pulsar_AnglePlot_h

#include "Pulsar/PhasePlot.h"
#include "Pulsar/Index.h"

#include "Functor.h"
#include "Estimate.h"

namespace Pulsar {

  //! Plots an angle and its error as a function of pulse phase
  class AnglePlot : public PhasePlot {

  public:

    enum Marker { ErrorTick=0x01, ErrorBar=0x02, Dot=0x04 };

    //! Default constructor
    AnglePlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the AnglePlot class
    class Interface : public TextInterface::To<AnglePlot> {
    public:
      Interface (AnglePlot* = 0);
    };

    //! Derived types compute the angles to be plotted
    virtual void get_angles (const Archive*) = 0;

    //! return the minimum and maximum value in degrees
    void prepare (const Archive*);

    //! draw the angle as a function of pulse phase
    void draw (const Archive*);

    std::string get_flux_label (const Archive* data);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const Index& _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (const Index& _ichan) { ichan = _ichan; }
    Index get_chan () const { return ichan; }

    //! the mark to be used
    void set_marker (Marker m) { marker = m; }
    Marker get_marker () const { return marker; }

    //! will draw only those points with linear > threshold * sigma
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

    //! span of value in degrees
    void set_span (float t) { span = t; }
    float get_span () const { return span; }

    Functor< float(float) > model;

  protected:

    //! Marker used to plot each point
    Marker marker;

    //! Noise threshold
    float threshold;

    //! Span of values in degrees
    float span;
    
    //! The angles
    std::vector< Estimate<double> > angles;

    Index ichan;
    Index isubint;
  };

  std::ostream& operator << (std::ostream& os, AnglePlot::Marker);
  std::istream& operator >> (std::istream& is, AnglePlot::Marker&);

}

#endif
