//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/setup_profiles.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/08 19:12:58 $
   $Author: straten $ */

#ifndef __Pulsar_setup_profiles_h
#define __Pulsar_setup_profiles_h

//! Set up a profile vector, const or not
template<typename I, typename P>
void setup_profiles_dat (I subint, P& profiles)
{
  const unsigned npol = subint->get_npol ();
  const unsigned nchan = subint->get_nchan ();

  profiles.resize (npol * nchan);

  for (unsigned ipol=0; ipol<npol; ipol++)
    for (unsigned ichan=0; ichan<nchan; ichan++)
      profiles[ipol*nchan + ichan] = subint->get_Profile (ipol, ichan);
}

//! Set up a profile vector, const or not from an extension
template<class E, typename I, typename P>
void setup_profiles (I subint, P& profiles)
{
  E* ext = subint->get_Profile(0,0)->Pulsar::Profile::get<E>();
  if (!ext)
    throw Error (InvalidState, "setup_profiles<Extension>",
		 "first profile is missing required Extension");

  const unsigned naux = ext->get_size();
  const unsigned nchan = subint->get_nchan ();

  profiles.resize (naux * nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    ext = subint->get_Profile(0,ichan)->Pulsar::Profile::get<E>();
    if (!ext)
      throw Error (InvalidState, "setup_profiles<Extension>",
		   "profile[%u] is missing required Extension", ichan);

    for (unsigned iaux=0; iaux<naux; iaux++)
      profiles[iaux*nchan + ichan] = ext->get_Profile (iaux);
  }
}

#endif
