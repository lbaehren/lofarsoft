//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Pulsar.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/23 18:22:53 $
   $Author: straten $ */

#ifndef __Pulsar_Pulsar_h
#define __Pulsar_Pulsar_h

#include "Warning.h"

//! Defines the PSRCHIVE library
namespace Pulsar {

  //! Container classes should check array indeces at the cost of performance
  extern bool range_checking_enabled;

  //! Warning messages filter
  extern Warning warning;

}

#endif
