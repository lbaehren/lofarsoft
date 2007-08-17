/***************************************************************************
 *   Copyright (C) 2006                                                  *
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

/* $Id: tinverseFFT.cc,v 1.4 2007/04/03 14:03:07 bahren Exp $*/

#include <fstream>
#include <iostream>

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>

#include <templates.h>
#include <Analysis/inverseFFT.h>
#include <Data/ITSCapture.h>
#include <Data/LopesEvent.h>
#include <IO/DataReader.h>

using casa::DComplex;
using casa::Matrix;
using casa::String;
using casa::Vector;
using CR::inverseFFT;

/*!
  \file tinverseFFT.cc

  \ingroup Analysis

  \brief A collection of test routines for inverseFFT
 
  \author Kalpana Singh
 
  \date 2006/12/19
*/

// -----------------------------------------------------------------------------

uint dataBlockSize (1024) ;

double pi = 3.14159265 ;

/*!
  \brief Get a vector with the names of the test data files to work with
*/
Vector<String> filenamesVector ()
{
  uint nofFiles (1);
  Vector<String> files(nofFiles);
  
  files(0) = "/data/LOPES/cr/sel/2004.02.10.01:48:14.776.event" ;
  
  //2004.01.12.00:28:11.577.event" ;
  return files ;
}


/*!
  \brief Test constructors for a new inverseFFT object

  \return nofFailedTests -- The number of failed tests.
*/

int test_inverseFFT ()
{
  int nofFailedTests (0);
  
  cout << "[1] Testing default constructor..." << endl;
  try {
    inverseFFT inft;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }
  
  return nofFailedTests;
}


Bool test_inverseFFTS ()
  
{
  Bool ok (True) ;
  ifstream infile;
  
  try{
    
    Vector<double> samples(dataBlockSize*16*4 ) ;
    
    for( uint t = 0; t < dataBlockSize*16*4;  t++ ){
      
      if ( t <= dataBlockSize*16*2+500 )
	
	{ samples (t) = 0.0 ; }
      
      else if(t == dataBlockSize*16*2+601)
	{     samples(t) = 1.0 ; }
      else
	{     samples(t) = 0.0 ; }
      
    } 
    //   cout << "sampled value :" <<  samples <<  endl ;   
        
    int s (0);
    double filterCoefficient (0.0);
    Vector<double> ppf_coeff(16384) ;
    
    infile.open("Coeffs16384Kaiser-quant.dat", ios::in ) ;
    
    if(!infile) {
      cerr<< "Error:file could not be opened "<< endl;
      exit(1) ;
    }
    
    while(!infile.eof() ) { 
      // read element from file ...
      infile >> filterCoefficient ;
      // ... and append it to the vector collecting the values
      ppf_coeff(s) = filterCoefficient ;
      s++;
    }
    
    infile.close () ;
    
    //return 0;
    uint nofelements = samples.nelements();
    
    uint nofcolumns = nofelements / 1024 ;
    
    cout << " number of columns in the sampled matrix :" << nofcolumns << endl ;
    
    uint nofrows = nofelements / nofcolumns ;
    
    Vector<double> blockSamples (dataBlockSize);
    
    Matrix<double> sampledValueMatrix( dataBlockSize, nofcolumns );
    
    uint m =0 ;
    
    
    for (uint j= 0; j < nofcolumns; j++ ) {
      
      for( uint i = 0; i < nofrows ; i++ ){
	
	//cout << "Element : " << m << " sampled value  :" << samples(m) << endl ; 
	
	sampledValueMatrix( i, j) = samples (m);
	
	m = m+1 ;   
      }
    }     
    ofstream logfile1;
    
    logfile1.open("sampledata",ios::out);
    //   	
    
    for (uInt sample(0); sample< nofrows; sample++) {
      for (uInt antenna(0); antenna< nofcolumns; antenna++) {
	logfile1 << sampledValueMatrix (sample, antenna) 
		 << " \t";
      }
      logfile1 << endl;
      
    }
    
    
    Vector<double> multipliedVector ( dataBlockSize, 0.0);
    
    Matrix<double> output( dataBlockSize, nofcolumns, 0.0 );
    
    uint r = 15;
    
    for(uint p =0 ; p<16; p++ ) {
      
      output.column(p) = 0.0 ; }
    
    uint p = 16 ;
    
    while( r < (nofcolumns-1)){
      
      for(uint k= 0; k < 16; k++ ) {
	
	multipliedVector = multipliedVector + sampledValueMatrix.column(r-k)*ppf_coeff(k) ;
	
	cout << " Data Block Set : " << (r-k) << endl;
	
	//  cout << " Column of Polyphase Filter Matrix : " << k  << endl ;
	
	//  cout<<" Multiplied Vector Components : " << multipliedVector << endl ;
	
      }    
      
      
      output.column(p) = multipliedVector ;
      
      cout << " Column of the output Matrix :" << (p) << endl ;
      
      p = p +1 ;
      
      multipliedVector =0.0;
      
      r = r +1 ;
      
      cout << " No. of column : " << r << endl ;
      
    }
    
    uint nofColumn = output.ncolumn () ;
    
    uint nofRow = output.nrow () ;
    
    cout << "No of columns of output matrix: " << nofColumn << " and No of rows of output matrix: " << nofRow << endl ;
    
    
     ofstream logfile2;

   logfile2.open("FIRimplementeddata",ios::out);
//   	
   
   for (uInt sample(0); sample< nofRow; sample++) {
    for (uInt antenna(0); antenna< nofColumn; antenna++) {
          logfile2 << output (sample, antenna) 
  	        << " \t";
      }
      logfile2 << endl;
      
   }
//   Int size = ppf_coeff.nelements () ;
//   
//   cout << "no of elements :"<< size << endl ;
// 
//   for (int r = 0; r < size+1; r++){
//    cout << "Coefficient in line :" << ppf_coeff(r) << endl;
//    cout << " line index:" << r+1 << endl ;
//  }

  //inverseFFT invft ;
  
   
  //******************************************** FFT of raw data**************************************
  
  
   uint NOfColumn = sampledValueMatrix.ncolumn () ;
    
    uint NOfRow = sampledValueMatrix.nrow () ;
    
    cout << "No of columns of sampledValueMatrix matrix: " << NOfColumn 
         << " and No of rows of sampledValueMatrix matrix: " << NOfRow << endl ;
    
  Matrix<DComplex> FFTRawMatrix (NOfRow/2+1,NOfColumn ) ;
  
   FFTServer<double,DComplex> server;
   
   Vector<DComplex> FFTRawVector(NOfRow/2+1);
    
   for (uint f =0 ; f < NOfColumn; f++ ) {
   
   server.fft( FFTRawVector,sampledValueMatrix.column(f)) ;
   
  // cout << "No. of column : " << f+1 << endl ;
   
 //  cout << " raw data vector send for FFT : " << sampledValueMatrix.column(f) << endl ;
      
   FFTRawMatrix.column(f) = FFTRawVector ;
   
 //  cout << " FFT Vector :" <<  FFTRawMatrix.column(f) << endl ;
   
   }
   
  // cout << " FFT Matrix : " << FFTMatrix << endl ;
   
   ofstream logfile3;

   logfile3.open("FFTRawdata",ios::out);
  
   
  for (uInt sample(0); sample< NOfRow/2+1; sample++) {
     for (uInt antenna(0); antenna< NOfColumn; antenna++) {	
               logfile3 << FFTRawMatrix (sample, antenna) 
	      << " \t";
      }
      logfile3 << endl;
    
   }
   
   
   Matrix<double> AmplRawofFFT = amplitude(FFTRawMatrix);
   
   ofstream logfile4;

   logfile4.open("AmplRawFFT",ios::out);
//   
     
     for (uInt sample(0); sample< NOfRow/2+1; sample++) {
     for (uInt antenna(0); antenna< NOfColumn; antenna++) {	
            logfile4 << AmplRawofFFT (sample, antenna) 
	      << " \t";
      }
      logfile4 << endl;
     
   }
   
   //%%%%%%%%%%%%%%%%%%% Inverse FFT of raw data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   
    uint NOFColumn = FFTRawMatrix.ncolumn () ;
    
  //  uint NOFRow = FFTRawMatrix.nrow () ;
    
  //    cout << " No of columns in FFT Raw Matrix :" << NOFColumn << endl ;
  
  Matrix<double> IFFTRawMatrix (NOfRow,NOFColumn ) ;
  
   FFTServer<double,DComplex> server1;
   
   Vector<double> IFFTRawVector(NOfRow );
    
   for (uint f =0 ; f < NOFColumn; f++ ) {
   
    server1.fft( IFFTRawVector,FFTRawMatrix.column(f)) ;
           
   IFFTRawMatrix.column(f) = IFFTRawVector ;
   
   }
   
  
   
     ofstream logfile5;

   logfile5.open("IFFTRawdata",ios::out);
//   
      
      for (uInt sample(0); sample< dataBlockSize; sample++) {
     for (uInt antenna(0); antenna< NOfColumn; antenna++) {	
               logfile5 << IFFTRawMatrix (sample, antenna) 
	      << " \t";
      }
      logfile5 << endl;
    
   }
   
   
   
  //************************************************************************************************ 
  
  //******************************************** FFT of FIR implemented data**************************************
  
  //*************************************************************************************************
  
  Matrix<DComplex> FFTMatrix (dataBlockSize/2+1,nofColumn ) ;
  
  // FFTServer<double,DComplex> server;
   
   Vector<DComplex> FFTVector(dataBlockSize/2+1);
    
   for (uint f =0 ; f < nofColumn; f++ ) {
   
   server.fft( FFTVector,output.column(f)) ;
   
 //  cout << "No. of column : " << f+1 << endl ;
   
  // cout << " raw data vector send for FFT : " << output.column(f) << endl ;
      
   FFTMatrix.column(f) = FFTVector ;
   
   //cout << " FFT Vector of FIR implemented data:" <<  FFTMatrix.column(f) << endl ;
   
   }
   
  // cout << " FFT Matrix : " << FFTMatrix << endl ;
   
   ofstream logfile6;

   logfile6.open("FFTdata",ios::out);
//   
     
     for (uInt sample(0); sample< dataBlockSize/2+1; sample++) {
     for (uInt antenna(0); antenna< nofColumn; antenna++) {	
           logfile6 << FFTMatrix (sample, antenna) 
	      << " \t";
      }
      logfile6 << endl;
    
   }
   
   
   Matrix<double> AmplofFFT = amplitude(FFTMatrix);
   
   ofstream logfile7;

   logfile7.open("AmplFFT",ios::out);
//   
     for (uInt sample(0); sample< dataBlockSize/2+1; sample++) {
     for (uInt antenna(0); antenna< nofColumn; antenna++) {	
              logfile7 << AmplofFFT (sample, antenna) 
	      << " \t";
      }
      logfile7 << endl;
     
   }
  //************************************************************************************************ 

 //%%%%%%%%%%%%%%%%%%% Inverse FFT of FIR implemented data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   
    // uint nOfColumns = fourierTransformed.ncolumns () ;
    
   // FFTServer<double,DComplex> server;
    uint NOFCOLUMN = FFTMatrix.ncolumn () ;
    
    //uint NOFROW = FFTMatrix.nrow () ;
    
      
  Matrix<double> IFFTMatrix (NOfRow,NOFCOLUMN ) ;
  
  // FFTServer<double,DComplex> server1;
   
   Vector<double> IFFTVector(NOfRow );
    
   for (uint f =0 ; f < NOFCOLUMN; f++ ) {
   
   server1.fft( IFFTVector,FFTMatrix.column(f)) ;
   
  // cout << "No. of column : " << f+1 << endl ;
   
 //  cout << " raw data vector send for FFT : " << sampledValueMatrix.column(f) << endl ;
      
   IFFTMatrix.column(f) = IFFTVector ;
   
 //  cout << " FFT Vector :" <<  FFTRawMatrix.column(f) << endl ;
   
   }
     
     
   ofstream logfile8;

   logfile8.open("IFFTdata",ios::out);
//   
     
     for (uInt sample(0); sample< dataBlockSize; sample++) {
     for (uInt antenna(0); antenna< nofColumn; antenna++) {	
               logfile8 << IFFTMatrix (sample, antenna) 
	      << " \t";
      }
      logfile8 << endl;
    
   }
    
    
    
    
    
    
   
   
 // Matrix<double> ppf_coefficients( dataBlockSize, 16 );
  
//   Matrix<double> ppf_coefficients = (invft.ppfCoefficients( ppf_coeff,
//    				                           dataBlockSize )) ;
//   uint nofrows = ppf_coefficients.nrow () ;
//      
//      uint nofcolumns = ppf_coefficients.ncolumn() ; 
//      for (uint j=0; j<nofrows; j++ ){
//    for (uint i=0; i<nofcolumns; i++ ){
//    double ppfcoefficients = ppf_coefficients(j,i);
//    
//    cout << ppfcoefficients << "\t" ;
//    }
//    cout <<  endl ;
//    }							   
//  
//   Matrix<double> dataSamples =(invft.blockSamples( output,
//    			                          dataBlockSize )) ;
//  
//  
//     uint nofRows = dataSamples.nrow () ;
//      
//      uint nofColumns = dataSamples.ncolumn () ;
//      
//     Matrix<double> matchingValues( nofRows, nofColumns ) ; 
//           
//     for( uint i=0; i< nofColumns ; i++ ) {
//     
//     Vector<double> dataSampleColumn = dataSamples.column(i) ;
//     
// //    cout << " wave samples of a column : " << dataSampleColumn << endl ;
//     
//  //  Vector<double> multipliedVector = invft.multipliedVector( dataSampleColumn,
//  //   				                             ppf_coefficients ) ;
//    
// //   cout << " column of the data sampled : " << multipliedVector << endl ;
//     uint m = ppf_coefficients.ncolumn () ;   //ppfCoff.ncolumn() ;
//    
//   // Vector <double> sampleVal = dataSampleColumnwaveSamples ;
//    
//   
//   for( uint j=0; j < m; j++ ){
//   
//   Vector<double> ppfCoffColumn = ppf_coefficients.column(j) ;         //ppfCoff.column(j) ;
//    
//   dataSampleColumn = dataSampleColumn*ppfCoffColumn;        // sampleVal*ppfCoffColumn ;
//   
//   //cout << " column of the data sampled : " << dataSampleColumn << endl ;
//   
//   }
  
  
          
 //  Vector<DComplex> fourierTransformedVector = invft.FFTVector( output, //multipliedVector,
 //  								dataBlockSize ) ;
   
 //  cout << " fourier Transformed Vector :" <<  fourierTransformedVector << endl ;
    
 //  Vector<double> inverseFourierVector = invft.IFFTVector( fourierTransformedVector,
 //   							   dataBlockSize ) ; 
   
//   cout << " inverse Fourier Vector :" << inverseFourierVector << endl ;
   
//   matchingValues.column(i) = inverseFourierVector ;
     

    }
  catch( AipsError x) {
  cerr<< x.getMesg() << endl ;
  ok = False ;
  }
  return ok ;
  }

// ------------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofParameters (1);
  int nofFailedTests (0);

  // parameter check 
  if (argc < nofParameters) {
    std::cerr << "[tinverseFFT] Too few parameters!"  << std::endl;
    std::cerr << " -- Usage: tinverseFFT <eventfile>" << std::endl;
    return -1;
  }

  nofFailedTests += test_inverseFFTS () ;
  
  return nofFailedTests;
}
