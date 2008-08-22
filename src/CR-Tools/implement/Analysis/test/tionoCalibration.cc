/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                                     *
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

/* $Id: tionoCalibration.cc,v 1.2 2007/08/08 15:30:04 singh Exp $*/


#include <cmath>
#include <iostream>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/namespace.h>

#include <Analysis/ppfimplement.h>
#include <Analysis/ppfinversion.h>
#include <Analysis/ionoCalibration.h>
#include <Analysis/SubbandID.h>

using CR::ionoCalibration ;
using CR::ppfimplement ;
using CR::ppfinversion ;
using CR::SubbandID ;

/*!
  \file tionoCalibration.cc

  \ingroup Analysis

  \brief A collection of test routines for ionoCalibration
 
  \author Kalpana Singh
 
  \date 2007/06/07
*/
 uint dataBlockSize (1024);
 uint nofsegmentation(16*6) ;
 
 Vector<Double> samples( dataBlockSize*nofsegmentation, 0.0 ) ;
 
 Double hrAngle( 1.034 );
 
 Double declinationAngle( 1.078 );
 
 Double geomagLatitude( 0.915708 ) ;
 
 Double height_ionosphere( 4e5 ) ;
 
 Double TEC_value( 10e16 ) ;
 
 Vector<Double> subband_frequencies( 500, 0.0 );
 
 Double subbandfrequencies[24] ={ 242187500, 243750000, 245312500, 246875000, 248437500, 250000000, 251562500, 253125000, 254687500,
                                  256250000, 257812500, 259375000, 285937500, 287500000, 289062500, 290625000, 291287500, 293750000,
				  295312500, 296875000, 298437500, 300000000, 301562500, 303125000 } ;
 
 Double sampling_frequency( 1600e6 );
// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ionoCalibration object

  \return nofFailedTests -- The number of failed tests.
*/
int test_ionoCalibration ()
{
  int nofFailedTests (0);
  try {
     
     ppfimplement ppf_impl;
     
     ppfinversion ppf_inv ;
     
     SubbandID sbID ;
     
     ionoCalibration iono_cal ;
     }
     
    catch ( AipsError x){
    cerr << "test_ppfimplement :--- testing the default constructor.... " << x.getMesg() << endl;
    }
   return nofFailedTests ;
}

 
 Bool  test_ionoCalibrations ()
 {
   Bool ok(True) ;
   
     try {
    
	//---------------------- to generate Gaussian noise ---------------------------
	
	cout << "testing the MLCG generator " << endl ;
	{
	  ACG gen(1, dataBlockSize*nofsegmentation );
	  
	  for( uint k =0; k < dataBlockSize*nofsegmentation; k++ ){
	       Normal rnd(&gen, 0.0, 0.1 );
	       Double nextExpRand = rnd() ;
	       samples(k) = nextExpRand ;
	       }
	  gen.reset () ;
	  // resetting the generator, Should get the same numbers
	  }
	samples( 16*1024*3+10 ) = 10.0 ;
	
	ofstream logfile1;
     
        logfile1.open( "sample", ios::out );
        for( uint sample(0); sample < (dataBlockSize*nofsegmentation ) ; sample++ ){
             logfile1 << samples(sample) << endl;
          }
                     	

// setting filter coefficients and its inverse ...............

	Vector<Double> ppfcoeff(16384) ;
	
	readAsciiVector (ppfcoeff,"Coeffs16384Kaiser-quant.dat");
	
	Vector<Double> ppfcoeff_inv(16384) ;
	
	readAsciiVector (ppfcoeff_inv,"ppf_inv.dat");
	
// making object of the classes for ppf implementation and inversion ..........
	
	ppfimplement ppf_impl;
	
	ppfinversion ppf_inv ;
	
// generating subbandiDs to make complete array of ppf implemented data ...........	

   	
   Matrix<DComplex> ppf_data_first =  ppf_impl.FFTSamples( samples,
                                                          ppfcoeff );
				    
  
    // to check for the ionospheric dispersion ----------------------------------------

  ionoCalibration iono_cal ;
  
  subband_frequencies(0) = 1562500. ;
  
   for( uint t=1; t<500; t++ ){
      subband_frequencies(t) = (sampling_frequency/1024.) +subband_frequencies(t-1) ;
      }
    cout << " subband frequencies :" << subband_frequencies << endl;
    
     SubbandID sbID ;
     
    Vector<uint> subband_IDs = sbID.calcbandIDVector( sampling_frequency,
	                                               subband_frequencies ) ;
						       
   cout << " write subband ID s "<< subband_IDs << endl ;
 
  Matrix<DComplex>FFT_data_mid( 500, nofsegmentation, 0.0 );
  
  for( uint r=0; r<500; r++ ){
    
    uint which_row = subband_IDs(r) ;
    
    FFT_data_mid.row(r) = ppf_data_first.row(which_row) ;
  }
  
  Matrix<DComplex> FFT_phase_dispersed = iono_cal.phaseRECorrection( FFT_data_mid,
								     hrAngle,
								     declinationAngle,
								     geomagLatitude,
								     height_ionosphere,
								     TEC_value,
								     sampling_frequency,
								     subband_frequencies )   ;
  
  
  
//   Vector<Double> output_mid = ppf_inv.FIR_inversion( ppfcoeff_inv,
// 						     FFT_phase_dispersed );	
//   
//   ofstream logfile2;
//   
//   logfile2.open( "output_mid", ios::out );
//   for( uint f(0); f < ( dataBlockSize*nofsegmentation ) ; f++ ){
//     logfile2 << output_mid(f) << endl;
//   }	
//   
/*  Matrix<DComplex> ppf_data_second =  ppf_impl.FFTSamples( output_mid,
							   ppfcoeff );	  
  
  
  
  Matrix<DComplex>FFT_data_final( 500, nofsegmentation, 0.0 );
  
  for( uint r=0; r<500; r++ ){
    
    uint which_row = subband_IDs(r) ;
    
    FFT_data_final.row(r) = ppf_data_second.row(which_row) ;
  }
  
  cout << " developed FFT_data according to the subband frequency : " << endl ;
  Matrix<DComplex> FFT_phase_corrected = iono_cal.phaseCorrection( FFT_data_final,
								   hrAngle,
								   declinationAngle,
								   geomagLatitude,
								   height_ionosphere,
								   TEC_value,
								   sampling_frequency,
								   subband_frequencies )   ;
  
  uint nofrows = FFT_phase_corrected.nrow() ;
  cout << "number of generated in phase corrected FFT array : " << nofrows<< endl;		     */
//   Vector<Double> output_dispersion = ppf_inv.FIR_inversion( ppfcoeff_inv,
// 							    FFT_phase_corrected );
//   
/*  
  ofstream logfile3;
  
  logfile3.open( "output_dispersion", ios::out );
  for( uint f(0); f < ( dataBlockSize*nofsegmentation ) ; f++ ){
    logfile3 << output_dispersion(f) << endl;
  }*/	 							         
  
     } catch (AipsError x) {
       cerr << x.getMesg()<< endl;
       ok = False;
     }
     
     return ok;
 }
// -----------------------------------------------------------------------------
int main ()
{
  Bool ok(True);
  
  Int retval(0) ;
  if(ok) {
    ok= test_ionoCalibrations ();
    if(!ok){
      retval = 1;
      cout <<"Error............... early exit " << endl;
    }
  }
  return retval;
}
