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

/*!
  \file ascii_export_spectra.cc
  
  \brief An export of an ASCII table of FFT spectra from DataReader.
  
  \author Brett Deaton
  
  \date 2005/07

  This application uses DataReader to extract real data from disk and
  exports that to an ASCII table. It provides a minimal command line interface.
*/

// CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>

// LOPES-tools header files
#include <lopes/Data/ITSCapture.h>
#include <lopes/IO/DataReader.h>
#include <lopes/Utilities/ProgressBar.h>

#include <casa/namespace.h>



// *****************************************************************************
// *** Global Variables ********************************************************
// *****************************************************************************

// Constants for this specific Data Set
// These must be changed if a new set of data is used
const Double MIN_FREQ ( 0.0 ); // 0.0 MHz
const Double MAX_FREQ ( 40000000 ); // 40 MHz
const Int N_TOT_SAMPLES ( 33554432 ); // 32 MSamples (32 * 2^20)
const Int N_ANT ( 10 ); // number of antennas; specific to the dataset
// Constants set (explicitly and implicitly) by the user
//// Definitions:
//// block   : subsection of the time-domain samples to FFT into a single spectra
Int blocksize; // sample width of the blocks used by DataReader object
Int nTotBlocks; // number of blocks available with blockwidth = blocksize


// *****************************************************************************
// *** Support routines ********************************************************
// *****************************************************************************

// --- makeFilenamesVector -----------------------------------------------------
// -----------------------------------------------------------------------------

// must adjust this function according to where the data one wants to use
//   is stored locally. UGLY Hack.
//   The data I am using for this test is a set that used only 10 antennas. It
//   was recorded 2 Nov 2004.
String makeFilenamesVector()
{
  String filename;

  filename = "/data/ITS/2004.11/24h/2004.11.02-23:00:14.86-24h/experiment.meta";

  return filename;
}


// --- makeFreqVector ----------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Generate a frequency reference vector
*/
Vector<Double> makeFreqVector( const Int& nOfChan )
{
  Vector<Double> freqVector ( nOfChan, 0.0 ); // length=nOfChan, initialize zero
  // calculate frequency sub band interval
  Double interval ( ( MAX_FREQ - MIN_FREQ ) / nOfChan );
  // Declare frequency storage double. Set to midpoint of first channel.
  Double frequency = MIN_FREQ + ( interval / 2 );
  // loop through each band and calculate the midpoint frequency
  for ( Int chan ( 0 ); chan < nOfChan; chan++ ) {
    freqVector( chan ) = frequency;
    frequency += interval; // increment frequency by one channel interval
  } // end for: chan
  return freqVector;
}


// --- exportTable -------------------------------------------------------------
// -----------------------------------------------------------------------------

// Export a matrix to an ASCII table. Append a frequency reference vector.
//
// block      -- matrix to export, [freq,ant]
// fileNumber -- number of file to be exported (1st, 2nd, etc)
void exportTable( const Matrix<Double>& block,
		  const Int& fileNumber )
{
  // use nOfAntennas instead of global N_ANT, to be flexible for errors
  Int nOfAntennas ( block.ncolumn() );
  Int nOfFreqChan ( block.nrow() );
  Vector<Double> frequencies ( makeFreqVector( nOfFreqChan ) );
  ostringstream filename;
  ofstream datafile;
  // create filename
  filename << "spectra" << fileNumber << ".dat";
  cout << " -- Exporting data to file "
       << filename.str()
       << " ... "
       << endl;
  // open filestream for this data
  datafile.open( filename.str().c_str(), ios::out );
  // BUILD METADATA HEADER **************
  datafile << "#" << endl;
  datafile << "# ASCII table of FFT amplitudes (spectra)" << endl;
  datafile << "# - blocksize:          "
	   << ( blocksize / 1024 )
	   << "K"
	   << endl;
  datafile << "# - Antennas:           "
	   << nOfAntennas
	   << endl;
  datafile << "# - Frequency Channels: "
	   << nOfFreqChan
	   << endl;
  datafile << "#" << endl;
  for ( Int col ( -1 ); col < nOfAntennas; col++ ) {
    if ( col == -1 ) {
      datafile << "#Freq"
	       << "   \t"
	       << flush;
    } else if ( col == ( nOfAntennas - 1 ) ) {
      datafile << "#Ant"
	       << col
	       << flush;
    } else {
      datafile << "#Ant"
	       << col
	       << "   \t"
	       << flush;
    }
  } // end for: col
  datafile << "\n" << flush;
  // FILL FIELDS WITH DATA **************
  // Display progress
  LOPES::ProgressBar bar ( nOfFreqChan );
  bar.update( 0 );
  // loop across individual channels
  for ( Int chan ( 0 ); chan < nOfFreqChan; chan++ ) {
    // loop across frequency column (ant=-1) and antennas (ant!=-1)
    for ( Int ant ( -1 ); ant < nOfAntennas; ant++ ) {
      if ( ant == -1 ) {
	datafile << frequencies( chan )
		 << "\t"
		 << flush;
      } else if ( ant == ( nOfAntennas - 1 ) ) {
	datafile << block( chan, ant )
		 << flush;
      } else {
	datafile << block( chan, ant )
		 << "\t"
		 << flush;
      }
    } // end for: ant
    datafile << "\n" << flush;
    bar.update( chan + 1 );
  } // end for: chan
  // close filestream
  datafile.close();
  cout << " -- ...ok" << endl;
}


// --- setParameters -----------------------------------------------------------
// -----------------------------------------------------------------------------

// Get parameters from the user.
void setParameters()
{
  // User-defined Parameters
  Bool set (False);
  Int KSamples (0);
  cout << " -- Enter Integer Value Parameters" << endl;
  // blocksize
  do {
    cout << " blocksize (in KSamples): ";
    cin >> KSamples;
    if ( ( KSamples >= 1 ) and ( KSamples <= 256 ) ) {
      set = True;
      blocksize = KSamples * 1024;
      nTotBlocks = N_TOT_SAMPLES / blocksize;
      cout << " -- total number of blocks available: "
	   << nTotBlocks
	   << endl;
    } else {
      cout << " [ERROR] Input value out of range (1K-256K)\n";
      set = False;
    }
  } while (!set);
}



// *****************************************************************************
// *** Main routine ************************************************************
// *****************************************************************************

int main()
{
  // test declaration
  cout << " ---------------------------------------\n"
       << " -- Export ASCII Tables of FFTs --------\n"
       << " -- Data from LOFAR ITS ----------------\n"
       << " -- 2004.11.02 -------------------------\n"
       << " ---------------------------------------"
       << endl;
  // Construct DataReader object; stride default is zero
  setParameters();
  DataReader *dr;
  ITSCapture *capture = new ITSCapture ( makeFilenamesVector(),
					 blocksize );
  dr = capture;
  Int fileNumber ( 1 ); // number of the file (and block to be exported)
  Char xport ('n'); // sentinel loop control
  do {
    cout << " Export block "
	 << fileNumber
	 << " to 'spectra" << fileNumber << ".dat'? (y/n): ";
    cin >> xport;
    if ( ( xport == 'y' ) or ( xport == 'Y' ) ) {
      // export this block to an ascii table
      dr->setBlock( fileNumber );
      exportTable( amplitude( dr->fft() ),
		   fileNumber );
      fileNumber++;
    } else {
      xport = 'n';
    }
  } while ( ( ( xport == 'y' )
	      or
	      ( xport == 'Y' ) )
	    and
	    ( fileNumber <= nTotBlocks) );
  // close
  cout << " -- Closing Application" << endl;
}
