//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/TwoBitPlot.h,v $
   $Revision: 1.4 $
   $Date: 2007/10/02 05:08:15 $
   $Author: straten $ */

#ifndef __Pulsar_TwoBitPlot_h
#define __Pulsar_TwoBitPlot_h

#include "Pulsar/SimplePlot.h"
#include "JenetAnderson98Plot.h"

namespace Pulsar {

  //! Plots the measured distribution of the fraction of low voltage states
  class TwoBitPlot : public SimplePlot {

  public:

    //! Default constructor
    TwoBitPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the TwoBitPlot class
    class Interface : public TextInterface::To<TwoBitPlot> {
    public:
      Interface (TwoBitPlot* = 0);
    };

    //! Derived classes must draw in the current viewport
    void draw (const Archive*);

    //! Perform any preprocessing steps
    void prepare (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Get the default label for the x axis
    std::string get_xlabel (const Archive*);

  protected:

    JenetAnderson98::Plot plot;

  };

}

#endif
