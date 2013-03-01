/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorTypeInterface.h"

using namespace Pulsar;

// Text interface to a Calibrator::Type extension
Calibrator::Type::Interface::Interface (const Calibrator::Type *s_instance)
{
  if( s_instance )
    set_instance( s_instance );

  add( &Calibrator::Type::get_name,
       "name", "Name of calibrator model" );

  add( &Calibrator::Type::get_nparam,
       "nparam", "Number of calibrator model parameters" );
}
