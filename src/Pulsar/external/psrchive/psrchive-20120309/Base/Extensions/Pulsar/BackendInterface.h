//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/BackendInterface.h,v $
   $Revision: 1.1 $
   $Date: 2008/05/12 02:00:26 $
   $Author: straten $ */

#ifndef __Pulsar_BackendInterface_h
#define __Pulsar_BackendInterface_h

#include "Pulsar/Backend.h"
#include "TextInterface.h"

namespace Pulsar
{
  //! Text interface to Backend extension
  class Backend::Interface : public TextInterface::To<Pulsar::Backend>
  {
  public:
    Interface (Backend* = 0);
  };

}

#endif
