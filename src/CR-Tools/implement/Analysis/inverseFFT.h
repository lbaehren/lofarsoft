/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Kalpana Singh (<mail>)                                                *
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

/* $Id: inverseFFT.h,v 1.5 2007/04/03 14:03:07 bahren Exp $*/

#ifndef _INVERSEFFT_H_
#define _INVERSEFFT_H_

// AIPS++ header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

// CR header files
#include <Math/StatisticsFilter.h>
#include <IO/DataIterator.h>

using casa::DComplex;
using casa::Double;
using casa::FFTServer;
using casa::Matrix;
using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class inverseFFT
    
    \ingroup Analysis
    
    \brief Brief description for class inverseFFT
    
    \author Kalpana Singh

    \date 2006/12/19

    \test tinverseFFT.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class inverseFFT {

    //! Coefficients of the Poly-Phase filter
    Vector<Double> ppfCoefficients_p;
    //! Size of a block of data
    uint dataBlocksize_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    inverseFFT ();
    
    /*!
      \brief Argumented constructor

      \param samples         -- 
      \param ppfCoefficients -- 
      \param dataBlocksize   -- 
    */
    inverseFFT (const Vector<Double>& samples,
                const Vector<Double>& ppfCoefficients,
		const uint& dataBlocksize);
	
    /*!
      \brief Copy constructor 

      \param other -- Another inverseFFT object
    */
    inverseFFT (inverseFFT const &other);
      
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~inverseFFT ();
    
    // ---------------------------------------------------------------- Operators
   

    /*!
      \brief Get the coefficients of the poly-phase filter
      
      \param ppfCoefficients -- 
      \param dataBlockSize   -- 

      \return ppfCoefficients -- 
    */
    Matrix<Double> ppfCoefficients(const Vector<Double>& ppfCoefficients,
				   const uint& dataBlockSize);

    /*!
      \param samples       -- 
      \param dataBlockSize -- 

      \return blockSamples -- 
    */
    Matrix<Double> blockSamples (const Vector<Double>& samples,
				 const uint& dataBlockSize);

    /*!
      \param waveSamples -- 
      \param ppfCoff     -- 

      \return multipliedVector -- 
    */
    Vector<Double> multipliedVector(const Vector<Double>& waveSamples,
    				    const Matrix<Double>& ppfCoff);

    /*!
      \param FIRimplemented -- 
      \param dataBlockSize  -- 

      \return FFTVector -- 
    */
    Vector<DComplex> FFTVector(const Vector<Double>& FIRimplemented,
			       const uint& dataBlockSize );
    
    Vector<Double> IFFTVector(Vector<DComplex> const &fourierTransformed,
    			      uint const &dataBlockSize) ;
    
    Matrix<Double> matchingValues( const Vector<Double>& samples,
                                  const Vector<Double>& ppfCoefficients,
			          const uint& dataBlockSize ) ;
    
     
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another inverseFFT object from which to make a copy.
    */
    inverseFFT& operator= (inverseFFT const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, inverseFFT.
    */
    std::string className () const {
      return "inverseFFT";
    }
    
    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (inverseFFT const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };

}  // Namespace CR -- end
  
#endif /* INVERSEFFT_H */
  
