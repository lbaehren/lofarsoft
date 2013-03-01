//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Polarization.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_polarization_h
#define __Pulsar_polarization_h

// this file defines some convenience routines for use in polarization work

namespace Pulsar {

  class Archive;
  class PolnProfile;
  class Profile;

  //! Return a newly constructed PolnProfile with state == Stokes
  const PolnProfile* new_Stokes (const Archive*, unsigned sub, unsigned chan);

  //! Return a possibly newly constructed Profile with state specified by code
  const Profile* new_Profile (const PolnProfile* data, char code);

}


#endif



