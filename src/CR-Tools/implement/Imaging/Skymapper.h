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
#include <images/Images/PagedImage.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>

#ifdef HAVE_HDF5
#include <images/Images/HDF5Image.h>
#endif

// LOPES-Tools header files
#include <Coordinates/SkymapCoordinate.h>
#include <Observation/ObservationData.h>
#include <Imaging/Beamformer.h>

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
      <li><a href="http://www.astron.nl/casacore/trunk/casacore/doc/html/group__coordinates.html">casacore Coordinates module</a>
      <li><a href="http://www.astron.nl/casacore/trunk/casacore/doc/html/group__images.html">casacore Images module</a>
      
      <li>Beamformer
      <li>ObservationData
      <li>SkymapCoordinate
      <li>SkymapQuantity
      <li>SkymapperTools
    </ul>
      
    <h3>Synopsis</h3>

    The bulk of the internal parameters and settings are stored within two 
    embedded objects of type SkymapCoordinate and Beamformer:
    <ul>
      <li>Since almost all of the parameters required by the emedded Beamformer
      object can be derived from the also embedded SkymapCoordinate object, only
      a minimal interface is provided to the outside world:
      <ul>
        <li>Skymapper::beamformer -- retrieve the embedded Beamformer object
	<li>Skymapper::skymapType
	<li>Skymapper::setSkymapType
	<li>Skymapper::setAntPositions -- as the Skymapper class itself is not
	handling the actual data I/O, the antenna positions required by the
	Beamformer need to be provided separately.
      </ul>
    </ul>

    Methods for inserting the computed beamformed data into the image file:
    \code
    // Function which extracts an array from the map.
    virtual Bool doGetSlice (Array<T> &buffer,
                             const Slicer &theSlice);

    // Function to replace the values in the map with soureBuffer. 
    virtual void doPutSlice (const Array<T> &sourceBuffer,
                             const IPosition &where,
			     const IPosition &stride);
    \endcode
    
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
    
  private:
    
    // -- fundamental internal data ---------------------------------------------
    
    //! Name of the image created on disk
    String filename_p;
    //! Container and handler for the coordinates
    SkymapCoordinate coord_p;
    //! Object to handle the beamforming of the input data
    Beamformer beamformer_p;
    //! Pointer to a paged image containing the generated data
    casa::PagedImage<Double> *image_p;

    // -- book-keeping
    
    //! Start position when writing new data to the image pixel array
    casa::IPosition start_p;
    //! Length/shape of the array written to the image pixel array
    casa::IPosition length_p;
    //! Stride along the axes within the exported image array
    casa::IPosition stride_p;
    //! Switch to enable/disable verbose mode
    short verbose_p;
    //! The number of processed data blocks so far
    uint nofProcessedBlocks_p;
    //! Direction mask to flag individual pixels
    Matrix<bool> directionMask_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
      
      \f$ 120 \times 120 \f$ pixel grid of \f$ ( 2^\circ, 2^\circ ) \f$ deg
      resolution (AZEL, STG) map centered on \f$ ( 0, 90 ) \f$.

      \param filename -- Name of the output file
    */
    Skymapper (std::string const &filename="skymap.img");
    
    /*!
      \brief Argumented constructor
      
      \param skymapCoord -- Coordinates information encapsulated in a
                            SkymapCoordinate object.
      \param filename -- Name of the output file
    */
    Skymapper (SkymapCoordinate const &skymapCoord,
	       std::string const &filename="skymap.img");
    
    /*!
      \brief Argumented constructor
      
      \param skymapCoord   -- Coordinates information encapsulated in a
             SkymapCoordinate object.
      \param antPositions  -- [nofAntennas,3] Antenna positions for which the
             delay is computed, \f$ (x,y,z) \f$
      \param skymapType -- The physical quantity (of the electromagnetic field)
             for which the skymap will be computed.
      \param filename -- Name of the output file
    */
    Skymapper (SkymapCoordinate const &skymapCoord,
	       Matrix<double> const &antPositions,
	       SkymapQuantity const &skymapType=SkymapQuantity::FREQ_POWER,
	       std::string const &filename="skymap.img");
    
    /*!
      \brief Argumented constructor
      
      \param skymapCoord   -- Coordinates information encapsulated in a
             SkymapCoordinate object.
      \param antPositions  -- Antenna positions for which the delay is computed,
             \f$ (x,y,z) \f$
      \param skymapType -- The physical quantity (of the electromagnetic field)
             for which the skymap will be computed.
      \param filename -- Name of the output file
    */
    Skymapper (SkymapCoordinate const &skymapCoord,
	       Vector<MVPosition> const &antPositions,
	       SkymapQuantity const &skymapType=SkymapQuantity::FREQ_POWER,
	       std::string const &filename="skymap.img");
    
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
      \brief Set the verbosity level during processing
      
      \param verbose -- The verbosity level during processing; if
             <tt>verbose=0</tt> no additional output will be produced.
    */
    inline void setVerboseLevel (short const &verbose) {
      verbose_p = verbose;
    }
    
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
      \brief Get the stride along the axes within the exported image array

      \return stride -- Stride along the axes within the exported image array
    */
    inline IPosition stride () const {
      return stride_p;
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
      
      \todo We need to check whether of not we have reached the maximum number of
      blocks to be processed; otherwise will operate outside the range of the 
      image pixel data array.
      
      \param data -- Array with the input data, [channels,antennas]; typically
             this will be frequency domain data after calibration.
      
      \return status -- Status of the operation; returns <tt>false</tt> if an
              error was encountered.
    */
    bool processData (Matrix<DComplex> const &data);

    // ------------------------------------------------------- Beamformer methods
    
    /*!
      \brief Get the type of beam to be used at processing
      
      \return beamType -- The type of beam to be used at data processing
    */
    inline SkymapQuantity skymapType () {
      return beamformer_p.skymapType();
    }
    
    /*!
      \brief Set the antenna positions

      \param antPositions -- [antenna,3] array with the antenna positions.
      \param type         -- Coordinate type as which the antenna positions are
             provided; if necessary conversion is performed internally.
      \param anglesInDegrees -- If the coordinates of the antenna positions
             contain an angular component, is this component given in degrees?
	     If set <tt>false</tt> the angular components are considered to be
	     given in radians.
    */
    inline bool setAntPositions (Matrix<double> const &antPositions,
				 CoordinateType::Types const &type,
				 bool const &anglesInDegrees) {
      return beamformer_p.setAntPositions(antPositions,
					  type,
					  anglesInDegrees);
    }

    /*!
      \brief Set the antenna positions

      \param antPositions -- Array with the antenna positions.
    */
    inline bool setAntPositions (Vector<MVPosition> const &antPositions) {
      return beamformer_p.setAntPositions(antPositions);
    }
    
    // ----------------------------------------------------------------- Beamformer
    
    /*!
      \brief Get the Beamformer object handling the combination of antenna signals
      
      \return beamformer -- Beamformer object, to handle the combination of
              antenna signals
    */
    inline Beamformer beamformer () const {
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
      - Observation ........  
      -- telescope          = WSRT
      -- observer           = LOFAR/CR KSP
      -- observation date   = Epoch: 53955::17:14:33.4210
      - Data I/O ..........   
      -- blocksize          = 1
      -- sampling rate [Hz] = 8e+07
      -- Nyquist zone       = 1
      - Coordinates ........  
      -- nof. coordinates   = 4
      -- names              = [Longitude, Latitude, Distance, Time, Frequency]
      -- units              = [rad, rad, m, s, Hz]
      -- ref. pixel         = [0, 0, 0, 0, 0]
      -- ref. value         = [0, 1.5708, -1, 0, 0]
      -- coord. increment   = [-0.0349066, 0.0349066, 0, 1, 1]
      - Image ..............  
      -- filename           = skymap.img
      -- image shape        = [120, 120, 1, 1, 1]
      -- blocks per frame   = 1
      \endverbatim
    */
    void summary (std::ostream &os);
    
    /*!
      \brief Provide a summary of basic properties of an image
      
      \param myimage -- AIPS++ image for which to provide the summary.
      \param os      -- Output stream to which the summary will be written.
    */
    template <class T>
      static void summary (const casa::PagedImage<T>& myimage,
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
    
    /*!
      \brief Unconditional copying
    */
    void copy (Skymapper const& other);
    
    //! Unconditional deletion 
    void destroy(void);
    
    /*!
      \brief Initialize the object's internal parameters
      
      \param coord -- Coordinates to be attached to the skymap.
    */
    void init (SkymapCoordinate const &coord);
    
    /*!
      \brief Initialize the object's internal parameters
      
      \param coord        -- Coordinates to be attached to the skymap.
      \param antPositions -- Antenna positions to be fed into the Beamformer
      \param skymapType   -- Electromagnetical quantity for which the skymap is
      getting computed.
    */
    void init (SkymapCoordinate const &skymapCoord,
	       Matrix<double> const &antPositions,
	       SkymapQuantity const &skymapType=SkymapQuantity::FREQ_POWER);
    
    /*!
      \brief Initialize the object's internal parameters
      
      \param coord        -- Coordinates to be attached to the skymap.
      \param antPositions -- Antenna positions to be fed into the Beamformer
      \param skymapType   -- Electromagnetical quantity for which the skymap is
      getting computed.
    */
    void init (SkymapCoordinate const &skymapCoord,
	       Vector<MVPosition> const &antPositions,
	       SkymapQuantity const &skymapType=SkymapQuantity::FREQ_POWER);
    
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
    
    /*!
      \brief Default direction coordinates axis
    */
    DirectionCoordinate defaultDirectionAxis ();
    
    /*!
      \brief Default settings for the distance axis
      
      If not specified otherwise, we assume generation of a "standard astrononomical
      sky map", i.e. beamforming in the far-field of the telescope.
    */
    LinearCoordinate defaultDistanceAxis ();
    
    /*!
      \brief Set the coordinates of the center of the skymap
    */
    bool setSkymapCenter ();
    
    /*!
      \brief Write the beamformed data to the PagedImage on disk
      
      \param beam -- [position,channel] Output of the Beamformer
      
      \return status -- Status of the operation; returns <tt>false</tt> if an error
      was encountered.
    */
    bool write_beam_to_image (Matrix<double> const &beam);
    
  };
  
}  // Namespace CR -- end

#endif /* _SKYMAPPER_H_ */
