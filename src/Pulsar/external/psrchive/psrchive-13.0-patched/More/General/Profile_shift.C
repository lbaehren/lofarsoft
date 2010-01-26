/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/PhaseGradShift.h"

using namespace std;

Reference::To<Pulsar::ProfileStandardShift> 
shift_strategy = new Pulsar::PhaseGradShift;

/*
  \param std the profile with respect to which the shift will be estimated
  \retval shift and its error in turns
*/
Estimate<double> Pulsar::Profile::shift (const Profile& std) const
{
  shift_strategy->set_standard (&std);
  shift_strategy->set_observation (this);
  return shift_strategy->get_shift ();
}

