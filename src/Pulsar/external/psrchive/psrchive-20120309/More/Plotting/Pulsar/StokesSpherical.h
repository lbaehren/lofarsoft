//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/StokesSpherical.h,v $
   $Revision: 1.12 $
   $Date: 2009/09/15 14:03:42 $
   $Author: straten $ */

#ifndef __Pulsar_StokesSpherical_h
#define __Pulsar_StokesSpherical_h

#include "Pulsar/MultiPhase.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/PosAngPlot.h"
#include "Pulsar/EllAngPlot.h"

namespace Pulsar {

  //! Plots a single pulse profile
  class StokesSpherical : public MultiPhase {

  public:

    //! Default constructor
    StokesSpherical ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the StokesSpherical class
    class Interface : public TextInterface::To<StokesSpherical> {
    public:
      Interface (StokesSpherical* = 0);
    };

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const Index& _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (const Index& _ichan) { ichan = _ichan; }
    Index get_chan () const { return ichan; }

    //! Get the StokesPlot
    StokesPlot* get_flux () { return &flux; }

    //! Get the PosAngPlot
    PosAngPlot* get_orientation () { return &orientation; }

    //! Get the EllAngPlot
    EllAngPlot* get_ellipticity () { return &ellipticity; }

  protected:

    StokesPlot flux;
    PosAngPlot orientation;
    EllAngPlot ellipticity;
    Index isubint;
    Index ichan;

    template<class T> void prepare (T* plot, const Archive* data = 0)
    {
      plot->set_subint (isubint);
      plot->set_chan (ichan);
      if (data)
	plot->prepare (data);
    }

  };

}

#endif
