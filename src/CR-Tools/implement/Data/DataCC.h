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

#if !defined(DATACC_H)
#define DATACC_H

// C++ Standard library
#include <ostream>
#include <fstream>
#include <sstream>
/* using namespace std; */

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/sstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>
#include <casa/IO/AipsIO.h>
#include <casa/BasicMath/Math.h>
#include <measures/Measures.h>
#include <measures/Measures/MEpoch.h>
#include <casa/Quanta.h>

#include <ApplicationSupport/ObservationMeta.h>
#include <Utilities/Masking.h>
#include <Utilities/StringTools.h>

#include <casa/namespace.h>

namespace CR {  //  Namespace CR -- begin
  
  /*!
    \class DataCC
    
    \ingroup Data
    
    \brief Storage for a set of cross-correlated data
    
    \author Lars B&auml;hren
    
    <h3>Prerequisite</h3>
    
    <ul>
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Array.html">Array</a>
      -- A templated N-D Array class with zero origin. 
      <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Vector.html">Vector</a>,
      <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Matrix.html">Matrix</a>,
      and <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Cube.html">Cube</a>
      are one, two, and three dimensional specializations of Array.
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/IPosition.html">IPosition</a>
      -- A Vector of integers, for indexing into Array objects.
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/ArrayMath.html">ArrayMath</a>
      -- Mathematical operations for Arrays.
    </ul>
    
    \test tDataCC.cc
  */
  
  class DataCC : public ObservationMeta {
    
    //! Path to the input datafile
    String datafile_p;
    //! Data cube containing the cross correlated data
    Cube<DComplex> ccCube_p;
    //! Ordering of the data cube axes within the disk file
    Vector<Int> axisOrder_p;
    //! Number of points (channels) in the fft.
    Int fftsize_p;
    //! Channels numbers of the frequencies
    Vector<Int> availableChannels_p;
    //! Antenna selection
    Vector<Bool> antennaSelection_p;
    //! Frequency channel selection
    Vector<Bool> frequencySelection_p;
    //! Baselines for a visibility dataset
    Matrix<Int> baselines_p;
    
  public:
    
    // === Construction / Deconstruction =========================================
    
    /*!
      \brief Argumented constructor
      
      Provides single parameter to ObservationMeta base-class.
      
      \param filename - Location where the metafile is stored on disk.
    */
    DataCC (String filename);
    
    /*!
      \brief Argumented constructor
      
      Provides parameters to ObservationMeta base-class.
      
      \param metafile   -- Location where the metafile is stored on disk.
      \param antposfile -- Location of the antenna position file.
    */
    DataCC (String metafile,
	    String antposfile);
    
    // === Mapping between data cube axes in disk file and storage ===============
    
    /*!
      \brief Get the ordering of the cross-correlation matrix axes.
      
      \return axisOrder -- Vector with the axis numbering of the cross-correlation
                           array.
    */
    inline Vector<Int> axisOrder () {
      return axisOrder_p;
    }
    
    /*!
      \brief Set the ordering of the cross-correlation matrix axes.
    */
    void setAxisOrder (const Vector<Int>&);
    
    // === Access to the cross-correlation data-cube ===================
    
    /*!
      \brief Get the shape of the stored cc datacube
      
      The shape of the stored cc datacube will be \f$ [ N_{\rm ant}, N_{\rm ant},
      N_{\rm chan}] \f$, where \f$N_{\rm ant}\f$ is the number of antennae (array
      elements) and \f$N_{\rm chan}\f$ the number of output channels of the FFT
      plugin.
    */
    IPosition shape();
    
    /*!
      \brief Get the complex data cube
    */
    Cube<DComplex> ccCube ();
    
  /*!
    \fn Cube<DComplex> ccCube (const Vector<Int>&, const Vector<Int>&)

    \brief Get the complex data cube
  */
  Cube<DComplex> ccCube (const Vector<Int>&, 
			 const Vector<Int>&);
  
  /*!
    \brief Get the contents of the complex data cube selectively

    \param antennaMask   - Boolean array for the selection of antennae.
    \param frequencyMask - Boolean array for the selection of frequency
                           channels.
    
    \return ccCube - Cross-correlations for the selected set of antennae and
                     frequency channels.
  */
  Cube<DComplex> ccCube (const Vector<Bool>&, 
			 const Vector<Bool>&);
  
  // === Convert the cc datacube to visibilities =====================

  /*!
    \brief Convert the cc datacube to visibilities.

    Convert the cc datacube to visibilities, using all antenna elements and
    frequency channels.
   */
  Matrix<DComplex> visibilities ();

  /*!
    \brief Convert the cc datacube to visibilities.

    \param antennaMask   - Boolean array for the selection of antennae.
    \param frequencyMask - Boolean array for the selection of frequency
                           channels.
   */
  Matrix<DComplex> visibilities (const Vector<Bool>& antennaMask,
				const Vector<Bool>& frequencyMask);
  
  /*!
    \brief Convert the cc datacube to visibilities.

    \param nofChannels - Number of frequency channels.

    \verbatim
    freqs:=rec.get('Frequency',ants[refant]);
    l:=len(freqs);
    nants:=len(ants);
    nbl:=nants*(nants-1)/2;
    blflag:=array(T,nbl,l);
    baseline:=array(0,nbl,2);
    ary:=array(0,nants,l);
    cary:=array(0,nbl,l);
    visib:=array(0,nbl,nchan);
    freqsel:=array(T,nchan,l);
    bw:=(freq2-freq1)/nchan;

    for (i in seq(nchan)) {
      freqsel[i,]:=(freqs >= (freq1+(i-1)*bw)) & (freqs < (freq1+i*bw))
    }
    \endverbatim
   */
  Matrix<DComplex> visibilities (const Int nofChannels);

  /*!
    \brief Convert the cc datacube to visibilities.

    \param band        - Frequency band
    \param nofChannels - Number of frequency channels.
   */
  Matrix<DComplex> visibilities (const Vector<Double>& band, 
				const Int nofChannels);

  /*!
    \brief Get the antenna baselines for the visibility data

    \return baselines -- Array with the baselines
  */
  Matrix<Int> baselines ();
  
  
  // === Auto-/Cross-correlation spectra =======================================
  
  /*!
    \brief Get the auto-correlation power spectrum for an  antennae
    
    \param ant - Array index of the antenna.
  */
  Vector<Double> spectrum (const Int ant);  
  
  /*!
    \brief Get the cross-correlation power spectrum for a combination of antennae

    \param ant1 - Array index of the first antenna.
    \param ant2 - Array index of the second antenna.
  */
  Vector<Double> spectrum (const Int ant1,
			   const Int ant2);

  /*!
    \brief Export cross-correlation power spectrum to an output stream

    \param ant1 - Array index of the first antenna.
    \param ant2 - Array index of the second antenna.
    \param os   - Output stream to which the spectrum data are written
  */
  void spectrum (const Int ant1,
		 const Int ant2,
		 std::ostream& os);
  
  /*!
    \brief Get the complex cross-correlation spectrum for two antennas

    \param ant1 - Array index of the first antenna.
    \param ant2 - Array index of the second antenna.
  */
  Vector<DComplex> ccSpectrum (const Int ant1,
			      const Int ant2);
  
  // === Data I/O from/to disk =================================================

  /*!
    \brief Read in the cross-correlation data from disk

    Read in the cross-correlation data-cube from disk; location is stored by
    ObservationMeta, which also holds the information describing the shape of 
    the data-cube (this is defined by the settings of the
    \ref its_aviary_plugins.
    
    <i>Important note:</i> The extact contents of the stored data not only will
    depend on the way the cross-correlation data are written to disk, but also
    on the sign-conventions made in the FFT plugin.
   */
  void readDataCC ();

  // === Frequency channels contained in the data ==============================

  /*!
    \brief Get the number of points (channels) in the FFT.

    \return fftsize -- The number of points in the FFT.
  */
  Int fftsize () {
    return fftsize_p;
  }
  
  /*!
    \brief Get the frequency channels

    \return channel -- The available frequency channels
  */
  Vector<Int> frequencyChannels () {
    return availableChannels_p; 
  }
  
  /*!
    \fn Vector<Double> frequencyValues ()

    \brief Get the frequency values along the third cc-datacube axis

    Get the frequency values, in [Hz], at the center of each channel along the 
    third axis of the cross-correlation data-cube.
  */
  Vector<Double> frequencyValues ();

  /*!
    \fn Vector<Double> frequencyValues (Bool)

    \brief Get the frequency values along the third cc-datacube axis

    Get the frequency values, in [Hz], at the center of each channel along the 
    third axis of the cross-correlation data-cube.

    \param onlySelected - Return only the selected frequency channels?
  */
  Vector<Double> frequencyValues (const Bool);

  /*!
    \fn Vector<Bool> frequencyMask ()

    \brief Get a frequency mask for the applied channel selection.

    \return mask - Boolean array indicating wether a certain channel is selected
                   or deselected.
   */
  Vector<Bool> frequencyMask ();

 private:
  
  void initDataCC ();

  //! Extract frequency channels from the plugin options
  void setFrequencyChannels ();

  //! Put the path to the input data file together
  void setDatafile ();

  //! Compute the matrix for mapping of the data cube axes
  void axisTransformation ();

  /*!
    \brief Check the provided mask for the antenna selection
    
    \param inputAntMask -- 
  */
  void setAntennaSelection (Vector<Bool> const &inputAntMask);
  
  //! Check the provided mask for the frequency channel selection
  void setFrequencySelection (const Vector<Bool>&);
  
  /*! 
    \param antennaMask   - Boolean array for the selection of antennae.
    \param frequencyMask - Boolean array for the selection of frequency
    channels.
    
    \todo Check if copying values via slicing operates correctly!
  */
  Matrix<DComplex> ccCube2Visibilities(const Vector<Bool>&,
				       const Vector<Bool>&);
  
  };
  
}  //  Namespace CR -- end

#endif
