 /*-------------------------------------------------------------------------*
  | $Id::                                                                 $ |
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

#include <casa/Arrays/ArrayIO.h>
#include <casa/HDF5/HDF5File.h>
#include <casa/HDF5/HDF5Record.h>

#include <dal/dalCommon.h>
#include <dal/dalDataset.h>
#include <dal/BeamFormed.h>
#include <dal/BeamGroup.h>
#include <dal/TBB_Timeseries.h>

#include <Analysis/ppfinversion.h>
#include <Analysis/ppfimplement.h>
#include <Analysis/SubbandID.h>

#include <Data/rawSubbandIn.h>
#include <Data/tbbctlIn.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>

using CR::ppfinversion ;
using CR::ppfimplement ;
using CR::tbbctlIn ;
using CR::rawSubbandIn ;
using CR::SubbandID ;
using DAL::TBB_Timeseries ;

using namespace std;

// -----------------------------------------------------------------------------

/*!
  \file tppfinversion.cc

  \ingroup Analysis

  \brief A collection of test routines for ppfinversion
 
  \author Kalpana Singh
 
  \date 2007/06/01

  <h3>Usage</h3>

  \verbatim
  tppfinversion <filename>
  \endverbatim

  - \e filename is a dataset containing beamformed data in HDF5 format.
*/
Double band_freq[48] = {43750000, 43906250, 44062500, 44218750, 44375000, 44531250, 44687500, 44843750, 45000000, 45156250, 45312500, 45468750, 45625000, 45781250, 45937500, 46093750, 46250000, 46406250, 46562500, 46718750, 46875000, 47031250, 47187500, 47343750, 47500000, 47656250, 47812500, 47968750, 48125000, 48281250, 48437500, 48593750, 48750000, 48906250, 49062500, 49218750, 49375000, 49531250, 49687500, 49843750, 50000000, 50156250, 50312500, 50468750, 50625000, 50781250, 50937500, 51093750 } ;

uint band_ID[48] ={286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334 } ;

Vector<Double> subband_freq(48,0.0) ;
Vector<uint> subband_ID( 48,0 ) ;

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ppfinversion object

  \return nofFailedTests -- The number of failed tests within this function.
  
  */
  
 //******************************************************
 //**********  to get beamformed data ******************

int test_ppfinversion (std::string const &filename)
{
  cout << "\n[test_ppfinversion]\n" << endl;
  
  int nofFailedTests = 0;
  
  cout << "[1] Testing default constructor ..." << endl;
    try {
    ppfinversion ppf_inv ;
    ppfimplement ppf_imp ;
    SubbandID band_ID ;
    
    tbbctlIn newtbbctlIn ;
    rawSubbandIn newrawSubbandIn ;
    
   for(uint p=0; p< 48; p++){
      subband_freq(p) = band_freq[p] ;
      }
    
    
    Vector<String> filenames( 2 );
    {
      filenames(0)="/mnt/lofar/ppfinversion/TBB.corr.h5";
      filenames(1)="/mnt/lofar/ppfinversion/rw_20080604_121347_2300.dat.h5"; 
    }
    cout << "[1] Testing attaching file...." << endl ;
  
    DAL::BeamFormed bf (filename );
    bf.summary();

    cout << "-- Extracting beam group from dataset ..." << endl;

  DAL::BeamFormed * file = NULL;
  DAL::BeamGroup * beam  = NULL;
  int subband            = 3 ;
  int start              = 0;
  int length             = 500; //441344;
  
  std::cout << "[1] Extract BeamGroup ..." << std::endl;
      file = new DAL::BeamFormed(filename.c_str());
    beam = file->getBeam(0);
   
    beam->summary();
    std::cout << "[2] Read in data from X-component of subband..." << std::endl;
    


    std::vector< std::complex<short> > xvals;
    std::vector< std::complex<short> > yvals;
    xvals.clear();
    yvals.clear();
    
    // step through the data for subband 0, 10 samples at a time, 10 times
  //  for (unsigned int count=0; count < 10; count++ )
 //     {
	beam->getSubbandData_XY( subband, start, length, xvals, yvals );
	Vector<float> total_intensity(length,0.0)  ;
//	Vector<float> Y_amplitude(length,0.0)  ;
//	printf( "Values %d through %d\n", start, start + length );
	for (unsigned int ii=0; ii < xvals.size(); ii++ )
	  {
	   float x_real = float(xvals[ii].real()) ;
           float x_imag = float(xvals[ii].imag()) ;
	   float y_real = float(yvals[ii].real()) ;
           float y_imag = float(yvals[ii].imag()) ;
	   float Xreal = x_real + y_real ;
	   float Ximag = x_imag + y_imag ;
           float squared_term =Xreal*Xreal +Ximag*Ximag ;
           total_intensity(ii)=sqrt(squared_term) ;
	  }
//	printf("\n");
//	xvals.clear();
//	yvals.clear();
//	start += length;
	
      ofstream logfile2;
      logfile2.open( "band3xy", ios::out );
      for( int sample=start ; sample < length ; sample++ ){
         logfile2 << total_intensity(sample) << endl;
       }
    logfile2.close() ;
    
    Double sampleFrequency = 156250e3 ;
    
   Vector<uint> subband_ID = band_ID.calcbandIDVector ( sampleFrequency,
	                  		                subband_freq ) ;

   std::cout << " subband ID vector : " << subband_ID << endl ;
					      

  
} catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  return nofFailedTests;
 }


//*********************************************************************
//******** to get TBB time series data *******************************



int test_data (std::string const &filename)
{
  cout << "\n[test_data]\n" << std::endl;

  int nofFailedTests = 0;
  int start          = 0;
  int nofSamples     = 516*1024 ; //528384 ;
  TBB_Timeseries timeseries (filename);

  cout << "[1] Retrieve time-series data without channel selection"
	    << std::endl;
  try {
    casa::Matrix<double> data = timeseries.fx (start,
					       nofSamples);
    // feedback 
    cout << "-- Data start     = " << start        << std::endl;
    cout << "-- Data blocksize = " << nofSamples   << std::endl;
    cout << "-- Data array     = " << data.shape() << std::endl;
    cout << "-- Data [0,]      = " << data.row(0)  << std::endl;
    cout << "-- Data [1,]      = " << data.row(1)  << std::endl;
    
    //cout << data << endl ;
    Vector<double> data_X = data.column(0);
   // Vector<double> data_Y = data.column(1);
    ofstream logfile1;
      logfile1.open( "timeseriesX", ios::out );
      for( int sample=start ; sample < nofSamples ; sample++ ){
             logfile1<< data_X(sample) << endl;
       }
    logfile1.close() ;
    
//       ofstream logfile2;
//       logfile2.open( "timeseriesY", ios::out );
//       for( int sample=start ; sample < nofSamples ; sample++ ){
//              logfile2<< data_Y(sample) << endl;
//        }
//     logfile2.close() ;
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
//*********************************************************************
// ----------------------------------------------------------- test_ppfinversions

int test_inversions (std::string const &filename)
{
  int nofFailedTests = 0;
  try {
  std::cout << "\n[tppfinversion::test_inversions]\n" << std::endl;
  
  Vector<Double> ppfcoeff(16384,0.0) ;
  readAsciiVector( ppfcoeff,"Coeffs16384Kaiser-quant.dat" ) ;
  Vector<Double> ppfcoeff_inv(16384, 0.0) ;
  readAsciiVector( ppfcoeff_inv,"ppf_inv.dat" ) ;
  
  int start          = 0;
  int nofSamples     = 516*1024;
  TBB_Timeseries timeseries (filename);
  Vector<Double> samples( nofSamples,0.0 );
  
  casa::Matrix<double> data = timeseries.fx (start,
					     nofSamples);
    
    cout << "-- Data start     = " << start        << std::endl;
    cout << "-- Data blocksize = " << nofSamples   << std::endl;
    cout << "-- Data array     = " << data.shape() << std::endl;
    cout << "-- Data [0,]      = " << data.row(0)  << std::endl;
    cout << "-- Data [1,]      = " << data.row(1)  << std::endl;
    					     
  Vector<double> data_X = data.column(0);
       
  for( int sample=start ; sample < nofSamples ; sample++ ){
          samples(sample)= data_X(sample) ;
       } 
       
  for(uint p=0; p< 48; p++){
      subband_ID(p) = band_ID[p] ;
      }
    ppfinversion ppf_inv ;
    ppfimplement ppf_imp ;
    tbbctlIn newtbbctlIn ;
    rawSubbandIn newrawSubbandIn ;     
    
    Matrix<DComplex> ppfimplement = ppf_imp.FFTSamples( samples,
                                                        ppfcoeff );
							
//    int ROWS = ppfimplement.nrow() ;
    int COLUMNS = ppfimplement.ncolumn() ;
    
    Vector<DComplex> ppfimplement_vector = ppfimplement.row(288) ;
    
//     for (int i=0; i< COLUMNS; i++ ){
//        cout << "real part (" << ppfimplement_vector(i).real() << ", "
//             << ppfimplement_vector(i).imag() << " )" <<endl ;
// 	    }
	Vector<float> X_amplitude(COLUMNS,0.0)  ;
    
    for (int ii=0; ii < COLUMNS; ii++ ) {
        float x_real = float(ppfimplement_vector[ii].real()) ;
        float x_imag = float(ppfimplement_vector[ii].imag()) ;
        float squared_term = x_real*x_real +x_imag*x_imag ;
        X_amplitude(ii)=sqrt(squared_term) ;
       }    
     ofstream logfile2;
      logfile2.open( "band289", ios::out );
      for( int sample=0 ; sample < COLUMNS ; sample++ ){
           logfile2<< X_amplitude(sample) << endl;
       }
    logfile2.close() ;

    
   } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  return nofFailedTests ;
}
 

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);
// std::string fileBeamformed;
// int test_ppfinversion (std::string const &filename) 
  std::string filename;
  if (argc > 1) {
    filename = std::string(argv[1]);
  } else {
    std::cerr << "Please provide a HDF5 filename.\n";
    return(DAL::FAIL);
  }
  
  nofFailedTests += test_inversions( filename );
//nofFailedTests += test_getData(filename);
//   if (nofFailedTests == 0) {
//     nofFailedTests += test_attributes (filename);
//     nofFailedTests += test_getData (filename);
//   }
  /* Check parameter provided from the command line */
//   if ( argc < 2 ) {
//     std::cerr << "[tppfinversion] Missing name of input data file!" << endl;
//     std::cerr << endl;
//     std::cerr << "  tppfinversion <filename>" << endl;
//     std::cerr << endl;
//     return 1;
//   } else {
//     fileBeamformed = std::string (argv[1]);
//   }
  
  /* Run the tests */
  
//  nofFailedTests += test_inversions (fileBeamformed);
  
  return nofFailedTests;
}
