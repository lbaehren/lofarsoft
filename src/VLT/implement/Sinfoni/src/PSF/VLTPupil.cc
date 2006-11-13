//#  VLTPupil.cc: Class representing the pupil of the VLT telescope
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
//#  $Id: VLTPupil.cc,v 1.6 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/VLTPupil.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/Debug.h>
#include <Sinfoni/Exceptions.h>
#include <Sinfoni/Math.h>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      //# Initialize static data members. 
      const Real VLTPupil::theirPupilDiameter = 8.0;
      const Real VLTPupil::theirSecondaryDiameter = 1.12;
      const Real VLTPupil::theirSpiderAngle = 101;
      const Real VLTPupil::theirSpiderWidth = 0.039;


      VLTPupil::VLTPupil(int size, 
                         bool hasCentralObstruction, 
                         bool hasSpiderMount) :
        itsDelta(theirPupilDiameter/size),
        itsMatrix(size, size)
      {
        createPrimaryMask();
        if (hasCentralObstruction) addSecondaryMask();
        if (hasSpiderMount) addSpiderMount(hasCentralObstruction);
        DBG_DUMP("VLTPupil.out", itsMatrix);
      }


      void VLTPupil::createPrimaryMask()
      {
        //# The diameter of the primary mask should be equal to the size of
        //# the matrix. The radius \a r, obviously, should be half that size.
        Real r(0.5*itsMatrix.rows());

        //# The center of the circular mask.
        Real c(0.5*(itsMatrix.rows()-1));

        //# All points with a distance smaller than the radius fall within the
        //# mask; they will be set equal to one, the others to zero.
        using namespace tensor;
        itsMatrix = where(sqr(i-c) + sqr(j-c) < pow2(r), 1, 0);
      }


      void VLTPupil::addSecondaryMask()
      {
        //# Radius of the secondary mask (the central obstruction) in grid
        //# samples.
        Real r(0.5*theirSecondaryDiameter/itsDelta);

        //# The center of the circular mask
        Real c(0.5*(itsMatrix.rows()-1));

        //# All points with a distance smaller than the radius of the central
        //# obstruction fall outside the mask; they will be set equal to zero.
        using namespace tensor;
        itsMatrix = itsMatrix && where(sqr(i-c) + sqr(j-c) < pow2(r), 0, 1);
      }


      void VLTPupil::addSpiderMount(bool hasSecondaryMask)
      {
        //# Radius of the secondary mask (the central obstruction) in grid
        //# samples.
        Real r(hasSecondaryMask ? 0.5*theirSecondaryDiameter/itsDelta : 0);

        //# The center of the circular mask
        Real c(0.5*(itsMatrix.rows()-1));

        //# Width of the legs of the spider mount in grid samples.
        Real sw(theirSpiderWidth/itsDelta);

        //# Semi-angle \a phi of the legs of the spider mount in radians;
        //# its sine \a sinphi, and its tangent \a tanphi.
        Real phi(0.5*theirSpiderAngle*pi/180);
        Real sinphi(sin(phi));
        Real tanphi(tan(phi));

        //# All points that "match" with the spider legs are not part of the
        //# mask; they will be set equal to zero.
        using namespace tensor;
        itsMatrix = itsMatrix && 
          where(abs(i-c) > 0 && abs(j-c) > 0 &&
                abs(j-c) > tanphi*(abs(i-c) - r) &&
                abs(j-c) < tanphi*(abs(i-c) - r + sw/sinphi), 0, 1);

        //# The data should be stored column-major. However, C++ stores data
        //# row-major. Hence we must transpose the matrix.
        itsMatrix.transposeSelf(secondDim, firstDim);
      }


      const VLTPupil& theVLTPupil()
      {
        static VLTPupil pupil(theNrOfPupilSamples);
        return pupil;
      }


    } // namespace PSF

  } // namespace SINFONI
  
} // namespace ASTRON
