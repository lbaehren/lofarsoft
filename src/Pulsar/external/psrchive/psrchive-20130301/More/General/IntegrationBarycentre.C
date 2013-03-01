/***************************************************************************
 *
 *   Copyright (C) 2012 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IntegrationBarycentre.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Site.h"
#include "sky_coord.h"

using namespace std;

Pulsar::IntegrationBarycentre::IntegrationBarycentre ()
{
  subint = NULL;
}

Pulsar::IntegrationBarycentre::IntegrationBarycentre (
    const Integration *_subint)
{
  set_Integration(_subint);
}

double Pulsar::IntegrationBarycentre::get_Doppler() const
{
  return bary.get_Doppler();
}

MJD Pulsar::IntegrationBarycentre::get_barycentric_epoch() const
{
  return bary.get_barycentric_epoch();
}

void
Pulsar::IntegrationBarycentre::set_Integration (const Integration *_subint) try
{
  if (subint == _subint)
    return;

  subint = _subint;

  bary.set_epoch(subint->get_epoch());
  bary.set_coordinates(subint->get_coordinates());

  double x, y, z;
  const Site *site = Site::location(subint->get_telescope());
  site->get_xyz(x,y,z);
  bary.set_observatory_xyz(x,y,z);
}
catch (Error &error)
{
  throw error += "Pulsar::IntegrationBarycentre::set_Integration";
}
