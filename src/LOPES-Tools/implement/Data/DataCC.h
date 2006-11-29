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

/* $Id: DataCC.h,v 1.7 2006/05/04 11:46:29 bahren Exp $ */

#ifndef DATACC_H
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

#include <Observation/ObservationMeta.h>
#include <Utilities/Masking.h>
#include <Utilities/StringTools.h>

using casa::Complex;
using casa::Cube;
using casa::IPosition;
using casa::Matrix;
using casa::String;
using casa::Vector;

namespace LOPES {  // namespace LOPES -- begin

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

    <li>[LOPES-Tools] StringTools
    <li>[LOPES-Tools] Masking
    <li>[LOPES-Tools] ObservationMeta
  </ul>

  \test tDataCC.cc
*/
  class DataCC : public ObservationMeta {
    
    // Path to the input datafile
    String datafile_p;
    // Data cube containing the cross correlated data
    Cube<Complex> ccCube_p;
    // Ordering of the data cube axes within the disk file
    Vector<int> axisOrder_p;
    // Number of points (channels) in the fft.
    int fftsize_p;
    // Channels numbers of the frequencies
    Vector<int> availableChannels_p;
    // Antenna selection
    Vector<bool> antennaSelection_p;
    // Frequency channel selection
    Vector<bool> frequencySelection_p;
    // Baselines for a visibility dataset
    Matrix<int> baselines_p;
    
 public:
  
  // === Construction / Deconstruction =========================================

  /*!
    \fn DataCC (String)

    \brief Argumented constructor

    Provides single parameter to ObservationMeta base-class.

    \param filename - Location where the metafile is stored on disk.
  */
  DataCC (String);

  /*!
    \fn DataCC (String, String)

    \brief Argumented constructor

    Provides parameters to ObservationMeta base-class.

    \param metaFile   - Location where the metafile is stored on disk.
    \param antposFile - Location of the antenna position file.
  */
  DataCC (String, String);

  // === Mapping between data cube axes in disk file and storage ===============

  /*!
    \fn Vector<int> axisOrder ()

    \brief Get the ordering of the cross-correlation matrix axes.
  */
  Vector<int> axisOrder () {
    return axisOrder_p;
  }
  
  /*!
    \fn void setAxisOrder (const Vector<int>&)

    \brief Set the ordering of the cross-correlation matrix axes.
  */
  void setAxisOrder (const Vector<int>&);

  // === Access to the cross-correlation data-cube ===================

  /*!
    \fn IPosition shape()

    \brief Get the shape of the stored cc datacube

    The shape of the stored cc datacube will be \f$ [ N_{\rm ant}, N_{\rm ant},
    N_{\rm chan}] \f$, where \f$N_{\rm ant}\f$ is the number of antennae (array
    elements) and \f$N_{\rm chan}\f$ the number of output channels of the FFT
    plugin.
  */
  IPosition shape();

  /*!
    \fn Cube<Complex> ccCube ()

    \brief Get the complex data cube
  */
  Cube<Complex> ccCube ();

  /*!
    \fn Cube<Complex> ccCube (const Vector<int>&, const Vector<int>&)

    \brief Get the complex data cube
  */
  Cube<Complex> ccCube (const Vector<int>&, 
			const Vector<int>&);
  
  /*!
    \fn Cube<Complex> ccCube (const Vector<bool>&, const Vector<bool>&)

    \brief Get the contents of the complex data cube selectively

    \param antennaMask   - boolean array for the selection of antennae.
    \param frequencyMask - boolean array for the selection of frequency
                           channels.
    
    \return ccCube - Cross-correlations for the selected set of antennae and
                     frequency channels.
  */
  Cube<Complex> ccCube (const Vector<bool>&, 
			const Vector<bool>&);

  // === Convert the cc datacube to visibilities =====================

  /*!
    \fn Matrix<Complex> visibilities ()

    \brief Convert the cc datacube to visibilities.

    Convert the cc datacube to visibilities, using all antenna elements and
    frequency channels.
   */
  Matrix<Complex> visibilities ();

  /*!
    \brief Convert the cc datacube to visibilities.

    \param antennaMask   - boolean array for the selection of antennae.
    \param frequencyMask - boolean array for the selection of frequency
                           channels.
   */
  Matrix<Complex> visibilities (const Vector<bool>& antennaMask,
				const Vector<bool>& frequencyMask);
  
  /*!
    \fn Matrix<Complex> visibilities (const int)

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
  Matrix<Complex> visibilities (const int nofChannels);

  /*!
    \brief Convert the cc datacube to visibilities.

    \param band        - Frequency band
    \param nofChannels - Number of frequency channels.
   */
  Matrix<Complex> visibilities (const Vector<double>& band, 
				const int nofChannels);

  /*!
    \brief Get the antenna baselines for the visibility data
   */
  Matrix<int> baselines ();


  // === Auto-/Cross-correlation spectra =======================================

  /*!
    \brief Get the auto-correlation power spectrum for an  antennae

    \param ant - Array index of the antenna.
  */
  Vector<double> spectrum (const int ant);  

  /*!
    \brief Get the cross-correlation power spectrum for a combination of antennae

    \param ant1 - Array index of the first antenna.
    \param ant2 - Array index of the second antenna.
  */
  Vector<double> spectrum (const int ant1,
			   const int ant2);

  /*!
    \brief Export cross-correlation power spectrum to an output stream

    \param ant1 - Array index of the first antenna.
    \param ant2 - Array index of the second antenna.
    \param os   - Output stream to which the spectrum data are written
  */
  void spectrum (const int ant1,
		 const int ant2,
		 std::ostream& os);
  
  /*!
    \brief Get the complex cross-correlation spectrum for two antennas

    \param ant1 - Array index of the first antenna.
    \param ant2 - Array index of the second antenna.
  */
  Vector<Complex> ccSpectrum (const int ant1,
			      const int ant2);
  
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
  int fftsize () {
    return fftsize_p;
  }
  
  /*!
    \brief Get the frequency channels

    \return channel -- The available frequency channels
  */
  Vector<int> frequencyChannels () {
    return availableChannels_p; 
  }
  
  /*!
    \fn Vector<double> frequencyValues ()

    \brief Get the frequency values along the third cc-datacube axis

    Get the frequency values, in [Hz], at the center of each channel along the 
    third axis of the cross-correlation data-cube.
  */
  Vector<double> frequencyValues ();

  /*!
    \fn Vector<double> frequencyValues (bool)

    \brief Get the frequency values along the third cc-datacube axis

    Get the frequency values, in [Hz], at the center of each channel along the 
    third axis of the cross-correlation data-cube.

    \param onlySelected - Return only the selected frequency channels?
  */
  Vector<double> frequencyValues (const bool);

  /*!
    \fn Vector<bool> frequencyMask ()

    \brief Get a frequency mask for the applied channel selection.

    \return mask - boolean array indicating wether a certain channel is selected
                   or deselected.
   */
  Vector<bool> frequencyMask ();

 private:
  
  void initDataCC ();

  //! Extract frequency channels from the plugin options
  void setFrequencyChannels ();

  //! Put the path to the input data file together
  void setDatafile ();

  //! Compute the matrix for mapping of the data cube axes
  void axisTransformation ();

  //! Check the provided mask for the antenna selection
  void setAntennaSelection (const Vector<bool>&);

  //! Check the provided mask for the frequency channel selection
  void setFrequencySelection (const Vector<bool>&);

  /*! 
    \param antennaMask   - boolean array for the selection of antennae.
    \param frequencyMask - boolean array for the selection of frequency
                           channels.

    \todo Check if copying values via slicing operates correctly!
   */
  Matrix<Complex> ccCube2Visibilities(const Vector<bool>&,
				      const Vector<bool>&);

  };  // class DataCC -- end

}  // namespace LOPES -- end

#endif
