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

/* $Id: AverageGain.h,v 1.14 2006/10/31 18:23:24 bahren Exp $ */

#ifndef _AVERAGEGAIN_H_
#define _AVERAGEGAIN_H_

// Base class
#include <Calibration/BaselineGain.h>

// CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
// #include <casa/iostream.h>
// #include <casa/fstream.h>
// #include <casa/string.h>
// #include <casa/Arrays/Array.h>
// #include <casa/BasicSL/Complex.h>
// #include <casa/Quanta.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ArrayColumn.h>


#include <casa/namespace.h>

/*!
  \class AverageGain

  \ingroup Calibration

  \brief Extract and average baseline gain curves; also apply spectral normalization

  \author Brett Deaton

  \date 2005/08

  \test tAverageGain.cc

  <h3>Synopsis</h3>

  Inherits gain extraction from BaselineGain class to extract gain curves
  from complex spectra, averages these extracted curves, and provides
  the functionality to normalize a set of Complex spectra by
  the gain curves. See BaselineGain class
  documentation for more exhaustive explanation of gain curve extraction.

  <h3>Prerequisite</h3>
  <ul type="square">
    <li>[LOPES Tools] BaselineGain -- implementation of various methods to
        scan a power spectrum for a baseline gain curve.
  </ul>

  <h3>Terminology</h3>
  
  In the AverageGain class, we employ specific terminology. To clarify subtle
  differences, the important terms (and their derivatives) are explained
  below in logical groups:
  <ul>
  <li><em>Subdivisions of data</em>
      <ul>
      <li><b>block</b> -- subset of the time series (identical to `block' in
                   DataReader class). Also used to refer to the FFT of a
		   block (the spectrum). The `blocksize' is the number
		   of time samples in the block.
      <li><b>group</b> -- a collection of blocks scanned for a
                   common baseline gain
                   curve. Also used to refer to this baseline gain curve.
		   A group can consist
		   of any number of blocks ranging from 1 to the total number
		   of blocks in a dataset, where
		   \f[
		   N_{\rm TotalBlocks} =
	           \frac{\rm number\,of\,samples\,per\,dataset}
		   {\rm number\,of\,samples\,per\,block\,(i.e.\,`blocksize')}
	           \f]
		   
      </ul>
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
      <li><b>average</b> gain curve -- the average of the gain curves
                   of more than one group of blocks.
      </ul>
  <li><em>Reference Frequencies</em>
      <ul>
      <li><b>abscissa</b> -- the reference values of a function. For a point,
                   \f$f(x)\f$, in cartesian coordinates,
		   the \f$x\f$-value is considered the abscissa, and
		   \f$f\f$ evaluated at \f$x\f$ is the ordinate.
      </ul>
  </ul>

  <h3>Baseline Gain Curve Extraction and Averaging</h3>
  As an overview of the terminology and the system, gain curve extraction
  and averaging is described below. (Note: this description entails
  functions that AverageGain does not provide. It also describes an
  algorithm of baseline gain extraction and averaging which is not
  exactly identical to the one implemented. It is just a didactic tool,
  a description of the whole gain curve extraction system
  for which AverageGain was constructed, and in which it plays a part):

  <ol>
  <li>The FFT of one block yields a spectrum which is scanned for one
      gain curve.
  <li>A second block is then fourier transformed, and its spectrum
      is scanned for one gain curve.
  <li>This second gain curve is then compared with the first gain
      curve and only the minimal points along the two gain curves
      are kept (regardless of the scanning method). Hence
      this new (minimal) gain curve is called the baseline gain curve.
  <li>This process is then continued for each block in the group,
      continuing to minimize the baseline gain curve.
  <li>Once all of the blocks in the group have been scanned, the baseline
      gain curve of the group is frozen and stored.
  <li>After all the groups have been scanned, the gain curves of each group
      are averaged together. This is called the average gain curve.
  </ol>

  <h3>Example</h3>
  
  Example application code for getting an average gain curve using 'nOfGroups'
  groups each consisting of 'nOfBlocksPerGroup' blocks:

  \verbatim
  Matrix<Double> makeAverageGains ( IPosition spectraShape,
                                   Vector<Double> spectraAbscissa,
				   Int nOfSubBands )
  {
    Matrix<DComplex> spectra ( spectraShape );
    AverageGain avgg ( spectraShape,
                       spectraAbscissa,
		       nOfSubBands );
    // loop across the groups
    for ( Int group( 0 ); group < nOfGroups; group++ ) {
      // loop across the blocks
      for ( Int block( 0 ); block < nOfBlocksPerGroup; block++ ) {
        spectra = someGetSpectraFunction( block );
        avgg.extractBaselineGains( spectra );
      }
      // Add the group's baseline gain curve to the average.
      //   This also resets the baseline gain curve for a new group to begin
      avgg.addGainCurvesToAverage();
    }
    return avgg.getAverageGains();
  }
  \endverbatim

 */


class AverageGain : public BaselineGain {
  
 public:
  
  // --- Construction / Destruction --------------------------------------------

  /*!
    \brief Default Constructor

    Default Constructor.
    Initialize the maximum dimensions to
    \f$[N_{\rm FreqChan},N_{\rm Ant}]=[50,10]\f$
    and set the number of sub bands to be sampled to 10. The gain curves for
    each antenna are initialized to zero. Default frequency domain is 0-40 MHz
    (LOFAR ITS). And scanning Method is default, 'MED'.
  */
  AverageGain();
  
  /*!
    \brief Argumented Constructor

    \param spectraShape -- Dimensions of the blocks to be scanned and averaged. 
                           the two entries are \f$[N_{FreqChan},N_{Ant}]\f$.
    \param spectraAbscissa -- \f$[N_{\rm FreqChan}]\f$
                              Vector of the frequency reference values for the
			      spectra to be scanned (Hz). This is called
			      'Frequency' in the glish data record for
			      LOPES-Tools.
    \param nOfSubBands -- number of sub bands into which the spectra are
                          to be divided for scanning.
    \param whichMethod -- 'MED' (default), 'MEAN', or 'MIN'. Must be of
                          type BaselineGain::Method.
  */
  AverageGain (const IPosition& spectraShape,
	       const Vector<Double>& spectraAbscissa,
	       const Int& nOfSubBands,
	       BaselineGain::Method whichMethod = MED );
  
  /*!
    \brief Argumented constructor

    \todo At the moment there only is a function prototype here, in order to
    ascii_export_baseline.cc compile -- apart from that no use can be made of
    this constructor, since the implementation is still missing.

    \param spectraShape -- Dimensions of the blocks to be scanned and averaged. 
                           the two entries are \f$[N_{FreqChan},N_{Ant}]\f$.
    \param freqMin      -- Minimum value of the covered frequency range
    \param freqMax      -- Maximum value of the covered frequency range
    \param nOfSubBands  -- Number of subband, into which the frequency band will
                           be subdivided
  */
  AverageGain (const IPosition& spectraShape,
	       const Double& freqMin,
	       const Double& freqMax,
	       const Int& nOfSubBands);
  
  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Empty Destructor
  */
  ~AverageGain();
  
  // --- Computation -----------------------------------------------------------

  /*!
    \brief Reset the averaged gain curves to average new groups of baselines

    Reset the number of blocks scanned and the number of groups averaged
    to zero. Leave the spectra dimensions
    and the number of subbands at their current settings. This function allows
    the client to scan a new group of blocks to get new average
    baseline gain curves. Invoking this function sets all internal attributes
    just like constructing a new object,
    except that this function does not provide the  ability to set the
    scanning method. That must be done through the setMethod() function.
  */
  void reset();

  /*!
    \brief Reset the averaged gain curves to average new groups of baselines

    Reset the averaged gain curves and the number of groups that have been
    averaged to zero. Also reset the baseline gains matrix from which the
    average is derived. Invoking this function sets all internal attributes just
    like constructing a new object. Except that this function does not provide
    the ability to set the scanning method. That must be done through the
    setMethod() function.

    \param spectraShape -- two entries: \f$[N_{\rm Freq},N_{\rm Ant}]\f$
    \param spectraAbscissa -- \f$[N_{\rm FreqChan}]\f$
                              Vector of the frequency reference values for the
			      spectra to be scanned (Hz). Called 'Frequency' in
			      the glish data record for LOPES-Tools.
    \param nOfSubBands -- number of bands along which to sample the spectra
                             for a minimum
  */
  void reset( const IPosition& spectraShape,
	      const Vector<Double>& spectraAbscissa,
	      const Int& nOfSubBands );
  
  /*!
    \brief Set the scanning method.

    Set the scanning method. Also reinitialize the stored baseline gain curves,
    and the averaged curves so that
    we avoid overlapping two methods. 'MED' is recommended as the most robust
    scanning method, and is therefore the default.

    Example client code:
    \verbatim
    avggain.setMethod( BaselineGain::MEAN );
    \endverbatim
    
    \param method -- 'MED' (default and recommended), 'MEAN', 'MIN'
  */
  void setMethod( const BaselineGain::Method& whichMethod = MED );

  /*!
    \brief Return number of groups added so far

    \return nOfGroups -- number of groups that have been added to the
                         sum matrix thus far. nOfGroups is incremented
			 every time addGroupToAverage() is successfully
			 executed.
  */
  Int getNGroupsAveraged();

  /*!
    \brief Add the group of baseline gain curves to the set to be averaged

    This function is in a sense the foundation of this class. It adds the
    baseline gain curves---which have been extracted by
    the extractBaselineGains function---to the average curves. It also executes
    the resetBaselineGains() function (if the flag is set) in
    order to start with a fresh group of
    gain curves. In other words, addGroupToAverage() adds whatever is stored
    in the baselineGains_p private member to the sum that is stored in the
    sumAvgGains_p private member, then it resets baselineGains_p to zero. When
    scanning for a baseline gain curve commences again, the blocks scanned
    constitute a new group (as long as the flag isn't `False'.)
    
    \param alsoResetBaselineGains -- boolean flag for reset of baseline gains.
                                     Default = True.
                                     If true, this function call will not
				     only add the group of scanned
				     baseline gain curves to the average,
				     but will also reset the baseline
				     gain curves and the number of
				     blocks scanned to zero. The only
				     reason one may wish to set this
				     flag to false, is to add a group to
				     the average several times ( and
				     thereby weight the average.)
  */
  void addGroupToAverage( const Bool& alsoResetBaselineGains = True );

  /*!
    \brief Normalize spectra with respect to the averaged baselines

    Normalize argument spectra with respect to the averages of the baseline
    gain curves extracted. Divide an antenna's spectrum by that
    antenna's average gain curve. This default flavor of the function assumes
    that the argument spectra is of the same dimensions as those spectra that
    were being scanned for the baseline and averaged.

    \param spectra  -- \f$[N_{\rm FreqChan},N_{\rm Ant}]\f$
                       matrix of spectra to be normalized (for instance, from fft)
  */
  void normalizeSpectra( Matrix<DComplex>& spectra );

  /*!
    \brief Normalize spectra with respect to the averaged baselines

    Normalize argument spectra with respect to the averages of the baseline
    gain curves extracted. Simply divide an antenna's spectrum by that
    antenna's gain curve. Flexibility is provided to normalize any
    frequency band on the spectrum of any antenna(s). Default is all
    antennas, over the whole frequency domain.

    \param spectra  -- \f$[N_{\rm FreqChan},N_{\rm Ant}]\f$
                       matrix of spectra to be normalized (for instance, from fft)
    \param spectraAbscissa -- \f$[N_{\rm FreqChan}]\f$ (Hz)
                          Reference frequencies for each spectral channel in
			  spectra. (Abscissa.) Must be sorted; and the \f$n\f$-th 
			  entry must correspond to the \f$n\f$-th row of argument
			  spectra.
    \param antennas -- \f$[N_{\rm Ant}]\f$
                       True/False entry correponds to each antenna
		       (true = this antenna is included in the spectra,
		       false = not included) Vector must have
		       length equal to number of antennas at construction, or
		       at resetBaslineGains.
		       For example: if one wishes to normalize only the
		       3rd and 5th antennas out of 5, the antennas
		       vector would be the following: \f$[F,F,T,F,T]\f$.
  */
  void normalizeSpectra( Matrix<DComplex>& spectra,
			 const Vector<Double>& spectraAbscissa,
			 const Vector<Bool>& antennas );

  /*!
    \brief Default Average Gains Retrieval

    Return sampled frequencies given at construction for all antennas.
    See fully argumented function for more detail.

    \return averageGains -- \f$[N_{\rm SubBands},N_{\rm Ant}]\f$
                             Matrix of average gain values.
  */
  Matrix<Double> getAverageGains();

  /*!
    \brief Average Gains Retrieval (with default frequencies)

    Return requested antennas for sampled frequencies given at construction.
    See fully argumented function for more detail.

    \param antReturn   -- \f$[N_{\rm Ant}]\f$
                          True/False entry correponds to each antenna
			  (true = return the gain curve for this antenna
			  false = do not return this antenna.) Vector must have
			  length equal to number of antennas at construction.
			  For example: if one wishes to retrieve an average curve
			  from the 3rd and 5th antennas out of 5, the antReturn
			  vector would be the following: \f$[F,F,T,F,T]\f$.

    \return averageGains -- \f$[N_{\rm SubBands},N_{\rm AntRequested}]\f$
                             Matrix of average gain values.
  */
  Matrix<Double> getAverageGains( const Vector<Bool>& antReturn );

  /*!
    \brief Average Gains Retrieval (with default antennas)

    Return requested frequencies for all antennas.
    See fully argumented function for more detail.

    \param freqReturn -- \f$[N_{\rm Freq}]\f$ (Hz)
                          Particular frequencies at which to return the average
			  gain curves. Must be sorted.

    \return averageGains -- \f$[N_{\rm FreqRequested},N_{\rm Ant}]\f$
                             Matrix of average gain values.
  */
  Matrix<Double> getAverageGains( const Vector<Double>& freqReturn );

  /*!
    \brief Average baseline gains retrieval

    Return a matrix of average baseline gain curves for each antenna requested.
    Calculate a natural cubic spline interpolation of the averages of the
    groups of baseline gain values generated from the extractBaselineGains()
    function. Client specifies the frequencies for which it wants an
    average baseline gain value, and the antennas it wants returned.
    Each row of the return matrix corresponds in sequence to
    a requested frequency from the freqReturn vector. Each column of the return
    matrix corresponds in sequence to the requested antennas.

    \param freqReturn -- \f$[N_{\rm Freq}]\f$ (Hz)
                          Particular frequencies at which to return the average
			  baseline gain curves. Must be sorted.
    \param antReturn   -- \f$[N_{\rm Ant}]\f$
                          True/False entry correponds to each antenna
			  (true = return the average curve for this antenna
			  false = do not return this antenna.) Vector must have
			  length equal to number of antennas at construction.
			  For example: if one wishes to retrieve an average curve
			  from the 3rd and 5th antennas out of 5, the antReturn
			  vector would be the following: \f$[F,F,T,F,T]\f$.

    \return averageGains -- \f$[N_{\rm FreqRequested},N_{\rm AntRequested}]\f$
                             Matrix of Average baseline gain values.
  */
  Matrix<Double> getAverageGains( const Vector<Double>& freqReturn,
				 const Vector<Bool>& antReturn );

  /*!
    \brief Default export average gains to an AIPS++ table

    Export sampled frequencies given at construction for all antennas.
    See fully argumented function for more detail.

    \param tableName   -- name of file to be generated
  */
  void exportAverageGains( const String& tableName );

  /*!
    \brief Export average gains to an AIPS++ table (with default frequencies)

    Export requested antennas for sampled frequencies given at construction.
    See fully argumented function for more detail.

    \param antReturn   --  \f$[N_{\rm Ant}]\f$
                          True/False entry correponds to each antenna
			  (true = export the gain curve for this antenna
			  false = do not export this antenna.) Vector must have
			  length equal to number of antennas at construction.
			  For example: if one wishes to export an average curve
			  from the 3rd and 5th antennas out of 5, the antReturn
			  vector would be the following: \f$[F,F,T,F,T]\f$.
    \param tableName   -- name of file to be generated
  */
  void exportAverageGains( const Vector<Bool>& antReturn,
			   const String& tableName );

  /*!
    \brief Export average gains to an AIPS++ table (with default antennas)

    Export requested frequencies for all antennas.
    See fully argumented function for more detail.

    \param freqReturn -- \f$[N_{\rm Freq}]\f$ (Hz)
                          Particular frequencies at which to return the Average
			  gain curves. Must be sorted.
    \param tableName   -- name of file to be generated
  */
  void exportAverageGains( const Vector<Double>& freqReturn,
			   const String& tableName );

  /*!
    \brief Export average gains to an AIPS++ table

    Do the same as getAverageGains() (natural cubic spline interpolation, etc)
    but export the whole array (with appended frequency reference column) to a
    labeled AIPS++ table in the current working directory.

    \param freqReturn -- \f$[N_{\rm Freq}]\f$ (Hz)
                          Particular frequencies at which to return the average
			  gain curves. Must be sorted.
    \param antReturn   -- \f$[N_{\rm Ant}]\f$
                          True/False entry correponds to each antenna
			  (true = export the gain curve for this antenna
			  false = do not export this antenna.) Vector must have
			  length equal to number of antennas at construction.
			  For example: if one wishes to export an average curve
			  from the 3rd and 5th antennas out of 5, the antReturn
			  vector would be the following: \f$[F,F,T,F,T]\f$.
    \param tableName   -- name of file to be generated
  */
  void exportAverageGains( const Vector<Double>& freqReturn,
			   const Vector<Bool>& antReturn,
			   const String& tableName );

 private:
  
  // Set gainShape_p;
  //   dimensions -- [ nSubBands, nAnt ]
  void setGainShape( const Int& nOfSubBands,
		     const Int& nOfAnt );

  // --- Private Variables -----------------------------------------------------

  // private member sum of the groups of gain curves. Divide by nGroupsAveraged_p
  //   to get the average gain curve of the groups.
  Matrix<Double> sumAvgGains_p;

  // private member counts the groups added
  Int nGroupsAveraged_p;

  // private member stores dimensions of the groups of gain curves being
  //   averaged. Its two entries are {nOfSubBands, nOfAnt}.
  IPosition gainShape_p;

};

#endif
