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

/* $Id: tRFIMitigation.cc,v 1.12 2007/03/07 14:36:33 singh Exp $*/


/*!
  \file tRFIMitigation.cc

  \ingroup Calibration
  
  \brief A collection of test routines for RFIMitigation
 
  \author Kalpana Singh
 
  \date 2006/03/15
*/

//casa header files
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

#include <Calibration/PluginBase.h>
#include <Calibration/RFIMitigation.h>
#include <Data/ITS_Capture.h>
#include <Data/LopesEvent.h>
#include <IO/DataReader.h>

#include <casa/namespace.h>

using CR::RFIMitigation;

// ------------------------------------------------------------------------------
//
//  Global variables
//
// ------------------------------------------------------------------------------

Int dataBlockSize ( 131072 );
// slope parameter for the HanningFilter
Double alpha (0.5);
  
//------------------------to test RFIMitigation class------------------------

  
  
//  return nofFailedTests;

//}	



// 
// int main ()
// {
//   Int nofFailedTests (0);
// 
//   // Test for the constructor(s)
//   {
//     nofFailedTests += test_RFIMitigation ();
//   }
// 
//   return nofFailedTests;
// }
// /*!
//   \brief Export frequency domain data to an ASCII table
// 
//   \param filename -- Name of the output file.
//   \param data     -- Array with the data to be exported
//  */
//  
 Int nOfSegments( 25 );
 //Vector<Int> segmentation( nOfSegment-1 );

// ------------------------------------------------------------------------------
//
//  Function prototypes
//
// ------------------------------------------------------------------------------

/*!
  \brief Get a vector with the names of the files from which to read data

  \return files -- vector with the names of the files from which to read data
*/
 Vector<String> filenamesVector ();

 // ------------------------------------------------------------------------------
//
//  Implementation
//
// ------------------------------------------------------------------------------


Vector<String> filenamesVector()
{
 Vector<String> files( 1 );
 
 files(0)="/data/LOPES/2008/08/06/2008.08.06.16:02:56.047.event";
//  files(1)="/data/ITS/2004.11/2004.11.02-15:00:17.00-24h/experiment.meta";
//  files(2)="/data/ITS/2004.11/2004.11.02-17:30:13.27-24h/experiment.meta";
//  files(3)="/data/ITS/2004.11/2004.11.02-18:00:13.04-24h/experiment.meta";
//  files(4)="/data/ITS/2004.11/2004.11.02-21:30:10.39-24h/experiment.meta";
//  files(5)="/data/ITS/2004.11/2004.11.03-00:00:11.96-24h/experiment.meta";
//  files(6)="/data/ITS/2004.11/2004.11.03-00:30:14.33-24h/experiment.meta";
//  files(7)="/data/ITS/2004.11/2004.11.03-04:00:10.07-24h/experiment.meta";
//  files(8)="/data/ITS/2004.11/2004.11.03-06:30:15.18-24h/experiment.meta";
//  files(9)="/data/ITS/2004.11/2004.11.03-07:00:13.22-24h/experiment.meta";
//  files(10)="/data/ITS/2004.11/2004.11.03-10:30:13.51-24h/experiment.meta";
//  files(11)="/data/ITS/2004.11/2004.11.03-13:00:13.54-24h/experiment.meta";
//  files(12)="/data/ITS/2004.11/2004.11.03-14:00:14.31-24h/experiment.meta";
 
 return files;
}

// ----------------------------------------------------------- test_RFIMitigation

/*!
  \brief Test constructors for a new RFIMitigation object
  
  \return nofFailedTests -- The number of failed tests.
*/
 Int test_RFIMitigation ()
 {
   Int nofFailedTests (0);
   
   try {
     RFIMitigation rfim;
   } catch (AipsError x) {
     cerr << "[test_RFIMitigation] " << x.getMesg() << endl;
   }
 
   return nofFailedTests;
 }

// ----------------------------------------------------------- test_RFIMitigation

/*!
  \brief Test constructors for a new RFIMitigation object
  
  \return ok -- Status of the test routine.
*/

 Bool test_RFIMitigations ()
 {
  Bool ok ( True );
 
 try {
 
  //for ( Int ns=0; ns < (nOfSegment-1) ; ns++ ){
 
 // segmentation(ns) = (ns+1)*2621   ;
 // }
  
  /* 
   
   plot 'rawdata' using 1 thru log(x) with line
  replot '25RFIrejectedSpectra17' using 1 thru log(x) with line
  
  
*/
 Vector<String> filenames = filenamesVector();
  Vector<String> testfile ();
  
  // to read raw data after application of Hanning Filter
  DataReader *dr;
  ITS_Capture *capture = new ITS_Capture (filenames(0),
					dataBlockSize );
  dr = capture;
  dr->setHanningFilter (alpha);

  Matrix<DComplex> fft_filtered (dr->fft());
  
  //Int rowsfft = fft_filtered.nrow();
 // Int columnsfft = fft_filtered.ncolumn();
  
  Matrix<Double> absoluteArray(amplitude (fft_filtered));
  
  uInt Rows = absoluteArray.nrow();
 
  uInt Columns = absoluteArray.ncolumn(); 
  
  cerr << " number of rows :" << Rows<< "\n"
       << "number of columns :" << Columns << endl ;
  //Matrix<DComplex> fft_filtered( rowsfft, columnsfft );
  
 // convertArray( fft_filtered, fft ); 
  
  //Matrix<Double> rawdata( rowsfft, columnsfft );

  ofstream logfile1;
 
  logfile1.open("raw_data",ios::out);
 
   for (uInt sample(0); sample< Rows; sample++) {
     for (uInt antenna(0); antenna< Columns; antenna++) {
       logfile1 << absoluteArray (sample,antenna) 
                << " \t";
	}
	logfile1 << endl ;
  }

  logfile1.close();
  
  RFIMitigation rfim ;
  
  Matrix<Double> optimizedSpectra( rfim.getOptimizedSpectra(  fft_filtered,
					                      dataBlockSize,
				                              nOfSegments ));
  
 // Matrix<Double> interpolatedSpectra( rf.getinterpolatedGains( fft_filtered,
 //						           segmentation ));
  
//  Matrix<Double> normalisedSpectra( rf.getnormalizeSpectra( fft_filtered,
 //						           segmentation ));
   
// Matrix<Double> optimizedSpectra ( rf.retraceOriginalSpectra( normalisedSpectra,
//					                      segmentation ) ) ;
							      
 //Matrix<Double> optimized2Spectra ( rf.retraceOriginalSpectra( optimized1Spectra,
//					                      segmentation ) ) ;
							      			      
 Matrix<Double> RFIrejectedSpectra = absoluteArray*optimizedSpectra ;

 uInt nOfRows = RFIrejectedSpectra.nrow();
 
 uInt nOfColumns = RFIrejectedSpectra.ncolumn();   

  ofstream logfile2;

   logfile2.open("rfirejectedspectra",ios::out);
//   	
    for (uInt sample(0); sample< nOfRows; sample++) {
      for (uInt antenna(0); antenna< nOfColumns; antenna++) {
              logfile2 << RFIrejectedSpectra (sample, antenna) 
  	        << " \t";
      }
      logfile2 << endl;
      //cout << " RFI rejected Data has been stored " << "\n" ;
   }
 }
    catch( AipsError x ){
   cerr << x.getMesg () << endl;
  ok = False ;
 }
 return ok;
 } // end test_RFIMitigation
 
// ------------- test_parameterRanges

void test_parameterRanges ()
{
  ;
}
 
//---------------------------------------- main routine--------------------------

int main()
{
 
 Bool ok ( True ) ;
 Int retval ( 0 );
 
 if ( ok ){
 
 ok = test_RFIMitigations ();

 
 if (!ok){
 retval = 1;
 cout << "Error...........early exit"
      << endl ;
    }
   }
  return retval ;
} //------------------------------------------------- end main


