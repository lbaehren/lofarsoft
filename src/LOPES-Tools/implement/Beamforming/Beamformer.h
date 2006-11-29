/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

/* $Id: Beamformer.h,v 1.10 2006/10/31 18:24:08 bahren Exp $*/

#ifndef BEAMFORMER_H
#define BEAMFORMER_H

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta/QC.h>
#include <scimath/Mathematics.h>
#include <tasking/Glish.h>

#include <Beamforming/Phasing.h>
#include <LopesBase/DataFrequency.h>
#include <Utilities/ProgressBar.h>

using casa::AipsError;
using casa::Array;
using casa::ArraySlicer;
using casa::Cube;
using casa::DComplex;
using casa::GlishArray;
using casa::IPosition;
using casa::Matrix;
using casa::Slicer;
using casa::String;
using casa::Vector;

namespace LOPES {  // namespace LOPES -- begin
  
  /*!
    \class Beamformer
    
    \ingroup Beamforming
    
    \brief An implementation for various beamforming methods.
    
    \author Lars B&auml;hren
    
    \test tBeamformer.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Some basic knowledge on beamforming with phased arrays of antennas 
      <li>[AIPS++] <a href="http://www.fredosaurus.com/notes-cpp/arrayptr/23two-dim-array-memory-layout.html">2-D Array Memory Layout</a>
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Array.html">AIPS++
      Array class</a>  - A templated N-D Array class with zero origin. 
      <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Vector.html">Vector</a>,
      <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Matrix.html">Matrix</a>,
      and <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Cube.html">Cube</a>
      are one, two, and three dimensional specializations of Array.
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/ArrayMath.html">Mathematical
      operations for Arrays</a>.
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Slicer.html">AIPS++
      Slicer class</a>  - Specify which elements to extract from an n-dimensional
      array. If you need to extract or operate upon a portion of an array, the
      Slicer class is the best way to specify the subarray you are interested in.
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/IPosition.html">AIPS++
      IPosition class</a> -- A Vector of integers, for indexing into Array objects.
      
      <li>[LOPES-Tools] DataFrequency -- Frequency domain settings of an observation.
      <li>[LOPES Tools] Phasing -- Computation of delays for beamforming
      <li>[LOPES-Tools] ccBeam -- Methods for the computation of the 'cc-Beam'
    </ul>
    
    <h3>Synopsis</h3>
    
    Since computation of a skymap from the antenna signals of a phased array 
    results in the repeated evaluation of the complex antenna signal weighting 
    factors, it is reasonable - and time saving - to once compute these factors
    and later on fetch them from memory (given we are about to process a sequence
    of data blocks).
    <p>
    Let \f$ s_j[n] \f$ be the discretized time domain signal of antenna j.
    Given the discrete fourier transform of the antennas signal, \f$ \tilde s_j[k] \f$,
    beamforming then can be performed by
    <ul type="square">
    <li><a name="addSignals">addition of individual antenna signals,
    \f[ S(\rho)[k] = \frac{1}{N} \sum_{i=1}^{N_A} w_i(\rho)[k] \, \tilde s_i[k] \f]
    <li>addition of all baseline combinations,
    \f[ P[k] = \left( \frac{N_A^2 - N_A}{2} \right)^{-1} \sum_{i=1}^{N_A}
    \sum_{j>i}^{N_A} w_i(\rho)[k] \tilde s_i[k] \cdot \overline{w_j(\rho)[k]
    \tilde s_j[k]} \f]
    <li><a name="ccMatrix">cross-correlation of the antenna signals,
    \f[ R_{ij}[k] = \overline{\tilde s_i[k]} \cdot \tilde s_j[k] \f]
    </ul>
    Aiming for the widest range of possible data outputs, namely
    <ul type="square">
    <li>the electrical fields a function of time, \f$ E(\rho,t) \f$,
    <li>the electrical power as function of time, \f$ P(\rho,t) \f$,
    <li>the spetrum of the electrical field, \f$ E(\rho,\nu) \f$,
    <li>the electrical power spectrum, \f$ P(\rho,\nu) \f$,
    </ul>
    only one of the before-mentioned beamforming methods is able to support all
    data output type, which is the beamforming by addition of individual antenna
    signals.
    
    <h3>Weighting factors</h3>
    
    The above used weighting factors \f$ w \equiv w (\vec \rho, \vec x, \nu) \f$
    are a function of sky position, \f$ \vec \rho \f$, array element position,
    \f$ \vec x \f$, and frequency, \f$ \nu \f$,
    \f[
    w (\vec \rho, \vec x, \nu)
    = \exp \Bigl( i \, \phi(\vec \rho, \vec x, \nu) \Bigr)
    = \exp \Bigl( 2 \pi i \, \nu \, \tau_{\rm geom}(\vec \rho, \vec x) \Bigr)
    \f]
    where \f$ \tau_{\rm geom} \f$ is the geometrical time travel delay,
    \f[ \tau_{\rm geom} = \frac{1}{c} \, \vec \rho \cdot \vec x \f]
    
    <h3>Usage</h3>
    \verbatim
    1 Matrix<DComplex> skymap;
    2 Beamformer bf = Beamformer();
    3 bf.BFsetPhaseGradients (coordinates,antPos,freq);
    4
    5 for (int block=0; block<numBlocks; ++block) {
    6  someroutine(datablock);
    7  bf.addAntennas (skymap,datablock);
    8 }
    9
    10 skymap /= numBlocks;
    \endverbatim
  */
  
  class Beamformer : public DataFrequency {
    
  public:
    
    /*!
      \brief Beamforming method to use
    */
    typedef enum {
      //! Add up the signals from the individual antennas
      ANTENNAS,
      //! Add up signal on a per-baseline basis
      BASELINES,
      //! Cross-correlation beam (cc-beam)
      CCBEAM,
      //! Excess-beam
      XBEAM
    } method;
    
  private:
    
    // Be verbose (i.e. show progress) during computations?
    bool showProgress_p;
    // Beamforming method to use
    String BFMethod_p;
    // Flag to keep track of how the phase gradient array was set up.
    bool inputWasCube_p;
    // The shape of the input array for the native spherical coordinates.
    IPosition inputShape_p;
    
    //! 3-dimensional positions of the antenna elements (rel to phase center)
    Matrix<double> antennaPositions_p;
    //! Frequency values
    Vector<double> frequencyValues_p;
    
    // The array with beamforming weights, [freq,pixel,antenna]
    Cube<DComplex> weights_p;
    // The shape of the saved phase gradients data cube, [freq,pixel,antenna]
    IPosition shape_p;
    
  public:

  // --------------------------------------------------------------- Construction

  //! Empty constructor
  Beamformer ();

  /*!
    \brief Argumented constructor.

    \param frequencyValues -- Frequency value associated with a bin in the frequency
                          array, in [Hz].
    \param directions  -- Coordinates of the positions on the sphere (in AZEL
                          coordinates) 
    \param posAntennas -- 3D Position of the antennas, in [m]; [nofAntennas,3]
  */
  Beamformer (Vector<double> frequencyValues,
	      Matrix<double> directions,
	      Matrix<double> posAntennas);

  /*!
    \brief Argumented constructor.

    If providing the set of native spherical coordinates as data cube, i.e.
    as array of shape \f$ [ N_\phi, N_\theta, 2 ] \f$, the cube in the first
    step will be reorganized internally into a matrix of shape
    \f$ [ N_\phi \cdot N_\theta, 2 ] \f$.

    \param frequencyValues -- Frequency value associated with a bin in the frequency
                          array, in [Hz].
    \param directions  -- Coordinates of the positions on the sphere (in AZEL
                          coordinates) 
    \param posAntennas -- 3D Position of the antennas, in [m]; [nofAntennas,3]
  */
  Beamformer (Vector<double> frequencyValues,
	      Cube<double> directions,
	      Matrix<double> posAntennas);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~Beamformer ();

  // ------------------------------------------------------------- Initialization

  //! Initialize the Beamformer
  void initBeamformer ();

  /*!
    \brief Set the Beamformer member data from a Glish record

    \param rec -- A GlishRecord containing the required parameters
  */
  void setBeamformer(GlishRecord& rec);

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Switch on/off verbose mode during computations

    \brief showProgress -- Show progress via ProgressBar during computations?
  */
  void showProgress (bool const &showProgress=true) {
    showProgress_p = showProgress;
  }

  /*!
    \brief Set the method used for the beamforming

    \param bfMethod - Reference code for the method used for the beamforming.

    \todo this need to be changed to make use of the enum list of the various 
    beamforming methods available
  */
  void BFMethod (String const &bfMethod);

  /*!
    \brief Get the method used for the beamforming

    \return bfMethod - Reference code for the method used for the beamforming.
  */
  String BFMethod () const {
    return BFMethod_p;
  }

  /*!
    \brief Get the 3-dim positions of the individual array elements

    \param antennnaPositions -- Array with the 3-dim positions of the individual
                                array elements, as w.r.t. the array phase center;
				[3,nofAntennas]
  */
  Matrix<double> antennaPositions () const {
    return antennaPositions_p;
  };

  /*!
    \brief Set the 3-dim positions of the individual array elements

    \param antennnaPositions -- Array with the 3-dim positions of the individual
                                array elements, as w.r.t. the array phase center;
				[3,nofAntennas]
    \param setWeights        -- Update the beamforming weights?
  */
  void setAntennaPositions (Matrix<double> const &antennaPositions,
			    bool const &setWeights=false);

  /*!
    \brief Get the frequency values for the channels
    
    \return frequencyValues -- Frequency values for the frequency channels, [Hz]
  */
  Vector<double> frequencyValues () {
    return frequencyValues_p;
  }

  /*!
    \brief Set the frequency values for the frequency channels

    \param frequencyValues -- Frequency values for the frequency channels, [Hz]
    \param setWeights      -- Update the beamforming weights?
  */
  void setFrequencyValues (Vector<double> const &frequencyValues,
			   bool const &setWeights=false);
  
  // === I/O of the phase gradient array =============================

  /*!
    \brief Set up the array with the phase gradients.

    Set up the phase gradients for a single beam towards a position in native
    spherical coordinates, \f$ (\phi, \theta) \f$, centered on the position
    of the observatory.

    \param frequencyValues      -- Frequency value associated with a bin in the
                               frequency array, in [Hz].    
    \param direction        -- Native spherical coordinates,
                               \f$ (\phi, \theta) \f$, towards which the beam
			       will be formed.
    \param antennaPositions -- 3D Position of the antennas, in [m];
                              \f$ [N_{\rm Ant} \times 3] \f$
   */
  void setPhaseGradients (const Vector<double>& frequencyValues,
			  const Vector<double>& direction,
			  const Matrix<double>& antennaPositions);
  
  /*!
    \brief Set up the array with the phase gradients.

    Set up the array with the phase gradients for (i) a set of (az,el)
    coordinates, (ii) a set of antennas with positions [x,y,z] and (iii)
    a set of frequency channels.

    \param frequencyValues      -- Frequency value associated with a bin in the
                               frequency array, in [Hz].
    \param directions       -- Native spherical coordinates, (az,el), towards
                               beams are to be formed; Matrix of shape [pixels,2].
    \param antennaPositions -- 3D Position of the antennas, in [m];
                              \f$ [N_{\rm Ant} \times 3] \f$
  */
  void setPhaseGradients (const Vector<double>& frequencyValues,
			  const Matrix<double>& directions,
			  const Matrix<double>& antennaPositions);

  /*!
    \brief Set up the array with the phase gradients

    Set up the array with the phase gradients for (i) a set of (az,el)
    coordinates, (ii) a set of antennas with positions [x,y,z] and (iii)
    a set of frequency channels.

    \param frequencyValues - Frequency value associated with a bin in the frequency
                         array, in [Hz].
    \param directions  - Native spherical coordinates, (az,el), towards beams
                         are to be formed; data cube of shape
			 \f$ [ N_\phi, N_\theta, 2 ] \f$.
    \param antennaPositions - 3D Position of the antennas, in [m]; 
                              \f$ [N_{\rm Ant} \times 3] \f$
  */
  void setPhaseGradients (const Vector<double>& frequencyValues,
			  const Cube<double>& directions,
			  const Matrix<double>& antennaPositions);

  /*!
    \brief Get the full array with the phase gradients

    \return phaseGradients -- Array containing the values of the currently used
                              beamforming weights
  */
  Cube<DComplex> phaseGradients () const {
    return weights_p;
  }

  /*!
    \brief Get a specific element from the array with the phase gradients.
    
    Get a specific element from the array with the phase gradients;
    remembering that we save one value per [pixel,antenna,frequency], we must
    provide arguments for all three position variables:
    
    \param frequency - The number of the frequency channel.
    \param pixel     - The number of the image pixel.
    \param antenna   - The number of the antenna.
  */
  DComplex getPhaseGradients (int const &frequency,
			     int const &pixel,
			     int const &antenna) {
    return weights_p(frequency,pixel,antenna);
  }

  
  /*!
    \brief Get all frequency channels of a combination of position and antenna.

    \retval phaseGrad - (frequency dependent) phase gradients for a certain
                        antenna and pixel.
    \param pixel      - The number of the image pixel.
    \param antenna    - The number of the antenna.

    Obtain a vector with the phase gradients for a position in native spherical
    coordinates, (az,el), and a given antenna. This uses a slicer to extract the
    data parallel to the frequency axis of the phase gradients data cube.

    \todo Check if the <tt>getPhaseGradients (Vector<DComplex>&,int,int)</tt>
    indeed returns the correct set of data; this can be done by comparison
    against the output obtained using the <tt>getPhaseGradients (int,int,int)</tt>
    method.
  */
  void getPhaseGradients (Vector<DComplex>& phaseGrad,
			  int pixel,
			  int antenna);
  
  /*!
    \brief Export the values of the phase gradients array to a file.
    
    \param s - Output stream.

    Export the contents of the phase gradients array as ASCII table to file. 
    Attention: this function should be handled with care, since the resulting
    file can be rather large!
   */
  void phaseGradients (std::ostream&);

  // === Metadata for the phase gradients array ======================

  //! Was the set of native spherical coordinates provided as data cube?
  /*! Since we allow providing the set of native spherical coordinates both as
    matrix, [pixels,2], or as cube, \f$ [ N_\phi, N_\theta, 2 ] \f$, we may
    want to know at a later point, what the original input format was. This
    function returns <i>true</i> if the input was provided as data cube. */
  bool wasInputCube () const {
    return inputWasCube_p;
  }

  /*!
    \brief Get the shape of the coordinates input array.

    \return shape -- The shape of the array, which was used for the input of
                     the set of native spherical coordinates.
  */
  IPosition inputShape () const {
    return inputShape_p;
  }

  /*!
    \brief Get the shape of the phase gradients data cube.

    \return shape -- The shape of the phase gradients data cube
  */
  IPosition getGradientsShape () const {
    return weights_p.shape();
  }

  /*!
    \brief Get the shape of the phase gradient data cube.

    \param nofFrequencyValues -- The number of frequency channels.
    \param nofPixels          -- The number of map pixels.
    \param nofAntennas        -- The number of antennas.
  */
  void getGradientsShape (int& nofFrequencyValues,
			  int& nofPixels,
			  int& nofAntennas);
  
  // === Beamforming routines ========================================
  
  /*!
    \brief Beamforming by addition of individual antenna signals.

    Add up the contribution of each individual antenna on a per-channel
    basis.

    \param skymap -- [frequencyValues,pixels] matrix. The provided antenna fourier
           are added to its contents, so if you only want results from the single
	   provided datablock, you have to initialize this matrix before passing
	   it to this function.
    \param data -- Array with one datablock of Fourier tranformed data
           per antenna, [frequency,antenna].
  */
  void addAntennas (Matrix<DComplex> &skymap,
		    const Matrix<DComplex> &data);
  
  /*!
    \brief Beamforming by addition of all baseline combinations.

    \param skymap  -- Array storing the pixels values of the skymap.
    \param data    -- Array with one datablock of Fourier tranformed data per
                      antenna, [frequency,antenna].
   */
  void BFaddBaselines (Matrix<DComplex>& skymap,
		       const Matrix<DComplex>& data);

  /*!
    \brief Beamforming by cross-correlation of the antenna signals.
    \todo Still needs to be implemented.
   */
  void BFcrossCorrelation (Matrix<DComplex>&,
			   Matrix<DComplex>&);

  /*!
    \brief Beamforming using the 'cc-beam'

    This function essentially is a wrapper around ccBeam::ccbeam.

    The data are processed in a two-step approach: first the Fourier data,
    \f$ \tilde s_j[k] \f$, are multiplied with the phase gradients
    \f$ \tilde s_j[k] \rightarrow \tilde s_j(\vec \rho)[k] = w_j(\vec \rho)[k]
    \cdot s_j(\vec \rho)[k] \f$, then the beamformed spectra are passed to the
    ccBeam.

    \retval skymap   -- Array storing the pixels values of the skymap,
                        [channel,pixel]
    \param data      -- Array with one datablock of Fourier tranformed data per
                        antenna, [frequency,antenna].
    \param blocksize -- Blocksize in the time domain (required for the inverse
                        Fourier transform).
   */
  void crossCorrBeam (Matrix<double>& skymap,
		      const Matrix<DComplex>& data,
		      int const &blocksize);
  
  /*!
    \brief Beamforming using the antenna cross-correlation matrix.

    Given the <a href="#ccMatrix">cross-correlation matrix</a> \f$ R_{ij}[k] \f$
    this method relies on the availability of an array with the weighting
    factors \f$ w_i(\vec \rho)[k] \f$, as previously computed and stored by 
    Beamformer::setPhaseGradients

    \param skymap   -- Array storing the pixels values of the skymap.
    \param ccMatrix -- Array containing the cross-correlated antenna signals.
   */
  void BFccMatrix (Matrix<double>& skymap,
		   const Cube<DComplex>& ccMatrix);

  /*!
    \brief Beamforming using the antenna cross-correlation matrix.

    Given the <a href="#ccMatrix">cross-correlation matrix</a> \f$ R_{ij}[k] \f$
    this method computes the weighting factors \f$ w_i(\vec \rho)[k] \f$ on a
    pixel-by-pixel basis:
    \verbatim
    for (int pixel=0; pixel<nofPixels; ++pixel) {
      azel.row(0) = coordsNSC.row(pixel);
      Beamformer::calcPhaseGradients (itsFrequencyValues,azel,antennaPositions);
      \\ beamforming code
    }
    \endverbatim

    \param skymap           -- Array storing the pixels values of the skymap.
    \param ccCube           -- Array containing the cross-correlated antenna
                               signals.
    \param frequencyValues      -- Array with the frequency values, [Hz].
    \param coordsNSC        -- Array containing the native spherical coordinates,
                               \f$ (\phi, \theta) \f$, of the skymap pixels.
    \param antennaPositions -- 3-dimensional positions of the array antennae.
  */
  void BFccMatrix (Matrix<double>& skymap,
		   const Cube<DComplex>& ccCube,
		   const Vector<double>& frequencyValues,
		   Matrix<double>& directions,
		   const Matrix<double>& antennaPositions);
  
  // ------------------------------------------------------------------- feedback

  /*!
    \brief Provide a summary of the internal parameters to standard output

    Simply reroutes the request to the argumented version of the function
  */
  void summary ();

  /*!
    \brief Print the values of the member to the selected output stream

    \param os -- Output stream, to which the summary is written

    Print the values of the member to the selected output stream:
    \verbatim
    - Beamforming method .................... : addSignals
    - Phase gradients array derived from cube : 0
    - Shape of the input coordinates array .. : [0]
    - Shape of the phase gradients array .... : [257, 10284, 8]
    - Range of frequencies ............. [Hz] : 40000000 .. 80000000 
    \endverbatim
  */
  void printBeamformer (std::ostream& os);

 private:

  //! Set the number of elements along each axis of the phase gradient array.
/*   void setInputShape (const Matrix<double>&); */

  //! Set the number of elements along each axis of the phase gradient array.
/*   void setInputShape (const Cube<double>&); */

  //! Set the number of elements along each axis of the phase gradient array.
  void setInputShape (const Array<double>& directions);

  /*!
    \brief Compute the phase gradients

    Keep in mind that the phase gradients cube has shape [nfreq,npixel,nant]

    \param frequencyValues - Frequency value associated with a bin in the frequency
                         array, in [Hz].
    \param coordsNSC   - Native spherical coordinates, (az,el), towards beams
                         are to be formed; Matrix of shape [pixels,2].
   */
  void calcPhaseGradients (const Vector<double> &frequencyValues,
			   const Matrix<double> &coordsNSC);

  /*!
    \brief Compute the phase gradients

    Compute the weighting factors required for phasing up the antenna signals
    in the beamforming process. Keeping in mind that
    \f[ w = \exp (i \phi) = cos(\phi) + i\, \sin(\phi) \f]
    we in fact use the latter expression for the computation.

    \param coordsNSC   - 
    \param antennaPositions - Array with the 3D positions of the array antennae.
   */
  void calcPhaseGradients (Matrix<double> &coordsNSC,
			   Matrix<double> &antennaPositions);

  //! Cross-correlation of two FFT signals
  void crossCorrelation (Vector<DComplex>&,
			 Vector<DComplex>);
  
  //! Get the number of unique baselines of an antenna array.
  int nofBaselines () {
    return nofBaselines (shape_p(2));
  }

  int nofBaselines (const int nant); 

};

}  // namespace LOPES -- end

#endif
