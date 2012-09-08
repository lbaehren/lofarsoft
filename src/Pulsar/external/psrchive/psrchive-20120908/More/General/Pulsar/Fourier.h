//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Fourier.h,v $
   $Revision: 1.6 $
   $Date: 2008/08/04 09:43:27 $
   $Author: straten $ */

#ifndef __Pulsar_Fourier_h
#define __Pulsar_Fourier_h

#include "FTransformAgent.h"

namespace Pulsar {

  class Profile;
  class PolnProfile;

  //! Return the forward Fourier transform of the input Profile
  Profile* fourier_transform (const Profile*,
			      FTransform::Plan* = 0);

  //! Return the forward Fourier transform of the input PolnProfile
  PolnProfile* fourier_transform (const PolnProfile*,
				  FTransform::Plan* = 0);

  //! Return the forward Fourier transform of the input Profile
  Profile* complex_fourier_transform (const Profile*,
				      FTransform::Plan* = 0);

  //! Return the forward Fourier transform of the input PolnProfile
  PolnProfile* complex_fourier_transform (const PolnProfile*,
					  FTransform::Plan* = 0);

  //! Square-law detect the input Profile
  void detect (Profile* input);

  //! Square-law detect the input PolnProfile
  void detect (PolnProfile* input);

}

#endif
