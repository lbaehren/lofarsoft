/***************************************************************************
 *   Copyright (C) 2004-2006                                               *
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

/* $Id: Skymap.h,v 1.7 2006/10/31 18:24:08 bahren Exp $ */

#ifndef SKYMAP_H
#define SKYMAP_H

// C++ Standard library
#include <stdio.h>
#include <fstream>
using namespace std;

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/OS/Time.h>
#include <casa/Quanta.h>
#include <casa/BasicSL/Complex.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasTable.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
#include <tasking/Glish.h>

// include the WCS library
#include <wcslib/prj.h>

// custom
#include <LopesBase/DataFrequency.h>
#include <Observation/ObservationData.h>
#include <Observation/ObservationFrame.h>
#include <Skymap/Beamformer.h>
#include <Skymap/SkymapGrid.h>
#include <Skymap/SkymapQuantity.h>
#include <Utilities/ProgressBar.h>

/*!
  \class Skymap

  \ingroup Skymap

  \brief A Class to support the generation of skymaps.

  <h3>Prerequisite</h3>
  <ul type="square">
    <li>[AIPS++]
    <a href="http://aips2.nrao.edu/docs/user/General/node24.html">images</a>
    -- a module containing functionality to access, create, and analyze AIPS++
    images.
    <li>[AIPS++]
    <a href="http://aips2.nrao.edu/docs/aips/implement/Mathematics/FFTServer.html">FFTServer</a>
    -- An AIPS++ class with methods for Fast Fourier Transforms. The FFTServer
    class provides methods for performing n-dimensional Fast Fourier Transforms
    with real and complex Array's of arbitrary size and dimensionality. It can
    do either real to complex, complex to real, or complex to complex transforms
    with the "origin" of the transform either at the centre of the Array or at 
    the first element.
    <li>[AIPS++]
    <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Array.html">Array</a>
    -- A templated N-D Array class with zero origin. Vector, Matrix, and Cube
    are one, two, and three dimensional specializations of Array.
    <li>[AIPS++]
    <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/ArrayMath.html">ArrayMath</a>
    -- Mathematical operations for Arrays
    <li>[AIPS++]
    <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Slicer.html">Slicer</a>
    -- Specify which elements to extract from an n-dimensional array. If you
    need to extract or operate upon a portion of an array, the Slicer class is
    the best way to specify the subarray you are interested in.
    <li>[AIPS++]
    <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/IPosition.html">IPosition</a>
    -- A Vector of integers, for indexing into Array objects.
    <li>[AIPS++]
    <a href="http://aips2.nrao.edu/docs/coordinates/implement/Coordinates/CoordinateSystem.html">CoordinateSystem</a>
    --  CoordinateSystem is the normal interface to coordinate systems, typically
    attached to an ImageInterface, however the coordinate system can be
    manipulated on its own. CoordinateSystem is in turn composed from various
    classes derived from the base class Coordinate.

    <li>[LOPES Tools] Beamformer -- an implementation of various beamforming
        methods.
    <li>[LOPES-Tools] SkymapGrid -- (Regular) Coordinate grids for the creation
        of a Skymap.
  </ul>
*/

class Skymap : public Beamformer
{
 private:

  //! Electrical quantity stored in the sky map
  SkymapQuantity::Type quantity_p;

  //! Type of the electrical quantity stored (Field/Power)
  String signalQuantity_p;

  //! Domain for which the electrical quantity is provided (Time/Frequency)
  String signalDomain_p;

  //! Number of samples per datablock
  Int blocksize_p;

  //! The map data cube for the electric field (complex values).
  Matrix<DComplex> skymapComplex_p;

  //! The map data cube for the electric power (real values).
  Matrix<Double> skymapReal_p;

  //! Mapping of the stored matrix to a cube
  Matrix<Int> matrix2cube_p;

  //! The number of blocks added up to produce the map
  Int nofAddedBlocks_p;

  //! SkymapGrid object for the generation of pointing directions
  SkymapGrid grid_p;

  //! Distance parameter for the beamforming
  Double distance_p;

  //! CoordinateSystem object fro conversion between pixel and world coordinates
  CoordinateSystem csys_p;

  //! Flag to track if the map data cube has been resized.
  Bool itsMappingToCubeSet;
  Bool itsSkymapShapeSet;

  //! Flags to track which beamforming methods have been called
  Bool addBaselines_p;
  Bool crossCorrelation_p;

  //! Pointer to the function handling the imaging
  void (Skymap::*pFunction) (Matrix<DComplex> const &); 

 public:

  // === Construction / Deconstruction ===============================

  /*!
    \brief Empty Constructor
  */
  Skymap ();
  
  /*!
    \brief Argumented constructor

    \param obsInfo    -- Epoch and location of the observation.
    \param refcode    -- Reference code of the celestial coordinate system.
    \param projection -- WCS reference code of the spherical-to-cartesian
                         projection.
    \param shape      -- Shape of the coordinate grid, i.e. number of grid
                         nodes along each coordinate axis.
    \param center     -- Coordinates of the center of the grid, [deg].
    \param increment  -- Coordinate increment between two subsequent grid
                         nodes along a coordinate axis, [deg].
   */
  Skymap (const ObsInfo& obsInfo,
	  const String refcode,
	  const String projection,
	  const IPosition& shape,
	  const Vector<Double>& center,
	  const Vector<Double>& increment);
  
  ~Skymap ();

  // === Set multiple member data via GLishRecord ====================

  //! Set array size information (Blocksize, FFTlen, FFTsize)
  void setSkymap (GlishRecord&);

  // --- Access to the embedded SkymapGrid object --------------------

  /*!
    \brief Set the grid of coordinates

    Interface to the fully argumented SkymapGrid::SkymapGrid constructor.

    \param obsInfo    -- Epoch and location of the observation.
    \param refcode    -- Reference code of the celestial coordinate system.
    \param projection -- WCS reference code of the spherical-to-cartesian
                         projection.
    \param shape      -- Shape of the coordinate grid, i.e. number of grid
                         nodes along each coordinate axis.
    \param center     -- Coordinates of the center of the grid, [deg].
    \param increment  -- Coordinate increment between two subsequent grid
                         nodes along a coordinate axis, [deg].
   */
  void setSkymapGrid (const ObsInfo& obsInfo,
		      const String refcode,
		      const String projection,
		      const IPosition& shape,
		      const Vector<Double>& center,
		      const Vector<Double>& increment);
  
  /*!
    \brief Set the orientation of the skymap.
    
    Interface to SkymapGrid::setOrientation.

    \param directions - Vector with a description of the map orientation --
           the first element describing <i>longitude</i>. the second
	   <i>latitude</i>. Valid combinations are:
	   <tt>[East,North]</tt>,
	   <tt>[East,South]</tt>,
	   <tt>[West,North]</tt>,
	   <tt>[West,South]</tt>
  */
  void setOrientation (const Vector<String>& directions);

  /*!
    \brief Get the range in local elevation

    \return elevationRange -- Limits of the elevation range covered in the map
   */
  Vector<Double> elevationRange () const {
    return grid_p.elevationRange();
  }
  
  /*!
    \brief Set the limits for the local elevation range.

    Interface to SkymapGrid::setElevation.

    \param elevation -- Limits of the elevation range in the local reference
                        frame (AZEL).
  */
  void setElevationRange (const Vector<Double>& elevationRange) {
    grid_p.setElevationRange (elevationRange);
  }
  
  // --- Size of the datablocks processed ----------------------------

  /*!
    \brief Set the number of samples per block of data
    
    \param blocksize -- Number of samples per block of data.
  */
  void setBlocksize (Int const &blocksize) {
    blocksize_p = blocksize;
  }
  
  //! Get the number of samples per block of data
  Int blocksize () const {
    return blocksize_p;
  };
  
  // --- Electrial quantities stored in the map ----------------------

  /*!
    \brief Get the type of the electrical quantity stored.

    \return quantity -- Type of the electrical quantity, which is stored in the
                        map.
  */
  String signalQuantity () const {
    return signalQuantity_p;
  }

  /*!
    \brief Set the type of the electrical quantity stored.

    Set the Type of the electrical quantity, which is stored in the map; this
    can be either the electrical field, \f$ E(\rho) \f$, or the electrical power,
    \f$ P(\rho) \f$.
    
    \param signalQuantity = Type of the electrical quantity, which is stored in the
                            map; can be either <i>field</i>, for the electrical
			    field, or <i>power</i>, for the electrical power.
   */
  void setSignalQuantity (String const &quantity);

  //! Set the signal domain for which the electrical quantity is provided.
  /*!
    Set the signal domain for which the electrical quantity stored in the map
    is provided.

    \param signalDomain = The signal domain of the electrical quantity; can be
                          either <i>freq</i> or <i>time</i>.
   */
  void setSignalDomain (String const &domain);

  /*!
    \brief Get the signal domain for which the electrical quantity is provided.
    
    return domain -- The domain for which the electrical quantity is provided.
  */
  String signalDomain () {
    return signalDomain_p;
  }

  /*!
    \brief Set the electrical quantity stored in the sky map

    \param mapQuantity -- Electrical quantity stored in the sky map
   */
  void setSkymapQuantity (SkymapQuantity::Type const &mapQuantity);

  // === Coordinate system attached to the image =====================

  /*!
    \brief Set the coordinate system from parameters stored in a record

    \param rec -- GlishRecord containing the required parameters to put together
                  a CoordinateSystem object
   */
  void setCoordinateSystem (const GlishRecord &rec);

  /*!
    \brief Set coordinate system from aonther

    \param csys -- A CoordinateSystem object

    If the provided coordinate system does not math the current setup, at least
    check the provided coordinate system for the axes of interest.
  */
  void setCoordinateSystem (CoordinateSystem const &csys);

  /*!
    \brief Get coordinate system parameters of the direction coordinate.

    \param crval                -- Reference value
    \param cdelt                -- Coordinate increment
    \param xform                -- Linear coordinate transform
    \param crpix                -- Reference pixel
    \param worldAxisUnits       -- Physical unit associated with the axes
    \param projectionParameters -- Parameters of the spherial map projection
  */
  void directionCoordinate (Vector<Double>& crval,
			    Vector<Double>& cdelt,
			    Matrix<Double>& xform,
			    Vector<Double>& crpix,
			    Vector<String>& worldAxisUnits,
			    Vector<Double>& projectionParameters);

  // --- Distance parameter for the beamforming ----------------------

  /*!
    \brief Get the distance for near-field beamforming
    
    \return dist -- Distance, in [m].
  */
  Double distance () const {
    return distance_p;
  }
  
  /*!
    \brief Set the distance for near-field beamforming

    \param dist -- Distance, in [m], use by the Beamformer for beamforming
                   towards a source in the near-field.
  */
  void setDistance (Double const &dist) {
    distance_p = dist;
  }

  // === Interface to the Beamforming routines =======================

  /*!
    \brief Get the number of data block, which have been added in the map

    \return nofAddedBlocks -- The number of data blocks, which have been added
                              for each pixel in the created map. Unless highest 
			      time resolution is required, this whould be a 
			      standard way to increase the integration time.
  */
  Int AddedBlocks () const {
    return nofAddedBlocks_p;
  }
  
  //! Set up the phase gradients required for the beamforming.
  /*! Set up the array with the phase gradients for (i) a set of (az,el)
    coordinates, (ii) a set of antennas with positions \f$ (x, y, z) \f$
    and (iii) a set of frequency channels.

    \param frequency   - Frequency value associated with a bin in the frequency
                         array, in [Hz].
    \param posAntennas - 3D Position of the antennas, in [m].

    \todo Make fix for azimuth coordinate obsolete; this is required to compensate
    rotation by 180 deg.
   */
  void setPhaseGradients (Vector<Double> &frequencies,
			  Matrix<Double> &antennaPositions);

  /*!
    \brief Compute skymap from the input antenna data

    \param data - [antennas,frequencies] matrix with one datablock of
                  Fourier tranformed data per antenna.
  */
  void data2skymap (Matrix<DComplex> const &data) {
    (this->*pFunction)(data);
  };
  
  /*!
    \brief Beamforming by addition of individual antenna signals.
    
    Add up the contribution of each individual antenna on a per-channel
    basis; interface to Beamformer::BFaddAntennas
    
    \param data - [antennas,frequencies] matrix with one datablock of
                  Fourier tranformed data per antenna.
  */
  void addAntennas (Matrix<DComplex> const &data);

  /*!
    \brief Beamforming by addition of all baseline combinations.

    \param data - [antennas,frequencies] matrix with one datablock of Fourier
                  tranformed data per antenna.
   */
  void addBaselines (Matrix<DComplex> const &data);

  /*!
    \brief Beamforming by cross-correlation of the antenna signals.

    \todo Still needs to be implemented.
  */
  void crossCorrelation (Matrix<DComplex> const &data);

  /*!
    \brief Beamforming using the so-called <i>cc-Beam</i>

    \param data - [antennas,frequencies] matrix with one datablock of Fourier
                  tranformed data per antenna.

    \todo Still needs to be implemented.
   */
  void ccBeam (Matrix<DComplex> const &data);

  /*!
    \brief Beamforming by cross-correlation of the antenna signals.

    \param ccCube --- cross-correlation datacube, [nant,nant,nfreq].
  */
/*   void ccMatrix (const Cube<DComplex> &ccCube); */

  /*!
    \fn void ccMatrix (const Cube<DComplex>&, const Vector<Double>&)

    \brief Beamforming by cross-correlation of the antenna signals.

    Instead of obtaining the frequency values via the methods provided by
    DataFrequency, the values can be passed directly. This might be needed for
    processing cross-correlation datasets with preselection of frequency
    channels.

    \param ccCube           -- cross-correlation datacube, [nant,nant,nfreq].
    \param frequencies      -- Frequency values, [Hz], associated with the third
                               <i>ccCube</i> axis.
    \param antennaPositions -- 3-dimensional positions of the antenna elements.
  */
  void ccMatrix (const Cube<DComplex>& ccCube,
		 const Vector<Double>& frequencies,
		 const Matrix<Double>& antennaPositions);
  
  // === Access to the map data cube =================================
  
  /*!
    \brief Clear the skymap data cube.

    Clear the data cube holding the values of the sky brightness distribution.
    This method is requiered esp. when computing dynamic spectra of (a region of)
    the sky. All other internal setting -- esp. of the Beamformer -- are left
    untouched.
   */
  void clearSkymap ();

   /*!
     \brief Return the skymap matrix.

     All frequency channels are returned; works for \f$ E(\vec \rho, \nu) \f$.

    \param skymap -- The skymap data cube.
  */
  Matrix<DComplex> getSkymap (); 

  /*!
    \brief Return the skymap matrix.

    A specified number of frequeny channels per pixel are returned; works for
    \f$ E(\vec \rho, \nu) \f$.
    \param skymap   = [Matrix<DComplex>&] The skymap data cube.
    \param channels = [Int] The number of channels per pixels, i.e. the number
                      of elements along the third image axis.
  */
  void getSkymap (Matrix<DComplex>& skymap,
		  Int channels);

   /*!
     \brief Return the skymap matrix.

     All frequency channels are returned; works for \f$ E(\vec \rho, t) \f$,
     \f$ P(\vec \rho, t) \f$ and \f$ P(\vec \rho, \nu) \f$.

    \param skymap -- The skymap data cube.
  */
  void getSkymap (Matrix<Double>& skymap); 

  /*!
     \brief Return the skymap matrix.

    A specified number of frequeny channels per pixel are returned; works
    for \f$ E(\vec \rho, t) \f$, \f$ P(\vec \rho, t) \f$ and
    \f$ P(\vec \rho, \nu) \f$.

    \param skymap   -- Array with the skymap pixel values.
    \param channels -- The number of channels per pixels, i.e. the number
                       of elements along the third image axis.
  */
  void getSkymap (Matrix<Double>& skymap,
		  Int channels);

  /*!
    \brief Return the skymap as data cube

    Return the skymap as data cube, along with the masking array to flag invalid
    image pixels; this function applies to \f$ E(\vec \rho, \nu) \f$.

    \param skymap -- The skymap data cube.
    \param mask   -- The mask for the skymap data cube.
   */
  void getSkymap (Cube<DComplex>& skymap,
		  Cube<Bool>& mask);

  /*!
    \brief Return the skymap as data cube [DComplex]

    Return the skymap as data cube, along with the masking array to flag invalid
    image pixels; this function applies to \f$ E(\vec \rho, \nu) \f$.
    The matrix used internally is converted bback to a data cube; keep in mind
    that for the returned data cube we have [nphi,ntheta,nfreq], while the skymap
    internally is stored as [nfreq,npixels].

    \param skymap   -- The skymap data cube.
    \param mask     -- The mask for the skymap data cube.
    \param channels -- The number of channels per pixels, i.e. the number
                       of elements along the third image axis.
   */
  void skymap (Cube<DComplex>& skymap,
	       Cube<Bool>& mask,
	       Int channels);
  
  /*!
    \brief Return the skymap as data cube

    Return the skymap as data cube, along with the masking array to flag invalid
    image pixels; this function applies to \f$ E(\vec \rho, t) \f$,
    \f$ P(\vec \rho, t) \f$ and \f$ P(\vec \rho, \nu) \f$.

    \param skymap -- The skymap data cube.
    \param mask   -- The mask for the skymap data cube.
   */
  void skymap (Cube<Double>& skymap,
	       Cube<Bool>& mask);

  /*!
    \brief Return the skymap as data cube [Double]

    Return the skymap as data cube, along with the masking array to flag invalid
    image pixels; this function applies to \f$ E(\vec \rho, t) \f$,
    \f$ P(\vec \rho, t) \f$ and \f$ P(\vec \rho, \nu) \f$.

    \param skymap   -- The skymap data cube.
    \param mask     -- The mask for the skymap data cube.
    \param channels -- The number of channels per pixels, i.e. the number
                       of elements along the third image axis.
   */
  void skymap (Cube<Double>& skymap,
	       Cube<Bool>& mask,
	       Int channels);
  
  /*!
    \brief Dump the contents of the internal data array to an output stream.

    \param os -- Stream to which the data are written.
   */
  void dumpSkymap (std::ostream &os);

  /*!
    \brief Get the shape of the internal data array

    \retval skymapShape -- Number of elements per array axis.
   */
  IPosition shape ();

  /*!
    \brief Get the number of map pixels.

    \return nofPixels -- The number of pixels in the skymap.
  */
  Int nofPixels () {
    return grid_p.nelements();
  }

  /*!
    \brief Get the number of map pixels.

    \param which -- Which pixels to account for: if set <i>True</i> only the 
                    unflagged/unmasked pixels are accounted for.

    \return nofPixels -- The number of pixels in the skymap.
  */
  Int nofPixels (const Bool which) {
    return grid_p.nelements(which);
  }

  // === Display the member data =====================================

  /*!
    \brief Provide a summary of the internal parameters
    
    \param os -- Stream to which the information are written.
  */
  void printSkymap (std::ostream &os);

 private:

  //! Initialization of member data.
  void init ();

  /*!
    \brief Get the array of positions passed to the Beamformer
   */
  Matrix<Double> beamformingGrid ();

  /*!
    \brief Get the pixel mask as array conformant to the skymap pixel array

    \param channels -- The number of channels per pixel; if this number is 
                       different from the number of stored channels, regridding
		       will be performed.
  */
  Cube<Bool> mask (const Int channels);

  /*!
    \brief Convert internal skymap array (matrix) to exported skymap array (cube).

    Version for real valued skymap pixels.

    \param out       -- Array with the pixel values of the skymap, shaped
                        according the pixel shape and number of channels per
			image pixel.
    \param skymapIN  -- The internal array with the pixel values of the skymap,
                        containing only the subset of pixel values, which are 
			masked valid.
   */
  template <class T>
    void skymapMatrix2Cube (Cube<T>& out,
			    const Matrix<T>& skymapIN);
  
  //! Set the shape of the SkymapField matrix
  void setSkymapShape ();

  //! Set the Matrix-to-Cube mapping table
  void setMappingToCube (const Matrix<Int>& matrix2cube);

  //! Reconstruct data cube from internal matrix
  void mapToCube ();

  //! Construct a default coordinate system tool
  void setCoordinateSystem ();

  /* Adjust the signal domain for the skymap */
  void adjustSignalDomain (Matrix<DComplex>&);

  //! Get the array indices utilized for rebinning the third image axis.
  void getRebinningIndices (Matrix<Int>& indexValues,
			    Int channels);

  /*!
    Average the image data along the third axis (frequency/time); the input array
    is overwritten and returned with the rebinned data.
  */
  template <class T>
    void averageChannels (Vector<T>& channels,
			  Int nofChannels);
  /*!
    Average the image data along the third axis (frequency/time); the input array
    is overwritten and returned with the rebinned data.
  */
  template <class T>
    void averageChannels (Matrix<T> &channels,
			  Int const &nofChannels);
  
};

#endif 
