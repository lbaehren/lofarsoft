//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/RadiometerWeight.h,v $
   $Revision: 1.1 $
   $Date: 2008/08/05 13:02:43 $
   $Author: straten $ */

#ifndef _Pulsar_RadiometerWeight_H
#define _Pulsar_RadiometerWeight_H

#include "Pulsar/IntegrationWeight.h"

namespace Pulsar {
  
  //! Sets the weight of each Integration according to its duration
  class RadiometerWeight : public IntegrationWeight
  {
    double get_weight (const Integration*, unsigned ichan);
  };
  
}

#endif

