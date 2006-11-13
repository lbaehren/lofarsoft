//#  InputData.h: Class containing the input data.
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
//#  $Id: InputData.h,v 1.10 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_INPUTDATA_H
#define SINFONI_PSF_INPUTDATA_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>
#include <Sinfoni/Math.h>
#include <Sinfoni/Assert.h>
#include <Sinfoni/PSF/InputDataRep.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // This class represents the measurement data that will be input into
      // the PSF estimation program. The actual data is not stored in this
      // class, but in the InputDataRep class. We chose this approach in order
      // to facilitate different formats of input data (e.g. FITS, ASCII). The
      // actual loading of the data will be done by the InputDataRep
      // class. Any conversion that must be made will be done there as
      // well. Note that the result of such conversions will be cached
      // locally.
      //
      // \note This class acts like a Singleton class, but it is implemented a
      // bit different from the original design pattern. We use the global
      // function theInputData() to return a reference to an instance of
      // InputData. This function stores a static instance of InputData, hence
      // it will be constructed when theInputData() is called for the first
      // time.
      class InputData
      {
      public:
        // The data format of the input data file. Formats that are currently
        // supported are ASCII and FITS.
        enum DataFormat { UNKNOWN, ASCII, FITS };

        // Destructor. Delete the input data representation class, which in
        // turn will close the data file; flush all locally cached data.
        ~InputData();

        // Open the input data file \a filename, which has the data format \a
        // format. As a side effect, any open data file will be closed and all
        // locally cached data will be flushed.
        //
        // \note Actually, this method will create an instance of an input
        // data representation class (whose class type depends of the
        // specified data format) that will open the data file \a filename.
        void openFile(const std::string& filename, DataFormat format);

        // \name Accessor methods 
        // Return the requested input data. The data is cached in the
        // associated private data member, so the data only needs to be read
        // from file once, when first requested.
        //@{

        // Return the number of data samples in the input file.
        Integer nrOfSamples();

        // Return the mean of the sum of the photon counts in the intra-focal
        // plane and extra-focal plane for each sub-aperture.
        const VectorReal& sumCountsMean();

        // Return the variance of the sum of the photon counts in the
        // intra-focal plane and extra-focal plane for each sub-aperture.
        const VectorReal& sumCountsVariance();

        // Return the mean of the difference of the photon counts in the
        // intra-focal plane and extra-focal plane for each sub-aperture.
        const VectorReal& diffCountsMean();

        // Return the variance of the difference of the photon counts in the
        // intra-focal plane and extra-focal plane for each sub-aperture.
        const VectorReal& diffCountsVariance();

        // Return the mean of the measured curvature for each sub-aperture. 
        const VectorReal& curvatureMean();

        // Return the covariance of the measured curvature for all
        // sub-apertures.
        const MatrixReal& curvatureCovariance();

        // Return the mean of the voltages applied to each deformable mirror.
        const VectorReal& mirrorMean();

        // Return the covariance of the voltages applied to all deformable
        // mirrors.
        const MatrixReal& mirrorCovariance();

        // Return the mean of the applied modal gain for each sub-aperture.
        const VectorReal& gainMean();

        // Return the variance of the applied modal gain for each
        // sub-aperture.
        const VectorReal& gainVariance();
        //@}

      private:
        // The friend function is used to return an instance of InputData.
        friend InputData& theInputData();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        InputData();
        InputData(const InputData&);
        InputData& operator=(const InputData&);
        //@}
        
        // Reset will close any open data file, and flush all locally cached
        // data.
        void reset();

        // Pointer to an input data representation class.
        InputDataRep* itsRep;

        // Private data member that caches the input data, so that it only
        // needs to be read form file once.
        //@{
        VectorReal  itsSumCountsMean;
        VectorReal  itsSumCountsVariance;
        VectorReal  itsDiffCountsMean;
        VectorReal  itsDiffCountsVariance;
        VectorReal  itsCurvatureMean;
        MatrixReal  itsCurvatureCovariance;
        VectorReal  itsMirrorMean;
        MatrixReal  itsMirrorCovariance;
        VectorReal  itsGainMean;
        VectorReal  itsGainVariance;
        //@}
      };


      // Return a reference to an instance of InputData.
      InputData& theInputData();


      //## ----------------  Inline methods  ---------------- ##//

      inline Integer InputData::nrOfSamples()
      {
        AssertStr(itsRep, "openFile() should be called first");
        return itsRep->nrOfSamples();
      }


      inline const VectorReal& InputData::sumCountsMean()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsSumCountsMean)) {
          itsRep->sumCountsMean(itsSumCountsMean);
        }
        return itsSumCountsMean;
      }


      inline const VectorReal& InputData::sumCountsVariance()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsSumCountsVariance)) {
          itsRep->sumCountsVariance(itsSumCountsVariance);
        }
        return itsSumCountsVariance;
      }
        

      inline const VectorReal& InputData::diffCountsMean()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsDiffCountsMean)) {
          itsRep->diffCountsMean(itsDiffCountsMean);
        }
        return itsDiffCountsMean;
      }


      inline const VectorReal& InputData::diffCountsVariance()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsDiffCountsVariance)) {
          itsRep->diffCountsVariance(itsDiffCountsVariance);
        }
        return itsDiffCountsVariance;
      }


      inline const VectorReal& InputData::curvatureMean()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsCurvatureMean)) {
          itsRep->curvatureMean(itsCurvatureMean);
        }
        return itsCurvatureMean;
      }


      inline const MatrixReal& InputData::curvatureCovariance()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsCurvatureCovariance)) {
          itsRep->curvatureCovariance(itsCurvatureCovariance);
        }
        return itsCurvatureCovariance;
      }


      inline const VectorReal& InputData::mirrorMean()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsMirrorMean)) {
          itsRep->mirrorMean(itsMirrorMean);
        }
        return itsMirrorMean;
      }


      inline const MatrixReal& InputData::mirrorCovariance()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsMirrorCovariance)) {
          itsRep->mirrorCovariance(itsMirrorCovariance);
        }
        return itsMirrorCovariance;
      }


      inline const VectorReal& InputData::gainMean()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsGainMean)) {
          itsRep->gainMean(itsGainMean);
        }
        return itsGainMean;
      }


      inline const VectorReal& InputData::gainVariance()
      {
        AssertStr(itsRep, "openFile() should be called first")
        if (isEmpty(itsGainVariance)) {
          itsRep->gainVariance(itsGainVariance);
        }
        return itsGainVariance;
      }


    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
