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

/* $Id: tAverageGain.cc,v 1.7 2006/07/12 10:13:16 bahren Exp $ */

// LOPES tools header files
#include <templates.h>
#include <Calibration/AverageGain.h>

/*!
  \file tAverageGain.cc
  
  \ingroup Calibration
  
  \brief A collection of tests for AverageGain
  
  \author Brett Deaton
  
  \date 2005/06
  
  This test application merely generates fake blocks of data and sends them
  through the AverageGain functions.
*/


// *****************************************************************************
// *** Support Routines ********************************************************
// *****************************************************************************

// --- exportAverageData -------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Export a block to an ASCII table
  
  CURRENTLY UNUSED. Remains in order to provide means for a more robust test of
  real data blocks.
  
  \param block -- block of data to export, [freq,ant].
*/
// WARNING: this will overwrite any file bearing the same name in the relevant
//   directory. filename = "average_gains" + fileNumber + ".dat"
void exportTable( const Matrix<Float>& block,
		  const Int& fileNumber )
{
  Int nOfAntennas ( block.ncolumn() );
  Int nOfFreqBins ( block.nrow() );
  ostringstream filename;
  ofstream datafile;
  // create filename
  filename << "average_gains" << fileNumber << ".dat";
  cout << " - Exporting data to file "
       << filename.str()
       << " ... "
       << flush;
  // open filestream for this data
  datafile.open( filename.str().c_str(), ios::out );
  // loop across individual channels
  for ( Int chan ( 0 ); chan < nOfFreqBins; chan++ ) {
    // loop across antennas
    for ( Int ant ( 0 ); ant < nOfAntennas; ant++ ) {
      datafile << block( chan, ant ) << "\t" << flush;
    } // end for: ant
    datafile << "\n" << flush;
  } // end for: chan
  // close filestream
  datafile.close();
  cout << "ok" << endl;
}


// *****************************************************************************
// *** Test routines ***********************************************************
// *****************************************************************************

// --- test_AverageGain --------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Test AverageGain constructors.
  
  \return ok -- Status of the test routine.
*/
Bool test_AverageGain()
{
  Bool ok ( True );
  // default constructor
  cout << "[#] Default Constructor test" << endl;
  try {
    AverageGain average ();
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  // Argumented constructor
  cout << "[#] Argumented Constructor test"
       << endl;
  try {
    Int nAnt (3);
    Int nBin (10);
    Int nofSubBands (100);
    IPosition dimensions ( 2, nBin, nAnt );
    Float minFreq ( 40000000 ); // 40 MHz
    Float maxFreq ( 80000000 ); // 80 MHz
    AverageGain average (dimensions,
			 minFreq,
			 maxFreq,
			 nofSubBands);
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  // return test results
  return ok;
}

// --- stresstest_addBlock -----------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Stresstest addBlock.
  
  This functions tries to add a block of data that is of different dimensions
  than at construction. The correct response of AverageGain should be an error.
  Even though the called function throws an error, it doesn't call an AipsError,
  which is what the catch block is looking for. Hence, the return value of ok
  (as long as everything else executes correctly) is True.

  \todo Routine needs to be updated, since method <i>addBlock</i> no longer
  exists.
  
  \return ok -- Status of the test routine.
*/
Bool stresstest_addBlock( AverageGain& average )
{
  Bool ok ( True );
  cout << "[#] addBlock stresstest (should return error)" << endl;
  Int nBin = 15;
  Int nAnt = 5;
  Float initVal = 0.0;
  Matrix<Float> tooBigBlock ( nBin, nAnt, initVal ); // 15 * 5 matrix bigger
                                                     // than the one at
                                                     // construction ( 10 * 3 )
  try {
//     average.addBlock( tooBigBlock );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  return ok;
}


// --- test_getNBlocks ---------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Test getNBlocks.

  Function called (getNBlocks) should return 3 (three blocks were added in
  test_addBlock function).
  
  \todo Routine needs to be updated, since method <i>addBlock</i> no longer
  exists.
  
  \return ok -- Status of the test routine.
*/
Bool test_getNBlocks( AverageGain& average )
{
  Bool ok ( True );
  cout << "[#] getNBlocks test" << endl;
  Int nBlocks = 0;
  try {
//     nBlocks = average.getNBlocks();
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  if ( nBlocks != 3 )
    ok = False;
  return ok;
}


// --- test_getAverage ---------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Test getAverage
  
  Three blocks of data have been added (in test_addBlock function):
  block1 = 1.0
  block2 = 5.0
  block3 = 6.0
  Hence, the returned average matrix's members should be equal to 4.0.

  \todo Routine needs to be updated, since method <i>getAverages</i> no longer
  exists.
  
  \return ok -- Status of the test routine.
*/
Bool test_getAverage( AverageGain& average )
{
  Bool ok ( True );
  cout << "[#] getAverage test" << endl;
  Int nBin = 10;
  Int nAnt = 3;
  Float initVal = 0.0;
  Matrix<Float> avg ( nBin, nAnt, initVal );
  try {
//     avg = average.getAverages();
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  if ( ok )
    cout << " -- the averaged matrix is:\n"
	 << avg
	 << endl;
  return ok;
}


// --- test_exportAverages -----------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Test exportAverages
  
  Test the exportAverages function. Should export a 10*3 table of the
  averages of the added blocks (a table populated by 4.0).

  \todo Routine needs to be updated, since method <i>exportAverages</i> no longer
  exists.
  
  \return ok -- Status of the test routine.
*/
Bool test_exportAverages( AverageGain& average )
{
  Bool ok ( True );
  cout << "[#] exportAverages test" << endl;
  try {
//     average.exportAverages( "average_gains" );
  } catch( AipsError x ) {
    cerr << x.getMesg() << endl;
    ok = False;
  }
  return ok;
}



// *****************************************************************************
// *** Main routine ************************************************************
// *****************************************************************************

int main()
{
  // test declaration
  cout << " -----------------------------------------\n"
       << " --- Testing the AverageGain functions ---\n"
       << " -----------------------------------------"
       << endl;
  // set test variables
  Bool ok = True;
  Int retval = 0;
  // ---------------------------------------------------------------------------
  // test constructors
  ok = test_AverageGain();
  if ( !ok )
    retval = 1;
  // ---------------------------------------------------------------------------
  // remaining test functions depend on the first test function to have worked
  if ( ok ) {
    // construct AverageGain object to pass around the remaining test functions
    Int nAnt = 3;
    Int nBin = 10;
    Int nofSubBands (100);
    IPosition dimensions ( 2, nBin, nAnt );
    Float minFreq ( 40000000 ); // 40 MHz
    Float maxFreq ( 80000000 ); // 80 MHz
    AverageGain average ( dimensions,
			  minFreq,
			  maxFreq,
			  nofSubBands);

    // -------------------------------------------------------------------------
    // stresstest addBlock
    if ( ok ) {
      ok = stresstest_addBlock( average );
      if ( !ok )
	retval = 1;
    }

    // -------------------------------------------------------------------------
    // test getNBlocks
    if ( ok ) {
      ok = test_getNBlocks( average );
      if ( !ok )
	retval = 1;
    }

    // -------------------------------------------------------------------------
    // test getAverage
    if ( ok ) {
      ok = test_getAverage( average );
      if ( !ok )
	retval = 1;
    }

    // -------------------------------------------------------------------------
    // test exportAverages
    if ( ok ) {
      ok = test_exportAverages( average );
      if ( !ok )
	retval = 1;
    }
    // -------------------------------------------------------------------------
  } // end if
  return retval;
}
