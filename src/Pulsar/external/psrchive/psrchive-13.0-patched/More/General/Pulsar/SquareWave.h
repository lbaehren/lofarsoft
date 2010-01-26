//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SquareWave.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_SquareWave_h
#define __Pulsar_SquareWave_h

#include "Pulsar/Algorithm.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio using (on-off)/rms
  class SquareWave : public Algorithm {

  public:

    //! Default constructor
    SquareWave ();
    
    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

    //! Search for multiple level transitions
    void get_transitions (const Profile* profile,
			  std::vector<unsigned>& up,
			  std::vector<unsigned>& down);

    //! Count the level transitions
    unsigned count_transitions (const Profile* profile);

    //! Set the rise time of the square wave in turns of phase
    void set_risetime (float turns) { risetime = turns; }

    //! Get the rise time of the square wave in turns of phase
    float get_risetime () const { return risetime; }

    //! Set the threshold for square wave detection
    void set_threshold (float turns) { threshold = turns; }

    //! Get the threshold for square wave detection
    float get_threshold () const { return threshold; }

  protected:

    float risetime;
    float threshold;
    unsigned use_nbin;

  };

}

#endif
