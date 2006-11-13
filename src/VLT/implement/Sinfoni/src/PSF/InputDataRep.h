//#  InputdataRep.h: Abstract base class representing the input data.
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
//#  $Id: InputDataRep.h,v 1.8 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_INPUTDATAREP_H
#define SINFONI_PSF_INPUTDATAREP_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // This abstract base class defines the interface of the representation
      // classes that contain the measurement data that will be input into the
      // PSF estimation program.
      // \see InputData
      class InputDataRep
      {
      public:
        virtual ~InputDataRep() {}
        virtual Integer nrOfSamples() = 0;
        virtual void sumCountsMean(VectorReal& scm) = 0;
        virtual void sumCountsVariance(VectorReal& scv) = 0;
        virtual void diffCountsMean(VectorReal& dcm) = 0;
        virtual void diffCountsVariance( VectorReal& dcv) = 0;
        virtual void curvatureMean(VectorReal& cm) = 0;
        virtual void curvatureCovariance(MatrixReal& cc) = 0;
        virtual void mirrorMean(VectorReal& mm) = 0;
        virtual void mirrorCovariance(MatrixReal& mc) = 0;
        virtual void gainMean(VectorReal& gm) = 0;
        virtual void gainVariance(VectorReal& gv) = 0;
      };

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
