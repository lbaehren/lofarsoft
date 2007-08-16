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

// CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <scimath/Mathematics.h>

// LOPES-tools header files
#include <templates.h>
#include <Calibration/BaselineGain.h>
#include <Calibration/AverageGain.h>
#include <IO/DataReader.h>
#include <Data/ITSCapture.h>
#include <Utilities/ProgressBar.h>

using casa::Matrix;
using casa::Vector;

/*!
  \file ascii_export_baseline.cc
  
  \brief An export of ASCII tables from AmplitudeGain and AverageGain classes.
  
  \author Brett Deaton
  
  \date 2005/07

  This application uses DataReader to extract real data from disk, uses
  AmplitudeGain and AverageGain objects to scan for baseline, and exports the
  baseline to an ASCII table. It provides a minimal command line interface.

  <h3>Constants set (explicitly and implicitly) by the user.</h3>

  Definitions:
  <ul>
    <li>block   : subsection of the time-domain samples to FFT into a single
             spectra. Numbered from 1 (following DataReader convention.
             This is the only member numbered from 1.)
    <li>group   : subsection of blocks to scan for a minimum baseline. Groups are
             averaged together if nOfGroups is larger than unity. Groups
             are numbered from 0 (different than blocks.)
    <li>subband : subsection of freq-domain samples to search for a min value.
             Subbands are numbered from 0 (different from blocks.)
  </ul>
*/

// --- Function prototypes -----------------------------------------------------

/*!
  \brief Normalize the antenna spectra

  \param dr   -- A DataReader object, used for I/O
  \param avgg -- A AverageGain object, performing the normalization of the raw
                 spectra

  \return normSpectraReal -- normalized spectra
 */
Matrix<Double> makeNormalization (DataReader *dr,
				  AverageGain& avgg );

/*!
  \brief Set up the vector of frequency values
*/
Vector<Double> makeFreqVector (const int& nOfChan);

// --- Global variables ------------------------------------------------------

// Constants for this specific Data Set
// These must be changed if a new set of data is used
const Double MIN_FREQ ( 0.0 ); // 0.0 MHz
const Double MAX_FREQ ( 40000000 ); // 40 MHz
const int N_TOT_SAMP ( 33554432 ); // 32 MSamples (32 * 2^20)
const int N_ANT ( 10 ); // number of antennas; specific to the dataset
int blocksize; // sample width per block
int nOfTotBlocks; // number of blocks in whole data set (depends on blocksize)
int nOfBlocks; // number of blocks to be scanned for a baseline
int nOfTotGroups; // number of groups of blocks in whole dataset
int nOfGroups; // number of groups to be averaged
int nOfSubBands; // number of subbands; for baseline scanning
int whichMethod; // scanning method (MED=0 MEAN=1, MIN=2)
Bool doNormalization; // if true, export a normalized spectrum
IPosition dimGains ( 2 ); // dimensions of the gain curves matrix (2D)


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


// --- stringMethod ------------------------------------------------------------
// -----------------------------------------------------------------------------

String stringMethod()
{
  String stringMethod;
  switch ( whichMethod )
    {
    case 0: {
      stringMethod = "median";
      break;
    }
    case 1: {
      stringMethod = "mean";
      break;
    }
    case 2: {
      stringMethod = "minimum";
      break;
    }
    default: {
      stringMethod = "[ERROR] method unknown";
      break;
    }
    }
  return stringMethod;
}


// --- makeFreqVector ----------------------------------------------------------
// -----------------------------------------------------------------------------

/*!
  \brief Generate a frequency reference vector
*/
Vector<Double> makeFreqVector( const int& nOfChan )
{
  Vector<Double> freqVector ( nOfChan, 0.0 ); // length=nOfChan, initialize zero
  // calculate frequency sub band interval
  Double interval ( ( MAX_FREQ - MIN_FREQ ) / nOfChan );
  // Declare frequency storage double. Set to midpoint of first channel.
  Double frequency = MIN_FREQ + ( interval / 2 );
  // loop through each band and calculate the midpoint frequency
  for ( int chan ( 0 ); chan < nOfChan; chan++ ) {
    freqVector( chan ) = frequency;
    frequency += interval; // increment frequency by one channel interval
  } // end for: chan
  return freqVector;
}


// --- exportTable -------------------------------------------------------------
// -----------------------------------------------------------------------------

// Export a matrix to an ASCII table. Append a frequency reference vector.
//
// block      -- matrix to export, [freq,ant]. Numbered from 1.
// fileNumber -- number of file to be exported (1st, 2nd, etc)
// isNormSpectra -- whether or not data being exported is normalized spectra
void exportTable( const Matrix<Double>& block,
		  const int& fileNumber,
		  const Bool& isNormSpectra )
{
  // use nOfAntennas instead of global N_ANT, to be flexible for errors
  int nOfAntennas ( block.ncolumn() );
  int nOfFreqChan ( block.nrow() );
  Vector<Double> frequencies ( makeFreqVector( nOfFreqChan ) );
  ostringstream filename;
  ofstream datafile;
  // create filename
  if ( isNormSpectra ) {
    filename << "normalized_spectrum"<< fileNumber << ".dat";
  } else {
    filename << "gains" << fileNumber << ".dat";
  }
  cout << " -- Exporting data to file "
       << filename.str()
       << "... "
       << endl;
  // open filestream for this data
  datafile.open( filename.str().c_str(), ios::out );
  // BUILD METADATA HEADER **************
  if ( isNormSpectra ) {
    datafile << "# ASCII table of gain curves." << endl;
  } else {
    datafile << "# ASCII table of normalized spectrum." << endl;
  }
  datafile << "# - blocksize:                  "
	   << ( blocksize / 1024 )
	   << "K"
	   << endl;
  datafile << "# - number of sub bands:        "
	   << nOfFreqChan
	   << endl;
  datafile << "# - scanning method:            "
	   << stringMethod()
	   << endl;
  datafile << "# - number of blocks per group: "
	   << nOfBlocks
	   << endl;
  datafile << "# - number of groups averaged:  "
	   << nOfGroups
	   << endl;
  datafile << "#" << endl;
  for ( int col ( -1 ); col < nOfAntennas; col++ ) {
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
  CR::ProgressBar bar ( nOfFreqChan );
  bar.update( 0 );
  // loop across individual channels
  for ( int chan ( 0 ); chan < nOfFreqChan; chan++ ) {
    // loop across frequency column (ant=-1) and antennas (ant!=-1)
    for ( int ant ( -1 ); ant < nOfAntennas; ant++ ) {
      if ( ant == -1 ) {
	datafile << frequencies( chan )
		 << "   \t"
		 << flush;
      } else if ( ant == ( nOfAntennas - 1 ) ) {
	datafile << block( chan, ant )
		 << flush;
      } else {
	datafile << block( chan, ant )
		 << "   \t"
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

/*!
  \brief Get parameters from the user. Set the following:

  \param blocksize: sample width of blocks
  \param nOfTotBlocks: total number of blocks with width = blocksize in dataset
  \param dimGains: dimensions of the gain curves matrix
  \param whichMethod: scanning method (MED=0, MEAN=1, MIN=2)
  \param nOfSubBands: number of sub bands per block to scan for min baseline
  \param nOfBlocks: number of blocks to scan per group
  \param nOfGroups: number of groups to average together
  \param nOfTotGroups: total number of groups with width = nOfBlocks in dataset
  \param doNormalization: boolean whether or not to normalize a spectra also
*/
void setParameters()
{
  // User-defined Parameters
  Bool set (False);
  cout << " -- Enter Integer Value Parameters" << endl;
  // blocksize
  int KSamples (0);
  do {
    cout << " blocksize (in KSamples): ";
    cin >> KSamples;
    if ( ( KSamples >= 1 ) and ( KSamples <= 256 ) ) {
      set = True;
      blocksize = KSamples * 1024;
      nOfTotBlocks = N_TOT_SAMP / blocksize; // use this in last do-while loop
    } else {
      cout << " [ERROR: setParameters: blocksize]"
	   << " Input value out of range (1K-256K)\n";
      set = False;
    }
  } while (!set); // end do-while: blocksize
  // nOfSubBands
  do {
    cout << " number of sub bands: ";
    cin >> nOfSubBands;
    if ( ( nOfSubBands <= blocksize ) and ( nOfSubBands > 0 ) ) {
      // dimensions of spectra
      dimGains( 0 ) = nOfSubBands;
      dimGains( 1 ) = N_ANT;
      set = True;
    } else {
      cout << " [ERROR: setParameters: nOfSubBands]"
	   << " Input value out of range (1-blocksize)\n";
      set = False;
    }
  } while (!set); // end do-while: nOfSubBands
  // whichMethod
  do {
    cout << " scanning method (MED=0, MEAN=1, MIN=2): ";
    cin >> whichMethod;
    if ( whichMethod == 0
	 or
	 whichMethod == 1
	 or
	 whichMethod == 2
	 ) {
      set = True;
    } else {
      cout << " [ERROR: setParameters: whichMethod]"
	   << " Input value not a scanning method (MED=0, MEAN=1, MIN=2)\n";
      set = False;
    }
  } while (!set); // end do-while: nOfSubBands
  // Average baselines?
  Char avg ('n');
  do {
    cout << " average the baselines? (y/n): ";
    cin >> avg;
    if ( ( avg == 'n' ) or ( avg == 'N' ) ) {
      // nOfBlocks
      do {
	cout << " number of blocks to be scanned: ";
	cin >> nOfBlocks;
	if ( ( nOfBlocks <= nOfTotBlocks ) and ( nOfBlocks > 0 ) ) {
	  nOfTotGroups = ( nOfTotBlocks / nOfBlocks );
	  set = True;
	} else {
	  cout << " [ERROR: setParameters: nOfBlocks]"
	       << " Input value out of range (1-"
	       << ( nOfTotBlocks )
	       << ")\n";
	  set = False;
	}
      } while (!set);
      nOfGroups = 1; // since don't avg baselines, all blocks are in 1 grp
      set = True;
    } else if ( ( avg == 'y' ) or ( avg == 'Y' ) ) {
      // nOfBlocks
      do {
	cout << " number of blocks to be scanned per group: ";
	cin >> nOfBlocks;
	if ( ( nOfBlocks <= nOfTotBlocks ) and ( nOfBlocks > 0 ) ) {
	  nOfTotGroups = ( nOfTotBlocks / nOfBlocks );
	  set = True;
	} else {
	  cout << " [ERROR: setParameters: nOfBlocks]"
	       << " Input value out of range (1-"
	       << ( nOfTotBlocks )
	       << ")\n";
	  set = False;
	}
      } while (!set);
      // nOfGroups
      do {
	cout << " number of groups to average: ";
	cin >> nOfGroups;
	if ( ( nOfGroups <= nOfTotGroups ) and ( nOfGroups > 0 ) ) {
	  set = True;
	} else {
	  cout << " [ERROR: setParameters: nOfGroups]"
	       << " Input value out of range (1-"
	       << ( nOfTotGroups )
	       << ")\n";
	  set = False;
	}
      } while (!set);
      set = True;
    } else {
      cout << " [ERROR: setParameters: Average Baselines?]"
	   << " Input character not 'y' or 'n'\n";
      set = False;
    }
  } while (!set); // end do-while: average baselines?
  // doNormalization
  char normalize('n');
  do {
    cout << " export normalized spectrum with respect to this baseline? (y/n): ";
    cin >> normalize;
    if ( ( normalize == 'y' ) or ( normalize == 'Y' ) ) {
      doNormalization = True; // do normalize a spectrum
      set = True;
    } else if ( normalize == 'n' or normalize == 'N' ) {
      doNormalization = False; // don't normalize a spectrum
      set = True;
    } else {
      cout << " [ERROR: setParameters: doNormalization]"
	   << " Input character not 'y' or 'n'\n";
      set = False;
    }
  } while (!set); // end do-while: nOfSubBands
}


// --- makeBaselines -----------------------------------------------------------
// -----------------------------------------------------------------------------

// Make the baselines.
void makeBaselines( DataReader *dr,
		    AverageGain& avgg )
{
//   // dimensions of the spectra (2D)
//   IPosition dimSpectra (dr->shape("fft"));
//   // prepare a frequency reference vector for interpolation of gain curve
//   Vector<Double> frequencies ( makeFreqVector( dr->fftLength() ) );
//   // construct AmplitudeGain object
//   AmplitudeGain ampg ( dimSpectra,
// 		       nOfSubBands,
// 		       MIN_FREQ,
// 		       MAX_FREQ,
// 		       whichMethod );
//   if ( doNormalization ) {
//     avgg.reset( dimSpectra,
// 		MIN_FREQ,
// 		MAX_FREQ );
//   } else {
  // set method
  switch ( whichMethod )
    {
    case 0: {
      avgg.setMethod( BaselineGain::MED );
      break;
    }
    case 1: {
      avgg.setMethod( BaselineGain::MEAN );
      break;
    }
    case 2: {
      avgg.setMethod( BaselineGain::MIN );
      break;
    }
    default: {
      avgg.setMethod( BaselineGain::MED );
      break;
    }
    }
  avgg.reset();
//   }
  cout << " -- Scanning for Baselines..." << endl;
  CR::ProgressBar bar ( nOfGroups );
  bar.update( 0 );
  // Each iteration of the outer for-loop scans a subgroup, then adds the
  //   baseline gain curve to the AverageGain object. The outer for-loop
  //   exits after the counter is equal to the final subgroup.
  //   If nOfGroups is set to 1, the outer loop acts as if it isn't there.
  //   This is what we want to happen if the user doesn't want to average
  //   the baselines.
  for ( int grp ( 0 ); grp < nOfGroups; grp++ ) {
//     // Reset the baseline gains so that the subgrp is scanned afresh.
//     ampg.resetBaselineGains();
    // Inner for-loop.
    //   Each iteration of the inner for-loop scans the current block for a
    //   baseline and readjusts the stored baseline gain curve accordingly
    //   (always fitting a minimum). (In this loop we will count blocks from
    //   zero instead of 1 even though DataReader counts from 1.
    for ( int blck ( 0 ); blck < nOfBlocks; blck++ ) {
      avgg.extractBaselineGains( dr->fft() ); // scan fft spectra for min baselines
      dr->nextBlock(); // increment dr to next block
    } // end for: blck
//     if ( doNormalization ) {
//       // interpolate the gains to all the points on the spectrum
//       avgg.addBlock( ampg.getBaselineGains( frequencies ) );
//     } else {
    avgg.addGroupToAverage(); // add baseline curve as new
                              // group to be averaged
//     }
    bar.update( grp + 1 );
  } // end for: grp
  cout << " -- ...ok" << endl;
}


// --- makeNormalization -------------------------------------------------------
// -----------------------------------------------------------------------------

// Make the normalized spectra.
Matrix<Double> makeNormalization( DataReader *dr,
				 AverageGain& avgg )
{
  // read raw spectra from disk
  Matrix<DComplex> rawSpectra (dr->fft());
  // normalize the spectra
  avgg.normalizeSpectra(rawSpectra);
  Matrix<Double> normSpectraReal (rawSpectra.shape());
  normSpectraReal = amplitude( rawSpectra );
//   cout << " -- ...ok" << endl;
  return normSpectraReal;
}



// *****************************************************************************
// *** Main routine ************************************************************
// *****************************************************************************

int main()
{
  // test declaration
  cout << " ---------------------------------------\n"
       << " -- Export ASCII Tables of Baselines ---\n"
       << " -- Data from LOFAR ITS ----------------\n"
       << " -- 2004.11.02 -------------------------\n"
       << " ---------------------------------------"
       << endl;
  // ************ Initial Build/Export ***************
  int fileNumber ( 1 ); // number of data set about to be built and exported
  cout << " -- Build and export 'gains"
       << fileNumber
       << ".dat'"
       << endl;
  // set the parameters
  setParameters();
  // Construct DataReader object; stride default is zero
  DataReader *dr;
  ITSCapture *capture = new ITSCapture (makeFilenamesVector(),
					blocksize );
  dr = capture;
  // dimensions of the spectra (2D)
  IPosition spectraShape (dr->nofSelectedChannels());
  // construct AverageGain object
  AverageGain avgg ( spectraShape,
		     MIN_FREQ,
		     MAX_FREQ,
		     nOfSubBands );
  // make the baseline gain curves
  makeBaselines( dr,
		 avgg );
  // export baselines
  exportTable( avgg.getAverageGains(),
	       fileNumber,
	       False );
  // make and export normalized spectrum if requested
  if ( doNormalization ) {
    exportTable( makeNormalization( dr, avgg ),
		 fileNumber,
		 True );
  } // end if: doNormalization
  fileNumber++;
  // ************ Subsequent Builds/Exports **********
  Char xport ('n'); // sentinel loop control
  do {
    cout << " Build and export 'gains"
	 << fileNumber
	 << ".dat'? (y/n): ";
    cin >> xport;
    if ( ( xport == 'y' ) or ( xport == 'Y' ) ) {
      setParameters();
      // reset DataReader Object
      dr->setBlocksize( blocksize );
      dr->setBlock(1);
      // make the baseline gain curves
      makeBaselines( dr,
		     avgg );
      // export baselines
      exportTable( avgg.getAverageGains(),
		   fileNumber,
		   False );
      // make and export normalized spectrum if requested
      if ( doNormalization ) {
	exportTable( makeNormalization( dr, avgg ),
		     fileNumber,
		     True );
      } // end if: doNormalization
      fileNumber++;
    } else {
      cout << " -- Closing Application" << endl;
      xport = 'n';
    }
  } while ( ( xport == 'y' ) or ( xport == 'Y' ) );

  return 0;
}

