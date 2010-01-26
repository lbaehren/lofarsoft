//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Statistics.h,v $
   $Revision: 1.7 $
   $Date: 2009/10/02 03:38:13 $
   $Author: straten $ */

#ifndef __Pulsar_Statistics_h
#define __Pulsar_Statistics_h

#include "Pulsar/Index.h"
#include "TextInterface.h"

namespace Pulsar {

  class Archive;
  class Profile;
  class ProfileStats;
  class PhaseWeight;

  //! Interface to a variety of useful statistics
  class Statistics : public Reference::Able
  {

  public:

    //! Default constructor
    Statistics (const Archive* = 0);

    //! Set the instance from which statistics will be drawn
    void set_Archive (const Archive*);
    //! Get the instance from which statistics will be drawn
    const Archive* get_Archive () const;

    //! Set the sub-integration from which statistics will be drawn
    void set_subint (Index _isubint);
    Index get_subint () const;
    
    //! Set the frequency channel from which statistics will be drawn
    void set_chan (Index _ichan);
    Index get_chan () const;

    //! Set the polarization to plot
    void set_pol (Index _ipol);
    Index get_pol () const;

    //! Get the signal-to-noise ratio
    double get_snr () const;

    //! Get the Fourier-noise-to-noise ratio
    double get_nfnr () const;

    //! Get the number of cal transitions
    unsigned get_cal_ntrans () const;

    //! Get the predicted level of 2-bit distortion
    double get_2bit_dist () const;
    
    //! Get the off-pulse baseline
    PhaseWeight* get_baseline ();

    //! Get the on-pulse phase bins
    PhaseWeight* get_onpulse ();

    //! Get all phase bins
    PhaseWeight* get_all ();

    //! Get the Profile statistics interface
    ProfileStats* get_stats ();

    //! Text interface to statistics
    class Interface;

    //! Get the text interface to this
    TextInterface::Parser* get_interface ();

  protected:

    Reference::To<const Archive> archive;
    Index isubint;
    Index ichan;
    Index ipol;

    void setup_stats ();
    mutable Reference::To<ProfileStats> stats;

    const Profile* get_Profile () const;
    mutable Reference::To<const Profile> profile;

  };
}

// standard interface constructor defined in More/General/standard_interface.C
TextInterface::Parser* standard_interface (Pulsar::Archive*);

#endif
