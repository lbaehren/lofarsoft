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
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/TabularCoordinate.h>
#include <coordinates/Coordinates/ObsInfo.h>

#include <Coordinates/CoordinateBase.h>
#include <Coordinates/CoordinateType.h>

using casa::Coordinate;
using casa::DirectionCoordinate;
using casa::IPosition;
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
      <li>casacore <a href="http://www.astron.nl/casacore/doc/html/classcasa_1_1CoordinateSystem.html">CoordinateSystem</a> class
      <li>CR::CoordinateType
      <li>CR::SkymapCoordinate
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
        <td class="indexkey">Coordinate objects</td>
        <td class="indexkey">Type</td>
	<td class="indexkey">Axis names</td>
	<td class="indexkey">Axis units</td>
	<td class="indexkey">Projection</td>
	<td class="indexkey">Description</td>
      </tr>
      <tr>
        <td>[Direction,Linear]</td>
        <td>DirectionRadius</td>
	<td>[Longitude, Latitude, Length]</td>
	<td>[rad, rad, m]</td>
	<td>yes</td>
	<td>Direction on the sky with radial distance</td>
      </tr>
      <tr>
        <td>[Linear]</td>
        <td>Cartesian</td>
	<td>[Distance, Distance, Distance]</td>
	<td>[m, m, m]</td>
	<td>no</td>
	<td>Cartesian coordinates</td>
      </tr>
      <tr>
        <td>[Linear]</td>
        <td>Spherical</td>
	<td>[Length, Longitude, Latitude]</td>
	<td>[m, deg, deg]</td>
	<td>no</td>
	<td>Spherical coordinates</td>
      </tr>
      <tr>
        <td>[Linear]</td>
        <td>Cylindrical</td>
	<td>[Length, Longitude, Length]</td>
	<td>[m, deg, m]</td>
	<td>no</td>
	<td>Cylindrical coordinates</td>
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

    <ol>
      <li>Create a spatial coordinate consisting of a directional component in 
      a J2000 reference frame using a SIN projection and a radial component:
      \code
      SpatialCoordinate coord (CoordinateType::DirectionRadius,
                               "J2000",
			       "SIN")
      \endcode
    </ol>
    
  */  
  class SpatialCoordinate {

    /*! Type of spatial coordinate */
    CoordinateType::Types type_p;
    /*! The number of coordinate axes */
    unsigned int nofAxes_p;
    /*! The number of coordinate objects */
    unsigned int nofCoordinates_p;
    //! Number of elements along the coordinate axes
    IPosition shape_p;
    /*! Directional component of the spatial coordinate */
    DirectionCoordinate directionCoord_p;
    /*! Linear component of the spatial coordinate */
    LinearCoordinate linearCoord_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructr
      
      Creates a new SpatialCoordinate of type \e DirectionRadius for AZEL and
      STG projection
    */
    SpatialCoordinate ()
      {
	init (CoordinateType::DirectionRadius,
	      "AZEL",
	      "STG");
      }
    
    /*!
      \brief Argumented constructor
      
      \param coordType  -- Type of coordinate for which to create an object.
      \param refcode    -- Reference code for the celestial coordinate system, in
             case the coordinate contains a DirectionCoordinate
      \param projection -- Identifier for the spherical map projection, in case
             the coordinate contains a DirectionCoordinate
    */
    SpatialCoordinate (CoordinateType::Types const &coordType,
		       casa::String const &refcode="AZEL",
		       casa::String const &projection="STG")
      {
	init (coordType,
	      refcode,
	      projection);
      }
    
    /*!
      \brief Argumented constructor
      
      \param coordType  -- Type of coordinate for which to create an object.
      \param refPixel   -- Reference pixel
      \param refValue   -- Reference value
      \param increment  -- Coordinate increment
      \param shape      -- The number of elements along the coordinate axes
      \param refcode    -- Reference code for the celestial coordinate system, in
             case the coordinate contains a DirectionCoordinate
      \param projection -- Identifier for the spherical map projection, in case
             the coordinate contains a DirectionCoordinate
    */
    SpatialCoordinate (CoordinateType::Types const &coordType,
		       casa::Vector<double> const &refPixel,
		       casa::Vector<double> const &refValue,
		       casa::Vector<double> const &increment,
		       casa::IPosition const &shape,
		       casa::String const &refcode="AZEL",
		       casa::String const &projection="STG");
    
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
      \brief Get the number of elements along the coordinate axes
      
      \return shape -- The number of elements along the individual coordinate
              axes.
    */
    inline IPosition shape () const {
      return shape_p;
    }

    /*!
      \brief Get the number of elements along the coordinate axes
      
      \return shape -- The number of elements along the individual coordinate
              axes.
    */
    bool setShape (IPosition const &shape);

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
      \brief Set the direction part of the spatial coordinate

      \param direction -- The direction coordinate to be used as part of the 
             spatial coordinate.

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered, e.g. because trying to set a
	      DirectionCoordinate when the spatial coordinate is purely a set of
	      linear coordinates.
    */
    bool setDirectionCoordinate (DirectionCoordinate const &direction);

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
      \brief Get the reference code for the type of direction coordinate
    */
    String directionRefcode ();

    /*!
      \brief Get the spherical map projection of the direction coordinate

      \return projection -- Identifier for the spherical map projection; in case
              the spatial coordinate does not contain an object of type
	      DirectionCoordinate the value "NONE" is returned.
    */
    String projection ();

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

      Depending on the type of spatial coordinate certain properties will not be
      defined; a such e.g. a direction reference code or a spherical map
      projection will only be set in case the coordinate actually contains a
      directional component.
      \verbatim
      [SpatialCoordinate::summary]
      -- Type of spatial coordinate = 1
      -- nof. coordinate axes       = 3
      -- nof. coordinate objects    = 2
      -- World axis names           = [Longitude, Latitude, Length]
      -- World axis units           = [rad, rad, m]
      -- Reference pixel    (CRPIX) = [0, 0, 0]
      -- Increment          (CDELT) = [-0.0174533, 0.0174533, 1]
      -- Reference value    (CRVAL) = [0, 1.5708, 0]
      -- Spherical map projection   = STG
      -- Direction reference code   = AZEL
      \endverbatim

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods

    /*!
      \brief Retrieve the world axis values of the spatial coordinate

      Fasted varying axis is last:
      \verbatim
      [0 0 0]
      [0 0 1]
      [0 0 2]
      ...
      [0 1 0]
      [0 1 1]
      \endverbatim
      Fasted varying axis is first:
      \verbatim
      [0 0 0]
      [1 0 0]
      [2 0 0]
      ...
      [0 1 0]
      [0 2 0]
      \endverbatim

      \return values --
    */
    Matrix<double> positionValues (bool const &fastedAxisFirst=true);

    /*!
      \brief Retrieve the world axis values of the spatial coordinate

      This method is nothing but an alternate call to
      SpatialCoordinate::positionValues, in order to provide a uniform interface
      to the coordinate classes.

      \return values -- The world axis values of the spatial coordinate
    */
    inline Matrix<double> worldAxisValues (bool const &fastedAxisFirst=true) {
      return positionValues (fastedAxisFirst);
    }
    
    /*!
      \brief Get the value of the reference pixel

      \return refPixel -- The value of the reference pixel, as retrieved through
              the <tt>casa::Coordinate::referencePixel()<tt> function.
    */
    Vector<double> referencePixel();
    /*!
      \brief Set the value of the reference pixel
    */
    bool setReferencePixel(Vector<double> const &refPixel);
    /*!
      \brief Get the matrix for the linear transformation

      \return Xform -- The matrix of the linear transformation, as retrieved
              through the <tt>casa::Coordinate::linearTransform()<tt> function.
     */
    Matrix<double> linearTransform();
    /*!
      \brief Get the value of the coordinate increment

      \return increment -- The value of the coordinate increment, as retrieved
              through the <tt>casa::Coordinate::increment()<tt> function.
    */
    Vector<double> increment();
    /*!
      \brief Set the value of the coordinate increment
     */
    bool setIncrement(Vector<double> const &incr);
    /*!
      \brief Get the reference value

      \return refPixel -- The reference value, as retrieved through the
              <tt>casa::Coordinate::referenceValue()<tt> function.
    */
    Vector<double> referenceValue();
    /*!
      \brief Set the value of the reference value
    */
    bool setReferenceValue(Vector<double> const &refValue);
    /*!
      \brief Get the names of the world axes

      \return names -- The names of the world axes, as retrieved through the
              <tt>casa::Coordinate::worldAxisNames()<tt> function.
    */
    Vector<String> worldAxisNames();
    /*!
      \brief Set the names of the world axes

      \param names -- The names of the world axes, as to be set through the
             <tt>casa::Coordinate::setWorldAxisNames()<tt> function.
    */
    bool setWorldAxisNames(Vector<String> const &names);
    /*!
      \brief Get the units of the world axes

      \return units -- The units of the world axes, as retrieved through the
              <tt>casa::Coordinate::worldAxisUnits()<tt> function.
    */
    Vector<String> worldAxisUnits();
    /*!
      \brief Set the units of the world axes

      \return units -- The units of the world axes, as to be set through the
              <tt>casa::Coordinate::setWorldAxisUnits()<tt> function.
    */
    bool setWorldAxisUnits(Vector<String> const &units);
    /*!
      \brief Conversion from pixel to world coordinates

      \retval world -- Values in world coordinates
      \param pixel  -- Values in pixel coordinates
    */
    void toWorld (Vector<double> &world,
		  const Vector<double> &pixel);
    /*!
      \brief Conversion from world to pixel coordinates

      \retval pixel -- Values in pixel coordinates
      \param world  -- Values in world coordinates
    */
    void toPixel (Vector<double> &pixel,
		  const Vector<double> &world);

    /*!
      \brief Add the coordinates to a coordinate system object

      \retval csys  -- Coordinate system object collecting the individual
              coordinate
      \param append -- Append the coordinates to the existing coordinate system
             object. If <tt>append=false</tt> then any already existing
	     coordinate objects will be removed from the coordinate system first.
    */
    void toCoordinateSystem (casa::CoordinateSystem &csys,
			     bool const &append=true);
    
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

      \param coordType  -- Type of coordinate for which to create an object.
      \param refcode    -- Reference code for the celestial coordinate system, in
             case the coordinate contains a DirectionCoordinate
      \param projection -- Identifier for the spherical map projection, in case
             the coordinate contains a DirectionCoordinate
    */
    void init (CoordinateType::Types const &coordType,
	       casa::String const &refcode="AZEL",
	       casa::String const &projection="STG");

  };
  
} // Namespace CR -- end

#endif /* CR_SPATIALCOORDINATE_H */
  
