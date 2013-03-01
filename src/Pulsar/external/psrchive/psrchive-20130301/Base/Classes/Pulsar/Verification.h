//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Verification.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/17 23:04:45 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveVerification_h
#define __Pulsar_ArchiveVerification_h

#include "Pulsar/Check.h"

namespace Pulsar {

  /*! Pure abstract base class of sanity checks performed before an
    Archive instance is written to disk. */
  class Verification : public Archive::Check {

  public:
    
    //! Check the Archive and throw an exception on error
    virtual void apply (const Archive*) = 0;

  };

}

#endif
