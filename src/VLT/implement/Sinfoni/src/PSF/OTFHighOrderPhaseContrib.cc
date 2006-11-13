//#  OTFHighOrderPhaseContrib.cc: Contribution of high-order phase to OTF.
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
//#  $Id: OTFHighOrderPhaseContrib.cc,v 1.7 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/OTFHighOrderPhaseContrib.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/AsciiReader.h>
#include <Sinfoni/Debug.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      //# An unnamed namespace is the C++ equivalent of the C static keyword.
      namespace 
      {
        inline MatrixReal contrib(const MatrixReal& structFunc, Real d_r0)
        {
          MatrixReal result(structFunc.shape());
          result = exp(-0.5 * structFunc * pow(d_r0, Real(5.0/3.0)));
          DBG_DUMP("OTFHighOrderPhaseContrib.out", result);
          return result;
        }

      } //# namespace

      OTFHighOrderPhaseContrib::OTFHighOrderPhaseContrib(Integer size) :
        itsStructFunc(size, size)
      {
        //# Load the matrix containing the structure function for the
        //# high-order component of the residual phase for \f$D/r_0=1\f$.
        DBG_TRACE("Reading the structure function for the high order "
                  "component ...");
        AsciiReader reader(DataFile::structFuncHighOrderPhase);
        reader.read(itsStructFunc);
        
        //# Dump the matrix to file if debugging is enabled.
        DBG_DUMP("StructFuncHighOrderPhase.out", itsStructFunc);
      }


      MatrixReal theOTFHighOrderPhaseContrib(Real d_r0)
      {
        static OTFHighOrderPhaseContrib otfhopc(2*theNrOfPupilSamples);
        return contrib(otfhopc.itsStructFunc, d_r0);
      }


    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
