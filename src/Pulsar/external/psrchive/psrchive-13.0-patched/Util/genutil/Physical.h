//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Physical.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_Physical_h
#define __Pulsar_Physical_h

namespace Pulsar {

  //! Speed of light in m/s
  const double speed_of_light = 299792458;

  //! Returns the dispersion delay between a frequency and reference frequency
  double dispersion_delay (double dm, double reference_freq, double freq);

  //! Returns the dispersion smearing across a given bandwidth
  double dispersion_smear (double dm, double reference_freq, double bw);

}

#endif
