//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ArchiveTemplates.h,v $
   $Revision: 1.2 $
   $Date: 2007/10/12 02:45:24 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveTemplates_h
#define __Pulsar_ArchiveTemplates_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Combination.h"

namespace Pulsar {

  class Archive;
  class Integration;
  class Profile;

  void foreach (Archive*, Reference::To< Transformation<Profile> >);

  void foreach (Integration*, Reference::To< Transformation<Profile> >);

  void foreach (Archive*, const Archive*, 
		Reference::To< Combination<Profile> >);

  void foreach (Integration*, const Integration*,
		Reference::To< Combination<Profile> >);

}

#endif
