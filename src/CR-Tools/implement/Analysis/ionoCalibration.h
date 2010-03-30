/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef IONOCALIBRATION_H
#define IONOCALIBRATION_H

#include <crtools.h>

// Standard library header files
#include <string.h>

// AIPS++/CASA header files
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>

#include <casa/Quanta.h>
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>
#include <scimath/Functionals/ScalarSampledFunctional.h>
#include <scimath/Mathematics/InterpolateArray1D.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ArrayColumn.h>

#include <Filters/StatisticsFilter.h>

#include <casa/namespace.h>

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class ionoCalibration
    
    \ingroup CR
    \ingroup Analysis
    
    \brief Brief description for class ionoCalibration
    
    \author Kalpana Singh

    \date 2007/06/07

    \test tionoCalibration.cc
        
    <h3>Synopsis</h3>
    
    For UHEP mode of observation, we are concerned with pulses that come from
    moon which will be influenced by by the earth's ionosphere. Since it
    propagates through ionized medium it gets dispersed. Therefore, our intension 
    is to make corrections for the phase corruptions caused by this ambient
    plasma medium. Our aim is to make corrections for maximum phase shift and
    time delays for each frequency subbands and the relative shift and delays for
    different subbands.

    <h3>Example(s)</h3>
    
  */  
  class ionoCalibration {

    //! Sampling frequency
    double sampleFrequency_p;
    
  public:
    
    // === Parameter access =====================================================
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, ionoCalibration.
    */
    std::string className () const {
      return "ionoCalibration";
    }

    //! Provide a summary of the internal status
    inline void summary () {
      summary (std::cout);
    }

    //! Provide a summary of the internal status
    void summary (std::ostream &os);    

    // === Methods ==============================================================

    /*!
      \brief calculating elevation angle of the observation
      
      \param hrAngle          -- Hour angle of the observing source in radians
      \param declinationAngle -- Equatorial coordinate declination of the source
             in radians
      \param geomagLatitude   -- Geomagnetic latitude of the observer in radians
      
      \return elevationAngle -- Elevation angle of the source.
    */
    Double calcElevationAngle (const Double& hrAngle,
			       const Double& declinationAngle,
			       const Double& geomagLatitude);
    
    /*!
      \brief calculating slant TEC value in the direction of the source
      
      \param height_ionosphere -- Maximum height of the ionosphere which will be
             400 or 450 Km maximum
      \param TEC_value         -- Total Electron Content (Vertical) of
             ionospehric plasma column.
      \param elevationAngle    -- Elevation angle of the source.
      
      \return tec -- Total electron content of the slant plasma column in the
              direction of source.
    */
    Double calcSTECvalue (const Double& height_ionosphere,
			  const Double& TEC_value,
			  const Double& elevationAngle);
    
    /*!			  
      \brief calculating relative phase delays with respect to first subband of the series which is used for observation
      
      param STEC_value		--	Slant Total Electron Content value of ionospehric plasma column
      param subband_frequencies	--	vector of starting frequency of each subband which is used for beamforming
      
      \return ralativePhase -- The vector of relative phase delays (caused by the
              dispersive ionospheric medium travelled by the wave) for each
	      subbands, which are used for beamforming with respect to first
	      subband.
    */
  Vector<Double> calRelativePhaseDiff (const Double& STEC_value, 
                                       const Vector<Double>& subband_frequencies);

				       
    /*!
   \brief calculating relative time delays with respect to first subband of the series which is used for observation

    param STEC_value		--	Slant Total Electron Content value of ionospehric plasma column
    
    param samplingFreq		--	frequency with which data is sampled
    
   param subband_frequencies	--	vector of starting frequency of each subband which is used for beamforming
    
   returns the vector of relative time delays for each subbands, which are used for beamforming with respect
   to first subband.

    */
  Vector<Double> calRelativeDelay( const Double& STEC_value, 
				   const Vector<Double>& subband_frequencies ) ;     
  
  /*!
    
   \brief set the negative frequency spectrum by substituting complex conjugate values, however, rest of the 
     subbands will have zero values.
   
   \param     FTData    --   Fourier transform data of the given subbands 
   
   \param  samplingFreq         -- sampling frequency with which data is sampled.
			    
   \param  subband_frequencies -- a vector of initial frequencies of all subbands on which station level processing is done.
   
   \return valus -- The complex values for the given and generated subbands
   
   */
   
    Matrix<DComplex> SubbandGenerator( const Matrix<DComplex>& FTData,
    				       const Double& samplingFreq,
                  		       const Vector<Double>& subband_frequencies ) ;
    
    
    /*!
      \brief Argumented constructor
      
      \param FFT_data            -- Fourier transform data in frequency domain
      \param hrAngle             -- Hour angle of the observing source 
      \param declinationAngle    -- Equatorial coordinate declination of the source
      \param geomagLatitude      -- Geomagnetic latitude of the observer
      \param height_ionosphere   -- Maximum height of the ionosphere which will be
             400 or 450 Km maximum
      \param TEC_value           -- Total Electron Content (Vertical) of ionospehric
             plasma column
      \param samplingFreq        -- Frequency with which data is sampled
      \param subband_frequencies -- Vector of starting frequency of each subband
             which is used for beamforming
    */					
    Matrix<DComplex> phaseCorrection (const Matrix<DComplex>& FFT_data,
				      const Double& hrAngle,
				      const Double& declinationAngle,
				      const Double& geomagLatitude,
				      const Double& height_ionosphere,
				      const Double& TEC_value,
				      const Double& samplingFreq,
				      const Vector<Double>& subband_frequencies);
    
    Matrix<DComplex> phaseRECorrection (const Matrix<DComplex>& FFT_data,
					const Double& hrAngle,
					const Double& declinationAngle,
					const Double& geomagLatitude,
					const Double& height_ionosphere,
					const Double& TEC_value,
					const Double& samplingFreq,
					const Vector<Double>& subband_frequencies);
    
  };
  
} // Namespace CR -- end

#endif /* IONOCALIBRATION_H */
  
