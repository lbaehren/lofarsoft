//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/tempo_impl.h,v $
   $Revision: 1.1 $
   $Date: 2008/05/24 04:00:58 $
   $Author: straten $ */

#ifndef __TEMPO_IMPLEMENTATION_H
#define __TEMPO_IMPLEMENTATION_H

#include "Observatory.h"

// these implementation details are hidden
namespace Tempo
{
  // observatories parsed from obsys.dat, defined in obsys.C
  extern std::vector< Reference::To<Observatory> > antennae;
  
  // parse obsys.dat, implemented in obsys.C
  void load_obsys ();

  // return the ITOA code for the given telescope name, implemented in itoa.C
  std::string itoa_code (const std::string& telescope_name);
}

#endif
