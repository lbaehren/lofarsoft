/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SNRWeight.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

//! Default constructor
Pulsar::SNRWeight::SNRWeight ()
{
  threshold = 0.0;
}

//! Get the weight of the specified channel
double Pulsar::SNRWeight::get_weight (const Integration* integration,
				      unsigned ichan)
{
  double snr = integration->get_Profile(0,ichan)->snr ();

  if (threshold && snr < threshold)
    snr = 0;

  return snr * snr;
}
