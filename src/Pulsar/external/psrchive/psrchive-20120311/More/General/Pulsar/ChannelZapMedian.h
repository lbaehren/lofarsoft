//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ChannelZapMedian.h,v $
   $Revision: 1.9 $
   $Date: 2011/01/19 10:13:49 $
   $Author: straten $ */

#ifndef _Pulsar_ChannelZapMedian_H
#define _Pulsar_ChannelZapMedian_H

#include "Pulsar/ChannelWeight.h"
#include "TextInterface.h"

namespace Pulsar {
  
  class Statistics;

  //! Uses a median smoothed spectrum to find birdies and zap them
  class ChannelZapMedian : public ChannelWeight {
    
  public:
    
    //! Default constructor
    ChannelZapMedian ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the ChannelZapMedian class
    class Interface : public TextInterface::To<ChannelZapMedian> {
    public:
      Interface (ChannelZapMedian* = 0);
    };

    //! Set up attributes that apply to the whole archive
    void operator () (Archive*);

    //! Set integration weights
    void weight (Integration* integration);

    //! Set the size of the window over which median will be computed
    void set_window_size (unsigned size) { window_size = size; }

    //! Get the size of the window over which median will be computed
    unsigned get_window_size () const { return window_size; }

    //! Set the cut-off threshold
    void set_cutoff_threshold (float t) { cutoff_threshold = t; }

    //! Get the cut-off threshold
    float get_cutoff_threshold () const { return cutoff_threshold; }

    //! Run the algorithm on the spectra of each bin
    void set_bybin (bool t) { bybin = t; }

    //! Run the algorithm on the spectra of each bin
    bool get_bybin () const { return bybin; }

    //! Print equivalent paz command on cout
    void set_paz_report (bool t) { paz_report = t; }

    //! Print equivalent paz command on cout
    bool get_paz_report () const { return paz_report; }

    //! Compute a single zap mask from the total
    void set_from_total (bool t) { from_total = t; }

    //! Compute a single zap mask from the total
    bool get_from_total () const { return from_total; }

    //! Set the statistical expression
    void set_expression (const std::string& exp) { expression = exp; }

    //! Get the statistical expression
    std::string get_expression () const { return expression; }

  protected:

    //! The size of the window over which median will be computed
    unsigned window_size;

    //! The cut-off threshold
    float cutoff_threshold;

    //! Median smooth the spectra of each bin
    bool bybin;

    //! Print the verbose message used by paz
    bool paz_report;

    //! Compute the zap mask from the tscrunched total
    bool from_total;

    //! The single mask to apply to every sub-integration
    std::vector<bool> single_mask;

    //! Set the expression to evaluate in each channel (as in psrstat)
    std::string expression;

    //! The Statistics estimator used to evaluate the expression
    Reference::To<Statistics> stats;
    Reference::To<TextInterface::Parser> parser;
  };
  
}

#endif
