//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Checks/Pulsar/Dedispersed.h,v $
   $Revision: 1.1 $
   $Date: 2006/12/01 22:40:17 $
   $Author: straten $ */

#ifndef __Pulsar_Dedispersed_h
#define __Pulsar_Dedispersed_h

#include "Pulsar/Verification.h"

namespace Pulsar {
  
  //! Verifies that each Integration has a consistent Dedisperse Extension
  /*! If the Archive::dedispersed attribute is set then, for each
    Integration, ensure that
    <OL>
    <LI> the Dedisperse Extension has been added;
    <LI> Dedisperse::dispersion_measure == Archive::dispersion_measure; and
    <LI> Dedisperse::reference_frequency == Archive::centre_frequency.
    </OL>
  */
  class Dedispersed : public Verification {

  public:
    
    //! Return the name of the check
    std::string get_name () { return "Dedispersed"; }

    //! Perform the check
    void apply (const Archive* archive);

  };

}

#endif
