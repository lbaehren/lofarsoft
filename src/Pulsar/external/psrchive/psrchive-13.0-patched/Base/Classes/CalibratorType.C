/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorTypes.h"

#include <string.h>

Pulsar::Calibrator::Type* 
Pulsar::Calibrator::Type::factory (const std::string& name)
{
  if ( strcasecmp(name.c_str(), "flux") == 0 )
    return new CalibratorTypes::Flux;

  if ( strcasecmp(name.c_str(), "single") == 0 ||
       strcasecmp(name.c_str(), "SingleAxis") == 0 )
    return new CalibratorTypes::SingleAxis;

  if ( strcasecmp(name.c_str(), "van02eA1") == 0 ||
       strcasecmp(name.c_str(), "Polar") == 0 )
    return new CalibratorTypes::van02_EqA1;

  if ( strcasecmp(name.c_str(), "bri00e19") == 0 )
    return new CalibratorTypes::bri00_Eq19;

  if ( strcasecmp(name.c_str(), "van04e18") == 0 ||
       strcasecmp(name.c_str(), "Britton" ) == 0 )
    return new CalibratorTypes::van04_Eq18;

  if ( strcasecmp(name.c_str(), "van09") == 0 ||
       strcasecmp(name.c_str(), "Hamaker") == 0 )
    return new CalibratorTypes::van09_Eq;

  throw Error (InvalidParam, "Pulsar::Calibrator::Type::factory",
	       "unrecognized calibrator type name '" + name + "'");
}

bool Pulsar::Calibrator::Type::is_a (const Type* that) const
{
  // cerr << "typeid(this)=" << typeid(*this).name() << endl;
  // cerr << "typeid(that)=" << typeid(*that).name() << endl;

  return typeid(*this) == typeid(*that);
}
