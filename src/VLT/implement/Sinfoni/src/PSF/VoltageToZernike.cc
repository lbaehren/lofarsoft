//#  VoltageToZernike.cc: Transformation matrix from voltage to Zernike basis.
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
//#  $Id: VoltageToZernike.cc,v 1.4 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/VoltageToZernike.h>
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

      VoltageToZernike::VoltageToZernike(Integer size) :
        itsMatrix(size, size)
      {
        //# Load the transformation matrix 
        DBG_TRACE("Reading the Voltage-to-Zernike transformation matrix ...");
        AsciiReader reader(DataFile::voltageToZernike);
        reader.read(itsMatrix);
      }


      const MatrixReal& theVoltageToZernike()
      {
        static VoltageToZernike v2z(theNrOfSubApertures);
        return v2z.itsMatrix;
      }

    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
