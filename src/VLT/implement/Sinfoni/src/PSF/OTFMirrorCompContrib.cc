//#  OTFMirrorCompContrib.cc: contribution of mirror component to OTF
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
//#  $Id: OTFMirrorCompContrib.cc,v 1.8 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/OTFMirrorCompContrib.h>
#include <Sinfoni/PSF/ModalCoordResidualPhase.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/AsciiReader.h>
#include <Sinfoni/Debug.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      OTFMirrorCompContrib::OTFMirrorCompContrib(Integer size) :
        itsStructFunc(size, size)
      {
        structFunc();
      }


      void OTFMirrorCompContrib::structFunc()
      {
        //# Initialize itsStructFunc to zero.
        itsStructFunc = 0;

        //# Matrix used for holding the different U-functions.
        MatrixReal u(itsStructFunc.shape());

        //# Covariance matrix of the modal coordinates of the residual phase.
        MatrixReal Cee(theModalCoordResidualPhase().covariance());

        //# Compute the structure function.
        DBG_TRACE("Computing the structure function for the mirror "
                  "component ... (be patient)");
        for (int i=1; i<theNrOfSubApertures; i++) {
          for (int j=i; j<theNrOfSubApertures; j++) {
            AsciiReader reader(DataFile::uFunction(i,j));
            reader.read(u);
            if (i == j) itsStructFunc += Cee(i,j) * u;
            else itsStructFunc += (Cee(i,j) + Cee(j,i)) * u;
          }
        }
        DBG_DUMP("StructFuncMirrorComp.out", itsStructFunc);
      }

      MatrixReal theOTFMirrorCompContrib()
      {
        static OTFMirrorCompContrib otfmcc(2*theNrOfPupilSamples);
        DBG_TRACE("Computing mirror component contribution to OTF ...");
        MatrixReal result(otfmcc.itsStructFunc.shape());
        result = exp(-0.5 * otfmcc.itsStructFunc);
        DBG_DUMP("OTFMirrorCompContrib.out", result);
        return result;
      }
        
    } // namespace PSF

  } // namespace SINFONI
  
} // namespace ASTRON
