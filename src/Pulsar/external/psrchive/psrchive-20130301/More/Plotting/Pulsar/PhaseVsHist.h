//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_PhaseVsHist_h
#define __Pulsar_PhaseVsHist_h

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/Index.h"

namespace Pulsar {

  //! Plots 2-D histograms of something vs pulse phase
  class PhaseVsHist : public PhaseVsPlot {

  public:

    //! Default constructor
    PhaseVsHist ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsTime class
    class Interface : public TextInterface::To<PhaseVsHist> {
    public:
      Interface (PhaseVsHist* = 0);
    };

    //! Set the minimum and maximum values on the y-axis
    void prepare (const Archive* data);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the number of rows
    unsigned get_nrow (const Archive* data);

    //! Derived classes must provide the Profile for the specified row
    const Profile* get_Profile (const Archive* data, unsigned row);

    //! Set the frequency channel to plot
    void set_chan (const Index& _ichan) { ichan = _ichan; }
    Index get_chan () const { return ichan; }

    void set_weight(const std::string _weight) { weight_scheme = _weight; }
    std::string get_weight() const { return weight_scheme; }
    
    void set_kernel(const std::string _kernel) { hist_kernel = _kernel; }
    std::string get_kernel() const { return hist_kernel; }
    
  protected:

    Index ichan;
    double *histarray;
    std::string weight_scheme;
    std::string hist_kernel;

  };

}

#endif
