//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/find_spike_edges.h,v $
   $Revision: 1.1 $
   $Date: 2006/11/06 20:25:01 $
   $Author: straten $ */

#ifndef __find_spike_edges_h
#define __find_spike_edges_h

namespace Pulsar {

  class Profile;

  //! Find the bin numbers at which the flux falls below a threshold
  void find_spike_edges(const Profile* profile, int& rise, int& fall,
                        float pc = 0.2, int spike_bin = -1);

}

#endif

