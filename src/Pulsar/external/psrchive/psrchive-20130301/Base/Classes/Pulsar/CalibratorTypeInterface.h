//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Types/Pulsar/CalibratorTypeInterface.h,v $
   $Revision: 1.1 $
   $Date: 2009/02/20 06:11:26 $
   $Author: straten $ */

#ifndef __Pulsar_CalibratorTypeInterface_h
#define __Pulsar_CalibratorTypeInterface_h

#include "Pulsar/CalibratorType.h"
#include "TextInterface.h"

namespace Pulsar
{
  //! Text interface to Calibrator::Type class
  class Calibrator::Type::Interface
    : public TextInterface::To<const Calibrator::Type>
  {
  public:
    Interface (const Calibrator::Type* = 0);
  };

}

#endif
