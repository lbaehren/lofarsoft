/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1378 2008-04-01 16:47:06Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#ifndef CR_SPATIALCOORDINATE_H
#define CR_SPATIALCOORDINATE_H

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <coordinates/Coordinates/Coordinate.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/TabularCoordinate.h>

#include <Coordinates/CoordinateType.h>

using casa::Coordinate;
using casa::DirectionCoordinate;
using casa::LinearCoordinate;
using casa::Matrix;
using casa::String;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class SpatialCoordinate
    
    \ingroup CR_Coordinates
    
    \brief Container to combine other coordinates into a spatial (3D) coordinate
    
    \author Lars B&auml;hren

    \date 2008/08/28

    \test tSpatialCoordinate.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>casacore <a href="http://www.astron.nl/casacore/doc/html/classcasa_1_1Coordinate.html">Coordinate</a> class
    </ul>
    
    <h3>Synopsis</h3>
    
    In order to be consistent we extract the celestial coordinates required for
    beamforming directly from the same coordinate system object which is going to
    be attached to the generated image. Since the Beamformer is expected to
    support both imaging in the near field -- as required for e.g. the radio
    emission from cosmic rays induced air showers -- as well as in the far field
    -- thereby enabling the calibration on astronomical sources -- we run into 
    an implementation problem for the spatial coordinates of the image: none of
    forseeable cases can be covered using a single of the existing Coordinate
    classes of the <i>casacore</i> library.
    <ol>
      <li>For imaging in the far-field and/or using a spherical map projection
      (such as SIN, STG, CAR, ...), the direction information for the angular
      coordinates (e.g. AZEL, J2000) is contained within a casa::DirectionCoordinate
      object.
      <li>For the imaging on in the near field without the usage of a spherical
      map projection the complete coordinate information can be encoded within a
      single casa::LinearCoordinate object.
    </ol>
      
    <table border="0">
      <tr>
        <td class="indexkey">Coordinate types</td>
	<td class="indexkey">Axis names</td>
	<td class="indexkey">Axis units</td>
	<td class="indexkey">Projection</td>
	<td class="indexkey">Description</td>
      </tr>
      <tr>
        <td>[Direction,Linear]</td>
	<td>[Longitude, Latitude,Length]</td>
	<td>[rad, rad,m]</td>
	<td>yes</td>
	<td>Direction on the sky with radial distance</td>
      </tr>
      <tr>
        <td>[Linear]</td>
	<td>[Distance, Distance, Distance]</td>
	<td>[m, m, m]</td>
	<td>no</td>
	<td>Cartesian coordinates</td>
      </tr>
      <tr>
        <td>[Linear]</td>
	<td>[Longitude, Latitude, Length]</td>
	<td>[deg, deg, m]</td>
	<td>no</td>
	<td>Spherical coordinates</td>
      </tr>
    </table>

    Given the fact that we are potentially working with more but a single object
    of type casa::Coordinate we need to wrap the calls to function handling
    either allowing access to internal parameters or carrying out coordinate
    conversions.
    <ul>
      <li>SpatialCoordinate::worldAxisNames
      <li>SpatialCoordinate::worldAxisUnits
      <li>SpatialCoordinate::referencePixel
      <li>SpatialCoordinate::referenceValue
      <li>SpatialCoordinate::linearTransform
      <li>SpatialCoordinate::increment
      <li>SpatialCoordinate::toWorld
      <li>SpatialCoordinate::toPixel
    </ul>
    
    <h3>Example(s)</h3>
    
  */  
  class SpatialCoordinate {

    /*! Type of spatial coordinate */
    CoordinateType::Types type_p;
    /*! The number of coordinate axes */
    unsigned int nofAxes_p;
    /*! The number of coordinate objects */
    unsigned int nofCoordinates_p;
    /*! Coordinate objects forming the spatial coordinate */
    DirectionCoordinate directionCoord_p;
    LinearCoordinate linearCoord_p;;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    SpatialCoordinate () {
      init (CoordinateType::DirectionRadius);
    }
    
    /*!
      \brief Argumented constructor

      \param coordType -- Type of coordinate for which to create an object.
    */
    SpatialCoordinate (CoordinateType::Types const &coordType) {
      init (coordType);
    }
    
    /*!
      \brief Argumented constructor for coordinate of type DirectionRadius
      
      \param direction -- <tt>casa::DirectionCoordinate</tt> object for the
             angular components of the position
      \param linear    -- <tt>casa::LinearCoordinate</tt> object for the radial
             component of the position.
    */
    SpatialCoordinate (casa::DirectionCoordinate const &direction,
		       casa::LinearCoordinate const &linear);
    
    /*!
      \brief Argumented constructor

      If internal checking yields that the provided coordinate type
      (<tt>coordType</tt>) is not consistent with the usage of a
      LinearCoordinate object, only that second parameter is used in the
      object construction.
      
      \param linear    -- <tt>casa::LinearCoordinate</tt> object
      \param coordType -- Type of coordinate for which to create an object.
    */
    SpatialCoordinate (casa::LinearCoordinate const &linear,
		       CoordinateType::Types const &coordType);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another SpatialCoordinate object from which to create this new
      one.
    */
    SpatialCoordinate (SpatialCoordinate const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~SpatialCoordinate ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another SpatialCoordinate object from which to make a copy.
    */
    SpatialCoordinate& operator= (SpatialCoordinate const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the type of the spatial coordinate

      \return type -- The type of the spatial coordinate, one of the values
              defined through CoordinateTypes::Type
    */
    inline CoordinateType::Types type () const {
      return type_p;
    }
    
    /*!
      \brief Get the number of coordinate axes
      
      \return nofAxes -- The number of coordinate axes.
    */
    inline unsigned int nofAxes () const {
      return nofAxes_p;
    }
 
    /*!
      \brief Get the number of casa::Coordinate object to make of this coordinate

      \return nofCoordinates -- The number of casa::Coordinate object to make of
              this coordinate
     */
    inline unsigned int nofCoordinates () const {
      return nofCoordinates_p;
    }

    /*!
      \brief Get the DirectionCoordinate object of the spatial coordinate

      \return directionCoord -- The DirectionCoordinate object of the spatial
              coordinate. This is set up properly only for
	      CR::CoordinateType::Direction and CR::CoordinateType::DirectionRadius
    */
    inline DirectionCoordinate directionCoordinate () const {
      return directionCoord_p;
    }

    /*!
      \brief Get the LinearCoordinate object of the spatial coordinate

      \return linearCoord -- The LinearCoordinate object of the spatial
              coordinate. This is set up properly for all coordinate types except
	      CR::CoordinateType::Direction
    */
    inline LinearCoordinate linearCoordinate () const {
      return linearCoord_p;
    }

    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SpatialCoordinate.
    */
    std::string className () const {
      return "SpatialCoordinate";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get the names of the world axes

      \return names -- The names of the world axes, as retrieved through the
              <tt>casa::Coordinate::worldAxisNames()<tt> function.
    */
    Vector<String> worldAxisNames();
    /*!
      \brief Get the value of the reference pixel

      \return refPixel -- The value of the reference pixel, as retrieved through
              the <tt>casa::Coordinate::referencePixel()<tt> function.
    */
    Vector<double> referencePixel();
    /*!
      \brief Get the matrix for the linear transformation

      \return Xform -- The matrix of the linear transformation, as retrieved
              through the <tt>casa::Coordinate::linearTransform()<tt> function.
     */
    Matrix<double> linearTransform();
    /*!
      \brief Get the value of the coordinate increment

      \return refPixel -- The value of the coordinate increment, as retrieved
              through the <tt>casa::Coordinate::increment()<tt> function.
    */
    Vector<double> increment();
    /*!
      \brief Get the reference value

      \return refPixel -- The reference value, as retrieved through the
              <tt>casa::Coordinate::referenceValue()<tt> function.
    */
    Vector<double> referenceValue();
    /*!
      \brief Get the units of the world axes

      \return units -- The units of the world axes, as retrieved through the
              <tt>casa::Coordinate::worldAxisUnits()<tt> function.
    */
    Vector<String> worldAxisUnits();
    /*!
      \brief Conversion from pixel to world coordinates

      \retval world -- 
      \param pixel  -- 
    */
    void toWorld (Vector<double> &world,
		  const Vector<double> &pixel);
    /*!
      \brief Conversion from world to pixel coordinates

      \retval pixel -- 
      \param world  -- 
    */
    void toPixel (Vector<double> &pixel,
		  const Vector<double> &world);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (SpatialCoordinate const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Initialize the object for a given coordinate type

      \param coordType -- Type of coordinate for which to create an object.
    */
    void init (CoordinateType::Types const &coordType);
    
  };
  
} // Namespace CR -- end

#endif /* CR_SPATIALCOORDINATE_H */
  
