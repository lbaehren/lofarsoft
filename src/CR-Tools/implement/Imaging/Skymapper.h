/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

#ifndef CR_SKYMAPPER_H
#define CR_SKYMAPPER_H

#include <crtools.h>

// C++ Standard library
#include <stdio.h>
#include <fstream>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Containers/Record.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/HDF5Image.h>
#include <images/Images/PagedImage.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>

// CR-Tools header files
#include <Coordinates/SkymapCoordinate.h>
#include <Data/ObservationData.h>
#include <Imaging/Beamformer.h>
#include <IO/DataReader.h>

using casa::Matrix;
using casa::String;
using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class Skymapper
    
    \ingroup CR_Imaging
    
    \brief Brief description for class Skymapper
    
    \author Lars B&auml;hren
    
    \date 2006/01/23
    
    \test tSkymapper.cc -- Basic testing of the Skymapper class
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>casacore modules:
      <ul>
        <li><a href="http://www.astron.nl/casacore/trunk/casacore/doc/html/group__coordinates.html">Coordinates</a>
	<li><a href="http://www.astron.nl/casacore/trunk/casacore/doc/html/group__images.html">Images</a>
      </ul>
      <li>SkymapCoordinate: Container for the coordinates associated with a sky map.
      <ul>
        <li>ObservationData: Store information describing an observation.
	<li>SkymapQuantity: Container for the settings to determine the quantity
	of a skymap.
	<li>SpatialCoordinate: Container to combine other coordinates into a
	spatial (3D) coordinate.
	<li>TimeFreqCoordinate: Container for the time-frequency domain parameters
	of a sky map.
      </ul>
      <li>Beamformer
      <li>DataReader
    </ul>
      
    <h3>Synopsis</h3>

    The bulk of the internal parameters and settings are stored within two 
    embedded objects of type SkymapCoordinate and Beamformer:
    <ul>
      <li>Since almost all of the parameters required by the emedded Beamformer
      object can be derived from the also embedded SkymapCoordinate object, only
      a minimal interface is provided to the outside world for the first of the 
      two:
      <ul>
        <li>Skymapper::beamformer -- retrieve the embedded Beamformer object
	<li>Skymapper::setAntPositions -- as the Skymapper class itself is not
	handling the actual data I/O, the antenna positions required by the
	Beamformer need to be provided separately.
	<li>Skymapper::setNearField -- switch the Beamformer to perform its
	computation for the near-field geometry
	<li>Skymapper::setFarField -- switch the Beamformer to perform its
	computation for the far-field geometry
      </ul>
    </ul>

    Here is the basic procedure when writing your own piece of code utilizing the
    Skymapper class:
    <ol>
      <li>Create a TimeFreqCoordinate
      <li>Create a SpatialCoordinate
      <li>Create a SkymapCoordinate object storing all the relevant coordiate
      information such as
      <ul>
        <li>Type of SpatialCoordinate
	<li>Reference code for the celestial coordinate system
	<li>Spherical map projection
	<li>Resolution and number of pixels
	<li>Input blocksize for the FFT, sampling frequency, Nyquist zone
      </ul>
    </ol>

    <h3>Example(s)</h3>
    
    Here is a example of how the interface and usage will look like in the near
    future; again keep in mind, that the actual access to the data is outside the
    scope of the Skymapper class, in order to keep it as independent of whatever
    circumstances it might be used in.
    \code
    bool status (true);
    uint nofBlocks (10);
    Matrix<DComplex> data;
    
    // Collect all the required coordinate information
    CR::SkymapCoordinate coord (...);
    
    // Create a new Skymapper object
    CR::Skymapper skymapper (coord);
    
    if (skymapper.initSkymapper()) {
      while (skymapper.nofProcessedBlocks()<=nofBlocks) {
        // get the current block of data
	data = some_function();
	
	// process the data block and integrate the result into the image
	status = skymapper.processData (data);
      }
    }
    \endcode
  */
  class Skymapper {

  public:

    /*!
      \brief Type of image create on disk

      At the time being the only two types supported are the two concrete
      implementations of the casacore \e ImageInterface class.
    */
    enum ImageType {
      //! Tables-based implementation of the casacore ImageInterface
      PagedImage,
      //! HDF5 implementation of the casacore ImageInterface
      HDF5Image
    };

  private:
    
    // -- fundamental internal data ---------------------------------------------
    
    //! Name of the image created on disk
    String filename_p;
    //! Data type of the created image (CASA_IMAGE or HDF5)
    Skymapper::ImageType imageType_p;
    //! Container and handler for the coordinates
    SkymapCoordinate coord_p;
    //! Object to handle the beamforming of the input data
    Beamformer beamformer_p;
    //! Pointer to a paged image containing the generated data
    casa::ImageInterface<float> *image_p;

    // -- book-keeping

    //! Aray buffering the data returned by the Beamformer 
    casa::Array<float> bufferArray_p;
    //! Position marking the start of the buffer within the output array
    casa::IPosition bufferStart_p;
    //! Stride through the output array to insert the buffer array
    casa::IPosition bufferStride_p;
    //! Increment of the position markers between two subsequent data-blocks
    casa::IPosition bufferStep_p;
    //! Index of the time-frequency axis of the target domain
    uint timeFreqAxis_p;
    
    //! The number of processed data blocks so far
    uint nofProcessedBlocks_p;
    //! The number of input data blocks combined into a time-frame
    uint nofBlocksPerFrame_p;
    //! The number of processed frames
    uint nofProcessedFrames_p;
    //! Direction mask to flag individual pixels
    Matrix<bool> directionMask_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
      
      \f$ 120 \times 120 \f$ pixel grid of \f$ ( 2^\circ, 2^\circ ) \f$ deg
      resolution (AZEL, STG) map centered on \f$ ( 0, 90 ) \f$.

      \param filename  -- Name of the output file
      \param imageType -- Data type of the created image (PagedImage or HDF5Image)
    */
    Skymapper (std::string const &filename="skymap.h5",
	       Skymapper::ImageType const &imageType=Skymapper::HDF5Image);
    
    /*!
      \brief Argumented constructor
      
      \param skymapCoord -- Coordinates information encapsulated in a
                            SkymapCoordinate object.
      \param filename  -- Name of the output file
      \param imageType -- Data type of the created image (PagedImage or HDF5Image)
    */
    Skymapper (SkymapCoordinate const &skymapCoord,
	       std::string const &filename="skymap.h5",
	       Skymapper::ImageType const &imageType=Skymapper::HDF5Image);
    
    /*!
      \brief Argumented constructor
      
      \param skymapCoord   -- Coordinates information encapsulated in a
             SkymapCoordinate object.
      \param antPositions  -- [nofAntennas,3] Antenna positions for which the
             delay is computed, \f$ (x,y,z) \f$
      \param filename  -- Name of the output file
      \param imageType -- Data type of the created image (PagedImage or HDF5Image)
    */
    Skymapper (SkymapCoordinate const &skymapCoord,
	       Matrix<double> const &antPositions,
	       std::string const &filename="skymap.h5",
	       Skymapper::ImageType const &imageType=Skymapper::HDF5Image);
    
    /*!
      \brief Argumented constructor
      
      \param skymapCoord   -- Coordinates information encapsulated in a
             SkymapCoordinate object.
      \param antPositions  -- Antenna positions for which the delay is computed,
             \f$ (x,y,z) \f$
      \param filename  -- Name of the output file
      \param imageType -- Data type of the created image (PagedImage or HDF5Image)
    */
    Skymapper (SkymapCoordinate const &skymapCoord,
	       Vector<MVPosition> const &antPositions,
	       std::string const &filename="skymap.h5",
	       Skymapper::ImageType const &imageType=Skymapper::HDF5Image);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Skymapper object from which to create this new
      one.
    */
    Skymapper (Skymapper const& other);
    
    // ---------------------------------------------------------------- Destruction
    
    //! Destructor
    ~Skymapper ();
    
    // ------------------------------------------------------------------ Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Skymapper object from which to make a copy.
    */
    Skymapper &operator= (Skymapper const &other); 
    
    // ----------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of (or path to) the created image file
      
      \return filename -- Name of (or path to) the created image file
    */
    inline String filename () const {
      return filename_p;
    }
    
    /*!
      \brief Get the pixel array shape of the created image
      
      \return shape -- Shape of the pixel array inside the created image
    */
    inline casa::IPosition imageShape () {
      return coord_p.shape();
    }

    /*!
      \brief How many blocks of data have been processed so far?
      
      \return nofProcessedBlocks -- The number of processed data blocks so far
    */
    inline uint nofProcessedBlocks () const {
      return nofProcessedBlocks_p;
    }
    
    /*!
    \brief Get the SkymapCoordinates object handling the coordinate operations
    
    \return skymapCoordinates -- Container and handler for the coordinates
    */
    inline SkymapCoordinate skymapCoordinate () {
      return coord_p;
    }
    
    /*!
      \brief Set the SkymapCoordinate object handling the coordinate operations
      
      \param coordinates -- Container and handler for the coordinates
      
      \return status -- Status of the operation; returns <tt>false</tt> if an error
      was encountered.
    */
    bool setSkymapCoordinate (SkymapCoordinate const &skymapCoord);
    
    /*!
      \brief Get the coordinate system associated with the image
      
      \return cs -- The coordinate system as created via SkymapCoordinates
    */
    inline casa::CoordinateSystem coordinateSystem () {
      return coord_p.coordinateSystem();
    }
    
    /*!
      \brief The quantity for which the skymap is computed
      
      \return skymapQuantity -- The skymap quantity for which to compute the
              Beamformer is configured
    */
    inline SkymapQuantity skymapQuantity () {
      return coord_p.skymapQuantity();
    }
    
    /*!
      \brief Get the mask to flag individual direction pixels
      
      \return directionMask -- [nof. Lon. pixels, nof. Lat. pixels] Matrix of
              boolean values to keep track of individual pixels of the direction
	      axes, which have been flagged (e.g. because their world value lies
	      beneath the local horizon).
    */
    inline Matrix<bool> directionMask () const {
      return directionMask_p;
    }
    
    // -------------------------------------------------------------------- Methods
    
    /*!
      \brief Process a block of data and add the result to the image being created
      
      \param data -- Array with the input data, [channels,antennas]; typically
             this will be frequency domain data after calibration.
      
      \return status -- Status of the operation; returns <tt>false</tt> if an
              error was encountered.
    */
    bool processData (Matrix<DComplex> const &data);

    /*!
      \brief Process the datato fill the image

      Instead of handling reading in the data yourself -- i.e. in the application
      program using this class -- one can also have the DataReader handle this
      part of the processing. This method will subsequenctly call
      DataReader::calfft until the sufficient number of data blocks have been
      read from the input dataset.

      \param dr -- DataReader object handling the access to the input data.
    */
    bool processData (DataReader *dr);

    // ==========================================================================
    //
    //  Beamformer 
    //
    // ==========================================================================

    //! Set the antenna positions
    bool setAntPositions (Matrix<double> const &antPositions,
			  CoordinateType::Types const &type,
			  bool const &anglesInDegrees);
    //! Set the antenna positions
    bool setAntPositions (Vector<MVPosition> const &antPositions);
    
    // ----------------------------------------------------------------- Beamformer
    
    /*!
      \brief Get the Beamformer object handling the combination of antenna signals
      
      \return beamformer -- Beamformer object, to handle the combination of
              antenna signals
    */
    inline Beamformer beamformer () {
      return beamformer_p;
    }
    
    /*!
      \brief Set a new Beamformer object handle the combination of antenna signals
      
      \param beamformer -- Beamformer object, to handle the combination of
      antenna signals
      
      \return status -- Status of the operation; returns <tt>false</tt> if an error
      was encountered.
    */
    bool setBeamformer (Beamformer const &beamformer);

    /*!
      \brief Is the beamforming performed for far-field geometry?
      
      \return farField -- Returns \e true in case the beamforming is performed for
              the far-field geometry.
    */
    inline bool farField () {
      return beamformer_p.farField();
    }
    
    /*!
      \brief Perform beamforming for far-field geometry
    */
    inline void setFarField () {
      beamformer_p.farField(true);
    }
    
    /*!
      \brief Perform beamforming for near-field geometry
    */
    inline void setNearField () {
      beamformer_p.nearField(true);
    }

    //! Enable/Disable buffering of delays, phases and weights
    bool setBeamformerBuffers (bool const &bufferDelays,
			       bool const &bufferPhases,
			       bool const &bufferWeights);
    
    // ------------------------------------------------------------------- feedback
    
    //! Provide a summary of the internal parameters to standard output
    inline void summary () {
      summary (std::cout);
    }
    
    /*!
      \brief Provide a summary of the internal parameters
      
      \param os -- Output stream, to which the summary is written
      
      The output typically will look something these lines:
      \verbatim
      [Skymapper] Summary of the internal parameters
      ::: Observation / Data :::
      --> Observation date         = Epoch: 54887::13:45:53.1101
      --> Telescope                = WSRT
      --> Observer                 = Lars Baehren
      --> nof. antennas            = 10
      --> Sampling rate       [Hz] = 8e+07
      --> Nyquist zone             = 1
      --> Blocksize      [samples] = 1024
      --> FFT length    [channels] = 513
      ::: Coordinates :::
      --> Skymap quantity          = FREQ_POWER
      --> Domain of image quantity = Frequency
      --> nof. coordinates         = 4
      --> World axis names ....... = [Longitude, Latitude, Length, Frequency, Time]
      --> World axis units ....... = [rad, rad, m, Hz, s]
      --> Reference pixel  (CRPIX) = [0, 0, 0, 0, 0]
      --> Reference value  (CRVAL) = [0, 1.5708, 0, 0, 0]
      --> coord. increment (CDELT) = [-0.0174533, 0.0174533, 1, 78125, 1.28e-05]
      ::: Image :::
      --> Filename                 = skymap.h5
      --> Shape of pixel array     = [20, 20, 10, 513, 1]
      --> Shape of beam array      = [513, 4000]
      --> buffer start position    = [0, 0, 0, 0, 0]
      --> buffer end position      = [19, 19, 9, 512, 0]
      --> buffer step size         = [0, 0, 0, 1, 0]
      --> index of the target axis = 3
      \endverbatim
    */
    void summary (std::ostream &os);
    
    /*!
      \brief Provide a summary of basic properties of an image
      
      \param myimage -- AIPS++ image for which to provide the summary.
      \param os      -- Output stream to which the summary will be written.
    */
    template <class T>
      static void summary (const casa::ImageInterface<T>& myimage,
			   std::ostream &os=std::cout)
      {
	casa::IPosition shape (myimage.shape());
	int nofAxes (shape.nelements());
	double nofPixes (1.0);
	bool stripPath (true);
	
	for (int n(0); n<nofAxes; n++) {
	  nofPixes *= shape(n);
	}
	
	os << " -- Name             : " << myimage.name(stripPath) << std::endl;
	os << " -- Image shape      : " << shape                   << std::endl;
	os << " -- Number of pixels : " << nofPixes                << std::endl;
	os << " -- is persistent    : " << myimage.isPersistent()  << std::endl;
	os << " -- is paged         : " << myimage.isPaged()       << std::endl;
	os << " -- is writable      : " << myimage.isWritable()    << std::endl;
	os << " -- has pixel mask   : " << myimage.hasPixelMask()  << std::endl;
      }
    
  private:
    
    //! Unconditional copying
    void copy (Skymapper const& other);
    //! Unconditional deletion 
    void destroy(void);
    /*!
      \brief Initialize the object's internal parameters
      
      \param coord -- Coordinates to be attached to the skymap.
    */
    void init (SkymapCoordinate const &coord);
    //! Initialize the object's internal parameters
    void init (SkymapCoordinate const &skymapCoord,
	       Matrix<double> const &antPositions);
    //! Initialize the object's internal parameters
    void init (SkymapCoordinate const &skymapCoord,
	       Vector<MVPosition> const &antPositions);
    
    /*!
      \brief Initialize the internal structure of the Skymapper for data processing
      
      \return status -- Status of the operation; returns <tt>false</tt> if an error
              was encountered.
    */
    bool initSkymapper ();
    
    /*!
      \brief Default function for retrival of data during the imging process
      
      Unless redirected to an external function, the Skymapper will be using the
      embedded DataReader object to retrieve the data itself.
      
      \return fft -- Spectral data, [freq,ant]
    */
    void getData (Matrix<DComplex> &data);
    
    //! Default direction coordinates axis
    DirectionCoordinate defaultDirectionAxis ();
    
    /*!
      \brief Default settings for the distance axis
      
      If not specified otherwise, we assume generation of a "standard astrononomical
      sky map", i.e. beamforming in the far-field of the telescope.
    */
    LinearCoordinate defaultDistanceAxis ();
    //! Set the coordinates of the center of the skymap
    bool setSkymapCenter ();
    
  };
  
}  // Namespace CR -- end

#endif /* _SKYMAPPER_H_ */
