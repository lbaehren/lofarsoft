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

/* $Id$*/

#ifndef PPFIMPLEMENT_H
#define PPFIMPLEMENT_H

#include <crtools.h>

// Standard library header files
#include <string.h>

// AIPS++/CASA header files
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Slice.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

#include <casa/Quanta.h>
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>


namespace CR { // Namespace CR -- begin
  
  /*!
    \class ppfimplement
    
    \ingroup Analysis
    
    \brief Brief description for class ppfimplement
    
    \author Kalpana Singh

    \date 2007/06/01

    \test tppfimplement.cc
    
    <h3>Prerequisite</h3>
    
    Polyphase filter is basically a combination of FIR (Finite Impulse Response)
    filters and an IDFT (Inverse Discrete Fourier Transform) stage. Each input
    at LOFAR station is filtered in a polyphase filter (PPF), consisting of FIR 
    filter banks of 15th order and an IDFT stage. This structure implements a
    filterbank for a computational cost that is approximately double that of the
    FFT alone, for the case where the prototype low pass (FIR) filter is an order
    of magnitude longer than the FFT. After implementation of polyphase filter
    bank at station, the observing frequency band is split into 1024 frequency 
    subbands, the negative part of the original spectrum is omitted, i.e., only
    512 subbands will be taken for further processing. Each subband signal is
    decimated with a factor of 1024 after filtering, therefore, the clock rate
    after filtering is reduced to (200 MHz /1024) ~ 195 kHz for sampling
    frequency 200 MHz. From the resulting 512 frequency subbands, a number of
    subbands can be selected. The selection of subband is controlled by Local
    Control Unit at LOFAR stations.
    
    <h3>Terminology</h3>

    In the ppfimplementation class, we employ specific terminology. To clarify
    subtle differences, the important terms (and their derivatives) are explained
    below in logical groups:
    
    <ul>
      <li><em> filterCoefficient </em>
      <li> inputSignal </b> -- Vector of digitized samples on which ppf is
      implemented.
      <li> filterCoefficient </b> -- Vector of FIR filter coefficients, which is
      1024 X 16 = 16384 for implemented FIR filter for LOFAR.
    </ul>
    
    <h3>Motivation</h3>
    
    This class is motivated by the need of inversion of polyphase filter bank, we
    try to simulate the implemented polyphase filter bank at LOFAR stations so
    that we can compare the raw data from Transient Buffer Boards (TBBs) on
    stations with station beamformed data. We wish to invert the effect of
    station level signal processing to get the original signal, full frequency
    spectrum and to understand the signal processing, we first simulate for the
    filterbanks which is implemented at stations.
    
    <h3>Example(s)</h3>
    
  */  
  class ppfimplement {
    
  private :
    
    //! Counter for the number of rows in the filter coefficients matrix
    uint filterRows_p;
    //! Counter for the number of columns in the filter coefficients matrix
    uint filterColumns_p;
    
  public:
    
    // === Construction =========================================================

    //! Default constructor without argument
    ppfimplement ();
    
    /*!
      \brief Argumented Constructor
      
      Sets the signal to the fourier transformed data in 1024 subbands after
      implementation of polyphase filter banks. If 1024 samples are fed into the
      filter bank, then the output is 1 sample per subband, i.e, clock rate is 
      decimated by a factor of 1024. However, in one shot of time if all the
      subbands are observed then all 1024 samples can be found.
      
      \param sample -- input signal on which polyphase filter has to be implemented.
      
      \param FIRCoefficients   --   a vector of FIR coefficients of size
      1024 X 16 = 16384.
    */
    
    ppfimplement ( const Vector<Double>& samples,
		   const Vector<Double>& FIRCoefficients );
    
    /*!
      \brief Copy constructor
      
      \param other -- Another ppfimplement object from which to create this new
      one.
    */
    ppfimplement (ppfimplement const &other);
    
    // === Destruction ==========================================================
    
    //! Destructor
    virtual  ~ppfimplement ();
    
    // === Operators ============================================================
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another ppfimplement object from which to make a copy.
    */
    ppfimplement& operator= (ppfimplement const &other); 

    // === Parameter access =====================================================
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, ppfimplement.
    */
    std::string className () const {
      return "ppfimplement";
    }
    
    /*!
      \brief Provide a summary of the internal status
    */
    void summary ();    
    
    // =========================================================================
    //
    //  Methods
    //
    // =========================================================================
    
    /*!
      \brief set the FIR coefficient vector into matrix of dimension 1024 X 16
      
      \ param FIRCoefficients -- Vector FIR filter coefficients  
    */    
    Matrix<Double> setFilterCoefficients (const Vector<Double> &FIRCoefficients);
    
    /*!
      
    \ brief defining matrix for Inverse Discrete Fourier Transform.
    
    \param  filterRows  --  dimension of IDFT matrix is equal to the rows filter matrix which will give 1024
    of subbands, however, because of the symmteric nature of the IDFT matrix,last 512 
    output  will be complex conjugate of the first 512 output after implementation of
    IDFT matrix.
    
    \ returns a matrix of complex values of dimension 1024 X 1024
    */
    
    Matrix<DComplex> setIDFTMatrix ( const uint& filterRows );
    
    
    /*!
      \brief implementation of FIR filters on signal.
      
      \param sample            --   input signal on which polyphase filter has to be implemented.
      
      \param   FIRCoefficientArray   --  Matrix of FIR coefficients of dimension 1024 X 16
      
      \ returns the vector of FIR implemented signal.
      
    */
    
    Matrix<Double> FIRimplementation ( Vector<Double>& samples,
				       const Matrix<Double>& FIRCoefficientArray );
    
    
    /*!
      
    \brief implementation of IDFT matrix on FIR implemented vector.
    
    \param sample                --   input signal on which polyphase filter has to be implemented.
    
    \ param    FIRCoefficients   --  Vector FIR filter coefficients
    
    \returns the fourier transformed data, where number of rows signify 1024 subbands, and number of columns are samples
    per subbands.
    
    */
    
    Matrix<DComplex> FFTSamples( Vector<Double>& samples,
				 const Vector<Double>& FIRCoefficients ) ;
    
    
    
    
  private:
    
    //! Unconditional copying
    void copy (ppfimplement const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* PPFIMPLEMENT_H */
