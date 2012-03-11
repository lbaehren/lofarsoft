//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/BackendCorrection.h,v $
   $Revision: 1.2 $
   $Date: 2008/05/28 08:32:34 $
   $Author: straten $ */

#ifndef __Pulsar_BackendCorrection_H
#define __Pulsar_BackendCorrection_H

#include "Reference.h"
#include "Jones.h"

namespace Pulsar {

  class Archive;

  //! Correct the backend convention
  class BackendCorrection : public Reference::Able
  {

  public:

    //! Calibrate the polarization of the given archive
    void operator () (Archive*) const;

    //! Return true if the operation is required
    bool required (const Archive*) const;

  };


}

#endif

