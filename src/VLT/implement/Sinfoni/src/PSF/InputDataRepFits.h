//#  InputDataRepFits.h: Representation of input data in FITS format.
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
//#  $Id: InputDataRepFits.h,v 1.7 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_INPUTDATAREPFITS_H
#define SINFONI_PSF_INPUTDATAREPFITS_H

#include <Sinfoni/Config.h>

#if !defined(HAVE_CCFITS)
#error CCfits package is required
#endif

//# Includes
#include <Sinfoni/PSF/InputDataRep.h>
#include <CCfits/CCfits>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      
      class InputDataRepFits : public InputDataRep
      {
      public:
        static const std::string hduName;
        InputDataRepFits(const std::string& file, const std::string& hdu);
        virtual ~InputDataRepFits();
        virtual Integer nrOfSamples();
        virtual void sumCountsMean(VectorReal& v);
        virtual void sumCountsVariance(VectorReal& v);
        virtual void diffCountsMean(VectorReal& v);
        virtual void diffCountsVariance(VectorReal& v);
        virtual void curvatureMean(VectorReal& v);
        virtual void curvatureCovariance(MatrixReal& m);
        virtual void mirrorMean(VectorReal& v);
        virtual void mirrorCovariance(MatrixReal& m);
        virtual void gainMean(VectorReal& v);
        virtual void gainVariance(VectorReal& v);
      private:
        void read(VectorReal& vector, const std::string& columnName);
        void read(MatrixReal& matrix, const std::string& columnName);
        CCfits::FITS*     itsFits;
        CCfits::BinTable* itsTable;
     };
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
