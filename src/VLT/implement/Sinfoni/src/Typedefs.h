//#  Typedefs.h: Sinfoni-specific typedefs.
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: Typedefs.h,v 1.8 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_TYPEDEFS_H
#define SINFONI_TYPEDEFS_H

#include <Sinfoni/Config.h>

//# Includes
#include <complex>

namespace ASTRON 
{
  namespace SINFONI 
  {
    // Define shorthands for various data types. The precision of the various
    // types depend on the compiler directive USE_SINGLE_PRECISION. The types
    // are in a namespace to prevent pollution of the global namespace.
    namespace TYPES 
    {
#if defined(USE_SINGLE_PRECISION)
      typedef int                  Integer;
      typedef unsigned int         Unsigned;
      typedef float                Real;
      typedef double               Double;
#else
      typedef long                 Integer;
      typedef unsigned long        Unsigned;
      typedef double               Real;
      typedef long double          Double;
#endif
      typedef std::complex<Real>   Complex;
      typedef std::complex<Double> DComplex;

    } // namespace TYPES

  } // namespace SINFONI

} // namespace ASTRON

#if defined(HAVE_BLITZ)
#include <Sinfoni/Blitz/Typedefs.h>
#endif

#endif
