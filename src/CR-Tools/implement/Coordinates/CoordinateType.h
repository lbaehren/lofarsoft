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

#ifndef CR_COORDINATETYPE_H
#define CR_COORDINATETYPE_H

// Standard library header files
#include <iostream>
#include <string>

/* casacore header files */
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Quanta/Quantum.h>
#include <coordinates/Coordinates/Coordinate.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/TabularCoordinate.h>

// CR-Tools
#include <crtools.h>
#include <Utilities/MConversions.h>

using casa::Coordinate;
using casa::DirectionCoordinate;
using casa::LinearCoordinate;
using casa::SpectralCoordinate;
using casa::Matrix;
using casa::MDirection;
using casa::Projection;
using casa::Quantum;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CoordinateType
    
    \ingroup CR_Coordinates
    
    \brief Definition of the various coordinate types we are dealing with
    
    \author Lars B&auml;hren

    \date 2008/08/29

    \test tCoordinateType.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>casacore coordinates module
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Examples</h3>

    Usage of the static functions to support the creation of various
    casa::Coordinate objects:
    <ol>
      <li>Create a linear coordinate for a single axis:
      \code
      casa::LinearCoordinate = CoordinateType::makeLinearCoordinate (1);
      \endcode
      <li>Create a linear coordinate for a 3-dim  cartesian coordinate:
      \code
      unsigned int nofAxes (3);
      casa::Vector<casa::String> names (nofAxes,"Length")
      casa::Vector<casa::String> units (nofAxes,"m")

      casa::LinearCoordinate = CoordinateType::makeLinearCoordinate (nofAxes,
                                                                     names,
								     units);
      \endcode
      <li>Create a direction coordinate for AZEL celestial coordinates in STG
      projection, using the default creation settings
      \code
      casa::DirectionCoordinate = CoordinateType::makeDirectionCoordinate();
      \endcode
      or by explicit definition:
      \code
      casa::DirectionCoordinate = CoordinateType::makeDirectionCoordinate("AZEL",
                                                                          "STG");
      \endcode
    </ol>

  */  
  class CoordinateType {

  public:

    /*!
      \brief Types of (spatial) coordinates
    */
    enum Types {
      //! Azimuth-Elevation-Height, \f$ \vec x = (Az,El,H) \f$
      AzElHeight,
      //! Azimuth-Elevation-Radius, \f$ \vec x = (Az,El,R) \f$
      AzElRadius,
      //! Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
      Cartesian,
      //! Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
      Cylindrical,
      //! Direction on the sky, \f$ \vec x = (Lon,Lat) \f$
      Direction,
      //! Direction on the sky with radial distance, \f$ \vec x = (Lon,Lat,R) \f$
      DirectionRadius,
      //! Frquency
      Frequency,
      //! Longitude-Latitude-Radius
      LongLatRadius,
      //! North-East-Height
      NorthEastHeight,
      //! Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
      Spherical,
      //! Time
      Time
    };
    
    // ------------------------------------------------------------- Construction

    /*!
      \brief Default constructor
    */
    CoordinateType () {
      type_p = CoordinateType::DirectionRadius;
    }

    /*!
      \brief Argumented constructor

      \param type -- Coordinate type for which to create the object.
    */
    CoordinateType (CoordinateType::Types const &type) {
      type_p = type;
    }
    
    /*!
      \brief Copy constructor
      
      \param other -- Another CoordinateType object from which to create this new
             one.
    */
    CoordinateType (CoordinateType const &other);
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CoordinateType.
    */
    std::string className () const {
      return "CoordinateType";
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
      \brief Get the coordinate type

      \return type -- The coordinate type
    */
    inline CoordinateType::Types type () const {
      return type_p;
    }

    /*!
      \brief Get the name of the coordinate type

      \return name -- The name of the coordinate type
    */
    std::string name ();

    /*!
      \brief Get the name of the coordinate type

      \return name -- The name of the coordinate type
    */
    static std::string getName (CoordinateType::Types const &type);

    /*!
      \brief Set the coordinate type

      \param type -- The coordinate type
    */
    inline void setType (CoordinateType::Types const &type) {
      type_p = type;
    }

    /*!
      \brief Does this coordinate type contain a spherical map projection?
      
      \return hasProjection -- <tt>true</tt> if the coordinate type contains a
              spherical map projection; this is the case if the coordinate is
	      composed of at least one object of type casa::DirectionCoordinate.
    */
    inline bool hasProjection () const {
      return hasProjection (type_p);
    }
    
    /*!
      \brief Does this coordinate type contain a spherical map projection?
      
      \param coord -- Coordinate type to check
      
      \return hasProjection -- <tt>true</tt> if the coordinate type contains a
              spherical map projection; this is the case if the coordinate is
	      composed of at least one object of type casa::DirectionCoordinate.
    */
    static bool hasProjection (CoordinateType::Types const &coord);
    
    /*!
      \brief Is the coordinate of type Cartesian?
    
      \return isCartesian -- Returns <tt>true</tt> is the coordinate is of type
              Cartesian.
    */
    inline bool isCartesian () const {
      return isCartesian (type_p);
    }
    
    /*!
      \brief Is the coordinate of type Cartesian?
    
      \param coord -- Coordinate type to check

      \return isCartesian -- Returns <tt>true</tt> is the coordinate is of type
              Cartesian.
    */
    static bool isCartesian (CoordinateType::Types const &coord);
    
    /*!
      \brief Is the coordinate of type Spherical?
    
      \return isSpherical -- Returns <tt>true</tt> is the coordinate is of type
              Spherical.
    */
    inline bool isSpherical () const {
      return isSpherical (type_p);
    }
    
    /*!
      \brief Is the coordinate of type Spherical?
    
      \param coord -- Coordinate type to check
    
      \return isSpherical -- Returns <tt>true</tt> is the coordinate is of type
              Spherical.
    */
    static bool isSpherical (CoordinateType::Types const &coord);

    /*!
      \brief Is the coordinate of type Cylindrical?
    
      \return isCylindrical -- Returns <tt>true</tt> is the coordinate is of type
              Cylindrical.
    */
    inline bool isCylindrical () const {
      return isCylindrical (type_p);
    }
    
    /*!
      \brief Is the coordinate of type Cylindrical?
    
      \param coord -- Coordinate type to check
    
      \return isCylindrical -- Returns <tt>true</tt> is the coordinate is of type
              Cylindrical.
    */
    static bool isCylindrical (CoordinateType::Types const &coord);

    /*!
      \brief Is the coordinate of type Space?
    
      \return isSpatial -- Returns <tt>true</tt> is the coordinate is of type
              Space.
    */
    inline bool isSpatial () const {
      return isSpatial (type_p);
    }

    /*!
      \brief Is the coordinate of type Space?
    
      \param coord -- Coordinate type to check
    
      \return isSpatial -- Returns <tt>true</tt> is the coordinate is of type
              Space.
    */
    static bool isSpatial (CoordinateType::Types const &coord);

    /*!
      \brief Is the coordinate of type Time?
    
      \return isTemporal -- Returns <tt>true</tt> is the coordinate is of type
              Time.
    */
    inline bool isTemporal () const {
      return isTemporal (type_p);
    }

    /*!
      \brief Is the coordinate of type Time?
    
      \param coord -- Coordinate type to check
    
      \return isTemporal -- Returns <tt>true</tt> is the coordinate is of type
              Time.
    */
    static bool isTemporal (CoordinateType::Types const &coord);

    /*!
      \brief Is the coordinate of type Frequency?
    
      \return isSpectral -- Returns <tt>true</tt> is the coordinate is of type
              Time.
    */
    inline bool isSpectral () const {
      return isSpectral (type_p);
    }

    /*!
      \brief Is the coordinate of type Frequency?
    
      \param coord -- Coordinate type to check
    
      \return isSpectral -- Returns <tt>true</tt> is the coordinate is of type
              Time.
    */
    static bool isSpectral (CoordinateType::Types const &coord);

    /*!
      \brief Create a DirectionCoordinate object
      
      \return coord -- Direction coordinate of AZEL coordinates in STG projection
    */
    static DirectionCoordinate makeDirectionCoordinate ();

    /*!
      \brief Create a DirectionCoordinate object

      \param directionType  -- Reference code for the celestial coordinate frame
      \param projectionType -- Reference code for the sphercial map projection
      
      \return coord -- DirectionCoordinate object for the selected celestial
              coordinate frame and map projection.
    */
    static
      DirectionCoordinate makeDirectionCoordinate (casa::String const &refcode,
						   casa::String const &projection);
    
    /*!
      \brief Create a DirectionCoordinate object

      \param directionType  -- Reference code for the celestial coordinate frame
      \param projectionType -- Reference code for the sphercial map projection
      
      \return coord -- DirectionCoordinate object for the selected celestial
              coordinate frame and map projection.
    */
    static
      DirectionCoordinate makeDirectionCoordinate (MDirection::Types const &directionType,
						   Projection::Type const &projectionType);
    
    /*!
      \brief Create a DirectionCoordinate object
      
      \param directionType  -- Reference code for the celestial coordinate frame
      \param projectionType -- Reference code for the sphercial map projection
      \param refValue       -- Reference value, CRVAL
      \param increment      -- Coordinate increment, CDELT
      \param refPixel       -- Reference pixel, CRPIX
      
      \return coord -- DirectionCoordinate object for the selected celestial
              coordinate frame and map projection, using the provided WCS
	      parameters for the pixel to world conversion.
    */
    static
      DirectionCoordinate makeDirectionCoordinate (MDirection::Types const &directionType,
						   Projection::Type const &projectionType,
						   Vector<Quantum<double> > const &refValue,
						   Vector<Quantum<double> > const &increment,
						   Vector<double> const &refPixel);
    
    /*!
      \brief Create a LinearCoordinate object
      
      \param nofAxes -- The number of axes for which to create the coordinate
      \param names     -- World axis names of the coordinate.
      \param units     -- World axis units of the coordinate.
      \param refValue  -- Reference value, CRVAL.
      \param increment -- Coordinate increment, CDELT.
      \param refPixel  -- Reference pixel, CRPIX.
      
      \return coord -- 
    */
    static LinearCoordinate makeLinearCoordinate (unsigned int const &nofAxes=1,
						  casa::String const &name="Length",
						  casa::String const &unit="m",
						  double const &refValue=0.0,
						  double const &increment=0.0,
						  double const &refPixel=1.0);
    
    /*!
      \brief Create a LinearCoordinate object

      \param nofAxes -- The number of axes for which to create the coordinate
      \param names   -- World axis names of the coordinate.
      \param units   -- World axis units of the coordinate.
      
      \return coord -- 
    */
    static LinearCoordinate makeLinearCoordinate (unsigned int const &nofAxes,
						  Vector<casa::String> const &names,
						  Vector<casa::String> const &units);
    
    /*!
      \brief Create a LinearCoordinate object
      
      \param names     -- World axis names of the coordinate.
      \param units     -- World axis units of the coordinate.
      \param refValue  -- Reference value, CRVAL.
      \param increment -- Coordinate increment, CDELT.
      \param refPixel  -- Reference pixel, CRPIX.
      
      \return coord -- 
    */
    static casa::LinearCoordinate makeLinearCoordinate (Vector<casa::String> const &names,
							Vector<casa::String> const &units,
							Vector<double> const &refValue,
							Vector<double> const &increment,
							Vector<double> const &refPixel);
    
    /*!
      \brief Create a SpectralCoordinate object to hold a frequency axis
      
      \param refValue   -- Reference value, CRVAL, [s]
      \param increment  -- Coordinate increment, CDELT, [s]
      \param refPixel   -- Reference pixel, CRPIX

      \return coord -- 
    */
    static SpectralCoordinate makeSpectralCoordinate (double const &refValue=0,
						      double const &increment=0,
						      double const &refPixel=0);

    /*!
      \brief Create a StokesCoordinate object
    */
    static casa::StokesCoordinate makeStokesCoordinate ();
    
    /*!
      \brief Create a LinearCoordinate object to hold a time coordinate

      \param refValue   -- Reference value, CRVAL, [s]
      \param increment  -- Coordinate increment, CDELT, [s]
      \param refPixel   -- Reference pixel, CRPIX

      \return coord --
    */
    static LinearCoordinate makeTimeCoordinate (double const &refValue=0,
						double const &increment=0,
						double const &refPixel=0);
    
    /*!
      \brief Create a LinearCoordinate object to hold a Faraday coordinate

      \param refValue   -- Reference value, CRVAL, [rad/m^2]
      \param increment  -- Coordinate increment, CDELT, [rad/m^2]
      \param refPixel   -- Reference pixel, CRPIX

      \return coord --LinearCoordinate object with
      <tt>worldAxisName="Faraday rotation"</tt> and <tt>worldAxisUnits="rad/(m)2"</tt>
    */
    static LinearCoordinate makeFaradayCoordinate (double const &refValue=.0,
						   double const &increment=.0,
						   double const &refPixel=.0);

    /*!
      \brief Provide a summary of the internal status
      
      \param os    -- Output stream to which the summary is written.
      \param coord -- casa::Coordinate object for which to provide the summary
    */
    static void summary (std::ostream &os,
			 casa::Coordinate const &coord);
    
  private:
    
    /*! Coordinate type */
    CoordinateType::Types type_p;
    
    /*! Unconditional copying */
    void copy (CoordinateType const &other);
    
  };
  
} // Namespace CR -- end

#endif /* CR_COORDINATETYPE_H */

