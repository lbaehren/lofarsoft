/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Kalpana Singh (<mail>)                                                     *
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

/* $Id: tPhaseCalibrationPlugin.cc,v 1.5 2007/03/26 14:23:34 singh Exp $*/

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>

#include <templates.h>
#include <Calibration/PhaseCalibration.h>
#include <Calibration/PluginBase.h>
#include <Calibration/PhaseCalibrationPlugin.h>
#include <Data/ITSCapture.h>
#include <Data/LopesEvent.h>
#include <Data/LopesEventIn.h>
#include <IO/DataReader.h>


using namespace CR;

/*!
  \file tPhaseCalibrationPlugin.cc

  \ingroup Calibration

  \brief A collection of test routines for PhaseCalibrationPlugin
 
  \author Kalpana Singh
 
  \date 2006/11/02
*/
// -----------------------------------------------------------------------------
// 
// Global Valriables
//
//------------------------------------------------------------------------------


uint dataBlockSize ( 1024 ) ;
Double alpha ( 0.5 ) ;

Vector<String> filenamesVector () 
{
  Vector<String> files( 1 );
  
  files( 0 ) = "2004.02.10.01:48:14.776.event";
  
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


//--------------testing default constructor -----------------
//**********************************************************

int test_PhaseCalibrationPlugins ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_PhaseCalibrationPlugin]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
  
    PhaseCalibrationPlugin Phclpgin;
  } catch (AipsError x) {
    cerr << "test_PhaseCalibrationPlugin :- Testing default constructor ..." << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}



Bool test_PhaseCalibrationPlugin ()
{
 Bool ok ( True ) ;
 
 try {
 
//  getting data files to test the class    

Vector<String> filenames = filenamesVector();
DataReader *dr ;
LopesEvent *anEvent = new LopesEvent( filenames(0), 
 			               dataBlockSize );

dr = anEvent ;
dr->setHanningFilter (alpha );

Matrix<DComplex> spectra (dr->fft());
Matrix<Double> phaseArray( phase( spectra ) ) ;
 
uInt row = phaseArray.nrow () ;
uInt column = phaseArray.ncolumn () ;
      
ofstream logfile1p ;

//--------rawdata will be saved with filename rawdata ----------

logfile1p.open( "rawdata", ios::out ) ;
 
 for( uInt sample(0); sample< row ; sample++ ) {
    for( uInt antenna(0); antenna< column; antenna++ ) {
       logfile1p << phaseArray( sample, antenna )
                 << "\t" ;
     }
    logfile1p << endl ;
  }
logfile1p.close();
 

PhaseCalibrationPlugin Phclpgin;

//---------creating  vector and matrices with input arrays --------------
//************************************************************************

  for( int i=0; i< 3; i++ ) {
   for( int j =0; j<2; j++ ) {
      frequencyRanges(i,j) = frequencyRange[i][j] ;
   }
  }
  
 for( int m =0; m<3; m++ ) {
     for( int n =0; n<10; n++ ){
	 expectedPhases(m,n) = expectedPhase[m][n];
     }
 }
       
 for( int k=0; k<3; k++){
     for( int l=0; l<10; l++){
	 phaseGradient(k,l) = Grad[k][l];
     }
 }

Double interval = 0.0 ;
 
for( uInt k =0; k< row ; k++ ){

   interval = (80-40)*1000000/row ;

    frequencyValues(k)= 40*1000000 + interval*k ;

    }
//cout<< " Frequency Values :" <<frequencyValues<< endl;

 for( int k=0; k<6; k++){
   	 sampleJumps(k) = sampleJump[k];
     }
 
// Double sampleRate = 80000000 ;
//  int referenceAntenna = 0 ;
// Double badnessWeight = 0.5 ;
// Double badnessThreshold = 0.15 ;

Vector<Bool> AntennaReturn ;

AntennaReturn =(Phclpgin.calcWeights( spectra )); 

 cout << " Antenna return " << AntennaReturn << endl ;
 //return AntennaReturn ;
}

 //PhaseCalibrationPlugin Phclpgin;
 
 catch ( AipsError x ) {
 
 cerr<< " Bool test_PhaseCalibration " << x.getMesg() << endl ;
 ok =False ;
 }
 
 return ok ;
 }
 
 
 
 
// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_PhaseCalibrationPlugin ();
  }

  return nofFailedTests;
}
// -----------------------------------------------------------------------------

