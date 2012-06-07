//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Dispersion.h,v $
   $Revision: 1.8 $
   $Date: 2010/09/22 02:18:38 $
   $Author: straten $ */

#ifndef __Pulsar_Dispersion_h
#define __Pulsar_Dispersion_h

#include "Pulsar/ColdPlasma.h"
#include "Pulsar/DispersionDelay.h"
#include "Pulsar/Dedisperse.h"

namespace Pulsar {

  //! Corrects dispersive delays
  /*! Rotates the phase of each profile in each frequency channel to
    remove dispersive delays with respect to the reference frequency.

    \post All profiles will be phase-aligned to the reference frequency
  */
  class Dispersion : public ColdPlasma<DispersionDelay,Dedisperse> {

  public:

    //! Default constructor
    Dispersion ();

    //! Return the dispersion measure due to the ISM
    double get_correction_measure (const Integration*);

    //! Return the auxiliary dispersion measure (0 if corrected)
    double get_absolute_measure (const Integration*);

    //! Return true if the Integration has been dedispersed
    bool get_corrected (const Integration* data);

    //! Return the effective dispersion measure that remains to be corrected
    double get_effective_measure (const Integration*);

    //! Return zero delay
    double get_identity () { return 0; }

    //! Combine delays
    void combine (double& result, const double& add) { result += add; }

    //! Setup all necessary attributes
    void set (const Integration*);

    //! Phase rotate each profile by the correction
    void apply (Integration*, unsigned channel);

    //! Apply the current correction to all sub-integrations in an archive
    void execute (Archive*);

    //! Undo the correction
    void revert (Archive*);

    //! Set the dispersion measure
    void set_dispersion_measure (double dispersion_measure)
    { set_measure (dispersion_measure); }
      
    //! Get the dispersion measure
    double get_dispersion_measure () const
    { return get_measure (); }

    //! Get the phase shift
    double get_shift () const;

  protected:

    double folding_period;

  };

}

#endif
