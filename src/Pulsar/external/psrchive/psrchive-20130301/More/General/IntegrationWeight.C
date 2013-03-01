/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IntegrationWeight.h"
#include "Pulsar/Integration.h"

//! Default constructor
Pulsar::IntegrationWeight::IntegrationWeight ()
{
  weight_absolute = true;
}

//! Set integration weights
void Pulsar::IntegrationWeight::weight (Integration* integration)
{
  unsigned nchan = integration->get_nchan();

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    double factor = get_weight (integration, ichan);
    float current_weight = integration->get_weight (ichan);

    // always ignore zapped channels
    if (!current_weight)
      continue;

    if (weight_absolute)
      current_weight = factor;
    else
      current_weight *= factor;

    integration->set_weight (ichan, current_weight);
  }
}
