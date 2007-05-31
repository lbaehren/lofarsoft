/***************************************************************************
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

/* $Id: Skymapper.h,v 1.21 2007/04/13 13:44:29 bahren Exp $*/

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
#include <Skymap/Skymap.h>
#include <Imaging/SkymapCoordinates.h>
#include <Skymap/SkymapQuantity.h>

using casa::IPosition;
using casa::Matrix;
using casa::String;
using casa::Vector;

namespace CR {  // Namespace CR -- begin

/*!
  \class Skymapper
  
  \ingroup CR
  \ingroup Imaging
  
  \brief Brief description for class Skymapper
  
  \author Lars B&auml;hren
  
  \date 2006/01/23
  
  \test tSkymapper.cc
  
  <h3>Prerequisite</h3>

  <ul type="square">
    <li><a href="http://casa.nrao.edu/docs/doxygen/group__Coordinates.html">CASA
    Coordinates package</a>
    <li><a href="http://casa.nrao.edu/docs/doxygen/group__Images.html">CASA
    Images package</a>

    <li>Beamformer
    <li>DataReader
    <li>DataReaderTools
    <li>ObservationData
    <li>Skymap
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
  
  if (skymapper.isOperational()) {
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

  // -- fundamental internal data -----------------------------------------------

  //! Name of the image created on disk
  String filename_p;

  //! Container and handler for the coordinates
  SkymapCoordinates coordinates_p;

  //! Skymap handler
  Skymap skymap_p;

  // -- book-keeping
  
  //! Switch to enable/disable verbose mode
  bool verbose_p;
  
  //! Track the status of the Skymapper
  bool isOperational_p;
  
  //! The number of processed data blocks so far
  uint nofProcessedBlocks_p;

  // -- questionable variables (might be thrown out later) ----------------------

  //! Electrical quantity, for which the sky map is generated
  SkymapQuantity quantity_p;

 public:

  // --------------------------------------------------------------- Construction

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

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Process a block of data and add the result to the image being created

    \todo We need to check whether of not we have reached the maximum number of 
    blocks to be processed; otherwise will operate outside the range of the 
    image pixel data array.

    \param data -- Array with the input data, [channels,antennas]; typically this
                   will be frequency domain data after calibration.
		   
    \return status -- Status of the operation; returns <tt>false</t> if an error
                      was encountered.
  */
  bool processData (Matrix<DComplex> const &data);

  // ---------------------------------------------------------- Coordinate system

  /*!
    \brief Get the coordinate system associated with the image
    
    \return cs -- The coordinate system as created via SkymapCoordinates
  */
  inline CoordinateSystem coordinateSystem () {
    return coordinates_p.coordinateSystem();
  }
  
  /*!
    \brief Set the coordinate system from a Glish record of another one
    
    \param csys -- Another CoordinateSystem object
  */
  void setCoordinateSystem (CoordinateSystem const &csys);

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

  // ------------------------------------------------------------------- feedback

  /*!
    \brief Provide a summary of the internal parameters to standard output
   */
  void summary ();

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
    \brief Default function for retrival of data during the imging process

    Unless redirected to an external function, the Skymapper will be using the
    embedded DataReader object to retrieve the data itself.

    \return fft -- Spectral data, [freq,ant]
  */
  void getData (Matrix<DComplex> &data);

  /*!
    \brief Create an image file on disk to be fillled later on

    Collect all the information we have so far an create a PagedImage on disk, to
    which the results from the processed data can be added to.

    \return status -- Status of the operation; returns <i>false</i> if an error
                      was encountered.
  */
  Bool createImage ();

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
