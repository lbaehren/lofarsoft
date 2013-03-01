//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/FixFluxCal.h,v $
   $Revision: 1.2 $
   $Date: 2007/05/01 05:50:00 $
   $Author: straten $ */

#ifndef __Pulsar_FixFluxCal_h
#define __Pulsar_FixFluxCal_h

#include "Pulsar/Correction.h"

namespace Pulsar {

  class StandardCandles;

  //! Corrects the Signal::Source type and name attributes of the Archive
  /*! If the Archive::type attribute is set to Signal::PolnCal and the
    coordinates are close to any of the known standard candles, then
    the Archive::type and Archive::name attributes may be corrected. */
  class FixFluxCal : public Correction {

  public:
    
    //! Default constructor
    FixFluxCal ();

    //! Destructor
    ~FixFluxCal ();

    // Return the name of the check
    std::string get_name () { return "FixFluxCal"; }

    //! Correct the Signal::Source type attribute of the Archive
    void apply (Archive* archive);

    //! Get a description of the corrections made to the Archive
    std::string get_changes () const { return changes; }

  protected:

    //! Database of standard candles and their coordinates
    Reference::To<StandardCandles> fluxcals;

    //! Description of corrections made in last call to apply
    std::string changes;

  };
 

}

#endif
