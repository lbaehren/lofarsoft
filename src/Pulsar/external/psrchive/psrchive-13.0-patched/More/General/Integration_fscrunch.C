/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/FrequencyIntegrate.h"

using namespace std;

static Pulsar::FrequencyIntegrate* operation = 0;
static Pulsar::FrequencyIntegrate::EvenlySpaced* policy = 0;

static void static_init ()
{
  operation = new Pulsar::FrequencyIntegrate;
  policy    = new Pulsar::FrequencyIntegrate::EvenlySpaced;

  operation->set_range_policy( policy );
}

/*!
  \param nscrunch number of neighbouring frequency channels to
  integrate; if zero, then all channels are integrated into one
 */
void Pulsar::Integration::fscrunch (unsigned nscrunch)
{
  if (!policy)
    static_init ();

  policy->set_nintegrate (nscrunch);
  operation->transform (this);
}
