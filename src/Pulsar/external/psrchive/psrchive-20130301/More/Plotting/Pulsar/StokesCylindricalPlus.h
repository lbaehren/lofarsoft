/***************************************************************************
 *
 *   Copyright (C) 2010 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_StokesCylindricalPlus_h
#define __Pulsar_StokesCylindricalPlus_h

#include "Pulsar/MultiPhase.h"
#include "Pulsar/PosAngPlot.h"
#include "Pulsar/StokesPlot.h"

namespace Pulsar {

  class PolnProfile;

  //! Plots a single pulse profile
  class StokesCylindricalPlus : public MultiPhase {

  public:
    //! Default constructor
    StokesCylindricalPlus (float crop = 0.0);

    //! Destructor
    ~StokesCylindricalPlus ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    TextInterface::Parser* get_interface_again ();

    // Text interface to the StokesCylindricalPlus class
    class Interface : public TextInterface::To<StokesCylindricalPlus> {
    public:
      Interface (StokesCylindricalPlus* = 0);
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

    //! Get the StokesPlot (lower, cropped)
    StokesPlot* get_flux_cropped () { return &flux_cropped; }

    //! Get the PosAngPlot
    PosAngPlot* get_orientation () { return &orientation; }

    //! Set the y-axis zoom value
    void set_flux_crop (const float _crop);
    float get_flux_crop () const { return crop; }

  protected:
    StokesPlot flux;
    StokesPlot flux_cropped;
    PosAngPlot orientation;
    Index isubint;
    Index ichan;
    float crop;

    template<class T> void prepare (T* plot)
    {
      plot->set_subint (isubint);
      plot->set_chan (ichan);
    }
  };
}

#endif
