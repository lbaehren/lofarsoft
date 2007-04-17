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

/* $Id: tPhaseCalibration.cc,v 1.15 2007/03/20 15:44:51 singh Exp $*/


/*!
  \file tPhaseCalibration.cc

  \ingroup Calibration

  \brief A collection of test routines for PhaseCalibration
 
  \author Kalpana Singh
 
  \date 2006/07/03
*/

// casa header files

#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>
#include <lopes/Calibration/PluginBase.h>
#include <lopes/Calibration/PhaseCalibration.h>

#include <lopes/Data/ITSCapture.h>
#include <lopes/Data/LopesEvent.h>

#include <lopes/Data/LopesEventIn.h>
#include <lopes/IO/DataReader.h>

#include <casa/namespace.h>


// -----------------------------------------------------------------------------
// 
// Global Valriables
//
//------------------------------------------------------------------------------

uint dataBlockSize ( 16384*2 ) ;
Double alpha ( 0.5 ) ;

Vector<String> filenamesVector () 
{
  Vector<String> files( 1 );
  
  files( 0 ) = "2004.02.10.01:48:14.776.event";
  
  
  //LOPES30_sel/2005.09.02.04:52:43.486.event" ;
  
  
  //2004.01.12.00:28:11.577.event" ;
 
  //2004.02.10.01:48:14.776.event"; 
  //2004.02.09.18:02:11.922.event"; 

  return files ;
 }
 
 Double frequencyRange[3][2] = { {62165865.6, 62223239.5},
                                 {67670000.0, 67712097.0 },
				 {67915000.0, 67947000.0 } };
				 
	
 Double expectedPhase[3][10] = { {0., 118., 4.8, 99.6, 172.4, -66., -113.6, 49., -138.1, 5.1},
                                 {0., 112., -143., -73., -14., -136., -151., -25., 11.4, -1.2},
				 {0., 122., -97., -42., 18.7, -113., -140., -14., 140.6, 29.9} } ;
				 
Double sampleJump[6]={ -2, 2, -1, 1, -3, 3 } ;

Double Grad[3][10] = { {-919.812673, -861.727128, -940.460636, -984.350944, -1459.81119, -1476.3772, -589.239534,                                   -695.093165, -631.581104, -1188.81975},
                       {-1000.96359, -937.753418, -1023.43323, -1071.19578, -1588.60373, -1606.63129, -641.225475,      -756.418093, -687.30265, -1293.70394},
                      {-1004.61041, -941.169949, -1027.16192, -1075.09848, -1594.39151, -1612.48476,  -643.561662,-759.173963, -689.806711, -1298.41731} };
			 

Vector<Double> frequencyValues(dataBlockSize+1, 0.0);

Matrix<Double> frequencyRanges( 3, 2, 0.0 ) ;

Matrix<Double> expectedPhases( 3, 10, 0.0 );

Matrix<Double> phaseGradient( 3, 10, 0.0 ) ;

Vector<Double> sampleJumps(6, 0);
   
 /*!
  \brief Test constructors for a new PhaseCalibration object

  \return nofFailedTests -- The number of failed tests.
*/

Int test_PhaseCalibrations ()
{
  Int nofFailedTests (0);
  
  try {
  
  PhaseCalibration phcl ; 
  
  } catch ( AipsError x) {
  
  cerr<< "test_PhaseCalibration :- Testing default constructor ..." << x.getMesg () << endl;
  }
    return nofFailedTests;
}

Bool test_PhaseCalibration ()
{
 Bool ok ( True ) ;
 
 try {
 
cout << " - getting filenames" << endl; 

Vector<String> filenames = filenamesVector();

//cout << " - setting up DataReader ..." << endl;
//cout << " -- filename  = " << filenames(0) << endl; 
//cout << " -- blocksize = " << dataBlockSize <<endl;

 DataReader *dr ;
LopesEvent *anEvent = new LopesEvent( filenames(0), 
 			               dataBlockSize );

dr = anEvent ;
dr->setHanningFilter (alpha );

//cout << " - getting FFT data" << endl; 

Matrix<DComplex> spectra (dr->fft());
 
//cout << " - extracting phases ..." << endl;

Matrix<Double> phaseArray( phase( spectra ) ) ;
 
uInt row = phaseArray.nrow () ;
 
uInt column = phaseArray.ncolumn () ;
 
//cout << "Number of Rows : " << row << "\n"
//      << "Number of Columns : " << column << endl;
      
ofstream logfile1p ;
 
logfile1p.open( "rawdata", ios::out ) ;
 
 for( uInt sample(0); sample< row ; sample++ ) {
    for( uInt antenna(0); antenna< column; antenna++ ) {
       logfile1p << phaseArray( sample, antenna )
                 << "\t" ;
     }
    logfile1p << endl ;
  }
logfile1p.close();
 
 PhaseCalibration phcl ;
 
 for( Int i=0; i< 3; i++ ) {
   for( Int j =0; j<2; j++ ) {
      frequencyRanges(i,j) = frequencyRange[i][j] ;
   }
  }
  
 for( Int m =0; m<3; m++ ) {
     for( Int n =0; n<10; n++ ){
	 expectedPhases(m,n) = expectedPhase[m][n];
     }
 }
       
 for( Int k=0; k<3; k++){
     for( Int l=0; l<10; l++){
	 phaseGradient(k,l) = Grad[k][l];
     }
 }

Double interval = 0.0 ;
 
for( Int k =0; k< row ; k++ ){

   interval = (80-40)*1000000/row ;

    frequencyValues(k)= 40*1000000 + interval*k ;

    }
//cout<< " Frequency Values :" <<frequencyValues<< endl;

 for( Int k=0; k<6; k++){
   	 sampleJumps(k) = sampleJump[k];
     }
 

Double sampleRate = 80000000 ;

 Int referenceAntenna = 0 ;

Double badnessWeight = 0.5 ;

Double badnessThreshold = 0.15 ;

Vector<Bool> AntennaReturn =(phcl.getAntennaReturn( spectra,
  		     				    frequencyRanges,
		     				    expectedPhases,
						    phaseGradient,
						    frequencyValues,
		     				    sampleJumps,
		     				    referenceAntenna,
		     				    sampleRate,
						    badnessWeight,
						    badnessThreshold ) ); 

//return AntennaReturn ;
}

 catch ( AipsError x ) {
 
 cerr<< " Bool test_PhaseCalibration " << x.getMesg() << endl ;
 ok =False ;
 }
 
 return ok ;
 }
 
 
 
 
// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_PhaseCalibration ();
  }

  return nofFailedTests;
}
