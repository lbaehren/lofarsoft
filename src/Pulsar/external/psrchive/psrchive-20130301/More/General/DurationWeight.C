/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DurationWeight.h"
#include "Pulsar/Integration.h"

double Pulsar::DurationWeight::get_weight (const Integration* integration,
					   unsigned ichan)
{
  return integration->get_duration();
}
