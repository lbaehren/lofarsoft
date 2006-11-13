//#  Typedefs.h: Blitz-specific typedefs.
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
//#  $Id: Typedefs.h,v 1.5 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_BLITZ_TYPEDEFS_H
#define SINFONI_BLITZ_TYPEDEFS_H

#include <Sinfoni/Config.h>

#if !defined(HAVE_BLITZ)
#error Blitz++ package is required
#endif

#include <Sinfoni/Typedefs.h>
#include <blitz/array.h>

namespace ASTRON 
{
  namespace SINFONI 
  {
    namespace TYPES 
    {

      // @name Vectors
      //@{
      typedef blitz::Array<Integer,1>  VectorInteger;
      typedef blitz::Array<Real,1>     VectorReal;
      typedef blitz::Array<Double,1>   VectorDouble;
      typedef blitz::Array<Complex,1>  VectorComplex;
      typedef blitz::Array<DComplex,1> VectorDComplex;
      //@}

      // @name Matrices
      //@{
      typedef blitz::Array<Integer,2>  MatrixInteger;
      typedef blitz::Array<Real,2>     MatrixReal;
      typedef blitz::Array<Double,2>   MatrixDouble;
      typedef blitz::Array<Complex,2>  MatrixComplex;
      typedef blitz::Array<DComplex,2> MatrixDComplex;
      //@}

    } // namespace TYPES

  } // namespace SINFONI

} // namespace ASTRON

#endif
