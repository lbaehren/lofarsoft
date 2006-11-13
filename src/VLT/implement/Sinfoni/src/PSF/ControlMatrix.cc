//#  ControlMatrix.cc: implementation of the control matrix
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
//#  $Id: ControlMatrix.cc,v 1.14 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/ControlMatrix.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/PSF/VoltageToZernike.h>
#include <Sinfoni/AsciiReader.h>
#include <Sinfoni/Debug.h>
#include <Sinfoni/Math.h>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      
      ControlMatrix::ControlMatrix(Integer size)
        : itsMatrix(size, size) 
      {
        //# Load the control matrix with the data on the so-called 'Voltage
        //# basis'.
        DBG_TRACE("Reading the Control matrix  ...");
        AsciiReader reader(DataFile::controlMatrix);
        reader.read(itsMatrix);

        //# Attention: The data in this file are stored column-major. However,
        //# C++ stores data row-major. Hence, we must transpose the matrix
        //# after we've read it into memory.
        itsMatrix.transposeSelf(secondDim, firstDim);

        //# Transform the control matrix from 'Voltage basis' to 'Zernike
        //# basis'.
        voltageToZernike();

        //# Dump the control matrix to file if debugging is enabled.
        DBG_DUMP("ControlMatrix.out", itsMatrix);
      }


      void ControlMatrix::voltageToZernike()
      {
        DBG_TRACE("Transforming matrix from Voltage to Zernike basis ...");

        //# Create a transformation matrix \a v2z and a result matrix \a
        //# result, both with dimension equal to those of \a matrix.
        MatrixReal v2z(theVoltageToZernike());
        MatrixReal result(itsMatrix.shape());

        //# Apply the wavelength correction to the transformation matrix \a
        //# v2z.
        v2z *= 2 * pi / theVLTWaveLength;

        //# Calculate the matrix product \a v2z * \a itsMatrix and assign the
        //# result to \c itsMatrix.
        using namespace tensor;
        result = sum(v2z(i,k) * itsMatrix(k,j), k);
        itsMatrix = result;
      }
       

      const MatrixReal& theControlMatrix()
      {
        static ControlMatrix cm(theNrOfSubApertures);
        return cm.itsMatrix;
      }

    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
