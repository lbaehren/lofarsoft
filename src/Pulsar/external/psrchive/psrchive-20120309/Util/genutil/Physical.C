/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Physical.h"
#include "Error.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar:: dispersion_delay
//
/*! 
  If the frequency is lower than the reference frequency, then the delay
  is positive.
  \return dispersion delay in seconds
  \param dm the dispersion measure in \f$ {\rm pc\,cm}^{-3} \f$
  \param ref_freq the frequency (in MHz) to which the delay is referenced
  \param freq the frequency (in MHz) of the delayed band

  Although the value:

  \f$ DM\,({\rm pc\,cm^{-3}})=2.410331(2)\times10^{-4}D\,({\rm s\,MHz^{2}}) \f$

  has been derived from "fundamental and primary physical and
  astronomical constants" (section 3 of Backer, Hama, van Hook and
  Foster 1993. ApJ 404, 636-642), the rounded value is in standard 
  use by pulsar astronomers (page 129 of Manchester and Taylor 1977).
*/
double Pulsar::dispersion_delay (double dm, double ref_freq, double freq)
{

  if (ref_freq == 0)
    throw Error (InvalidParam, "Pulsar::dispersion_delay", 
                 "invalid reference frequency");

  if (freq == 0)
    throw Error (InvalidParam, "Pulsar::dispersion_delay", 
                 "invalid frequency");

  return (dm/2.41e-4) * ( 1.0/(freq*freq) - 1.0/(ref_freq*ref_freq) );

}

double Pulsar::dispersion_smear (double dm, double reference_freq, double bw)
{
  return fabs (dispersion_delay (dm, reference_freq-0.5*bw,
                                     reference_freq+0.5*bw));
}

