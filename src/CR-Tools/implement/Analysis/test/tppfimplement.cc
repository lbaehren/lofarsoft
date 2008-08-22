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
 
 Double TEC_value( 10e17 ) ;
 
 Vector<Double> subband_frequencies( 512, 0.0 );
 
 Double sampling_frequency( 200e6 );
 
 int test_ppfimplement ()
{
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
  

 Bool  test_ppfimplements ()
 {
   Bool ok(True) ;
   
     try {

     
	//---------------------- to generate Gaussian noise ---------------------------
	
	cout << "testing the MLCG generator " << endl ;
	{
	  ACG gen(1, dataBlockSize*nofsegmentation );
	  
	  for( uint k =0; k < dataBlockSize*nofsegmentation; k++ ){
	//       Normal rnd(&gen, 0.0, 0.1 );
         //      Double nextExpRand = rnd() ;
	       samples(k) = 0.0 ; //nextExpRand ;
	       }
	  gen.reset () ;
	  // resetting the generator, Should get the same numbers
	  }
	samples( 16*1024*3+10 ) = 10.0 ;
	//samples( 16*1024*3+50 ) = 15.0 ;
	//samples( 16*1024*3+90 ) = 15.0 ;	
        cout << " samples with a peak has been saved " << endl ;
	
	ofstream logfile1;
     
        logfile1.open( "sample1", ios::out );
        for( uint sample(0); sample < (dataBlockSize*nofsegmentation ) ; sample++ ){
             logfile1 << samples(sample) << endl;
          }
        
	 

// setting filter coefficients and its inverse ...............

	Vector<Double> ppfcoeff(16384) ;
	
	readAsciiVector(ppfcoeff,"Coeffs16384Kaiser-quant.dat");
	
	Vector<Double> ppfcoeff_inv(16384) ;
	
	readAsciiVector(ppfcoeff_inv,"ppf_inv.dat");
	
// making object of the classes for ppf implementation and inversion ..........
	
	ppfimplement ppf_impl;
	
	ppfinversion ppf_inv ;
	
	ionoCalibration iono_cal ;
	
	SubbandID band_ID ;
	
	
// generating subbandiDs to make complete array of ppf implemented data ...........	

//***************************************************************************
/*
 uint filterRows(4) ;
 
 Vector<DComplex> input(4.0);
 
 input(1)= 1.0 ;
   
 Matrix<DComplex> IDFT = ppf_impl.setIDFTMatrix ( filterRows ) ;
  
  cout << " IDFT matrix : " << IDFT << endl ;
  
  Vector<DComplex> idft(4.0, 0.0) ;
  
  Vector<Double> dft( 4.0, 0.0 ) ;
    
  for( uint s= 0 ; s < filterRows ; s++ ){

        idft(s) = ( sum ( IDFT.row(s)*input ) ) ;
   
   }
  
  cout << " idft vector :" << idft << endl ;
  
 Matrix<DComplex> DFT = ppf_inv.setDFTMatrix( filterRows ) ;
 
 cout << " DFT matrix : " << DFT << endl ; 
 
 for( uint t= 0 ; t < filterRows ; t++ ){

        dft(t) = real( sum ( DFT.row(t)*idft ) ) ;
   
   }
  
  cout << " dft vector :" << dft << endl ;
 
 Matrix <Double> product(filterRows,filterRows,0.0);
  
 for( uint c= 0; c < filterRows; c++ ){
 
               for( uint r= 0 ; r < filterRows ; r++ ){

                product(c,r) = real( sum ( DFT.row(c)*IDFT.column(r)) ) ;

	          }
             }
 cout << " multiplied matrix of DFT and IDFT :"<< product <<endl ;
 */
 //***************************************************************
/*
 Matrix<Double> FIRCoefficientArray = ppf_impl.setFilterCoefficients ( ppfcoeff ) ;
 
 Matrix<Double> firimplemented = ppf_impl.FIRimplementation( samples,
					    FIRCoefficientArray ) ;
 
 Matrix<DComplex> IDFT = ppf_impl.setIDFTMatrix ( 1024 ) ;
 
 Matrix<DComplex> DFT = ppf_inv.setDFTMatrix( 1024 ) ;
 
 Vector<Double> firimp_vector = firimplemented.column(20) ;
 
 Vector<DComplex> firimp_complex( 1024 ) ;

 convertArray( firimp_complex, firimp_vector ) ;
 
 ofstream logfile2;
     
        logfile2.open( "firimp", ios::out );
        for( uint f(0); f < ( 1024 ) ; f++ ){
             logfile2 << firimp_vector(f) << endl;
          }	
	  
 Vector<DComplex> fftvector( 1024,0.0 ) ;
 
 Vector<Double> ifftvector( 1024, 0.0 ) ;
 
 for( uint c= 0; c < 1024; c++ ){
 
        fftvector(c) = ( sum ( IDFT.row(c)*firimp_complex) ) ;
   }
 
 ofstream logfile3;
     
        logfile3.open( "fftvector", ios::out );
        for( uint g(0); g < ( 1024 ) ; g++ ){
             logfile3 << fftvector(g) << endl;
          }	
	  
 for( uint t= 0; t < 1024; t++ ){
 
        ifftvector(t) = real( sum ( DFT.row(t)*fftvector) ) ;
   }

ofstream logfile4;
     
        logfile4.open( "ifftvector", ios::out );
        for( uint h(0); h < ( 1024 ) ; h++ ){
             logfile4 << ifftvector(h) << endl;
          }
*/	  
 //*****************************************************************   

 uint nofrows = 508 ;

   Vector<uint> subBand_IDs(nofrows,0);
   
   for(uint s=0; s< nofrows; s++ ){
   
           subBand_IDs(s)= s+2 ;

 }
  
  Vector<Double> freq_vector_fft( 513,0.0 );
  
  for( uint g=0; g<513; g++ ){
  
       freq_vector_fft(g)= (g+1)*(sampling_frequency/1024) ;
       
       }
       
  FFTServer <Double,DComplex> server ;
  
  Matrix<DComplex> fft_implemented(dataBlockSize/2+1 , nofsegmentation,0.0 ) ;
  
  Vector<Double> sliced_vector(1024, 0.0) ;
  
  uint sample(0);
	             
  for( uint i=0; i< nofsegmentation; i++ ){
	       
    sliced_vector = samples( Slice( sample, dataBlockSize ) ) ;
	      
    Vector<DComplex> FFTVector( dataBlockSize/2+1, 0.0 ) ;
	      
//     uint element = FFTVector.nelements();
	      
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
    
    cout << " before ionospheric corrupted data " << fft_shortened.row(3) << endl ;
    
   Matrix<DComplex> iono_corrupted = iono_cal.phaseCorrection( fft_shortened,
				    		               hrAngle,
				    		               declinationAngle,
				    		               geomagLatitude,
				    		               height_ionosphere,
				    		               TEC_value,
				    		               sampling_frequency,
				    		               freq_vector_fft) ;
 
   cout << " ionospheric corrupted data " << iono_corrupted.row(3) << endl ;       
							       
   uint N_col = iono_corrupted.ncolumn() ;
   
   Vector<DComplex> iono_corr(dataBlockSize/2+1, 0.0);
   
   Vector<Double> IFFTVector( dataBlockSize, 0.0 );
   
   Vector<Double> timeSeries( dataBlockSize*nofsegmentation, 0.0 ) ;
   
   for( uint k=0; k< N_col; k++ ) {
   
      iono_corr = iono_corrupted.column(k);
      
      server.fft( IFFTVector, iono_corr ) ;
  
      for( uint a=0; a<dataBlockSize; a++){
      
          timeSeries(k*1024+a)= IFFTVector(a);
	  
	  }
	  
    }
    
     ofstream logfile2;
     
        logfile2.open( "sample2", ios::out );
        for( uint sample(0); sample < (dataBlockSize*nofsegmentation ) ; sample++ ){
             logfile2 << timeSeries(sample) << endl;
          }
	
  
   Matrix<DComplex> ppfImp_data = ppf_impl.FFTSamples( timeSeries,
                                                       ppfcoeff ) ;
        
        uint nc = ppfImp_data.ncolumn() ;
	
	uint nOfRows = nofrows ;
	
	Matrix<DComplex> ppfimp_data(nOfRows,nc,0.0) ;
	
	for(uint t=0; t<(nOfRows); t++) {
	
	  ppfimp_data.row(t) = ppfImp_data.row(2+t) ;
	  
	  }
        uint NR = ppfimp_data.nrow() ;
	cout << " number of rows in pff implemented data :"  << NR <<endl ; 
		
	uint NC = ppfimp_data.ncolumn() ;
	cout << " number of columns in ppf implemented  data :" << NC <<endl ;
	
	Matrix<Double> absppf_impdata = amplitude(ppfimp_data) ;
	
	Matrix<Double> transposed_abs( NC, NR, 0.0) ;
	
	for( uint s(0); s<NC; s++ ){
    
            transposed_abs.row(s)=absppf_impdata.column(s) ;
	    
	    }
	    		
	
	 ofstream logfile3;
     
        logfile3.open( "sample3", ios::out );
        
	for( uint r(0); r < NC ; r++ ){
	   for( uint c(0); c< NR; c++ ) {
	        logfile3 << transposed_abs(r,c) 
		         << "\t" ;
		}
	   logfile3 << endl ;
	 }

	Matrix<DComplex> divided_matrix(nOfRows,nc,0.0);
	
	Vector<DComplex> ppfimp_vector(nc) ;
	Vector<Double> absolute_vector(nc) ;
	Vector<DComplex> divided_vector(nc) ;
	
	DComplex ppfimp_sample(0.,0.) ;
	Double absolute_sample(0.) ;
	DComplex divided_sample(0.,0.) ;
	DComplex comp(0,1) ;
	
	for(uint i=0; i<nOfRows; i++){
	    ppfimp_vector = ppfimp_data.row(i) ;
	    
	   absolute_vector = absppf_impdata.row(i);
	    for(uint j=0; j< nc; j++){
	   	
		ppfimp_sample= ppfimp_vector(j) ;
		
	   	absolute_sample = absolute_vector(j) ;
		Double absolute_value = 1./absolute_sample ;
		
	       	divided_sample = absolute_value*ppfimp_sample ;
		divided_vector(j)= divided_sample ;
	       
	       }
	      divided_matrix.row(i)= divided_vector ;
	       
	 }
	 

  Vector<Double> freqVector = band_ID.calcFreqVector ( sampling_frequency,
                                                       subBand_IDs ) ;
				  
   uint n_Elements = freqVector.nelements() ;
   cout << " Number of elements in frequency vector : " << n_Elements <<endl ;
   
   Matrix<DComplex> iono_corrected = iono_cal.phaseRECorrection( ppfimp_data,
                                                                 hrAngle,
                                                                 declinationAngle,
                                                                 geomagLatitude,
                                                                 height_ionosphere,
		                                                 TEC_value,
		                                                 sampling_frequency,
						                 freqVector )  ;
   

    uint cn =iono_corrected.ncolumn() ;
    cout << " number of columns in ionospheric corrected :" << cn <<endl ;
    
    uint rn =iono_corrected.nrow() ; 
    cout << " number of rows in ionospehric corrected : "<<rn <<endl ;
    
   Vector<Double> output_inv = ppf_inv.FIR_inversion( ppfcoeff_inv,
                                                      iono_corrected,
						      subBand_IDs ) ;
 
     ofstream logfile4;
     
        logfile4.open( "sample4", ios::out );
        for( uint f(0); f < ( dataBlockSize*nofsegmentation ) ; f++ ){
             logfile4 << output_inv(f) << endl;
          }	
	  
 /*   Vector<Double> output_divided = ppf_inv.FIR_inversion( ppfcoeff_inv,
                                                          divided_matrix,
						          subBand_IDs ) ;
       ofstream logfile3;
     
        logfile3.open( "output2", ios::out );
        for( uint f(0); f < ( dataBlockSize*nofsegmentation ) ; f++ ){
             logfile3 << output_divided(f) << endl;
          }	
  
    						 
  
    
    Vector<uint> subBandVector (300,0) ;
    
     for( uint p=0; p< 300; p++ ){
     
         subBandVector(p) = 200+p ;
	 }
	 
      Vector<Double> FFT_vector = ppf_inv.simple_fft( samples,
                                                      1024*16 ) ;
 
      ofstream logfile4;
     
        logfile4.open( "output3", ios::out );
        for( uint f(0); f < ( dataBlockSize*nofsegmentation ) ; f++ ){
             logfile4 << FFT_vector(f) << endl;
          }							
     
     */
	  
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
