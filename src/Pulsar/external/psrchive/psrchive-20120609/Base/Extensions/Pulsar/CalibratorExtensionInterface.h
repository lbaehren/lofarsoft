//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CalibratorExtensionInterface.h,v $
   $Revision: 1.1 $
   $Date: 2009/02/20 06:11:26 $
   $Author: straten $ */

#ifndef __Pulsar_CalibratorExtensionInterface_h
#define __Pulsar_CalibratorExtensionInterface_h

#include "Pulsar/CalibratorExtension.h"
#include "TextInterface.h"

namespace Pulsar
{
  //! Text interface to CalibratorExtension extension
  class CalibratorExtension::Interface
    : public TextInterface::To<Pulsar::CalibratorExtension>
  {
  public:
    Interface (CalibratorExtension* = 0);
  };

}

#endif
