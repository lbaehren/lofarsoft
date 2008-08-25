/*-------------------------------------------------------------------------*
 | $Id:: tcasa_measures.cc 1901 2008-08-20 13:47:09Z baehren             $ |
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

#include <crtools.h>

#include <cmath>
#include <iostream>
#include <string>

#include <casa/Arrays.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/namespace.h>

#include <Analysis/SubbandID.h>
#include <Analysis/ppfimplement.h>
#include <Analysis/ppfinversion.h>
#include <Analysis/ionoCalibration.h>
#include <scimath/Mathematics/FFTServer.h>

using CR::ppfimplement ;
using CR::ppfinversion ;
using CR::ionoCalibration ;
using CR::SubbandID ;

/*!
  \file tppfimplement.cc

  \ingroup Analysis

  \brief A collection of test routines for ppfimplement
 
  \author Kalpana Singh
 
  \date 2007/06/01
*/

// -----------------------------------------------------------------------------
//  Global variables used throughout the program

uint dataBlockSize (1024);
uint nofsegmentation(16*6) ;
Vector<Double> samples( dataBlockSize*nofsegmentation, 0.0 ) ;
Double hrAngle( 1.034 );
Double declinationAngle( 1.078 );
Double geomagLatitude( 0.915708 ) ;
Double height_ionosphere( 4e5 ) ;
Double TEC_value( 10e17 ) ;
Vector<Double> subband_frequencies( 512, 0.0 );
Double sampling_frequency( 200e6 );

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ppfimplement object
  
  \return nofFailedTests -- The number of failed tests.
*/

int test_ppfimplement ()
{
  std::cout << "\n[test_ppfimplement]\n" << std::endl;
  
  int nofFailedTests (0);

  try {
    ppfimplement ppf_impl;
    ppfinversion ppf_inv ;
    ionoCalibration iono_cal ;
    SubbandID band_ID ;
  }
  
  catch ( AipsError x){
    cerr << "test_ppfimplement :--- testing the default constructor.... " << x.getMesg() << endl;
  }
  return nofFailedTests ;
}


// -----------------------------------------------------------------------------

/*!
  \brief Test working with the functions of the ppfimplement class

  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_ppfimplements ()
{
  std::cout << "\n[test_ppfimplements]\n" << std::endl;

  int nofFailedTests (0);
  
  /* Provide some feedback on the variables used */
  std::cout << "-- Data block size   = " << dataBlockSize      << std::endl;
  std::cout << "-- nof. segments     = " << nofsegmentation    << std::endl;
  std::cout << "-- Vector of samples = " << samples.shape()    << std::endl;
  std::cout << "-- Sample frequency  = " << sampling_frequency << std::endl;
  
  cout << "[1] Testing the MLCG generator ..." << endl ;
  try {
    uint nSample (0);
    ACG gen(1, samples.nelements() );
    
    for(nSample=0; nSample < samples.nelements(); nSample++ ){
      //       Normal rnd(&gen, 0.0, 0.1 );
      //      Double nextExpRand = rnd() ;
      samples(nSample) = 0.0 ; //nextExpRand ;
    }
    /* Resetting the generator, should get the same numbers. */
    gen.reset () ;

    samples( 16*dataBlockSize*3+10 ) = 10.0 ;
    cout << "-- samples with a peak has been saved " << endl ;
    
    /* Write the results to a logfile */
    
    ofstream logfile1;
    
    logfile1.open( "sample1", ios::out );
    for(nSample=0; nSample < samples.nelements() ; nSample++ ){
      logfile1 << samples(nSample) << endl;
    }
    
    logfile1.close() ;
  } catch (std::string message) {
    std::cerr << "ERROR : " << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Test working with the PPF coefficients ..." << std::endl;
  try {
    
    Vector<Double> ppfcoeff(16384) ;
    Vector<Double> ppfcoeff_inv(16384) ;
    
    try {
      std::cout << "-- Reading in file with PPF coefficients ..." << std::endl;
      readAsciiVector(ppfcoeff,data_ppf_coefficients.c_str());
    } catch (std::string message) {
      std::cout << "-- Error reading in file with PPF coefficients: " << message
		<< std::endl;
      nofFailedTests++;
    }
    
    try {
      std::cout << "-- Reading in file with PPF inversion results ..." << std::endl;
      readAsciiVector(ppfcoeff_inv,data_ppf_inversion.c_str());
    } catch (std::string message) {
      std::cout << "-- Error reading in file with PPF inversion: " << message
		<< std::endl;
      nofFailedTests++;
    }
    
    // ***************************************************************
    
    ppfimplement ppf_impl;
    ppfinversion ppf_inv ;
    ionoCalibration iono_cal ;
    SubbandID band_ID ;
    uint nofrows (508);
    Vector<uint> subBand_IDs(nofrows,0);
    
    try {
      std::cout << "-- Assigning sub-band IDs ..." << std::endl;
      for(uint s=0; s< nofrows; s++ ) {
	subBand_IDs(s)= s+2 ;
      }
    } catch (std::string message) {
      std::cerr << "-- Error assigning sub-band IDs: " << message << std::endl;
    }
    
    Vector<Double> freq_vector_fft( 513,0.0 );
    Vector<Double> freq_vector_shortened( 510,0.0 );
    
    for( uint g=0; g<512; g++ ){
      
      freq_vector_fft(g)= (g+1)*(sampling_frequency/dataBlockSize) ;
      // freq_vector_shortened(g)=g ;
    }
    
    for( uint gs=512; gs <dataBlockSize; gs++ ){
      
      freq_vector_fft(gs)=0.0 ;
      
    }   
    
    FFTServer <Double,DComplex> server ;
    
    Matrix<DComplex> fft_implemented( dataBlockSize/2+1 , nofsegmentation,0.0 ) ;
    
    Matrix<DComplex> refft_implemented( dataBlockSize/2+1 , nofsegmentation,0.0 ) ;
    
    Vector<Double> sliced_vector(dataBlockSize, 0.0) ;
    
    uint sample(0);
    
    for( uint i=0; i< nofsegmentation; i++ ){
      
      sliced_vector = samples( Slice( sample, dataBlockSize ) ) ;
      
      Vector<DComplex> FFTVector( dataBlockSize/2+1, 0.0 ) ;
      
      server.fft( FFTVector, sliced_vector ) ;
      
      fft_implemented.column(i) = FFTVector ;
    
    sample = sample +dataBlockSize ;
    
    }
    
    Vector<DComplex> FFTVector_row ;
    
    Matrix<DComplex> fft_shortened( 513, nofsegmentation,0.0) ;
    
    for( uint q=0; q <513; q++ ){
    
       FFTVector_row = fft_implemented.row(q) ;
    
       fft_shortened.row(q)= FFTVector_row ;
    
    }
    
    Matrix<DComplex> ppfImp_data = ppf_impl.FFTSamples( samples,
							ppfcoeff ) ;
    for( uint qs=512; qs <dataBlockSize; qs++ ){
      
      ppfImp_data.row(qs)=0.0 ;
      
    }
    
    Matrix<DComplex> iono_corrupted = iono_cal.phaseCorrection( ppfImp_data,
								hrAngle,
								declinationAngle,
								geomagLatitude,
								height_ionosphere,
								TEC_value,
								sampling_frequency,
								freq_vector_fft) ;
    
    cout << " Phase corruption has been done :" << endl ;
    
    uint NC = ppfImp_data.ncolumn() ;
    
    Matrix<DComplex> iono_corrupted_final ( nofrows,NC,0.0 )  ;
    
    for( uint ss=0; ss< nofrows ; ss ++ ){
      
      iono_corrupted_final.row(ss)=iono_corrupted.row(ss+2);
      
    }
    
    
    Vector<Double> time_inv = ppf_inv.FIR_inversion( ppfcoeff_inv,
						     iono_corrupted_final ,
						     subBand_IDs ) ;
    
    cout << " ppf inversion has been performed aftre phase corruption : "<< endl ;
    
							 
	ofstream logfile2;
     
        logfile2.open( "Resample", ios::out );
        for( uint sample(0); sample < ( dataBlockSize*nofsegmentation ) ; sample++ ){
             logfile2 << time_inv(sample) << endl;
          }						 
	
	logfile2.close() ;						 
	
      Matrix<DComplex> ppfReImp_data = ppf_impl.FFTSamples( time_inv,
                                                          ppfcoeff ) ;
				
							  			  
   Matrix<DComplex> iono_corrected = iono_cal.phaseRECorrection( ppfReImp_data,
				    		               hrAngle,
				    		               declinationAngle,
				    		               geomagLatitude,
				    		               height_ionosphere,
				    		               TEC_value,
				    		               sampling_frequency,
				    		               freq_vector_fft) ;
       
       cout << " Phase correction has been done :" << endl ; 	
       
	Matrix<DComplex> iono_corrected_final ( nofrows,NC,0.0 )  ;

        for( uint tt=0; tt< nofrows ; tt ++ ){
	
	   iono_corrupted_final.row(tt)=iono_corrupted.row(tt+2);
	   
	   }
     					       
   	       
        Vector<Double> time_reinv = ppf_inv.FIR_inversion( ppfcoeff_inv,
                                                           iono_corrected_final ,
						           subBand_IDs ) ;
							   
    
      uint noelement_s = time_reinv.nelements() ;
      
     
    cout << " second ppf inversion has been performed aftre phase correction : "<< endl ;
    	 cout << "no of elements in time_reinv vector :" << noelement_s <<endl ;
      
     
	ofstream logfile3;
     
        logfile3.open( "ReResample", ios::out );
        for( uint samp(0); samp < samples.nelements() ; samp++ ){
             logfile3 << time_reinv(samp) << endl;
          }
    
    logfile3.close() ;						 
   
    cout << " number of elements in initial time vector :" << samples.nelements() <<endl ;
    
  } catch (AipsError x) {
    cerr << x.getMesg()<< endl;
    nofFailedTests++;
  }
  
  cout <<"finished calculations :" <<endl ;
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_ppfimplements ();

  if(nofFailedTests != 0) {
      cout <<"Error............... early exit " << endl;
  }

  return nofFailedTests;
}
