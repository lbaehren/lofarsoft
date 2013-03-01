//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/DurationWeight.h,v $
   $Revision: 1.4 $
   $Date: 2008/08/05 13:30:53 $
   $Author: straten $ */

#ifndef _Pulsar_DurationWeight_H
#define _Pulsar_DurationWeight_H

#include "Pulsar/IntegrationWeight.h"

namespace Pulsar {
  
  //! Sets the weight of each Integration according to its duration
  class DurationWeight : public IntegrationWeight
  {
    double get_weight (const Integration*, unsigned ichan);
  };
  
}

#endif

