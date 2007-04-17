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

/* $Id: inverseFFT.cc,v 1.4 2007/04/03 14:03:07 bahren Exp $*/

#include <Analysis/inverseFFT.h>

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ----------------------------------------------------------------- inverseFFT
  
  inverseFFT::inverseFFT ()
  {
    ppfCoefficients_p = Vector<Double> (1,1.0);
    dataBlocksize_p   = 1;
  }
  
  // ----------------------------------------------------------------- inverseFFT
  
  inverseFFT::inverseFFT( const Vector<Double>& samples,
			  const Vector<Double>& ppfCoefficients,
			  const uint& dataBlocksize)
  {
    ppfCoefficients_p = ppfCoefficients;
    dataBlocksize_p   = dataBlocksize;
  }
  
  // ----------------------------------------------------------------- inverseFFT

  inverseFFT::inverseFFT (inverseFFT const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  inverseFFT::~inverseFFT ()
  {
    destroy();
  }
  
  void inverseFFT::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  inverseFFT& inverseFFT::operator= (inverseFFT const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void inverseFFT::copy (inverseFFT const &other)
  {
    ppfCoefficients_p = other.ppfCoefficients_p;
    dataBlocksize_p   = other.dataBlocksize_p;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Matrix<Double> inverseFFT::ppfCoefficients( const Vector<Double>& ppfCoefficients,
					      const uint& dataBlockSize )
  {
    Matrix<Double> ppf_coefficients;
    
    try {
      //uint elements = nelements.ppf_coefficients ;
      uint m=0;
      
      for( uint i=0; i<15; i++ ) {
	for( uint j=0; j<(dataBlockSize ); j++ ) {
	  ppf_coefficients(j,i)= ppfCoefficients(m);
	  m = m+1 ;
	}
      }
    } catch( AipsError x ) {
      cerr << "inverseFFT::ppfCoefficients" <<x.getMesg() << endl ;
    } 
    
    return ppf_coefficients ;
  }
  
  Matrix<Double> inverseFFT::blockSamples( const Vector<Double>& samples,
    			                   const uint& dataBlockSize )
  {
    Matrix<Double> blocksamples ;
    
    try{
      
      uint m (0);
      
      //Vector<Double> Sample = samples ;
      
      uint sampleLength = samples.nelements() ;
      
      uint sampleSize = int(sampleLength/16*dataBlockSize) ;
      
      
      for( uint j = 0; j< ( sampleSize ); j++ ) {
	
	for( uint i=0; i<(dataBlockSize ); i++ ) {
	  
	  blocksamples(i,j) = samples(m) ;
	}
      }
    } catch( AipsError x ) {
      cerr << "inverseFFT::blockSamples" <<x.getMesg() << endl ;
    }
    
    return blocksamples ;
  }
  
  
  Vector<Double> inverseFFT::multipliedVector( const Vector<Double>& waveSamples,
					       const Matrix<Double>& ppfCoff )
    
  {
    Vector <Double> sampleVal (waveSamples);
    
    try{
      
      //uint elements = waveSamples.nelements() ;
      
      uint m = ppfCoff.ncolumn() ;
      
      
      for( uint j=0; j < m; j++ ){
	
	Vector<Double> ppfCoffColumn = ppfCoff.column(j) ;
	
	Vector <Double> sampleVal = sampleVal*ppfCoffColumn ;
	
      }
    } catch( AipsError x ){
      cerr<< "inverseFFT::multipliedVector" << x.getMesg() << endl ;
    }
    
    return sampleVal ;
  }

  // ------------------------------------------------------------------ FFTVector
  
  Vector<DComplex> inverseFFT::FFTVector( const Vector<Double>& FIRimplemented,
  					  const uint& dataBlockSize )
    
  {
    Vector<DComplex> FFTVector(dataBlockSize/2+1);
    
    try{
      
      //FFTServer< Double, DComplex> server( IPosition(1, dataBlockSize),FFTEnums::REALTOCOMPLEX ) ;
      FFTServer<Double,DComplex> server;
      //(IPosition(1,dataBlockSize),				    FFTEnums::REALTOCOMPLEX);
      
      
      server.fft( FFTVector,FIRimplemented ) ;
      
      //FFTVector = output ;
      
    }
    
    catch( AipsError x ){
      cerr<< "inverseFFT::FFTVector" << x.getMesg() << endl ;
    }

    return FFTVector ;
  }

  // ----------------------------------------------------------------- IFFTVector
  
  Vector<Double> inverseFFT::IFFTVector(Vector<DComplex> const &fourierTransformed,
					uint const &dataBlockSize )  
    
  {
    Vector<Double> IFFTVector(dataBlockSize);
    
    try{
      
      FFTServer<Double,DComplex> server(IPosition(1,dataBlockSize),
					FFTEnums::REALTOCOMPLEX);
      // Vector<DComplex> input (512+1) = fourierTransformed ;
      
      
      server.fft( IFFTVector,fourierTransformed ) ;
      
      
    } catch( AipsError x) {
      cerr<< "inverseFFT::IFFTVector" << x.getMesg() << endl ;
    }
    
    return IFFTVector ;
  }

  Matrix<Double> inverseFFT::matchingValues( const Vector<Double>& samples,
					     const Vector<Double>& ppfCoefficients,
					     const uint& dataBlockSize )
    
  {
    Matrix<Double> matchingValues;
      
    try {
      inverseFFT inft;
      
      Matrix<Double> ppf_Coefficients = inft.ppfCoefficients( ppfCoefficients,
							      dataBlockSize ) ;
      
      Matrix<Double> dataSamples = inft.blockSamples( samples,
						      dataBlockSize );
      
      uint nofRows = dataSamples.nrow ();
      uint nofColumns = dataSamples.ncolumn ();
      
      matchingValues.resize (nofRows,nofColumns) ;
      
      Vector<Double> multipliedVector (nofRows) ;
      
      // Vector<Double> fourierTransformedVector (nofRows) ;
      
      // Vector<Double> inverseFourierVector (nofRows) ;
      
      for( uint i=0; i< nofColumns ; i++ ) {
	
	Vector<Double> dataSampleColumn = dataSamples.column(i) ;
	
	Vector<Double> waveSamples = dataSampleColumn ;
	
	Vector<Double> multipliedVector = inft.multipliedVector( waveSamples,
								 ppf_Coefficients ) ;
	
        
	Vector<DComplex> fourierTransformedVector = inft.FFTVector( multipliedVector,
								    dataBlockSize ) ;
	
	Vector<Double> inverseFourierVector = inft.IFFTVector( fourierTransformedVector,
							       dataBlockSize ) ; 
	
	matchingValues.column(i) = inverseFourierVector ;
      }      
    } catch( AipsError x ) {
      cerr<< "inverseFFT::matchingValues" << x.getMesg() << endl ;
    }

    return matchingValues ;
  }
  
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
}  // Namespace CR -- end
