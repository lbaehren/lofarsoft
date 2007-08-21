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

/* $Id: tppfimplement.cc,v 1.6 2007/08/08 15:30:04 singh Exp $*/

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

using CR::ppfimplement ;
using CR::ppfinversion ;
using CR::ionoCalibration ;

/*!
  \file tppfimplement.cc

  \ingroup Analysis

  \brief A collection of test routines for ppfimplement
 
  \author Kalpana Singh
 
  \date 2007/06/01
*/

// -----------------------------------------------------------------------------
/*!
  \brief Test constructors for a new ppfimplement object

  \return nofFailedTests -- The number of failed tests.
*/
 uint dataBlockSize (1024);
 uint nofsegmentation(16*6) ;
 
 Vector<Double> samples( dataBlockSize*nofsegmentation, 0.0 ) ;
 
 Double hrAngle( 1.034 );
 
 Double declinationAngle( 1.078 );
 
 Double geomagLatitude( 0.915708 ) ;
 
 Double height_ionosphere( 4e5 ) ;
 
 Double TEC_value( 10e16 ) ;
 
 Vector<Double> subband_frequencies( 512, 0.0 );
 
 Double sampling_frequency( 260e6 );
 
 int test_ppfimplement ()
{
  int nofFailedTests (0);
  try {
     
     ppfimplement ppf_impl;
     
     ppfinversion ppf_inv ;
     
     ionoCalibration iono_cal ;
     }
     
    catch ( AipsError x){
    cerr << "test_ppfimplement :--- testing the default constructor.... " << x.getMesg() << endl;
    }
   return nofFailedTests ;
}
  

 Bool  test_ppfimplements ()
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
	samples( 16*1024*3+20 ) = 15.0 ;
	samples( 16*1024*3+30 ) = 5.0 ;	
        
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

   	
   Matrix<DComplex> ppf_data =  ppf_impl.FFTSamples( samples,
                                                     ppfcoeff );

   Vector<Double> output = ppf_inv.FIR_inversion( ppfcoeff_inv,
                                                 ppf_data );	
						 
    ofstream logfile2;
     
        logfile2.open( "output", ios::out );
        for( uint f(0); f < ( dataBlockSize*nofsegmentation ) ; f++ ){
             logfile2 << output(f) << endl;
          }					    
       
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
  ok= test_ppfimplements ();
  if(!ok){
  retval = 1;
  cout <<"Error............... early exit " << endl;
  }
 }
 return retval;
}
