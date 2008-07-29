/***************************************************************************
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

/* $Id: ppfinversion.h,v 1.4 2007/08/08 15:29:50 singh Exp $*/

#ifndef PPFINVERSION_H
#define PPFINVERSION_H

#include <crtools.h>

// Standard library header files
#include <string.h>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>

#include <casa/Quanta.h>
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>

#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

#include <Math.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class ppfinversion
    
    \ingroup Analysis
    
    \brief Brief description for class ppfinversion
    
    \author Kalpana Singh

    \date 2007/06/01

    \test tppfinversion.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[ Because of Polyphase filter implementation at LOFAR stations, the resulting voltage sum of each station beam 
            is presented as multiple spectral channels (subbands), each composed of a time series data with sample rate
	    (200 kHz /1024 ~ 195 kHz) set by the inverse of the subband bandwidth ~ 5 microsec. Since, we are interested 
	    in pulses of temporal width ~ nanosec. therefore, to search suitable pulses, data has to be transformed back 
	    to the time domain to produce high time resolution (~ nanosec). This is achieved by the exact inversion of what 
	    is implemented through hardware at station level, specifically by the inversion of polyphase filters (PPF) for 
	    perfect reconstruction of signal.
	      Since we have neglected the negative frequency spectrum in the output of polyphase filter bank but we can 
            regenerate them because those will have complex conjugate values of the data which is input of this class.
	    Therefore, if the number of rows (or subbands) of the input matrix which contains fourier transformed data is 
            50 then we can regenerate the other 50 rows or subbands just by substituting complex conjugate vales in those 
	    subbands. Hence, if we have all 512 subbands for further processing then in that case we will have all the 1024 
	    subbands, including the neglected the neglected subbands because the other half is simple complex conjugate.]
    </ul>
    
    <h3>Terminology</h3>
    
    In the ppfinversion class, we used the fourier transformed data and FIR filter coefficients again but now we want to 
    get rid off the effect of FIR filters, i.e., inversion of frequency response of FIR filters and to achieve this we have 
    to implement IIR filter but with the same filter coefficients as of FIR filters. Sampling frequency and subband_freqeuncies
    are needed to get the subband IDs of fourier transformed data, which can be derived by using class subbandID.
    
    <ul>
    <li><em> filterCoefficient </em>
        <ul>
	<li> FTData  </b>            --   Matrix of Fourier Transformed data (which is Double Complex) where number of rows
	                                  define the number of subbands and number of columns define samples per subband.
				
	<li>  samplingFreq           --   sampling frequency with which data is sampled.
	<li> subband_frequencies     --   a vector of initial frequencies of all subbands on which station level processing 
	                                  is done.
			      
	<li> filterCoefficient </b>   --  Vector of FIR filter coefficients, which is 1024 X 16 = 16384 for 
                                          implemented FIR filters for LOFAR.
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class ppfinversion {
    
  public:
   
   // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor without arguement
    */
    ppfinversion ();
    
    /*!
     \brief arguemented constructor
     
     sets the fourier transformed signal back to the time domain by inverting the processing of polyphase filter banks.
     
     \param    FTData             --  a matrix of fourier transformed data, in which number of rows define according to
                                      the number of subbands available, and number of columns is the number of samples
			              per subband.
				      
     \param  samplingFreq         -- sampling frequency with which data is sampled.
			    
     \param   subband_frequencies -- a vector of initial frequencies of all subbands on which station level processing is done.
			    
     \param  FIRCoefficients      --  A vector of FIR coefficients of size 1024 X 16 = 16384
			    
    */
    
    ppfinversion( const Matrix<DComplex>& FTData,
                  const Vector<Double>& FIRCoefficients ) ;
    
    /*!
    
      \brief Copy constructor
      
      \param other -- Another ppfinversion object from which to create this new
      one.
    */
    ppfinversion (ppfinversion const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~ppfinversion ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another ppfinversion object from which to make a copy.
    */
    ppfinversion& operator= (ppfinversion const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, ppfinversion.
    */
    std::string className () const {
      return "ppfinversion";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary ();    

    // ---------------------------------------Computational Methods

   
    /*!
    \brief set the FIR coefficient vector into the marix of dimension 1024 X 16
    
    \param    FIRCoefficients    --  Vector of FIR filter coefficients
    
    \returns the array of FIR filter  coefficients.
    
    */
    
    Matrix<Double> setInverseFilterCoefficients( const Vector<Double>& FIRCoeff_inv ) ;
    
   /*!
   \ brief defining matrix for Discrete Fourier Transform (DFT) twiddle factors.
   
   \param nofsubbands   --   dimensions of the DFT matrix is equal to the maximum number of subbands 
                             possible which is 1024.
               
   \returns a matrix of complex values of dimensions 1024 X 1024.
   */
   
   Matrix<DComplex> setDFTMatrix( const uint& nofsubbands ) ;
  
   /*!

   \brief inversion of IDFT implementation for polyphase filter bank, i.e., for inversion DFT will be applied.

   \param generatedSubbands  --  DFT (Discrete Fourier Transform) will be applied on the generated
                                 subbands, which is fourier transformed data after implementation of 
                                 polyphase filter bank.

   \param DFTMatrix          --  Matrix with twiddle factors

   \returns the fourier tranformed data points, after implementation DFT matrix.

   */

   Matrix<Double> DFTinversion( const Matrix<DComplex>& generatedSubbands,
                                const Matrix<DComplex>& DFTMatrix ) ;
    
   /*!
   
   \brief  implementation of IIR filter to invert the effect of FIR filters
   
   \param  DFTinverted    --  After DFT inversion, datapoints for each subfilters to get rid off the 
                              effect of FIR filters no. of rows will not be exceeding 1024, however, 
			      no of columns will be equal to the no of samples in a particular subband
			   
   \param IIRCoefficientArray -- IIR Coefficients are identical to FIR filters the only difference is 
                                 in the way of implementation.
				 

   \returns the ppf inverted signal
   */   
   
   						
   Vector<Double> FIR_inversion( const Vector<Double>& FIRCoeff_inv,
                               	 const Matrix<DComplex>& FTData ) ;
				 
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (ppfinversion const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* PPFINVERSION_H */
  
