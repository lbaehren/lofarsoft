/***************************************************************************
 *
 *   Copyright (C) 2007-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RemoveBaseline.h"

using namespace Pulsar;

Functor< void (Archive*) >
Archive::remove_baseline_strategy ( new RemoveBaseline::Total,
				    &RemoveBaseline::Total::transform );


void Pulsar::Archive::remove_baseline () try
{
  remove_baseline_strategy (this); 
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::remove_baseline";
}
