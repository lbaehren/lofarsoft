//#  InputData.cc: Class containing the input data
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
//#  $Id: InputData.cc,v 1.10 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/InputData.h>
#include <Sinfoni/PSF/InputDataRepAscii.h>
#include <Sinfoni/PSF/InputDataRepFits.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/Exceptions.h>
#include <Sinfoni/Debug.h>

using namespace std;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      InputData::InputData() : 
        itsRep(0)
      {
      }


      InputData::~InputData()
      {
        reset();
      }


      void InputData::reset()
      {
        delete itsRep;
        itsRep = 0;
        itsSumCountsMean.free();
        itsSumCountsVariance.free();
        itsDiffCountsMean.free();
        itsDiffCountsVariance.free();
        itsCurvatureMean.free();
        itsCurvatureCovariance.free();
        itsMirrorMean.free();
        itsMirrorCovariance.free();
        itsGainMean.free();
        itsGainVariance.free();
      }


      void InputData::openFile(const std::string& filename, DataFormat format)
      {
        DBG_TRACE("Opening input data file " << filename << " ...");

        //# Check to see if an input file has already been opened. If so, we
        //# need to clear the caches of all classes that have cached their
        //# calculation results. However, this is not yet implemented. To
        //# avoid that wrong data will be returned (namely the results of
        //# earlier calculations), we will throw an exception here. This
        //# safety net is only temporarily, until the flushing of cached data
        //# has been properly implemented.
        if (itsRep)
          THROW (IOException, "Cannot open more than one input data file");

//         reset();
        switch (format) {
        case ASCII:
          itsRep = new InputDataRepAscii(filename);
          break;
        case FITS:
          itsRep = new InputDataRepFits(filename, "PSF Data");
          break;
        default:
          THROW (IOException, "Unknown input file format specified");
        }

        //# Dump the input data to file if debugging is enabled
        //# Note: This triggers the actual reading of the input data!
        DBG_DUMP("InputData.out",  
                 "\nsumCountsMean :\n"      << sumCountsMean()      << endl <<
                 "\nsumCountsVariance :\n"  << sumCountsVariance()  << endl <<
                 "\ndiffCountsMean :\n"     << diffCountsMean()     << endl <<
                 "\ndiffCountsVariance :\n" << diffCountsVariance() << endl <<
                 "\ncurvatureMean :\n"      << curvatureMean()      << endl <<
                 "\ncurvatureCovariance :\n"<< curvatureCovariance()<< endl <<
                 "\nmirrorMean :\n"         << mirrorMean()         << endl <<
                 "\nmirrorCovariance :\n"   << mirrorCovariance()   << endl <<
                 "\ngainMean :\n"           << gainMean()           << endl <<
                 "\ngainVariance :\n"       << gainVariance()       << endl);
      }


      InputData& theInputData()
      {
        static InputData data;
        return data;
      }

    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
