/***************************************************************************
 *   Copyright (C) 2005 by Brett Deaton                                    *
 *   mbd02a@acu.edu                                                        *
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

/* $$ */

/*!
  \file tBaselineGain.cc

  \ingroup Calibration
  
  \brief A collection of tests for the BaselineGain class

  Because this tests the exportBaselineGains function, it will create three
  AIPS++ Table directories in the current working directory when executed:
  baselineGains_1,
  baselineGains_2,
  baselineGains_3
  
  \author Brett Deaton
  
  \date 2005/08
*/

#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>

#include <Calibration/AntennaGain.h>
#include <Calibration/BaselineGain.h>
#include <Data/ITSCapture.h>
#include <IO/DataReader.h>

// --- Function prototypes -----------------------------------------------------

Matrix<Complex> readSpectra();

Vector<String> makeFilenamesVector();

Vector<Float> makeFreqVector( const Float& minFreq,
			      const Float& maxFreq,
			      const Int& nOfSubBands );

Int test_BaselineGain();

Bool test_BaselineGains();

// *****************************************************************************
// *** Global Variables ********************************************************
// *****************************************************************************

// Constants for this specific Data Set
// These must be changed if a new set of data is used
const Float MIN_FREQ ( 0.0 );      // 0 MHz
const Float MAX_FREQ ( 40000000 ); // 40 MHz
const Int N_ANT ( 10 );            // number of antennas
const Int N_CHAN ( 513 );          // number of channels
const IPosition SPECTRA_SHAPE ( 2, N_CHAN, N_ANT );
// // Other global members
// DataReader *dr;


// *****************************************************************************
// *** Support routines ********************************************************
// *****************************************************************************

// --- makeFilenamesVector -----------------------------------------------------
// -----------------------------------------------------------------------------

// must adjust this function according to where the data one wants to use
//   is stored locally. UGLY Hack.
//   The data I am using for this test is a set that used only 10 antennas. It
//   was recorded 2 Nov 2004.
Vector<String> makeFilenamesVector()
{
  Vector<String> filenames ( 10 );
  filenames(0)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e05.i0001.dat";
  filenames(1)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e10.i0001.dat";
  filenames(2)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e17.i0001.dat";
  filenames(3)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e22.i0001.dat";
  filenames(4)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e29.i0001.dat";
  filenames(5)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e34.i0001.dat";
  filenames(6)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e41.i0001.dat";
  filenames(7)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e46.i0001.dat";
  filenames(8)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e53.i0001.dat";
  filenames(9)="/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/e59.i0001.dat";
  return filenames;
}

// --- makeFreqVector ----------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Generate a frequency Abscissa reference vector
*/
Vector<Float> makeFreqVector( const Float& minFreq,
			      const Float& maxFreq,
			      const Int& nOfSubBands )
{
  Vector<Float> freqVector ( nOfSubBands, 0.0 ); // length set

  // calculate frequency sub band interval
  Float interval = ( maxFreq - minFreq ) / nOfSubBands;
  // Declare frequency storage double. Set to midpoint of first sub band.
  Float frequency = minFreq + ( interval / 2 );
  // loop through each band and calculate the midpoint frequency
  for ( Int band ( 0 ); band < nOfSubBands; band++ ) {
    freqVector( band ) = frequency;
    frequency += interval; // increment frequency by one sub band interval
  } // end for: band
  return freqVector;
} // --- end makeFreqVector

Matrix<Complex> readSpectra()
{  
  Vector<String> filenames ( N_ANT );
  ifstream datafile;
  Matrix<Complex> spectra ( N_CHAN, N_ANT );
  Float re ( 0.0 );
  Float im ( 0.0 );
  // name the files
  filenames( 0 ) = "../../IO/test/test_ant1.dat";
  filenames( 1 ) = "../../IO/test/test_ant2.dat";
  filenames( 2 ) = "../../IO/test/test_ant3.dat";
  // Read in the test spectra from all data files
  for( int ant ( 0 ); ant < N_ANT; ant++ ) {
    // open file stream for this antenna
    cout << "    - Reading data from file " << filenames( ant ) << " ... " << flush;
    datafile.open( filenames( ant ).c_str(), ios::in );
    for( int freq ( 0 ); freq < N_CHAN; freq++ ) {
      datafile >> re >> im;
      spectra( freq, ant ) = Complex( re, im );
    }
    datafile.close();
    cout << "ok" << endl;
  }
  return spectra;
} // --- end readSpectra


// *****************************************************************************
// *** Test routines ***********************************************************
// *****************************************************************************

// --- test_AmplitudeGain ------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Test BaselineGain constructors.
  
  \return nFailed -- number failed tests after executing the function
*/
Int test_BaselineGain()
{
  Int nFailed ( 0 );
  // Default Constructor Test
  cout << "[#] Default Constructor test" << endl;
  try {
    BaselineGain gains ();
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    nFailed++;
  }


  // Wrote test down to here /\. The rest \/ is mostly garbage.


  // make variables for use in argumented constructors
  Matrix<Complex> spectra ( readSpectra() );
//   Int nOfSubBands ( 40 );
  // Argumented Constructor test (with default method)
  cout << "[#] Argumented Constructor test (with default method = MED)"
       << endl;
  try {
//     AmplitudeGain gains ( SPECTRA_SHAPE,
// 			  MIN_FREQ,
// 			  MAX_FREQ,
// 			  nOfSubBands);
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    nFailed++;
  }
  // Argumented Constructor test (method specfied MED)
  cout << "[#] Argumented Constructor test (method = MED)" << endl;
  try {
//     AmplitudeGain gains ( SPECTRA_SHAPE,
// 			  MIN_FREQ,
// 			  MAX_FREQ,
// 			  nOfSubBands,
// 			  AmplitudeGain::MED );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    nFailed++;
  }
  // Argumented Constructor test (method specfied MEAN)
  cout << "[#] Argumented Constructor test (method = MEAN)" << endl;
  try {
//     AmplitudeGain gains ( SPECTRA_SHAPE,
// 			  MIN_FREQ,
// 			  MAX_FREQ,
// 			  nOfSubBands,
// 			  AmplitudeGain::MEAN );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    nFailed++;
  }
  // Argumented Constructor test (method specfied MIN)
  cout << "[#] Argumented Constructor test (method = MIN)" << endl;
  try {
//     AmplitudeGain gains ( SPECTRA_SHAPE,
// 			  MIN_FREQ,
// 			  MAX_FREQ,
// 			  nOfSubBands,
// 			  AmplitudeGain::MIN );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    nFailed++;
  }
  return nFailed;
} // --- end test_AmplitudeGain


// --- test_BaselineGains ------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Test makeBaselineGains and getBaselineGains

  \todo Replace calls of non-existing method <i>makeBaselineGains</i>

  \return ok -- Status of the test routine.
*/
Bool test_BaselineGains()
{
  Bool ok ( True );
  Matrix<Complex> spectra ( readSpectra() );
  Int nOfSubBands ( 40 );
  Vector<Float> freqVector = makeFreqVector ( MIN_FREQ,
					      MAX_FREQ,
					      nOfSubBands );
  // construct new object, gains, default method = MED
//   AmplitudeGain gains ( SPECTRA_SHAPE,
// 			MIN_FREQ,
// 			MAX_FREQ,
// 			nOfSubBands);
  // begin makeBaselineGains test
  cout << "[#] makeBaselineGains test" << endl;
  try {
    // run spectra through to find gain curve
//     gains.makeBaselineGains( spectra );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  // begin test of getBaselineGains (fully argumented)
  cout << "[#] getBaselineGains test (fully argumented)" << endl;
  try {
    Vector<Bool> antReturn ( 3, True ); // return all antennas
//     gains.getBaselineGains( freqVector,
// 			    antReturn );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  // begin test of getBaselineGains (with default antennas)
  cout << "[#] getBaselineGains test (with default antennas)" << endl;
  try {
//     gains.getBaselineGains( freqVector );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  // begin test of getBaselineGains (with default frequencies)
  cout << "[#] getBaselineGains test (with default frequencies)" << endl;
  try {
//     Vector<Bool> antReturn ( 3, True );
//     antReturn ( 1 ) = False; // don't return 2nd antenna
//     Matrix<Float> gainCurves ( gains.getBaselineGains( antReturn ) );
//     cout << "    - requested 2 (of 3) antennas, returned "
// 	 << gainCurves.ncolumn()
// 	 << " antennas."
// 	 << endl;
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  // begin test of getBaselineGains (Default)
  cout << "[#] getBaselineGains test (default)" << endl;
  try {
//     gains.getBaselineGains();
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  return ok;
} // --- end test_BaselineGains


// --- test_exportBaselineGains ------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Test export of antenna baseline gain curves

  \todo Replace calls of non-existing method <i>makeBaselineGains</i>

  \return ok -- Status of the test routine.
*/
Bool test_exportBaselineGains( const Float& minFreq,
			       const Float& maxFreq,
			       const Int& nOfSubBands,
			       const String& tableName )
{
  Bool ok ( True );
  Matrix<Complex> spectra ( readSpectra() );
//   Int nOfSubBands_AtConst = 40; // nOfSubBands at construction
  Vector<Float> freqVector = makeFreqVector ( minFreq,
					      maxFreq,
					      nOfSubBands );
  // construct new object, gains
//   AmplitudeGain gains ( SPECTRA_SHAPE,
// 			MIN_FREQ,
// 			MAX_FREQ,
// 			nOfSubBands_AtConst);
  cout << "[#] makeBaselineGains test" << endl;
  try {
//     gains.makeBaselineGains( spectra ); // run spectra through to find gain curve
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  // begin exportBaselineGains test
  cout << "[#] exportBaselineGains test"
       << "\n    - exporting data to table "
       << tableName
       << " ... "
       << flush;
  try {
    Vector<Bool> antReturn ( 3 );
    antReturn = True; // return all antennae
//     gains.exportBaselineGains( freqVector,
// 			       antReturn,
// 			       tableName );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  cout << "ok" << endl; // export completed successfully
  return ok;
} // --- end test_exportBaselineGains

// *****************************************************************************
// *** Main routine ************************************************************
// *****************************************************************************

int main()
{
  // Report beginning of test
  cout << "-------------------------------------------\n"
       << "--- Testing the AmplitudeGain functions ---\n"
       << "-------------------------------------------"
       << endl;

  Bool ok ( True );
  Int retval ( 0 );

  // ---------------------------------------------------------------------------
  // test makeBaselineGains and getBaselineGains function
  if ( ok ) {
    ok = test_BaselineGains();
    if ( !ok ) {
      retval = 1;
      cout << "Error...early exit"
	   << endl;
    }
  }

  // ---------------------------------------------------------------------------
  // test exportBaselineGains function
  //   with same dimensions as at construction
  if ( ok ) {
    Int nOfSubBands = 40;
    String tableName = "baseline_gains_1";
    ok = test_exportBaselineGains( MIN_FREQ,
				   MAX_FREQ,
				   nOfSubBands,
				   tableName );
    if ( !ok ) {
      retval = 1;
      cout << "Error...early exit"
	   << endl;
    }
  }
  // ---------------------------------------------------------------------------
  // test exportBaselineGains function
  //   with smaller dimensions than at construction
  if ( ok ) {
    Float minFreq ( 200000000 ); // 20 MHz
    Float maxFreq ( 350000000 ); // 35 MHz
    Int nOfSubBands ( 40 );
    String tableName ( "baseline_gains_2" );
    ok = test_exportBaselineGains( minFreq,
				   maxFreq,
				   nOfSubBands,
				   tableName );
    if ( !ok ) {
      retval = 1;
      cout << "Error...early exit"
	   << endl;
    }
  }
  // ---------------------------------------------------------------------------
  // test exportBaselineGains function
  //   with same dimensions but much finer resolution
  if ( ok ) {
    Int nOfSubBands ( 500 );
    String tableName ( "baseline_gains_3" );
    ok = test_exportBaselineGains( MIN_FREQ,
				   MAX_FREQ,
				   nOfSubBands,
				   tableName );
    if ( !ok ) {
      retval = 1;
      cout << "Error...early exit"
	   << endl;
    }
  }
  // ---------------------------------------------------------------------------
  return retval;
} // --- end main
