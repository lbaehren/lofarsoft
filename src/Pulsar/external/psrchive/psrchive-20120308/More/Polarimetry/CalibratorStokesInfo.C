/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/CalibratorStokesInfo.h"
#include "Pulsar/CalibratorStokes.h"

#include <string.h>

//! Constructor
Pulsar::CalibratorStokesInfo::CalibratorStokesInfo (const CalibratorStokes* cs)
{
  calibrator_stokes = cs;
  together = false;
}

std::string Pulsar::CalibratorStokesInfo::get_title () const
{
  return "Reference Source Stokes Parameters";
} 

//! Return the number of parameter classes
unsigned Pulsar::CalibratorStokesInfo::get_nclass () const
{
  if (together)
    return 1;
  else
    return 3;
}

//! Return the name of the specified class
std::string Pulsar::CalibratorStokesInfo::get_name (unsigned iclass) const
{
  static char label [64] = "\\fiC\\fr\\dk\\u (\%\\fiC\\fr\\d0\\u)";
  static char* replace = strchr (label, 'k');

  if (together)
    *replace = 'k';
  else
    *replace = '1' + iclass;

  return label;
}


//! Return the number of parameters in the specified class
unsigned Pulsar::CalibratorStokesInfo::get_nparam (unsigned iclass) const
{
  if (together)
    return 3;
  else
    return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::CalibratorStokesInfo::get_param (unsigned ichan, unsigned iclass,
					 unsigned iparam) const
{
  if (!calibrator_stokes->get_valid (ichan))
    return 0.0;

  unsigned index = 0;

  if (together)
    index = iparam;
  else
    index = iclass;

  return 100.0 * calibrator_stokes->get_stokes(ichan)[index+1];

}

