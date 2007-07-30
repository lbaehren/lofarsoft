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

// $Id$

#ifndef _SKYMAPPER_H_
#define _SKYMAPPER_H_

// C++ Standard library
#include <stdio.h>
#include <fstream>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/PagedImage.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>

// LOPES-Tools header files
#include <IO/DataReaderTools.h>
#include <Observation/ObservationData.h>
#include <Imaging/Beamformer.h>
#include <Imaging/SkymapCoordinates.h>
#include <Skymap/SkymapQuantity.h>

using casa::IPosition;
using casa::Matrix;
using casa::String;
using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class Skymapper
    
    \ingroup Imaging
    
    \brief Brief description for class Skymapper
    
    \author Lars B&auml;hren
    
    \date 2006/01/23
    
    \test tSkymapper.cc -- Basic testing of the Skymapper class
    \test tUseArrays.cc -- Test working with the CASA Array classes (inludes slicing)
    \test tUseLattices.cc -- Test working with the CASA Lattice classes; keep in
    mind that CASA images are derived from the PagedArray and Lattice classes.
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li><a href="http://casa.nrao.edu/docs/doxygen/group__Coordinates.html">CASA
      Coordinates package</a>
      <li><a href="http://casa.nrao.edu/docs/doxygen/group__Images.html">CASA
      Images package</a>
      
      <li>Beamformer
      <li>ObservationData
      <li>SkymapCoordinates -- coordinates involved in the creation of a skymap
      <li>SkymapQuantity
      <li>SkymapperTools
    </ul>
    
    <h3>Synopsis</h3>
    
    By default -- i.e. with the default constructor -- we set up everything for
    a [120,120] pixel all-sky map in STG projection, centered on (0,90) in AZEL
    coordinates; the various ingredients for the created PagedImage are delivered
    by the functions:
    <ul type="square">
      <li>Skymapper::defaultImageShape
      <li>Skymapper::defaultCoordsys
    </ul>
    
    <h3>Example(s)</h3>
    
    Currently the whole Skymapper class is undergoing fundamental changes in order
    to make it fit into the prototype-pipeline under construction by A. Horneffer.
    Besides obvious changes in the interface, a number of internal adjustment need
    to be carried out, to provide the required functionality.
    
    Here is a example of how the interface and usage will look like in the near
    future:
    \code
    using LOPES::Skymapper;
    using LOPES::SkymapCoordinates;
    
    bool status (true);
    uint nofBlocks;
    Matrix<DComplex> data;
    
    // Collect all the required coordinate information
    SkymapCoordinates coords (...);
    
  // Create a new Skymapper object
  Skymapper skymapper (coords);
  
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
    SkymapCoordinates coordinates_p;
    
    //! Object to handle the beamforming of the input data
    Beamformer beamformer_p;
    
    //! Pointer to a paged image containing the generated data
    casa::PagedImage<Double> *image_p;
    
    // -- book-keeping
    
    //! Switch to enable/disable verbose mode
    short verbose_p;
    
    //! Track the status of the Skymapper
    bool isOperational_p;
    
    //! The number of processed data blocks so far
    uint nofProcessedBlocks_p;

    //! Direction mask to flag individual pixels
    Matrix<bool> directionMask_p;
    
    // -- questionable variables (might be thrown out later) --------------------
    
    //! Electrical quantity, for which the sky map is generated
    SkymapQuantity quantity_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
      
      \f$ 120 \times 120 \f$ pixel grid of \f$ ( 2^\circ, 2^\circ ) \f$ deg
      resolution (AZEL, STG) map centered on \f$ ( 0, 90 ) \f$.
    */
    Skymapper ();
    
    /*!
      \brief Argumented constructor
      
      \param coordinates -- Coordinates information encapsulated in a
                            SkymapCoordinates object.
    */
    Skymapper (SkymapCoordinates const &coordinates);
    
  /*!
    \brief Argumented constructor

    \param coordinates -- Coordinates information encapsulated in a
                          SkymapCoordinates object.
    \param beamformer  -- Beamformer object
  */
  Skymapper (SkymapCoordinates const &coordinates,
	     Beamformer const &beamformer);
  
  /*!
    \brief Copy constructor

    \param other -- Another Skymapper object from which to create this new
                    one.
  */
  Skymapper (Skymapper const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~Skymapper ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another Skymapper object from which to make a copy.
  */
  Skymapper &operator= (Skymapper const &other); 

  // ----------------------------------------------------------------- Parameters

  inline short verboseLevel () {
    return verbose_p;
  }

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
  inline IPosition imageShape () {
    return coordinates_p.shape();
  }
  
  /*!
    \brief Set the name of (or path to) the created image file
    
    \param  filename -- Name of (or path to) the created image file
  */
  inline void setFilename (String const &filename) {
    filename_p = filename;
  }

  /*!
    \brief How many blocks of data have been processed so far?

    \return nofProcessedBlocks -- The number of processed data blocks so far
  */
  inline uint nofProcessedBlocks () {
    return nofProcessedBlocks_p;
  }

  /*!
    \brief Set the electrical quantity, for which the sky map is generated

    \brief quantity -- Electrical quantity, for which the sky map is generated
  */
  void setSkymapQuantity (SkymapQuantity::Type quantity);
  
  /*!
    \brief Get the mask to flag individual direction pixels
    
    \return directionMask -- [nof. Lon. pixels, nof. Lat. pixels] Matrix of
                             boolean values to keep track of individual 
			     pixels of the direction axes, which have been
			     flagged (e.g. because their world value lies
			     beneath the local horizon).
  */
  inline Matrix<bool> directionMask () {
    return directionMask_p;
  }
  
  // -------------------------------------------------------------------- Methods

  /*!
    \brief Initialize the internal structure of the Skymapper for data processing

    \return status -- Status of the operation; returns <tt>false</tt> if an error
                      was encountered.
  */
  bool initSkymapper ();

  /*!
    \brief Process a block of data and add the result to the image being created

    \todo We need to check whether of not we have reached the maximum number of 
    blocks to be processed; otherwise will operate outside the range of the 
    image pixel data array.

    \param data -- Array with the input data, [channels,antennas]; typically this
                   will be frequency domain data after calibration.
		   
    \return status -- Status of the operation; returns <tt>false</tt> if an error
                      was encountered.
  */
  bool processData (Matrix<DComplex> const &data);

  // ---------------------------------------------------------- Coordinate system

  /*!
    \brief Get the SkymapCoordinates object handling the coordinate operations
    
    \return skymapCoordinates -- Container and handler for the coordinates
  */
  inline SkymapCoordinates skymapCoordinates () {
    return coordinates_p;
  }

  /*!
    \brief Set the SkymapCoordinates object handling the coordinate operations
    
    \param coordinates -- Container and handler for the coordinates
    
    \return status -- Status of the operation; returns <tt>false</tt> if an error
                      was encountered.
  */
  bool setSkymapCoordinates (SkymapCoordinates const &coordinates);

  /*!
    \brief Get the coordinate system associated with the image
    
    \return cs -- The coordinate system as created via SkymapCoordinates
  */
  inline CoordinateSystem coordinateSystem () {
    return coordinates_p.coordinateSystem();
  }
  
  /*!
    \brief Set the direction axis of the coordinate system
 
    \param refcode    -- Reference code identifying the coordinate reference
                         frame, e.g. <i>AZEL</i>
    \param projection -- Reference code identifying the spherical map projection,
                         e.g. <i>AIT</i>
    \param crval      -- Reverence value at the position of the sky map
    \param cdelt      -- Coordinate increment
  */
  Bool setDirectionAxis (String const &refcode,
			 String const &projection,
			 const Vector<Double> &crval,
			 const Vector<Double>& cdelt);    

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
    \brief Update the antenna positions used by the Beamformer
    
    \param antPositions  -- [nofAntennas,3] Antenna positions for which the
                            delay is computed, \f$ (x,y,z) \f$
    
    \return status -- Status of the operation; returns <tt>false</tt> if an error
                      was encountered.
  */
  bool setAntennaPositions (Matrix<double> const &antPositions);

  // ------------------------------------------------------------------- feedback

  /*!
    \brief Provide a summary of the internal parameters to standard output
   */
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

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (Skymapper const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Initialize internal parameters at construction

    \param verbose            -- Be verbose during processing?
    \param isOperational      -- Is the Skymapper operational to process data?
    \param nofProcessedBlocks -- nof. blocks to be processed
    \param filename           -- Name of the image file created on disk
    \param coordinates        -- SkymapCoordinates object encapsulating the
                                 characteristics of the coordinate axes
    \param beamformer         -- Beamformer object, to handle the combination of
                                 antenna signals
   */
  void init (short const &verbose,
	     bool const &isOperational,
	     uint const &nofProcessedBlocks,
	     std::string const &filename,
	     SkymapCoordinates const &coordinates,
	     Beamformer const &beamformer);

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

};

}  // Namespace CR -- end

#endif /* _SKYMAPPER_H_ */
