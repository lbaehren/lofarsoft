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

/* $Id: BaselineGain.h,v 1.9 2007/04/13 13:44:29 bahren Exp $ */

#ifndef BASELINEGAIN_H
#define BASELINEGAIN_H

// AIPS++ header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta.h>
#include <scimath/Functionals/Interpolate1D.h>
#include <scimath/Functionals/ScalarSampledFunctional.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ArrayColumn.h>

using casa::DComplex;
using casa::Double;
using casa::IPosition;
using casa::Matrix;
using casa::String;
using casa::Vector;

/*!
  \class BaselineGain

  \ingroup Calibration

  \brief Extract baseline gain curves; also apply spectral normalization

  \test tBaselineGain.cc

  <h3>Synopsis</h3>

  Extract a gain curve from a complex spectra of \f$N_{\rm Ant}\f$ array
  elements. Also provides the functionality to normalize a complex spectra
  by the extracted gain curves.
  What we refer to as the gain curve is simply the background curve of
  a frequency-domain spectrum. It can be loosely defined in terms of a
  least-squares condition. However, this is not fully accurate, because
  we wish to ignore regions of dense RFI (wide spikes in the spectrum), and
  interpolate a baseline across these. The baseline gain curve is extracted
  by the following method:

  Given a discrete complex spectrum
  \f[
  f[\nu_{k}] = A[\nu_{k}] \exp(i \phi[\nu_{k}])
  \f]
  where
  \f[
  k=0, 1, 2...(N_{\rm FreqChan}-1)
  \f]
  and
  \f[
  \nu_{\rm Bandwidth} \equiv \nu_{(N_{\rm FreqChan}-1)}-\nu_{0}
  \f]
  we extract the amplitudes of the terms,
  \f$A[\nu_{k}]\f$, and call these the terms of the absolute spectrum.
  The absolute spectrum is then divided into
  \f$N_{\rm SubBands}\f$ intervals, referred to as sub bands. Each sub band
  consists of \f$M_{\rm FreqChan}\f$ members, where
  \f[
  M_{\rm FreqChan} = \frac{N_{\rm FreqChan}}{N_{\rm SubBands}}
  \f]
  According to the scanning method (median, mean, minimum) one of these members
  is chosen as representative of the gain curve on that sub band. The
  representative gain value is assigned to the frequency in the middle of
  this sub band. On subsequent scans of new spectra (from the same
  antenna), the gain value chosen for each sub band is only
  assigned to the midpoint frequency if it is less than the
  gain value already stored. This allows us to extract the minimum
  gain curve across time (which is what we want--RFI is always
  additive rather than subtractive.)

  A natural cubic spline is used to interpolate the gain curve for any
  frequency across the original band. Also, median is the default scanning
  method. It is preferred to mean because it is more robust against outliers.
  Minimum is a poor measure of the gain curve, but we leave it as a
  possible scanning method for unforeseen uses.

  It is important to note the subtle difference between the two minimums
  which we are talking about: a minimum gain curve and a gain curve of minimums.
  The former (a minimum gain curve) is what we achieve by scanning many
  spectra from the same antenna, always reducing the curve if the extracted
  gain value is less than that which is already stored. It has nothing to do
  with the scanning method. We wish to extract the minimum gain curve
  whether the scanning method is median, mean, or minimum. The latter
  (a gain curve of minimums) is what we achieve by setting the scanning method
  to minimum. The other two scanning methods (median and mean) are preferred
  to this method.

  Note: although the frequency arguments are expected in the units of Hz,
  obviously, any unit is adequate--as long as client code is consistent.

  <h3>Terminology</h3>
  
  In the BaselineGainGain class, we employ specific terminology. To
  clarify subtle differences, the important terms
  (and their derivatives) are explained below in logical groups:
  <ul>
  <li><em>Frequency domain</em>
      <ul>
      <li><b>spectrum</b> -- power spectrum (fourier transform of
                   a block.)
      <li><b>gain</b> curve -- the background curve of a spectrum. It is
                   fundamentally lower-resolution than
		   the spectrum because it is determined by
		   subdividing a spectrum into sub bands. However
		   in the case of the BaselineGain and AverageGain
		   classes, interpolating across the gain curve
		   is possible.
      </ul>
  <li><em>Gain curves</em>
      <ul>
      <li><b>baseline</b> gain curve -- the minimized background curve
                   of a spectrum. It is achieved by comparing the gain curves
		   of more than one block.
      </ul>
  <li><em>Reference Frequencies</em>
      <ul>
      <li><b>abscissa</b> -- the reference values of a function. For a point,
                   \f$f(x)\f$, in cartesian coordinates,
		   the \f$x\f$-value is considered the abscissa, and
		   \f$f\f$ evaluated at \f$x\f$ is the ordinate.
      </ul>
  </ul>

  <h3>Motivation</h3>
  
  This class was motivated by two needs:
  <ol>
  <li>We wish to calibrate antennas. To do this, we need some relative measure
  of gain.
  <li>We wish to flag regions of RFI. One method is to normalize a spectrum
  by dividing it by its underlying gain curve. From this normalized spectrum,
  we are able to easily identify RFI.
  </ol>
*/

class BaselineGain {
  
 public:
  
  //! Number of frequency channels
  unsigned int nOfFreq;
  //! Number antennas in the data set
  unsigned int nOfAnt;
  //! Lower limit of the frequency band
  double minFreq;
  //! Upper limit of the frequency band
  double maxFreq;

  /*!
    \brief Methods possible for scanning the spectra
  */
  enum Method {
    //! median
    MED,
    //! average
    MEAN,
    //! minimum
    MIN,
    //! Standard deviation
    STDDEV		//###################################
  };
  
  // --- Construction ----------------------------------------------------------

  /*!
    \brief Default Constructor

    Default Constructor.
    Initializes the maximum dimensions to
    \f$[N_{\rm FreqChan},N_{\rm Ant}]=[50,10]\f$
    and sets the number of sub bands to be sampled to 10. The gain curves for
    each antenna are initialized to unity. Default frequency domain is 0-40 MHz
    (LOFAR ITS). And scanning Method is default, 'MED'.
  */
  BaselineGain();
    
  /*!
    \brief Argumented Constructor

    Sets the storage array for the gain curves to the specified dimensions,
    \f$[N_{\rm SubBands},N_{\rm Ant}]\f$, and initializes it to the maximum value.
    The arguments are used to set the maximum dimensions and frequency
    domain of the baseline gains array.
    
    \param spectraShape    -- two entries: \f$[N_{\rm FreqChan},N_{\rm Ant}]\f$
    \param spectraAbscissa -- \f$[N_{\rm FreqChan}]\f$
                              Vector of the frequency reference values for the
			      spectra to be scanned (Hz). This is called
			      'Frequency' in the glish data record for
			      LOPES-Tools.
    \param nOfSubBands     -- number of bands to divide the spectra into. Each
                              sub band is analyzed for a representative gain
	      		      according to the scanning Method.
    \param whichMethod     -- method used for determining the gain curve.
                              'MED' (default), 'MEAN', or 'MIN'. Must be
			      of type BaselineGain::Method.
  */
  BaselineGain( const IPosition& spectraShape,
		const Vector<Double>& spectraAbscissa,
		const int& nOfSubBands,
		const BaselineGain::Method& whichMethod = MED );
  
  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Default desctructor
   */
  virtual ~BaselineGain();

  // --- Computation routines --------------------------------------------------

  /*!
    \brief Reset stored gain curve for new scanning.

    Reset the number of blocks scanned to zero. Leave the expected dimensions
    and the number of subbands at their current settings. This function allows
    the client to scan a new set of blocks to get a new baseline. Calling
    this function sets parameters just like constructing a new object,
    except that this function does not provide the  ability to set the
    scanning method. That must be done through the setMethod() function.
  */
  virtual void reset();

  /*!
    \brief Reset stored gain curve for new scanning.

    Reset internally stored baseline gains to the dimensions specified, and
    reset the number of blocks scanned to zero. This allows the client to scan
    a new set of blocks to get a new baseline. Calling this function sets
    parameters just like constructing a new object, except that this function
    does not provide the ability to set the scanning method. That must
    be done through the setMethod() function.

    \param spectraShape -- two entries: \f$[N_{\rm Freq},N_{\rm Ant}]\f$
    \param spectraAbscissa -- \f$[N_{\rm FreqChan}]\f$
                              Vector of the frequency reference values for the
			      spectra to be scanned (Hz). Called 'Frequency' in
			      the glish data record for LOPES-Tools.
    \param nOfSubBands -- number of bands along which to sample the spectra
                             for a minimum
  */
  virtual void reset( const IPosition& spectraShape,
		      const Vector<Double>& spectraAbscissa,
		      const int& nOfSubBands );
  
  /*!
    \brief Set the scanning method.

    Set the scanning method. Also reinitialize the stored gain curves, so that
    we avoid overlapping two methods. 'MED' is recommended as the most robust
    scanning method.

    Example client code:
    \verbatim
    gain.setMethod( BaselineGain::MEAN );
    \endverbatim
    
    \param method -- 'MED' (default and recommended), 'MEAN', 'MIN'
  */
  virtual void setMethod( const BaselineGain::Method& whichMethod = MED );

  /*!
    \brief Retrieve the current scanning Method.

    Return the integer representing the scanning Method in use.
    MED = 0,
    MEAN = 1,
    MIN = 2,
    STDDEV = 3.				//####################################
    
    \return whichMethod -- 0, 1, 2 or 3.	//###########################
  */
  int getMethod() {
    return whichMethod_p;
  }
  
  /*!
    \brief Retrieve the frequency reference abscissa for the stored gains.

    Return the frequency references stored internally for each sampled sub
    band.

    \return gainAbscissa -- \f$[N_{\rm SubBands}]\f$
                                 The frequencies for which a gain point has been
                                 evaluated. Every member is the frequency
				 reference for a point on the scanned gain curve.
  */
  Vector<Double> getGainAbscissa() {
    return gainAbscissa_p;
  }
  
  /*!
    \brief Retrieve the number of blocks scanned so far.
    
    \return nBlocksScanned -- The number of blocks scanned so far
  */
  int getNBlocksScanned() {
    return nBlocksScanned_p;
  }

  /*!
    \brief Baseline gains extraction

    Scan the The argument matrix for a gain curve. Argument must be the same
    dimensions and span the same frequency domain as defined at construction, or
    at reset. This function will error check for incorrect dimensions, but
    obviously cannot do so for incorrect frequency domain. This function is
    intended to be run on multiple datablocks to find a minimum baseline
    across them all; each scan of a new block stores only those sub band
    gain values that are less than the already stored value. Note the difference
    between a minimum baseline and a baseline extracted with the scanning method
    set to MIN. The first is what this function does (every extracted gain
    value is checked against the value already stored for that sub band--from
    previous scans; the lesser value is kept.) This is a foundational part
    of the extractBaselineGains() function, whether the scanning method is set to
    MED, MEAN, or MIN.

    \param spectra -- \f$[N_{\rm Freq},N_{\rm Ant}]\f$
                      Spectra block to be scanned.
  */
  void extractBaselineGains( const Matrix<DComplex>& spectra );

  /*!
    \brief normalize spectra with respect to the extracted baseline gain curves

    Normalize argument spectra with respect to the baseline
    gain curves extracted. Simply divide an antenna's spectrum by that
    antenna's gain curve. This default flavor of the function assumes that the
    argument spectra is of the same dimensions as those spectra that were
    being scanned for the baseline.

    \param spectra  -- \f$[N_{\rm FreqChan},N_{\rm Ant}]\f$
                       matrix of spectra to be normalized (for instance, from fft)
  */
  virtual void normalizeSpectra( Matrix<DComplex>& spectra );

  /*!
    \brief normalize spectra with respect to the extracted baseline gain curves

    Normalize argument spectra with respect to the baseline
    gain curves extracted. Simply divide an antenna's spectrum by that
    antenna's gain curve. Flexibility is provided to normalize any
    frequency band on the spectrum of any antenna(s).

    \param spectra  -- \f$[N_{\rm FreqChan},N_{\rm Ant}]\f$
                       matrix of spectra to be normalized (for instance, from fft)
    \param frequencies -- \f$[N_{\rm FreqChan}]\f$ (Hz)
                          Reference frequencies for each spectral channel in
			  spectra. (Abscissa.) Must be sorted; and the \f$n\f$-th 
			  entry must correspond to the \f$n\f$-th row of argument
			  spectra.
    \param antennas -- \f$[N_{\rm Ant}]\f$
                       True/False entry correponds to each antenna
		       (true = this antenna is included in the spectra,
		       false = not included) Vector must have
		       length equal to number of antennas at construction, or
		       at reset.
		       For example: if one wishes to normalize only the
		       3rd and 5th antennas out of 5, the antennas
		       vector would be the following: \f$[F,F,T,F,T]\f$.
  */
  virtual void normalizeSpectra( Matrix<DComplex>& spectra,
				 const Vector<Double>& frequencies,
				 const Vector<bool>& antennas );

  /*!
    \brief Default Baseline Gains Retrieval

    Return sampled frequencies given at construction for all antennas.
    See fully argumented function for more detail.

    \return baselineGains -- \f$[N_{\rm SubBands},N_{\rm Ant}]\f$
                             Matrix of baseline gain values.
  */
  
  Matrix<Double> getBaselineGains();

  /*!
    \brief Baseline Gains Retrieval (with default frequencies)

    Return requested antennas for sampled frequencies given at construction.
    See fully argumented function for more detail.

    \param antReturn   -- \f$[N_{\rm Ant}]\f$
                          True/False entry correponds to each antenna
			  (true = return the gain curve for this antenna
			  false = do not return this antenna.) Vector must have
			  length equal to number of antennas at construction.
			  For example: if one wishes to retrieve a baseline
			  from the 3rd and 5th antennas out of 5, the antReturn
			  vector would be the following: \f$[F,F,T,F,T]\f$.

    \return baselineGains -- \f$[N_{\rm SubBands},N_{\rm AntRequested}]\f$
                             Matrix of baseline gain values.
  */
  
  Matrix<Double> getBaselineGains( const Vector<bool>& antReturn );

  /*!
    \brief Baseline Gains Retrieval (with default antennas)

    Return requested frequencies for all antennas.
    See fully argumented function for more detail.

    \param freqReturn -- \f$[N_{\rm Freq}]\f$ (Hz)
                          Particular frequencies at which to return the baseline
			  gain curves. Must be sorted.

    \return baselineGains -- \f$[N_{\rm FreqRequested},N_{\rm Ant}]\f$
                             Matrix of baseline gain values.
  */
  Matrix<Double> getBaselineGains( const Vector<Double>& freqReturn );

  /*!
    \brief Baseline Gains Retrieval

    Return a matrix of baseline gain curves for each antenna requested.
    Calculates a natural cubic spline interpolation of the baseline gain values
    generated from the extractBaselineGains() function. Clientcode specifies the
    frequencies for which it wants a baseline gain value, and the antennas it
    wants returned. Each row of the return matrix corresponds directly with
    a requested frequency from the freqReturn vector. Each column of the return
    matrix corresponds in sequence to the requested antennas.

    \param freqReturn -- \f$[N_{\rm Freq}]\f$ (Hz)
                          Particular frequencies at which to return the baseline
			  gain curves. Must be sorted.
    \param antReturn   -- \f$[N_{\rm Ant}]\f$
                          True/False entry correponds to each antenna
			  (true = return the gain curve for this antenna
			  false = do not return this antenna.) Vector must have
			  length equal to number of antennas at construction.
			  For example: if one wishes to retrieve a baseline
			  from the 3rd and 5th antennas out of 5, the antReturn
			  vector would be the following: \f$[F,F,T,F,T]\f$.

    \return baselineGains -- \f$[N_{\rm FreqRequested},N_{\rm AntRequested}]\f$
                             Matrix of baseline gain values.
  */
  Matrix<Double> getBaselineGains( const Vector<Double>& freqReturn,
				  const Vector<bool>& antReturn );

  /*!
    \brief Default Export baseline gains to an AIPS++ table

    Export sampled frequencies given at construction for all antennas.
    See fully argumented function for more detail.

    \param tableName   -- name of file to be generated
  */
  void exportBaselineGains( const String& tableName );

  /*!
    \brief Export baseline gains to an AIPS++ table (with default frequencies)

    Export requested antennas for sampled frequencies given at construction.
    See fully argumented function for more detail.

    \param antReturn   --  \f$[N_{\rm Ant}]\f$
                          True/False entry correponds to each antenna
			  (true = export the gain curve for this antenna
			  false = do not export this antenna.) Vector must have
			  length equal to number of antennas at construction.
			  For example: if one wishes to export a baseline
			  from the 3rd and 5th antennas out of 5, the antReturn
			  vector would be the following: \f$[F,F,T,F,T]\f$.
    \param tableName   -- name of file to be generated
  */
  void exportBaselineGains( const Vector<bool>& antReturn,
			    const String& tableName );

  /*!
    \brief Export baseline gains to an AIPS++ table (with default antennas)

    Export requested frequencies for all antennas.
    See fully argumented function for more detail.

    \param freqReturn -- \f$[N_{\rm Freq}]\f$ (Hz)
                          Particular frequencies at which to return the baseline
			  gain curves. Must be sorted.
    \param tableName   -- name of file to be generated
  */
  void exportBaselineGains( const Vector<Double>& freqReturn,
			    const String& tableName );

  /*!
    \brief Export baseline gains to an AIPS++ table

    Do the same as getBaselineGains (natural cubic spline interpolation, etc)
    but export the whole array (with appended frequency reference column) to a
    labeled AIPS++ table in the current working directory.

    \param freqReturn -- \f$[N_{\rm Freq}]\f$ (Hz)
                          Particular frequencies at which to return the baseline
			  gain curves. Must be sorted.
    \param antReturn   -- \f$[N_{\rm Ant}]\f$
                          True/False entry correponds to each antenna
			  (true = export the gain curve for this antenna
			  false = do not export this antenna.) Vector must have
			  length equal to number of antennas at construction.
			  For example: if one wishes to export a baseline
			  from the 3rd and 5th antennas out of 5, the antReturn
			  vector would be the following: \f$[F,F,T,F,T]\f$.
    \param tableName   -- name of file to be generated
  */
  void exportBaselineGains( const Vector<Double>& freqReturn,
			    const Vector<bool>& antReturn,
			    const String& tableName );

 private:

  // --- Private Functions -----------------------------------------------------
  
  // Set the spectra dimensions vector.
  void setSpectraShape (const IPosition& spectraShape);

  // Set the endpoint indeces vector.
  void setIntervalEndpoints( const int& nOfFreqBins,
			     const int& nOfSubBands );

  // Set the frequency domain of the spectra being scanned.
  //   The length of this vector is nFreqChan.
  void setSpectraAbscissa( const Vector<Double>& spectraAbscissa );

  // Set the baselineGains_p member to the gain value determined for this
  //   sub band, only if it is less than that already stored. The three
  //   flavors of scanning (median, mean, minimum) are placed into three
  //   separate functions below in order to increase the speed of the scan.
  void setGainValueAsMedian ( const Matrix<DComplex>& spectra );
  void setGainValueAsMean ( const Matrix<DComplex>& spectra );
  void setGainValueAsMinimum ( const Matrix<DComplex>& spectra );
  void setGainValueAsStddeviation (const Matrix<DComplex>& spectra);

  /*!
    Remove the RFIs from the baselinegain curves extracted
    by comparing the normalized gain curve dataset with the 
    3*(standard deviation value calculated for the whole data set)
    and if it is gain curve value at any frequency is less than 
    3*sigma than replace that gain curve value by the mean or average of 
    the gain value, else leave it as it is.
  */
  void remRFISpectra ( Matrix<DComplex>& spectra );

  // --- Private Variables -----------------------------------------------------

  // Matrix specifying the endpoint indeces of the sub bands to be sampled
  //   Its dimensions are [2, nSubBands]. Its entries are as follows:
  //
  //     _____________________________ ... _____________________________
  //  1 | ending index of 1st band,    ... ending index of last band    |
  //  0 | beginning index of 1st band, ... beginning index of last band |
  //     -------------0--------------- ... ---------------N-------------
  Matrix <int> intervalEndpoints_p;

 protected:

  // --- Protected Functions ---------------------------------------------------

  /*!
    \brief Get a domain vector of a given frequency range and number of channels
    
    Each member corresponds to the frequency of a channel midpoint.
    This method of calculating the abscissa takes the midpoint of
    each channel as the reference value.
    
    \param minFreq -- minimum frequency (Hz)
    \param maxFreq -- maximum frequency (Hz)
    \param nOfFreqChan -- number of channels the range is to be divided into
  */
  Vector<Double> makeFreqAbscissa( const Double& minFreq,
				  const Double& maxFreq,
				  const int& nOfFreqChan );

  /*! 
    \brief Count the number of antennas requested
    
    Count the number of true entries in anReturn. Will report an error
    to standard error output if no entries are True, or if antReturn vector
    is noncomformant with the length of the secondary axis (antennas) of the stored
    baselineGains_p.

    \param antReturn -- \f$[N_{Ant}]\f$ True = returned/requested antenna,
                        False = not a returned/requested antenna.

    \return nAnt -- number of True entries in antReturn. Or -1 if length of
                    antReturn is nonconformant with stored baselineGains_p.
  */
  int numRequestedAnt ( const Vector<bool>& antReturn );
  
  /*!
    \brief Determine one of three cases which require different interpolations

   Determine and return an integer corresponding to one of the following
   three cases:
   <ol>
   <li>CASE 1
     <ul>
     <li>request frequencies are identical to those stored in gainAbscissa_p
     <li>all antennas are requested
     </ul>
     (THEREFORE, neither interpolation nor antenna looping needed)
   <li>CASE 2
     <ul>
     <li>request frequencies are identical to stored gainAbscissa_p
     <li>some antennas are not requested
     </ul>
     (THEREFORE, interpolation not needed but antenna looping is)
   <li>CASE 3
     <ul>
     <li>request frequencies are different than stored gainAbscissa_p
     </ul>
     (THEREFORE, interpolation is needed. No check for antenna requests
       because that check is done within the interpolate funtion.)
   </ol>
   
   \param freqReturn -- request frequencies (Hz)
   \param antReturn  -- True/False entries corresponding to the antennas
                        requested. Must be the same length as the total number
			of antennas at construction or at reset.
   \return whichCase -- integer corresponding to case
  */
  int determineCase ( const Vector<Double>& freqReturn,
 		      const Vector<bool>& antReturn );

  /*!
    \brief Return the frequencies requested, interpolating across argument

    Construct a natural cubic spline across the argument 'gains'. The return
    matrix is of dimensions \f$[N_{\rm gainAbscissa},N_{\rm AntRequested}]\f$.
    It is composed of the resepective gains () of the frequencies in freqReturn.
    The freqReturn vector must be sorted, and are expected to be within the
    range specified at construction (extrapolation can be done, but it is
    highly unreliable.)

    \param gains -- \f$[N_{\rm gainAbscissa},N_{\rm Ant}]\f$
                    the gain values across which to
		    construct a spline. This matrix
		    is expected to have a secondary axis
		    (antennas) equal in length
		    to the number of antennas at construction or at reset.
    \param gainAbscissa -- \f$[N_{\rm gainAbscissa}]\f$
                           the frequency abscissa of the gain values in 'gains'.
    \param freqReturn -- \f$[N_{\rm FreqRequested}]\f$
                         the frequencies to be interpolated and returned.
    \param antReturn -- \f$[N_{\rm Ant}]\f$
                        True/False entries corresponding to those antennas
                        to be returned.
    \return interpolatedGains -- \f$[N_{\rm gainAbscissa},N_{\rm AntRequested}]\f$
                                 the matrix of gains for each antenna requested
                                 and for each frequency requested.
  */
  Matrix<Double> interpolateGains ( const Matrix<Double>& gains,
				   const Vector<Double>& gainAbscissa,
				   const Vector<Double>& freqReturn,
				   const Vector<bool>& antReturn );
  
  /*!
    \brief Export an AIPS++ table of data values and an appended reference vector

    \param dataValues -- the matrix of functional values to be exported
    \param freqReturn -- the reference vector to be appended as the
                         first column of
                         the table. Must have length equal to the primary axis of
			 'dataValues'.
    \param antReturn  -- True or False entries corresponding to the antenna in
                         sequence (whether it is represented in 'dataValues' or
			 not.) Must be equal in length to the number of antennas
			 at construction or at reset. Also, must have the same
			 number of true entries as the
			 length of the secondary axis
			 of 'dataValues'.
    \param tablename  -- name of the AIPS++ table to be exported.
  */
  void exportTable ( const Matrix<Double>& dataValues,
		     const Vector<Double>& freqReturn,
		     const Vector<bool>& antReturn,
		     const String& tablename);

  // --- Protected Variables ---------------------------------------------------

  /*!
    \brief Scanning method.
    
    Stores one of three Methods enumerated in BaselineGain class: MED, MEAN, MIN.
  */
  BaselineGain::Method whichMethod_p;

  /*!
    \brief Counter for number of blocks scanned so far.
    
    nBlocksScanned is reset to zero every time the reset() function is called,
    implicitly or explicitly.
  */
  int nBlocksScanned_p;

  /*!
    \brief Length and width dimensions of expected spectral block.

    The two entries are the following:
    \f$[N_{\rm FreqChannels},N_{\rm Ant}]\f$
  */
  IPosition spectraShape_p;

  /*!
    \brief Frequency abscissa vector of the expected spectral block.

    The dimensions are \f$[N_{\rm FreqChannels}]\f$. Every row in the spectral  
    blocks being scanned for a baseline corresponds in sequence to a member
    of this vector.

    For clarity, the difference between spectraAbscissa_p and gainAbscissa_p:
    <ul>
    <li> spectraAbscissa_p -- abscissa of each frequency channel of the
    matrices being scanned for baselines.
    <li> gainAbscissa_p -- abscissa of each frequency channel (sub band) of
    the baselineGains_p matrix stored internally.
    </ul>
    The two vectors span the same domain, but gainAbscissa_p is at a lower
    resolution than spectraAbscissa_p.
  */
  Vector<Double> spectraAbscissa_p;

  /*!
    \brief Frequency abscissa Vector of the stored baseline gains.
    
    The dimensions are \f$[N_{\rm SubBands}]\f$. Every row in the
    baselineGains_p matrix corresponds in sequence to a memberr of this
    vector.

    For clarity, the difference between spectraAbscissa_p and gainAbscissa_p:
    <ul>
    <li> spectraAbscissa_p -- abscissa of each frequency channel of the
    matrices being scanned for baselines.
    <li> gainAbscissa_p -- abscissa of each frequency channel (sub band) of
    the baselineGains_p matrix stored internally.
    </ul>
    The two vectors span the same domain, but gainAbscissa_p is at a lower
    resolution than spectraAbscissa_p.
  */
  Vector<Double> gainAbscissa_p;

  /*! 
    \brief Array with the baseline gain magnitudes
    
    This is the fundamental member of the class. It holds the baseline gain
    curves for each antenna. It is reset upon the function call reset().
    Dimensions are \f$[N_{\rm SubBands},N_{\rm Ant}]\f$.
  */
  Matrix<Double> baselineGains_p;

};

#endif

