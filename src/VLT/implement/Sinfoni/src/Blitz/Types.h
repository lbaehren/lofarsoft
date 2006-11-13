//#  Types.h : Make Blitz-specific typedefs available in global namespace.
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
//#  $Id: Types.h,v 1.5 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_BLITZ_TYPES_H
#define SINFONI_BLITZ_TYPES_H

//# Includes
#include <Sinfoni/Blitz/Typedefs.h>
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    // @name Types
    // Make the type names defined in Blitz/Typedefs.h available in the 
    // namespace ASTRON::SINFONI.
    //@{
    using TYPES::VectorInteger;
    using TYPES::VectorReal;
    using TYPES::VectorDouble;
    using TYPES::VectorComplex;
    using TYPES::VectorDComplex;
    
    using TYPES::MatrixInteger;
    using TYPES::MatrixReal;
    using TYPES::MatrixDouble;
    using TYPES::MatrixComplex;
    using TYPES::MatrixDComplex;
    //@}

  } // namespace SINFONI

} // namespace ASTRON

#endif
