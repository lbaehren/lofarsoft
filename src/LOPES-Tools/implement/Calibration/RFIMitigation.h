/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: RFIMitigation.h,v 1.8 2006/11/02 13:39:26 singh Exp $*/

#ifndef RFIMITIGATION_H
#define RFIMITIGATION_H

// AIPS++ header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta.h>
#include <scimath/Functionals/Interpolate1D.h>
#include <scimath/Mathematics/InterpolateArray1D.h>
#include <scimath/Functionals/ScalarSampledFunctional.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ArrayColumn.h>

// LOPES-Tools header files
#include <Functionals/StatisticsFilter.h>

using std::endl;
using std::cerr;
using std::cout;
using casa::AipsError;
using casa::DComplex;
using casa::InterpolateArray1D;
using casa::Matrix;
using casa::Slice;
using casa::Vector;

namespace LOPES {

/*!
  \class RFIMitigation.h
  
  \ingroup Calibration
  
  \brief Brief description for class RFIMitigation
  
  \author Kalpana Singh

  \date 2006/03/15

  \test tRFIMitigation.cc
  
  <h3>Synopsis</h3>

  Extract a gaincurve from a complex spectra of \f$N_{\rm Ant}\f$ array elements. 
  Also provides the functionality to normalize a complex gain spectra by the 
  extracted the gaincurves. Then normalized gain spectra is subjected for the rejection
  of noise signals by adopting a certain kind of filter. 
    
  The gain curve is extracted by the following method:

  Given a discrete complex spectrum
  \f[
  f[\nu_{k}] = A[\nu_{k}] \exp(i \phi[\nu_{k}])
  \f]
  where
  \f[
  k=0, 1, 2...(N_{\rm FreqChan}-1)
  \f]
  and
  \f[
  \nu_{\rm Bandwidth} \equiv \nu_{(N_{\rm FreqChan}-1)}-\nu_{0}
  \f]
  we extract the amplitudes of the terms,
  \f$A[\nu_{k}]\f$, and call these the terms of the absolute spectrum.
  The absolute spectrum is then divided into
  \f$N_{\rm Segments}\f$ segments, referred to as frequency sub bands. 
  Each sub band consists of \f$M_{\rm FreqChan}\f$ members, where
  \f[
  M_{\rm FreqChan} = \frac{N_{\rm FreqChan}}{N_{\rm SubBands}}
  \f]
  According to the scanning method ( by impelementing median filter) one of these
  members is chosen as representative of the gain curve on that sub band. 
  
  A natural cubic spline is used to interpolate the gain curve for any
  frequency sub bands across the original segment. 
  Also provides the functionality to normalize a complex gain spectra by
  the extracted the gaincurves. Then normalize gain spectra is subjected 
  for the rejection of noise signals. 
  
  <h3>Terminology</h3>
  
  In the RFIMitigation class, we employ specific terminology. To clarify subtle
  differences, the important terms (and their derivatives) are explained below in
  logical groups:
  <ul>
    <li><em>segmentationVector</em>
    <ul>
      <li><b>gainValues</b> -- Extract a gaincurve from a complex spectra of
      \f$N_{\rm Ant}\f$ array elements.
      <li><b> dataBlockSize </b> -- Has to be provided by the user, which will
      consist the number of frequency channel and number of antennas to be
      analyzed.
      <li><b> nOfSegments </b> -- no of segments in which array of gain curves
      has to be segmented.
      <li><b> segmentationIndices </b> -- Vector which consist the frequency
      indices, with respect to which whole curve will be divided into number of
      sgementation.
    </ul>
    <li><em>Gain curves</em>
    <ul>
      <li><b>gainValueSpectra</b> -- the background curve of a spectrum after
      normalisation. 
    </ul>
    <li><em>Reference FrequencyIndices</em>
    <ul>
      <li><b>abscissa</b> -- the reference values of frequency abscissa for the
      array of gain curves. For a point, \f$f(x)\f$, in cartesian coordinates,
      the \f$x\f$-value is considered the abscissa, and \f$f\f$ evaluated at
      \f$x\f$ is the ordinate which is the amplitude of the gainvalue at that
      frequency index.
    </ul>
  </ul>
  
  <h3>Motivation</h3>
  
  This class was motivated by two needs:
  <ol>
    <li>We wish to to remove RFIs from gaincurves extracted from the antennas.
   To do this, we need some relative measure of gain.
   <li>We wish to flag regions of RFI. The method applied is to normalize a
   spectrum by dividing it by its underlying gain curve. From this normalized
   spectrum, we are able to easily identify RFI flooded regions. And then
   deviations from average gaincurves is measured, minimum noisee level has been
   calculated, and for the frequencyindices where RFI is identified that 
   gaincurve values is substituted by the inverse of the normalized gaincurve
   value. 
  </ol>
*/
class RFIMitigation {
  
  int segmentInit;
  
  int segmentFinal;
  
 public:
  
  // --- Construction ----------------------------------------------------------
  
  /*!
    \brief Default constructor
  */
  RFIMitigation ();
  
  /*!
    \brief Argumented Constructor
    
    Sets the data array of the gain curves to the 2-dimensional specifications, 
    number of rows indicate the number of frequency indices, however no of columns 
    indicate the number of antennas to be scanned.
    
    \param spectra       -- two dimensional array of data that depends on 
                            number of antennas and the whole frequency range
			    to be scanned.
    \param dataBlockSize -- dimension of the array of gaincurves.
  */
  RFIMitigation (const Matrix<DComplex>& spectra,
  		 const int& nOfSegments,
		 const int& dataBlockSize );
  
  // --- Destruction -----------------------------------------------------------
  
  /*!
    \brief Destructor
  */
  virtual ~RFIMitigation ();
  

  // --- Computation methods ----------------------------------------------------

   /*!
 
 \brief absolute gainvalues of the input complex (fft) data

 \param spectra	--	two dimensional complex array of data that depends on 
  			number of antennas and the whole frequency range
			to be scanned.

 \ returns the array of absolute values of gain
 */
 
 
 Matrix<double> getAbsoluteGainValues( const Matrix<DComplex>& spectra ) ;
 
 
  /*!
    \brief Get the array indices defining the frequency range segmentation

    Frequency indices with respect to which segmentation of the gain curve array
    has to be done.
    
    \param dataBlockSize -- dimension of the array of gaincurves.   
    \param nOfSegments   -- No of segments in which the gainvalue array has to
                            be divided.

    \return indices --  Segmentation vector which consist of frequency indices
                        for segmentation.
  */
 Vector<int> getSegmentationVector( const int& dataBlockSize, 
				    const int& nOfSegments ) ;
 
 
 /*!
   \brief interpolation of the gains

   interpolation of spectral trend after extraction with one of the scanning
   method for requested antenna and frequency index.
      
   Scan the argument matrix with any statistical filter, the arguement must be 
   of the same dimensions, i.e., no. of frequency bins and number of antennas
   must be same as defined at construction.

   Construct a linear interpolation across the argument 'gains' after extraction
   of gainvalues within each segment with median filter. The returned matrix will
   be of the same dimension of the input matrix.
   
   \param gainValues	-- gain Values across which to construct a linear
                           interpolation.
   \param dataBlockSize -- dimension of the array of gaincurves.
   \param nOfSegments   -- No of segments in which the gainvalue array has to be
                           divided.

   \returns interpolatedGains --  the matrix of gain Values for each antenna
                                  requested.   
 */
 Matrix<double>  getinterpolatedGains (const Matrix<DComplex>& spectra,
				       const int& dataBlockSize,
				       const int& nOfSegments );
 
 /*!
   \brief Get difference of extracted gainValues with the interpolated gains.
   
   Give difference spectra of interpolated gainvalues with respect to the
   filtered spectra.
   
   \param gainValues	-- gain Values across which to construct a linear
                           interpolation.
   \param dataBlockSize -- dimension of the array of gaincurves.
   \param nOfSegments   -- No of segments in which the gainvalue array has to be
                           divided.
   
   \returns an array of difference spectra .
 */
 Matrix<double> getdifferenceSpectra (const Matrix<DComplex>& spectra,
				      const int& dataBlockSize,
				      const int& nOfSegments);
 
 /*!
   \brief Get the normalized spectra

   Get normalized(division) extracted gainValues with respect to the interpolated
   gains.

   Give normalized spectra of interpolated gainvalues with respect to the
   filtered spectra.
   
   \param gainValues	-- gain Values across which to construct a linear
                           interpolation.
   \param dataBlockSize -- dimension of the array of gaincurves.
   \param nOfSegments   -- No of segments in which the gainvalue array has to
                           be divided.

   \return spectra -- an array of normalized spectra .
 */
 Matrix<double> getnormalizeSpectra( const Matrix<DComplex>& spectra,
				     const int& dataBlockSize,
				     const int& nOfSegments ) ;
 
 
 /*!
\brief detect the position of RFI and remove it. 

 Position of RFI is detected by comparing the standard deviation value with the 
 minimum rms value for whole set of data of the normalized gainvalues array, and then at that position 
 gainvalue is replaced by inverse of the gainvalue.
  
 \param gainValues	--	gain Values across which to construct a spline.
 
 \param dataBlockSize  --	dimension of the array of gaincurves.
  
  \param nOfSegments    --      No of segments in which the gainvalue array has to be divided.
 
 \returns gainValue array with the same dimension as that of input spectra after removal of RFI.
 
 */

 
 Matrix<double> getRFIRemovedSpectra( const Matrix<DComplex>& spectra,
				     const int& dataBlockSize,
				     const int& nOfSegments ) ;
 

/*!
\brief retrace original dimension of gainValues array after implementation of various steps. 

 Divides the original gainValue array in various segments according to the input 
 segmentation vector, various steps implemented to reduce the noise level, and then 
 original dimension of the spectra array is retraced again.
 
 \param gainValues	--	gain Values across which to construct a spline.
 
 \param segmentVector  --	A vector which gives the refrence indices in which a 
  				frequency band (row of array) has to be divided.
 
 \returns an array of gainValues with the same dimension as to that of the input spectra .
 
 */

 
// Matrix<double> retraceOriginalSpectra( const Matrix<double>& gainValueSpectra,
// 	        		       const Vector<int>& segmentationIndices ) ;


};

}

#endif /* _RFIMITIGATION_H_ */
