//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsTime.h,v $
   $Revision: 1.11 $
   $Date: 2008/11/21 19:04:51 $
   $Author: demorest $ */

#ifndef __Pulsar_PhaseVsTime_h
#define __Pulsar_PhaseVsTime_h

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/Index.h"

namespace Pulsar {

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsTime : public PhaseVsPlot {

  public:

    //! Default constructor
    PhaseVsTime ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsTime class
    class Interface : public TextInterface::To<PhaseVsTime> {
    public:
      Interface (PhaseVsTime* = 0);
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
    
    //! Set the polarization to plot
    void set_pol (const Index& _ipol) { ipol = _ipol; }
    Index get_pol () const { return ipol; }

    //! Set use hour angle flag
    void set_use_ha(bool flag=true) { use_hour_angle = flag; }
    bool get_use_ha() const { return use_hour_angle; }

  protected:

    std::string time_string;
    Index ichan;
    Index ipol;
    bool use_hour_angle;

  };

}

#endif
