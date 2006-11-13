//#  TransformedNollMatrix.cc: Representation of the transformed Noll matrix.
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
//#  $Id: TransformedNollMatrix.cc,v 1.5 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/TransformedNollMatrix.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/AsciiReader.h>
#include <Sinfoni/Debug.h>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      TransformedNollMatrix::TransformedNollMatrix(Integer size) : 
        itsMatrix(size, size)
      {
        //# Load the transformed Noll matrix from file.
        DBG_TRACE("Reading the Transformed Noll matrix ...");
        AsciiReader reader(DataFile::transformedNollMatrix);
        reader.read(itsMatrix);

        //# Dump the matrix to file if debugging is enabled
        DBG_DUMP("TransformedNollMatrix.out", itsMatrix);
      }


      const MatrixReal& theTransformedNollMatrix()
      {
        static TransformedNollMatrix nm(theNrOfSubApertures);
        return nm.itsMatrix;
      }

    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
