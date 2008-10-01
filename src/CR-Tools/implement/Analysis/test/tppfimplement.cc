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
#include <scimath/Mathematics/FFTServer.h>

#include <Data/rawSubbandIn.h>
#include <Data/tbbctlIn.h>
#include <Analysis/SubbandID.h>
#include <Analysis/ppfimplement.h>
#include <Analysis/ppfinversion.h>
#include <Analysis/ionoCalibration.h>

using namespace CR ;


using CR::ppfimplement ;
using CR::ppfinversion ;
using CR::ionoCalibration ;
using CR::SubbandID ;
using CR::tbbctlIn ;
using CR::rawSubbandIn ;

/*!
  \file tppfimplement.cc

  \ingroup Analysis

  \brief A collection of test routines for ppfimplement
 
  \author Kalpana Singh
 
  \date 2007/06/01
*/

// -----------------------------------------------------------------------------
//  Global variables used throughout the program

const uint dataBlockSize        = 1024;
const uint nofsegmentation      = 16*10;
Vector<Double> samples( dataBlockSize*nofsegmentation, 0.0 ) ;
const Double hrAngle            = 1.034;
const Double declinationAngle   = 1.078;
const Double geomagLatitude     = 0.915708;
const Double height_ionosphere  = 4e5;
const Double TEC_value          = 10e15;
Vector<Double> subband_frequencies( 512, 0.0 );
const Double sampling_frequency = 200e6;

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ppfimplement object
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_ppfimplement ()
{
  std::cout << "\n[test_ppfimplement]\n" << std::endl;
  
  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    ppfimplement ppf_impl;
    ppfinversion ppf_inv ;
    ionoCalibration iono_cal ;
    SubbandID band_ID ;
    tbbctlIn newtbbctlIn ;
    rawSubbandIn newrawSubbandIn ;  
  } catch ( AipsError x){
    cerr << x.getMesg() << endl;
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
  int nofFailedTests (0);
  
  try {
    std::cout << "\n[test_ppfimplements]\n" << std::endl;
    
    //  int nofFailedTests (0);
    
    /* Provide some feedback on the variables used */
    std::cout << "-- Data block size   = " << dataBlockSize      << std::endl;
    std::cout << "-- nof. segments     = " << nofsegmentation    << std::endl;
    std::cout << "-- Vector of samples = " << samples.shape()    << std::endl;
    std::cout << "-- Sample frequency  = " << sampling_frequency << std::endl;
    
    
    ppfimplement ppf_impl;
    ppfinversion ppf_inv ;
    ionoCalibration iono_cal ;
    SubbandID band_ID ;
    tbbctlIn newtbbctlIn ;
    rawSubbandIn newrawSubbandIn ;  
    
    /*
     *  Gaussian (random) noise generation
     */
    
    cout << "[1] Testing the MLCG generator ..." << endl ;
    
    uint nSample (0);
    ACG gen(1, samples.nelements() );
    
    for(nSample=0; nSample < samples.nelements(); nSample++ ){
      //       Normal rnd(&gen, 0.0, 0.1 );
      //      Double nextExpRand = rnd() ;
      samples(nSample) = 0.0 ; //nextExpRand ;
    }
    // Resetting the generator, should get the same numbers.
    gen.reset () ;
    
    samples( 16*dataBlockSize*3+400 ) = 10.0 ;
    cout << "-- samples with a peak has been saved " << endl ;
    
    
    /*
     * Calling TBB data for Nyquist sampling
     */

    //     int nofFailedTests (0);
    //     
    //    Vector<String> filenames(1) ;
    //     filenames(0) = "/mnt/lofar/kalpana/rw_20080306_141000_2301.dat" ;
    //     
    //     std::cout << "[2] Testing attaching a file... " << std::endl ;
    //     
    //     cout << "[3]Testing retrieval of fx() data....." << endl ;
    // 	
    // 	if(!newtbbctlIn.attachFile(filenames)) {
    // 	  cout << " Failed to attach file(s)!" <<endl ;
    // 	  nofFailedTests++ ;
    // 	  return nofFailedTests ;
    // 	  } ;
    // 	
    // 	Matrix<Double> data ;
    // 	data = newtbbctlIn.fx() ;
    // 	uint rows_data = data.nrow() ;
    // 	cout << "number of rows of real raw data : " <<  rows_data << endl ;
    // 	uint columns_data = data.ncolumn() ;
    // 	cout << "number of columns of real raw data : " << columns_data<< endl ;
    //     
    //     Vector<Double> data_input = data.column(0) ;
    // 	uint ndatasamples_raw = data_input.nelements() ;
    // 	
    // 	cout << " number of elements in the raw vector samples :" << ndatasamples_raw << endl ;
    // 	
    // 	Vector<Double> chopped_data(163840) ;
    // 	chopped_data = data_input(Slice(0,163840)) ;
    // 	
    // 	chopped_data(16384*6+500)=1000 ;
    // 	
    // 	uint n_chopped_data= chopped_data.nelements() ;
    // 	cout << "number of elements in the chopped data vector: " << n_chopped_data <<endl ;
    
    ofstream logfile1;
    
    logfile1.open( "sample1", ios::out );
    for( uint nSample=0; nSample < samples.nelements() ; nSample++ ){
      logfile1 << samples(nSample) << endl;
    }
    
    logfile1.close() ;
    
    
    Vector<Double> ppfcoeff(16384) ;
    Vector<Double> ppfcoeff_inv(16384) ;
    
    readAsciiVector(ppfcoeff,data_ppf_coefficients.c_str());
    
    readAsciiVector(ppfcoeff_inv,data_ppf_inversion.c_str());
    
    
    //***************************************************************
    
   
    uint nofrows (1024);
    Vector<uint> subBand_IDs(nofrows,0);
    
    for(uint s=0; s< nofrows; s++ ) {
	subBand_IDs(s)= s+1 ;
      }
        
//******************* FOR SIMPLE FFT *******************************
//******************************************************************
//************** Ionospheric Corruption ****************************
//******************************************************************
//******************** IFFT to get back time series ****************

    Vector<Double> freq_vector_fft( 513,0.0 );

    Vector<Double> freq_vector_shortened( 510,0.0 );

    
    for( uint g=0; g<513; g++ ){
      
      freq_vector_fft(g)= (g+1)*(sampling_frequency/dataBlockSize) ;
      // freq_vector_shortened(g)=g ;
    }
    
    FFTServer <Double,DComplex> server ;
    
    Matrix<DComplex> fft_implemented( dataBlockSize/2+1 , nofsegmentation,0.0 ) ;
    
    Vector<Double> sliced_vector(dataBlockSize, 0.0) ;
    
    uint sample(0);
    
    for( uint i=0; i< nofsegmentation; i++ ){
      
      sliced_vector = samples( Slice( sample, dataBlockSize ) ) ;
      
      Vector<DComplex> FFTVector( dataBlockSize/2+1, 0.0 ) ;
      
      server.fft( FFTVector, sliced_vector ) ;
      
      fft_implemented.column(i) = FFTVector ;
    
    sample = sample +dataBlockSize ;
    
    }
    
    Matrix<DComplex> iono_corrupted = iono_cal.phaseCorrection( fft_implemented,
                                                                hrAngle,
								declinationAngle,
								geomagLatitude,
								height_ionosphere,
								TEC_value,
								sampling_frequency,
								freq_vector_fft );
								
    uint N_col = iono_corrupted.ncolumn() ;
    
    Vector<DComplex> iono_corr(dataBlockSize/2+1, 0.0 ) ;
    
    Vector<Double> IFFTVector( dataBlockSize, 0.0 );
    
    Vector<Double> timeSeries( dataBlockSize*nofsegmentation, 0.0 ) ;
    
    for( uint k=0; k< N_col; k++ ) {
    
          iono_corr = iono_corrupted.column(k) ;
	  
	  server.fft(IFFTVector, iono_corr ) ;
	  
	  for( uint a=0; a< dataBlockSize; a++ ) {
	  
	       timeSeries(k*1024+a)= IFFTVector(a) ;
	       
	       }
	       
        }
	
	
      	ofstream logfile2;
     
        logfile2.open( "Resample", ios::out );
        for( uint sample(0); sample < ( dataBlockSize*nofsegmentation ) ; sample++ ){
             logfile2 << timeSeries(sample) << endl;
          }						 
	
	logfile2.close() ; 
  //**************** FOR Polyphase Filter FFT ********************
  //**************************************************************
  //**************** Ionospheric Corruption **********************
  //***************** ********************************************
  //*** Polyphase filter inversion to get back time series *******
  
//     Matrix<DComplex> ppfImp_data = ppf_impl.FFTSamples( chopped_data,
// 							ppfcoeff ) ;
// 							
//    uint nc = ppfImp_data.ncolumn() ;
//    //nofrows
//    
//    Matrix<DComplex> ppfimp_data(nofrows,nc,0.0 ) ;
//    
//    for( uint t=0; t< nofrows; t++ ){
//    
//       ppfimp_data.row(t)= ppfImp_data.row(2+t) ;
//     }
//     
   Vector<Double> freqVector = band_ID.calcFreqVector( sampling_frequency,
   						       subBand_IDs );
// 						              
//     Matrix<DComplex> iono_corrupted = iono_cal.phaseCorrection( ppfimp_data,
// 								hrAngle,
// 								declinationAngle,
// 								geomagLatitude,
// 								height_ionosphere,
// 								TEC_value,
// 								sampling_frequency,
// 								freqVector ) ;
//     
//     cout << " Phase corruption has been done :" << endl ;
//     
//    Vector<Double> time_inv = ppf_inv.FIR_inversion( ppfcoeff_inv,
// 						    iono_corrupted ,
// 						    subBand_IDs ) ;
//     
//     cout << " ppf inversion has been performed aftre phase corruption : "<< endl ;
//     
// 							 
// 	ofstream logfile2;
//      
//         logfile2.open( "Resample", ios::out );
//         for( uint sample(0); sample < ( dataBlockSize*nofsegmentation ) ; sample++ ){
//              logfile2 << time_inv(sample) << endl;
//           }						 
// 	
// 	logfile2.close() ;						 
// 	
	
 //***************** for Polyphase filter inversion ************************
 //*************************************************************************
 //******************* Ionospheric Correction ******************************
 //**************************************************************************
 //**** Polyphase filter inversion to get back time series*******************
 
      Matrix<DComplex> ppfReImp_data = ppf_impl.FFTSamples( samples,
                                                            ppfcoeff ) ;
				
	 uint ncRe = ppfReImp_data.ncolumn() ;
	 
	 cout << " number of columns in ppf implemented data :" << ncRe <<endl;
	 
   //nofrows
   
//    Matrix<DComplex> ppfReimp_data(nofrows,ncRe,0.0 ) ;
//    
//    for( uint t=0; t< nofrows; t++ ){
//    
//       ppfReimp_data.row(t)= ppfReImp_data.row(2+t) ;
//     }
//     						  			  
//    Matrix<DComplex> iono_corrected = iono_cal.phaseRECorrection( ppfReimp_data,
// 				    		               hrAngle,
// 				    		               declinationAngle,
// 				    		               geomagLatitude,
// 				    		               height_ionosphere,
// 				    		               TEC_value,
// 				    		               sampling_frequency,
// 				    		               freqVector ) ;
       
       cout << " Phase correction has been done :" << endl ; 	
       

        Vector<Double> time_reinv = ppf_inv.FIR_inversion( ppfcoeff_inv,
                                                           ppfReImp_data ,
						           subBand_IDs ) ;
							   
    	ofstream logfile3;
     
        logfile3.open( "ReResample", ios::out );
        for( uint samp(0); samp < samples.nelements() ; samp++ ){
             logfile3 << time_reinv(samp) << endl;
          }
    
    logfile3.close() ;						 
   
    cout << " number of elements in initial time vector :" << samples.nelements()
	 <<endl ;
    
  } catch (AipsError x) {
    cerr << x.getMesg()<< endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_ppfimplements ();

  return nofFailedTests;
}
