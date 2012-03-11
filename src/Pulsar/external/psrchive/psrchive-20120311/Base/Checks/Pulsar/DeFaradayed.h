//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Checks/Pulsar/DeFaradayed.h,v $
   $Revision: 1.1 $
   $Date: 2006/12/01 22:40:49 $
   $Author: straten $ */

#ifndef __Pulsar_DeFaradayed_h
#define __Pulsar_DeFaradayed_h

#include "Pulsar/Verification.h"

namespace Pulsar {
  
  //! Verifies that each Integration has a consistent DeFaraday Extension
  /*! If the Archive::faraday_corrected attribute is set then, for each
    Integration, ensure that
    <OL>
    <LI> the DeFaraday Extension has been added;
    <LI> DeFaraday::rotation_measure == Archive::rotation_measure; and
    <LI> DeFaraday::reference_frequency == Archive::centre_frequency.
    </OL>
  */
  class DeFaradayed : public Verification {

  public:
    
    //! Return the name of the check
    std::string get_name () { return "DeFaradayed"; }

    //! Perform the check
    void apply (const Archive* archive);

  };

}

#endif
