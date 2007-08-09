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

// Class AverageGain
// Implementation of the AverageGain functions
// 
// author: Brett Deaton
// 8-05


// include class definition
#include <Calibration/AverageGain.h>



// *****************************************************************************
// *** PUBLIC FUNCTIONS ********************************************************
// *****************************************************************************

// -----------------------------------------------------------------------------
//
//  Construction
//
// -----------------------------------------------------------------------------

// ------------------------------------------------------------------ AverageGain

AverageGain::AverageGain()
  : BaselineGain()
{
  // default dimensions are 50(frequency channels) * 10(antennas) matrix
  Int nOfFreq ( 50 );
  Int nOfAnt ( 10 );
  IPosition spectraShape ( 2, nOfFreq, nOfAnt );
  Double minFreq ( 0.0 ); // 0.0 MHz
  Double maxFreq ( 40000000 ); // 40 MHz
  Vector<Double> spectraAbscissa ( BaselineGain::makeFreqAbscissa( minFreq,
								  maxFreq,
								  nOfFreq ) );
  // default number of subbands is 10
  Int nOfSubBands ( 10 );
  AverageGain::reset( spectraShape,
		      spectraAbscissa,
		      nOfSubBands );
}

// ------------------------------------------------------------------ AverageGain

AverageGain::AverageGain( const IPosition& spectraShape,
			  const Vector<Double>& spectraAbscissa,
			  const Int& nOfSubBands,
			  BaselineGain::Method whichMethod )
  : BaselineGain( spectraShape,
		  spectraAbscissa,
		  nOfSubBands,
		  whichMethod )
{
  AverageGain::reset( spectraShape,
		      spectraAbscissa,
		      nOfSubBands );
}

// ------------------------------------------------------------------ AverageGain

AverageGain::AverageGain (const IPosition& spectraShape,
			  const Double& freqMin,
			  const Double& freqMax,
			  const Int& nOfSubBands)
  : BaselineGain()
{;}

// --- Destruction -------------------------------------------------------------
// -----------------------------------------------------------------------------

// Empty Destructor
AverageGain::~AverageGain()
{;}


// --- reset -------------------------------------------------------------------
// -----------------------------------------------------------------------------

// default dimensions reset
void AverageGain::reset()
{
  // reset base class members
  BaselineGain::reset();
  // reset AverageGain class members
  nGroupsAveraged_p = 0;
  sumAvgGains_p = 0;
}


// reset just like at construction (except not the scanning method)
void AverageGain::reset( const IPosition& spectraShape,
			 const Vector<Double>& spectraAbscissa,
			 const Int& nOfSubBands )
{
  // reset base class members
  BaselineGain::reset( spectraShape,
		       spectraAbscissa,
		       nOfSubBands );
  // reset AverageGain class members
  AverageGain::setGainShape( nOfSubBands,
			     spectraShape( 1 ) );
  nGroupsAveraged_p = 0;
  sumAvgGains_p.resize( nOfSubBands,
			spectraShape( 1 ) ); 
  sumAvgGains_p = 0;
}

// --- setMethod ---------------------------------------------------------------
// -----------------------------------------------------------------------------

void AverageGain::setMethod( const BaselineGain::Method& whichMethod )
{
  // set the method and reset the gain curves
  whichMethod_p = whichMethod;
  AverageGain::reset();
} // --- end function: setMethod


// --- getNGroupsAveraged ------------------------------------------------------
// -----------------------------------------------------------------------------

// Retrieve number of groups averaged so far
Int AverageGain::getNGroupsAveraged()
{
  return nGroupsAveraged_p;
}


// --- addGroupToAverage -------------------------------------------------------
// -----------------------------------------------------------------------------

// Add the current group to be averaged.
// (i.e. add whatever is stored in baselineGains_p to sumAvgGains_p.)
void AverageGain::addGroupToAverage( const Bool& alsoResetBaselineGains )
{
  // first make sure that at least one baseline has been scanned
  if ( nBlocksScanned_p != 0 ) {
    for ( Int ant ( 0 ); ant < gainShape_p( 1 ); ant++ ) {
      for ( Int band = 0; band < gainShape_p( 0 ); band++ ) {
	// add the relevant baselineGains_p member to present sum of gain curves
	sumAvgGains_p( band, ant ) += baselineGains_p( band, ant );
      } // end for: band
    } // end for: ant
    nGroupsAveraged_p++; // increment number of groups added to average
    if ( alsoResetBaselineGains ) {
      baselineGains_p = 0;
      nBlocksScanned_p = 0;
    }
  } else {
    cerr << "[Error: addGroupToAverage]"
	 << " No blocks have been scanned in the group. No action taken."
	 << endl;
  }
}


// --- normalizeSpectra --------------------------------------------------------
// -----------------------------------------------------------------------------

// Divide the argument spectra by the average baseline gain curves
void AverageGain::normalizeSpectra( Matrix<DComplex>& spectra )
{
  Vector<Bool> antennas ( spectraShape_p( 1 ), True );
  AverageGain::normalizeSpectra( spectra,
				 spectraAbscissa_p,
				 antennas);
} // --- end function: normalizeSpectra

// Divide the argument spectra by the averaged baseline gain curves
//   Identical to the implimentation of the normalizeSpectra function in the
//   BaselineGain class, except that this version, uses the Average gain curve
//   whereas that version uses the baseline gain curve.
void AverageGain::normalizeSpectra( Matrix<DComplex>& spectra,
				    const Vector<Double>& spectraAbscissa,
				    const Vector<Bool>& antennas )
{
  // get the gain curves for the spectra
  Matrix<Double> avgGains ( AverageGain::getAverageGains( spectraAbscissa,
							 antennas ) );
  // determine dimensions based on the two argument vectors
  uInt nOfChan ( spectraAbscissa.nelements() );
  uInt nofAntennas ( BaselineGain::numRequestedAnt( antennas ) );
  Bool allDimensionsAreOK ( True );
  if ( nofAntennas <= 0
       or
       nofAntennas != spectra.ncolumn()
       or
       nOfChan != spectra.nrow()
       ) {
    allDimensionsAreOK = False;
  }
  // counting flag for division by zero error
  Int wasDivError ( 0 );
  // normalize the spectra
  if ( allDimensionsAreOK ) {
    for ( uInt ant(0); ant < nofAntennas; ant++ ) {
      for ( uInt chan(0); chan < nOfChan; chan++ ) {
	// division by zero error flagging
	if ( avgGains( chan, ant ) == 0 ) {
	  wasDivError++;
	}
	spectra( chan, ant ) /= avgGains( chan, ant );
      } // end for: chan
    } // end for: ant
  } else {
    cerr << "[Error: normalizeSpectra]"
	 << " Arguments are of incompatible dimensions."
	 << " Spectra not normalized."
	 << endl;
  }
  if ( wasDivError ) {
    cerr << "[Error: normalizeSpectra]"
	 << " Division by zero. "
	 << wasDivError
	 << " out of "
	 << avgGains.nelements()
	 << " elements."
	 << endl;
  }
}


// --- getAverageGains ---------------------------------------------------------
// -----------------------------------------------------------------------------

Matrix<Double> AverageGain::getAverageGains()
{
  Vector<Bool> antReturn ( gainShape_p( 1 ), True );
  //
  return AverageGain::getAverageGains( gainAbscissa_p,
				       antReturn );
}

Matrix<Double> AverageGain::getAverageGains( const Vector<Bool>& antReturn )
{
  return AverageGain::getAverageGains( gainAbscissa_p,
				       antReturn );
}

Matrix<Double> AverageGain::getAverageGains( const Vector<Double>& freqReturn )
{
  Vector<Bool> antReturn ( gainShape_p( 1 ), True );
  return AverageGain::getAverageGains( freqReturn,
				       antReturn );
}

// Retrieve average baseline gain curve
Matrix<Double> AverageGain::getAverageGains( const Vector<Double>& freqReturn,
					    const Vector<Bool>& antReturn )
{
  // determine number of rows needed for return matrix (number of freq bins)
  Int nRows ( freqReturn.nelements() );
  // determine number columns needed for return matrix (number antennas)
  Int nCols ( BaselineGain::numRequestedAnt( antReturn ) );
  Bool antReturnVectorIsIncorrect ( False );
  if ( nCols <= 0 ) {
    // reset nCols to a value that will make sense for matrix construction
    nCols = 1;
    antReturnVectorIsIncorrect = True;
  }
  // construct return matrix
  Matrix<Double> avgGains ( nRows, nCols, 0.0 );
  // flag for fatal errors, exit if true
  Bool everythingIsAlright ( True );
  // error catch for division by zero
  if ( nGroupsAveraged_p == 0 ) {
    cerr << "[Error: getAverageGains]"
	 << " Exiting to avoid division by zero;"
	 << " no spectra have been scanned yet."
	 << endl;
    everythingIsAlright = False;
  } // end if: nGroupsAveraged_p...
  if ( antReturnVectorIsIncorrect ) {
    cerr << "[Error: getAverageGains]"
	 << " Exiting; antReturn vector is incompatible with expected vector."
	 << endl;
    everythingIsAlright = False;
  } // end if: antReturnVectorIsIncorrect
  if ( everythingIsAlright ) {
    // Cases are to catch any frequency arguments that require no interpolation.
    Int whichCase ( BaselineGain::determineCase( freqReturn,
						 antReturn ) );
    switch ( whichCase )
      {
      case 1: {
	// CASE 1 means:
	//   requested frequencies are identical to stored in gainAbscissa_p
	//   AND
	//   all antennas are requested
	avgGains = sumAvgGains_p / Double ( nGroupsAveraged_p );
	break;
      } // end case: 1
      case 2: {
	// CASE 2 means:
	//   requested frequencies are identical to stored in gainAbscissa_p
	//   BUT
	//   some antennas are not requested
	Int returnAntCounter ( 0 ); // the antenna column in the return matrix
	for ( Int ant ( 0 ); ant < spectraShape_p( 1 ); ant++ ) {
	  // only return requested antennas
	  if ( antReturn( ant ) ) {
	    // return this antenna's column
	    avgGains.column( returnAntCounter )
	      =
	      sumAvgGains_p.column( ant ) / Double ( nGroupsAveraged_p );
	    returnAntCounter++; // increment the return counter
	  } // end if
	} // end for: ant
	break;
      } // end case: 2
      case 3: {
	// CASE 3 means:
	//   requested frequencies different than stored in gainAbscissa_p
	//   (THEREFORE must interpolate the gains)
	Matrix<Double> avgGainCurves ( sumAvgGains_p/Double(nGroupsAveraged_p) );
	avgGains = BaselineGain::interpolateGains( avgGainCurves,
						   gainAbscissa_p,
						   freqReturn,
						   antReturn );
	break;
      } // end case: 3
      default: {
	// if none of the cases, then interpolate nontheless
	Matrix<Double> avgGainCurves ( sumAvgGains_p/Double(nGroupsAveraged_p) );
	avgGains = BaselineGain::interpolateGains( avgGainCurves,
						   gainAbscissa_p,
						   freqReturn,
						   antReturn );
	break;
      } // end case: default
      } // end switch
  } // end if: everythingIsAlright
  return avgGains;
}


// --- exportAverageGains ------------------------------------------------------
// -----------------------------------------------------------------------------

// Default Export Average Gains to AIPS++ Data Table
void AverageGain::exportAverageGains( const String& tableName )
{
  Vector<Bool> antReturn ( spectraShape_p( 1 ), True );
  AverageGain::exportAverageGains( gainAbscissa_p,
				   antReturn,
				   tableName );
} // --- end function: exportAverageGains (default)

// Export Average Gains to AIPS++ Data Table (with default frequencies)
void AverageGain::exportAverageGains( const Vector<Bool>& antReturn,
				      const String& tableName )
{
  AverageGain::exportAverageGains( gainAbscissa_p,
				   antReturn,
				   tableName );
} // --- end function: exportAverageGains (default frequencies)

// Export Average Gains to AIPS++ Data Table (with default antennas)
void AverageGain::exportAverageGains( const Vector<Double>& freqReturn,
				      const String& tableName )
{
  Vector<Bool> antReturn ( spectraShape_p( 1 ), True );
  AverageGain::exportAverageGains( freqReturn,
				   antReturn,
				   tableName );
} // --- end function: exportAverageGains (default antennas)

// Export an average baseline gains to an AIPS++ table. Append a frequeny
//   reference vector. Same as exportBaselineGains function in
//   BaselineGain class.
// Parameters:
// freqReturn -- frequencies which client wants to export
// antReturn  -- antennas which client wants to export (T=export,F=don't export)
// tableName  -- Filename for the exported table
void AverageGain::exportAverageGains( const Vector<Double>& freqReturn,
				      const Vector<Bool>& antReturn,
				      const String& tableName )
{
  // get averageGains matrix
  Matrix<Double> averageGains ( AverageGain::getAverageGains( freqReturn,
							     antReturn ) );
  // export the table
  BaselineGain::exportTable( averageGains,
			     freqReturn,
			     antReturn,
			     tableName );
} // --- end function: exportAverageGains (FUNDAMENTAL)


// *****************************************************************************
// *** PRIVATE FUNCTIONS *******************************************************
// *****************************************************************************

// --- setGainShape ------------------------------------------------------------
// -----------------------------------------------------------------------------

// Set gainShape_p [ nFreq, nofAntennas ]
void AverageGain::setGainShape( const Int& nOfSubBands,
				const Int& nOfAnt )
{
  gainShape_p.resize( 2 );
  gainShape_p( 0 ) = nOfSubBands;
  gainShape_p( 1 ) = nOfAnt;
}

