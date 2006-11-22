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
  \class BaselineGain
*/

#include <Calibration/BaselineGain.h>
#include <Math/MathAlgebra.h>

namespace LOPES {
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // --------------------------------------------------------------- BaselineGain
  
  BaselineGain::BaselineGain ()
  {
    // DComplex Matrix antennaSpectra
    int nOfFreq ( 50 );
    int nOfAnt ( 10 );
    IPosition spectraShape ( 2, nOfFreq, nOfAnt );
    
    // Defining the bandwidth
    double minFreq ( 0.0 );      //  0.0 MHz
    double maxFreq ( 40000000 ); // 40.0 MHz
    
    Vector<double> spectraAbscissa ( BaselineGain::makeFreqAbscissa( minFreq,
								     maxFreq,
								     nOfFreq ) );
    
    int nofSubBands ( 10 );
    
    // supply default parameters to argumented constructor
    BaselineGain::reset( spectraShape,
			 spectraAbscissa,
			 nofSubBands );
  }
  
  // ----------------------------------------------------------------- BaselineGain
  
  BaselineGain::BaselineGain( const IPosition& spectraShape,
			      const Vector<double>& spectraAbscissa,
			      const int& nofSubBands,
			      const BaselineGain::Method& whichMethod )
  {
    whichMethod_p = whichMethod;
    //
    BaselineGain::reset( spectraShape,
			 spectraAbscissa,
			 nofSubBands );
  }
  

// ==============================================================================
//
//  Construction
//
// ==============================================================================

BaselineGain::~BaselineGain ()
{;}


// ==============================================================================
//
//  Methods
//
// ==============================================================================

// ------------------------------------------------------------------------ reset

void BaselineGain::reset()
{
  // reset baselineGains_p to zero. (Really, this is not needed, because
  //  the setGainValue...(*) functions will reset the gain curves if
  //  nBlocksScanned_p
  //  is equal to zero, as we set it below. However, we will leave this
  //  for safety. It is more obvious if your gain curves are zero that something
  //  has gone wrong, than if your gain curve is the previous gain curve.)
  baselineGains_p = 0.0;
  // reset number of blocks scanned to zero
  nBlocksScanned_p = 0;
} // --- end function: reset

// ------------------------------------------------------------------------ reset

void BaselineGain::reset( const IPosition& spectraShape,
			  const Vector<double>& spectraAbscissa,
			  const int& nofSubBands )
{
  //   //  --- Debugging output ---------------------------------
  //   cout << "[BaselineGain::reset]" << endl;
  //   cout << " - spectraShape      : " << spectraShape << endl;
  //   cout << " - (minFreq,maxFreq) : "
  //        << min( spectraAbscissa ) << ", "
  //        << max( spectraAbscissa ) << endl;
  //   cout << " - nofSubBands       : " << nofSubBands << endl;
  //   // -------------------------------------------------------
  // reset the size of the expected spectra blocks
  BaselineGain::setSpectraShape( spectraShape );
  // reset spectral domain
  BaselineGain::setSpectraAbscissa( spectraAbscissa );
  // reset baselineGains_p to specified dimensions
  baselineGains_p.resize( nofSubBands, spectraShape( 1 ) );
  // reset baselineGains_p to zero. (Really, this is not needed, because
  //  the setGainValue...(*) functions will reset the gain curves if
  //  nBlocksScanned_p
  //  is equal to zero, as we set it below. However, we will leave this
  //  for safety. It is more obvious if your gain curves are zero that something
  //  has gone wrong, than if your gain curve is the previous gain curve.)
  baselineGains_p = 0.0;
  // reset gainAbscissa_p
  gainAbscissa_p = BaselineGain::makeFreqAbscissa( min( spectraAbscissa ),
						   max( spectraAbscissa ),
						   nofSubBands );
  BaselineGain::setIntervalEndpoints( spectraShape( 0 ),
				      nofSubBands );
  // reset number of blocks scanned to zero
  nBlocksScanned_p = 0;
} // --- end function: reset

// -------------------------------------------------------------------- setMethod

void BaselineGain::setMethod (const BaselineGain::Method& whichMethod )
{
  // set the method and reset the gain curves
  whichMethod_p = whichMethod;
  BaselineGain::reset();
} // --- end function: setMethod

// --------------------------------------------------------- extractBaselineGains

// baseline gains extractor
void BaselineGain::extractBaselineGains( const Matrix<DComplex>& spectra )
{
  // check to ensure that the argument's dimensions are correct
  if ( spectraShape_p.isEqual( spectra.shape() ) ) {
    switch ( whichMethod_p )
      {
      case MED: {
	setGainValueAsMedian( spectra );
	nBlocksScanned_p++; // increment the number of blocks scanned so far.
	break;
      }
      case MEAN: {
	setGainValueAsMean( spectra );
	nBlocksScanned_p++; // increment the number of blocks scanned so far.
	break;
      }
      case MIN: {
	setGainValueAsMinimum( spectra );
	nBlocksScanned_p++; // increment the number of blocks scanned so far.
	break;
      }
      case STDDEV: {				//##############################
      	setGainValueAsStddeviation( spectra );	//##############################
      	nBlocksScanned_p++;	//increment the number of blocks scanned  so far. 	//###################
      	break;
      }
      default: {
	cerr << "[Error: extractBaselineGains]"
	     << " No scanning method specified; gain curve not extracted."
	     << endl;
	break;
      }
      } // end switch: whichMethod_p
  } // end if
  else {
    // throw error if argument matrix is not same size as given at construction
    cerr << "[ERROR: extractBaselineGains]"
	 <<" Argument matrix is of incompatible shape. Expected dimensions: "
	 << spectraShape_p
	 << endl;
  } // end else
} // --- end function: extractBaselineGains


// --- normalizeSpectra --------------------------------------------------------
// -----------------------------------------------------------------------------

// Divide the argument spectra by the baseline gain curves
void BaselineGain::normalizeSpectra( Matrix<DComplex>& spectra )
{
  Vector<bool> antennas ( spectraShape_p( 1 ), true );
  BaselineGain::normalizeSpectra(spectra,
				 spectraAbscissa_p,
				 antennas);
} // --- end function: normalizeSpectra

// Divide the argument spectra by the baseline gain curves
void BaselineGain::normalizeSpectra( Matrix<DComplex>& spectra,
				     const Vector<double>& frequencies,
				     const Vector<bool>& antennas )
{
  // interpolate gain curves for the spectra
  Matrix<double> gainCurves ( BaselineGain::getBaselineGains( frequencies,
							     antennas ) );
  // determine dimensions based on the two argument vectors
  int nOfChan ( frequencies.nelements() );
  int nAnt ( BaselineGain::numRequestedAnt( antennas ) );
  bool allDimensionsAreOK ( true );
  if ( nAnt <= 0
       or
       abs( nAnt ) != spectra.ncolumn()
       or
       abs( nOfChan ) != spectra.nrow()
       ) {
    allDimensionsAreOK = false;
  }
  // counting flag for division by zero error
  int wasDivError ( 0 );
  // normalize the spectra
  if ( allDimensionsAreOK ) {
    for ( int ant ( 0 ); ant < nAnt; ant++ ) {
      for ( int chan ( 0 ); chan < nOfChan; chan++ ) {
	// division by zero error flagging
	if ( gainCurves( chan, ant ) == 0 ) {
	  wasDivError++;
	}
	spectra( chan, ant ) /= gainCurves( chan, ant );
      } // end for: chan
    } // end for: ant
  } else {
    cerr << "[Error: noInt nOfFreq(BaselineGain::BaselineGain());rmalizeSpectra]"
	 << " Arguments are of incompatible dimensions."
	 << " Spectra not normalized."
	 << endl;
  }
  if ( wasDivError ) {
    cerr << "[Error: normalizeSpectra]"
	 << " Division by zero. "
	 << wasDivError
	 << " out of "
	 << gainCurves.nelements()
	 << " elements."
	 << endl;
  }
} // --- end function: normalizeSpectra


// ---------------------------------------------------------------- remRFISpectra

void BaselineGain::remRFISpectra ( Matrix<DComplex>& spectra )
{
  double gainValue(0.0);    // to store gain value for a sub band
  double temp_sigma (0.0);  // to store the sigma value for a sub band  
  double temp_mean (0.0);   // to store the mean value for a sub band
  
  //vector to hold a single extracted antenna column; initialize to unity
  Vector<double> antSpectrum (spectraShape_p(0), 1.0);
  
  //declare IPositions to be used in identifying freq band to be looked at
  IPosition lc(1, 0);    // left corner
  IPosition rc(1, 0);    // right corner
  int band (0); 	 // counter for inner loop
  
  
  //loop through the antennas (columns)
  for (int ant (0); ant <spectraShape_p(1); ant++)
    {
      //extract spectrum for this antenna
      antSpectrum = amplitude (spectra.column(ant));
      
      //loop through the frequency sub-bands (rows)
      int temp = baselineGains_p.nrow();
      
      //double minFreq;
      // double maxFreq;
      // int nOfFreq;
      
      double interval=((maxFreq- minFreq)/(1.0*nOfFreq));
      
      int freqrange =temp*int(interval);     // fequencyrange for a particular antenna
      
      //left corner of the whole frequency range of a single antenna
      lc(0)=intervalEndpoints_p(0, freqrange); //sub bands numbering started from 0
      
      //right corner of the whole frequency range of a single antenna
      rc(0)=intervalEndpoints_p(0,freqrange);
      
      //to calculate standard deviation value for a single antenna over 
      //the whole frequency range
      
      temp_sigma = stddev( antSpectrum( lc,rc));
      
      //to calculate mean value for a single antenna over 
      //the whole frequency range
      
      temp_mean = mean( antSpectrum( lc,rc));
      
      for (band =0; abs(band) < temp; band++)
	{
	  //reset lc and rc
	  lc(0)=intervalEndpoints_p(0, band);
	  rc(0)=intervalEndpoints_p(1, band);
	  
	  //set gain value for this sub band
	  gainValue = median(antSpectrum(lc, rc));
	  
	  //only set baselineGains_p member to gainValue if gainValue 
	  //is smaller or if this is the first block being scanned
	  
	  if( gainValue < baselineGains_p(band, ant) or nBlocksScanned_p ==0)
	    {
	      baselineGains_p(band , ant) = gainValue;
	    }
	  
	  // if gainvalue for this bsub and is less than the standard deviation of the
	  //whole band for the same antenna 
	  if( baselineGains_p(band , ant) < 3*temp_sigma)
	    {
	      // then replace the gain value with the mean value obtained
	      // for the whole band of the same antenna
	      baselineGains_p(band , ant) = temp_mean;
	    } // end if: temp_sigma
	  
	}// end for: band
    } // end for: ant
  
}//---------end function : remRFISpectra



// --- getBaselineGains --------------------------------------------------------
// -----------------------------------------------------------------------------

// Default Baseline Gains Retrieval
Matrix<double> BaselineGain::getBaselineGains()
{
  Vector<bool> antReturn ( spectraShape_p( 1 ), true );
  return BaselineGain::getBaselineGains( gainAbscissa_p,
					 antReturn );
} // --- end function: getBaselineGains (default)

// Baseline Gains Retrieval (with default frequencies)
Matrix<double> BaselineGain::getBaselineGains( const Vector<bool>& antReturn )
{
  return BaselineGain::getBaselineGains( gainAbscissa_p,
					 antReturn );
} // --- end function: getBaselineGains (default frequencies)

// Baseline Gains Retrieval (with default antennas)
Matrix<double> BaselineGain::getBaselineGains( const Vector<double>& freqReturn )
{
  Vector<bool> antReturn ( spectraShape_p( 1 ), true );
  return BaselineGain::getBaselineGains( freqReturn,
					 antReturn );
} // --- end function: getBaselineGains (default antennas)

// Base Function: getBaselineGains
// baseline gains retrieval
Matrix<double> BaselineGain::getBaselineGains( const Vector<double>& freqReturn,
					      const Vector<bool>& antReturn )
{
  // determine number of rows needed for return matrix (number of freq bins)
  int nRows ( freqReturn.nelements() );
  // determine number columns needed for return matrix (number antennas)
  int nCols ( BaselineGain::numRequestedAnt( antReturn ) );
  bool antReturnVectorOK ( true );
  if ( nCols <= 0 ) {
    // reset nCols to a value that will make sense for matrix construction
    nCols = 1;
    antReturnVectorOK = false;
  }
  // construct return matrix
  Matrix<double> baselineGains ( nRows, nCols, 0.0 );
  if ( antReturnVectorOK ) {
    // Cases are to catch any frequency arguments that require no interpolation.
    int whichCase ( BaselineGain::determineCase( freqReturn,
						 antReturn ) );
    switch ( whichCase )
      {
      case 1: {
	// CASE 1 means:
	//   requested frequencies are identical to stored in gainAbscissa_p
	//   AND
	//   all antennas are requested
	baselineGains = baselineGains_p;
	break;
      } // end case: 1
      case 2: {
	// CASE 2 means:
	//   requested frequencies are identical to stored in gainAbscissa_p
	//   BUT
	//   some antennas are not requested
	int antCounter ( 0 ); // use for loop
	for ( int ant ( 0 ); ant < spectraShape_p( 1 ); ant++ ) {
	  // only return requested antennas
	  if ( antReturn( ant ) ) {
	    // return this antenna's column
	    baselineGains.column( antCounter ) = baselineGains_p.column( ant );
	    antCounter++; // increment the counter
	  } // end if
	} // end for: ant
	break;
      } // end case: 2
      case 3: {
	// CASE 3 means:
	//   requested frequencies different than stored in gainAbscissa_p
	//   (THEREFORE must interpolate the gains)
	baselineGains = BaselineGain::interpolateGains( baselineGains_p,
							gainAbscissa_p,
							freqReturn,
							antReturn );
	break;
      } // end case: 3
      default: {
	// if none of the cases, then interpolate nontheless
	baselineGains = BaselineGain::interpolateGains( baselineGains_p,
							gainAbscissa_p,
							freqReturn,
							antReturn );
	break;
      } // end case: default
      } // end switch
  } // end if: antReturnVectorOK...
  return baselineGains;
} // --- end function: getBaselineGains (FUNDAMENTAL)


// --- exportBaselineGains -----------------------------------------------------
// -----------------------------------------------------------------------------

// Default Export Baseline Gains to AIPS++ Data Table
void BaselineGain::exportBaselineGains( const String& tableName )
{
  Vector<bool> antReturn ( spectraShape_p( 1 ), true );
  BaselineGain::exportBaselineGains( gainAbscissa_p,
				     antReturn,
				     tableName );
} // --- end function: exportBaselineGains (default)

// Export Baseline Gains to AIPS++ Data Table (with default frequencies)
void BaselineGain::exportBaselineGains( const Vector<bool>& antReturn,
					const String& tableName )
{
  BaselineGain::exportBaselineGains( gainAbscissa_p,
				     antReturn,
				     tableName );
} // --- end function: exportBaselineGains (default frequencies)

// Export Baseline Gains to AIPS++ Data Table (with default antennas)
void BaselineGain::exportBaselineGains( const Vector<double>& freqReturn,
					const String& tableName )
{
  Vector<bool> antReturn ( spectraShape_p( 1 ), true );
  BaselineGain::exportBaselineGains( freqReturn,
				     antReturn,
				     tableName );
} // --- end function: exportBaselineGains (default antennas)

// FUNDAMENTAL: exportBaselineGains
// Export Baseline Gains to AIPS++ Data Table
void BaselineGain::exportBaselineGains( const Vector<double>& freqReturn,
					const Vector<bool>& antReturn,
					const String& tableName )
{
  // get baselineGains matrix
  Matrix<double> baselineGains = BaselineGain::getBaselineGains( freqReturn,
								antReturn );
  // export the table
  BaselineGain::exportTable( baselineGains,
			     freqReturn,
			     antReturn,
			     tableName );
} // --- end function: exportBaselineGains (FUNDAMENTAL)



// *****************************************************************************
// *** PROTECTED FUNCTIONS *****************************************************
// *****************************************************************************

// --- makeFreqAbscissa ----------------------------------------------------
// -----------------------------------------------------------------------------

// get frequency domain vector of a given range and nOfChannels. Each member
//   corresponds to the frequency of one of the channels. This method of
//   calculating the abscissa takes the midpoint of each channel as the
//   reference value. Of course, this shrinks the domain by half a channel
//   width on each side, but in the case of millions of channels, this is
//   insignificant. And in the case of using this function to set the
//   internal vector gainAbscissa_p corresponding to each sub band, the
//   shrinking is overcome because the interpolate function is able to handle
//   this.
Vector<double> BaselineGain::makeFreqAbscissa( const double& minFreq,
					      const double& maxFreq,
					      const int& nOfFreqChan )
{
  Vector<double> freqAbscissa ( nOfFreqChan,
				    0.0 );
  // calculate frequency channel interval
  double interval ( ( maxFreq - minFreq ) / ( 1.0 * nOfFreqChan ) );
  // Declare frequency storage double. Set to midpoint of first channel.
  double frequency ( minFreq + ( interval / 2.0 ) );
  // loop through each channel and calculate the midpoint frequency
  for ( int chan ( 0 ); chan < nOfFreqChan; chan++ ) {
    freqAbscissa( chan ) = frequency;
    frequency += interval; // increment frequency by one sub band interval
  } // end for: band
  return freqAbscissa;
} // --- end function: makeFreqAbscissa


// --- numRequestedAnt ---------------------------------------------------------
// -----------------------------------------------------------------------------

// Count the number of antennas requested
//   Returns -l on dimensions error
int BaselineGain::numRequestedAnt ( const Vector<bool>& antReturn )
{
  int antCounter ( 0 );  
  // get the length of the argument vector
  int nMembers ( 0 );
  antReturn.shape( nMembers );
  if ( nMembers == spectraShape_p( 1 ) ) {
    for ( int ant ( 0 ); ant < nMembers; ant++ ) {
      // increment antCounter if entry is true
      if ( antReturn( ant ) ) {
	antCounter++;
      } // end if: antReturn...
    } // end for: ant
  } // end if: nMembers...
  else {
    cerr << "[Error: numRequestedAnt]"
	 << " antReturn vector does not have correct dimensions."
	 << endl;
    antCounter = -1;
  }
  if ( antCounter == 0 ) {
    cerr << "[Error: numRequestedAnt]"
	 << " Zero antennas requested."
	 << endl;
  }
  return antCounter;
} // end function: numRequestedAnt
  

// --- determineCase -----------------------------------------------------------
// -----------------------------------------------------------------------------

// Determine and return one of the following three cases:
// CASE 1
//   --request frequencies are identical to stored gainAbscissa_p
//   AND
//   --all antennas are requested
//   (THEREFORE, neither interpolation nor antenna looping needed)
// CASE 2
//   --request frequencies are identical to stored gainAbscissa_p
//   AND
//   --some antennas are not requested
//   (THEREFORE, interpolation not needed but antenna looping is)
// CASE 3
//   --request frequencies are different than stored gainAbscissa_p
//   (THEREFORE, interpolation is needed. No check for antenna requests
//     because that check is done within the interpolate funtion.)
int BaselineGain::determineCase ( const Vector<double>& freqReturn,
				  const Vector<bool>& antReturn )
{
  int whichCase ( 0 ); // Return value. Initialize to Zero
  int antennaCounter ( BaselineGain::numRequestedAnt( antReturn ) );
  // error report
  if ( antennaCounter == 0 ) {
    cerr << "[Error: determineCase]"
	 << " Zero antennas requested; or antReturn incorrect dimensions."
	 << endl;
  }
  // If length of frequency vector is identical to stored...
  if ( freqReturn.nelements() == gainAbscissa_p.nelements() ) {
    // And if members of frequency vector are identical to stored...
    if ( sum( abs( freqReturn - gainAbscissa_p ) ) == 0.0 ) {
      // And if number of antennas requested is identical to stored...
      if ( antennaCounter == spectraShape_p( 1 ) ) {
	// ######## CASE 1 #################################
        // request is identical to stored
	whichCase = 1;
      } // end if: number of antennas identical
      else {
	// ######## CASE 2 #################################
	// request frequencies are identical to stored,
	// request antennas are different from stored
	whichCase = 2;
      } // end else: number of antennas not identical
    } // end if: members of frequency vectors identical
    else {
      // ########## CASE 3 (version a) #####################
      // request frequencies are different (but same length)
      // know nothing about the antennas
      whichCase = 3;
    } // end else: members of frequency vectors not identical
  } // end if: length of frequency vectors identical
  else {
    // ############ CASE 3 (version b) #####################
    // request frequencies are different (length and content)
    // know nothing about the antennas
    whichCase = 3;
  } // end else: length of frequency vectors not identical
  return whichCase;
} // --- end function: determineCase


// --- interpolateGains --------------------------------------------------------
// -----------------------------------------------------------------------------

// Return the frequencies requested, interpolating across argument.
//   Construct a natural cubic spline across the argument 'gains'. The return
//   matrix is of dimensions \f$[N_{\rm gainAbscissa},N_{\rm AntRequested}]\f$.
//   It is composed of the resepective gains () of the frequencies in freqReturn.
//   The freqReturn vector must be sorted, and are expected to be within the
//   range specified at construction (extrapolation can be done, but it is
//   highly unreliable.)
// Parameters:
//   gains -- the gain values across which to construct a spline
//   gainAbscissa -- the frequency abscissa of the gain values
//   freqReturn -- the frequencies to be interpolated and returned
//   antReturn -- True/false entries corresponding to those antennas
//                to be returned
// Return value:
//   interpolatedGains -- the matrix of gains for each antenna requested and for
//                        each frequency requested.
Matrix<double> BaselineGain::interpolateGains ( const Matrix<double>& gains,
					       const Vector<double>& gainAbscissa,
					       const Vector<double>& freqReturn,
					       const Vector<bool>& antReturn )
{
  // set dimensions of
  int nOfFreqChan;
  freqReturn.shape( nOfFreqChan );
  int nOfAnt;
  antReturn.shape( nOfAnt );
  int nOfReturnColumns ( BaselineGain::numRequestedAnt( antReturn ) );
  Matrix<double> interpolatedGains ( nOfFreqChan, nOfReturnColumns, 0.0 );
  // set the scalar sampled functionals, which Interpolate1D requires
  ScalarSampledFunctional<double> fx ( gainAbscissa );
  ScalarSampledFunctional<double> fy ( gains.column( 0 ) );
  bool sorted ( true ); // vectors are ordered (expected to be so)
  Interpolate1D<double,double> gainValue ( fx, fy, sorted );
  // set the interpolation method to natural cubic spline
  gainValue.setMethod( Interpolate1D<double,double>::spline );
  // antenna counter for the returned gains
  int returnColumn ( 0 );
  // loop through antennas
  for ( int ant ( 0 ); ant < nOfAnt; ant++ ) {
    // only for requested antennas
    if ( antReturn( ant ) ) {
      // reset sampled functional to gain values for this antenna
      fy = gains.column( ant );
      gainValue.setData( fx, fy );
      // loop through the requested frequencies
      for ( int chan ( 0 ); chan < nOfFreqChan; chan++ ) {
	// get the interpolated gainValue for the requested frequency
	interpolatedGains( chan, returnColumn ) = gainValue( freqReturn( chan ) );
      } // end for: chan
      returnColumn++; // increment antenna counter for returned gains
    } // end if: antReturn...
  } // end for: ant
  return interpolatedGains;
} // --- end function: interpolateGains


// --- exportTable -------------------------------------------------------------
// -----------------------------------------------------------------------------

// Export a matrix to an AIPS++ table. And append a reference vector as
//   the first column. Copied from Lars Bahren.
//   The parameters are as follows:
//
//   dataValues    -- matrix with the table values;
//                    dimensions are [ nFreq, nOfAnt ].
//   freqReturn   -- [ nFreq ] reference frequency vector to be appended as
//                    first column. (Hz)
//   antReturn     -- [ nOfAnt ];
//                    true/false entry correponds to each antenna
//	              (true = return the baseline value for this ant,
//	              false = do not return the baseline value for this ant).
//		      Must have vector length equal to number of antennas at
//		      construction.
//   tablename     -- Filename for the exported table.
void BaselineGain::exportTable( const Matrix<double>& dataValues,
				const Vector<double>& freqReturn,
				const Vector<bool>& antReturn,
				const String& tablename )
{
  // dimensions of the data matrix [ nFreq, nOfAnt ]
  IPosition dataShape ( dataValues.shape() );
  //
  // Set up the labels for the antenna gain columns
  Vector<String> columnLabels ( dataShape( 1 ) ); // labels for the columns
  ostringstream columnLabel; // output string stream object for labels
  int origNOfAnt ( antReturn.nelements() ); // original number of antennas
  // label appropriately (skip over unrequested antennas)
  for ( int ant = 0; ant < origNOfAnt; ant++ ) {
    if ( antReturn( ant ) ) {
      columnLabel << "Antenna" << ( ant + 1 );
      columnLabels( ant ) = columnLabel.str();
      columnLabel.seekp( 0, std::ios::beg ); // reset the stream pointer to beginning
    } // end if
  } // end for: origAnt
  //
  // Start setting up and filling the table
  TableDesc td ( "gaintable", "1", TableDesc::Scratch );
  td.comment() = "A table of gain curves";
  // Frequency column
  td.addColumn (ScalarColumnDesc<double> ("Frequency"));
  // Gain columns
  for ( int ant = 0; ant < dataShape( 1 ); ant++ ) {
    td.addColumn( ScalarColumnDesc<double>( columnLabels( ant ) ) );
  }
  SetupNewTable newtab ( tablename, td, Table::New );
  Table tab ( newtab ); // Create new table
  //
  // Put the frequency values into the table
  ScalarColumn<double> freq ( tab, "Frequency" );
  for ( int index = 0; index < dataShape( 0 ); index++ ) {
    tab.addRow();
    freq.put( index, freqReturn( index ) );
  } // end for: index
  //
  // Put the antenna gain values into the table
  for ( int ant = 0; ant < dataShape( 1 ); ant++ ) {
    ScalarColumn<double> gain ( tab, columnLabels( ant ) );
    for ( int index = 0; index < dataShape( 0 ); index++ ) {
      gain.put( index, dataValues( index, ant ) );
    }
    columnLabel.seekp( 0, std::ios::beg );
  }
} // --- end function: exportTable



// *****************************************************************************
// *** PRIVATE FUNCTIONS *******************************************************
// *****************************************************************************


// --- setSpectraShape ----------------------------------------------------
// -----------------------------------------------------------------------------

// set the spectra dimensions IPosition vector
void BaselineGain::setSpectraShape( const IPosition& spectraShape )
{
  // [ nOfFreq , nOfAnt ] i.e. [ nRows, nCols ]
  spectraShape_p = spectraShape;
} // --- end function: setSpectraShape


// --- setIntervalEndpoints ----------------------------------------------------
// -----------------------------------------------------------------------------

// set the endpoint indeces vector
void BaselineGain::setIntervalEndpoints( const int& nOfFreqChan,
					 const int& nofSubBands )
{
  Vector<int> indexRange ( 2, 0 ); // vector that defines the range to be split
  indexRange( 1 ) = nOfFreqChan - 1; // range = 0 -- (nOfFreqChan - 1)
  //
  intervalEndpoints_p.resize( 2, nofSubBands );
  intervalEndpoints_p = LOPES::splitRange( indexRange,
					   nofSubBands,
					   "lin" );
} // --- end function: setIntervalEndpoints


// --- setSpectraAbscissa ------------------------------------------------------
// -----------------------------------------------------------------------------

// set frequency domain of the spectra being scanned.
void BaselineGain::setSpectraAbscissa( const Vector<double>& spectraAbscissa )
{
  spectraAbscissa_p.resize( spectraAbscissa.shape() );
  spectraAbscissa_p = spectraAbscissa;
}


// --- setGainValue...(in defense of the next three functions) -----------------
// These three functions, setGainValueAsMedian, setGainValueAsMean, and
// setGainValueAsMinimum are identical except for the inner gain point
// extraction statement which varies according to the flavor of scanning
// method chosen at construction or at reset. This is an unfortunate hack,
// but I cannot think of a better way to implement this, whithout reevaluating
// the scanning method at the inner part of the loop (which would mean
// evaluating it nAnt*nSubBands times.) The current implementation is certainly
// faster, but if anyone can think of a better method, by all means, apply it.
// -----------------------------------------------------------------------------


// --- setGainValueAsMedian ----------------------------------------------------
// -----------------------------------------------------------------------------

// Compute the baseline gain value for a sub band, and set the gain curve
//   at this point, if less than already stored value.
//   scanning method: MED
void BaselineGain::setGainValueAsMedian( const Matrix<DComplex>& spectra )
{
  // to store gain value for a sub band
  double gainValue ( 0.0 );
  
  // Vector to hold a single extracted antenna column; initialize to unity
  Vector<double> antSpectrum ( spectraShape_p(0), 1.0 );
  // declare IPositions to be used in identifying freq band to be looked at
  IPosition lc ( 1, 0 ); // left corner
  IPosition rc ( 1, 0 ); // right corner
  int band ( 0 ); // counter for inner loop
  // loop through the antennas (columns)
  for ( int ant ( 0 ); ant < spectraShape_p( 1 ); ant++ ) {
    // extract spectrum for this antenna
    antSpectrum = amplitude( spectra.column( ant ) );
    // loop through the frequency sub-bands (rows)
    for ( band = 0; abs( band ) < baselineGains_p.nrow(); band++ ) {
      // reset lc and rc
      lc( 0 ) = intervalEndpoints_p( 0, band );
      rc( 0 ) = intervalEndpoints_p( 1, band );
      // set gain value for this sub band
      gainValue = median( antSpectrum( lc, rc ) );
     // only set baselineGains_p member to gainValue if gainValue is
      //   smaller or if this is the first block being scanned.
      if ( gainValue < baselineGains_p( band, ant )
	   or
	   nBlocksScanned_p == 0
	   ) {
	baselineGains_p( band, ant ) = gainValue;
      } // end if: gainValue...
    } // end for: band
  } // end for: ant
} // --- end function: setGainValueAsMedian


// --- setGainValueAsMean ----------------------------------------------------
// -----------------------------------------------------------------------------

// Compute the baseline gain value for a sub band, and set the gain curve
//   at this point, if less than already stored value.
//   scanning method: MEAN
void BaselineGain::setGainValueAsMean( const Matrix<DComplex>& spectra )
{
  // to store gain value for a sub band
  double gainValue ( 0.0 );
  // Vector to hold a single extracted antenna column; initialize to unity
  Vector<double> antSpectrum ( spectraShape_p(0), 1.0 );
  // declare IPositions to be used in identifying freq band to be looked at
  IPosition lc ( 1, 0 ); // left corner
  IPosition rc ( 1, 0 ); // right corner
  int band ( 0 ); // inner loop
  // loop through the antennas (columns)
  for ( int ant ( 0 ); ant < spectraShape_p( 1 ); ant++ ) {
    // extract spectrum for this antenna
    antSpectrum = amplitude( spectra.column( ant ) );
    // loop through the frequency sub-bands (rows)
    for ( band = 0; abs( band ) < baselineGains_p.nrow(); band++ ) {
      // reset lc and rc
      lc( 0 ) = intervalEndpoints_p( 0, band );
      rc( 0 ) = intervalEndpoints_p( 1, band );
      // set gain value for this sub band
      gainValue = mean( antSpectrum( lc, rc ) );
      // set baselineGains_p member to gainValue if gainValue is
      //   smaller (or if this is the first block being scanned.)
      if ( gainValue < baselineGains_p( band, ant )
	   or
	   nBlocksScanned_p == 0
	   ) {
	baselineGains_p( band, ant ) = gainValue;
      } // end if: gainValue...
    } // end for: band
  } // end for: ant
} // --- end function: setGainValueAsMean


// --- setGainValueAsMinimum ---------------------------------------------------
// -----------------------------------------------------------------------------

// Compute the baseline gain value for a sub band, and set the gain curve
//   at this point, if less than already stored value.
//   scanning method: MIN
void BaselineGain::setGainValueAsMinimum( const Matrix<DComplex>& spectra )
{
  // to store gain value for a sub band
  double gainValue ( 0.0 );
  // Vector to hold a single extracted antenna column; initialize to unity
  Vector<double> antSpectrum ( spectraShape_p(0), 1.0 );
  // declare IPositions to be used in identifying freq band to be looked at
  IPosition lc ( 1, 0 ); // left corner
  IPosition rc ( 1, 0 ); // right corner
  int band ( 0 ); // inner loop
  // loop through the antennas (columns)
  for ( int ant ( 0 ); ant < spectraShape_p( 1 ); ant++ ) {
    // extract spectrum for this antenna
    antSpectrum = amplitude( spectra.column( ant ) );
    // loop through the frequency sub-bands (rows)
    for ( band = 0; abs( band ) < baselineGains_p.nrow(); band++ ) {
      // reset lc and rc
      lc( 0 ) = intervalEndpoints_p( 0, band );
      rc( 0 ) = intervalEndpoints_p( 1, band );
      // set gain value for this sub band
      gainValue = min( antSpectrum( lc, rc ) );
      // set baselineGains_p member to gainValue if gainValue is
      //   smaller (or if this is the first block being scanned.)
      if ( gainValue < baselineGains_p( band, ant )
	   or
	   nBlocksScanned_p == 0
	   ) {
	baselineGains_p( band, ant ) = gainValue;
      } // end if: gainValue...
    } // end for: band
  } // end for: ant
} // --- end function: setGainValueAsMinimum

// --------------------------------------------------- setGainValueAsStddeviation

void BaselineGain::setGainValueAsStddeviation ( const Matrix<DComplex>& spectra )
{
  double gainValue ( 0.0 );
  // Vector to hold a single extracted antenna column; initialize to unity
  Vector<double> antSpectrum ( spectraShape_p(0), 1.0 );
  // declare IPositions to be used in identifying freq band to be looked at
  IPosition lc ( 1, 0 );   // ... left corner
  IPosition rc ( 1, 0 );   // ... right corner
  int band ( 0 );          // ... inner loop

  // loop through the antennas (columns)
  for ( int ant ( 0 ); ant < spectraShape_p( 1 ); ant++ ) {
    // extract spectrum for this antenna
    antSpectrum = amplitude( spectra.column( ant ) );
    // loop through the frequency sub-bands (rows)
    for ( band = 0; abs( band ) < baselineGains_p.nrow(); band++ ) {
      // reset lc and rc
      lc( 0 ) = intervalEndpoints_p( 0, band );
      rc( 0 ) = intervalEndpoints_p( 1, band );
      // set gain value for this sub band
      gainValue = stddev( antSpectrum( lc, rc ) );
      // set baselineGains_p member to gainValue if gainValue is
      //   smaller (or if this is the first block being scanned.)
      if ( gainValue < baselineGains_p( band, ant )
	   or
	   nBlocksScanned_p == 0
	   ) {
	baselineGains_p( band, ant ) = gainValue;
      } // end if: gainValue...	
    } // end for: band
  } // end for: ant
 } // --- end function: setGainValueAsMinimum

}  // NAmespace LOPES -- END
