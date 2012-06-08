//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FrequencyScale.h,v $
   $Revision: 1.7 $
   $Date: 2009/10/16 17:35:28 $
   $Author: straten $ */

#ifndef __Pulsar_FrequencyScale_h
#define __Pulsar_FrequencyScale_h

#include "Pulsar/PlotScale.h"

namespace Pulsar {

  class Archive;

  //! Represents an axis with radio frequency ordinate
  class FrequencyScale : public PlotScale {

  public:

    //! Default constructor
    FrequencyScale ();

    // Text interface to the FrequencyScale class
    class Interface : public TextInterface::To<FrequencyScale>
    {
    public:
      Interface (FrequencyScale* = 0);
    };

    TextInterface::Parser* get_interface () { return new Interface(this); }

    //! Initialize internal attributes according to Archive data
    void init (const Archive*);

    //! Return the min and max as the phase bin index
    void get_indeces (const Archive*, unsigned& min, unsigned& max) const;

    //! Get the ordinate values
    void get_ordinates (const Archive*, std::vector<float>& x_axis) const;

    //! Get a description of the units
    std::string get_label ();

    void set_reverse (bool flag=true) { reverse = flag; }
    bool get_reverse () const { return reverse; }

  protected:

    //! Reverse the order on the axis
    bool reverse;

  };

}

#endif
