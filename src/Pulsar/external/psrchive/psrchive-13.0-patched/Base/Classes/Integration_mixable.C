/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Integration.h"

using namespace std;

/*!
  Maximum frequency difference in MHz
*/
double Pulsar::Integration::match_max_frequency_difference = 0.1; // 100 kHz

// ///////////////////////////////////////////////////////////////////////
//
// Integration::mixable
//
/*!  This function defines the set of observing and processing
  parameters that should be equal (within certain limits) before two
  Integrations may be combined using operators.
  \param integ pointer to Integration to be compared with this
  \retval reason if match fails, describes why in English
  \return true if archive matches this
*/
bool Pulsar::Integration::mixable (const Integration* integ, string& reason) const
{
  bool result = true;
  
  if (fabs(get_centre_frequency() - integ->get_centre_frequency()) > 
      match_max_frequency_difference) {
    reason = "Centre frequency mismatch";
    return false;
  }
  
  if (get_bandwidth() != integ->get_bandwidth()) {
    reason = "Bandwidth mismatch";
    return false;
  }
  
  if (get_nchan() != integ->get_nchan()) {
    reason = "Number of channels mismatch";
    return false;
  }

  if (get_npol() != integ->get_npol()) {
    reason = "Number of polarisations mismatch";
    return false;
  }
  
  if (get_nbin() != integ->get_nbin()) {
    reason = "Number of bins mismatch";
    return false;
  }
  
  if (get_dispersion_measure() != integ->get_dispersion_measure()) {
    reason = "Dispersion measure mismatch";
    return false;
  }
  
  if (get_basis() != integ->get_basis()) {
    reason = "Basis mismatch";
    return false;
  }
  
  if (get_state() != integ->get_state()) {
    reason = "State mismatch";
    return false;
  }

  return result;
}









