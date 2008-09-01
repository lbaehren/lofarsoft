/*-------------------------------------------------------------------------*
 | $Id:: NewClass.cc 1159 2007-12-21 15:40:14Z baehren                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars Baehren (lbaehren@gmail.com)                                     *
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

#include <Coordinates/SpatialCoordinate.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ---------------------------------------------------------- SpatialCoordinate

  SpatialCoordinate::SpatialCoordinate (casa::LinearCoordinate const &linear,
					CoordinateType::Types const &coordType)
  {
    switch (coordType) {
    case CoordinateType::Direction:
    case CoordinateType::DirectionRadius:
      {
	/* issue error message */
	std::cerr << "[SpatialCoordinate] Inconsistent combination of coordinate"
		  << " type and coordinate object!" << std::endl;
	/* use the coordinate type only for the creation of a new object */
	init (coordType);
      }
      break;
    default:
      {
	init (coordType);
	linearCoord_p = linear;
      }
      break;
    }
  }
  
  // ---------------------------------------------------------- SpatialCoordinate

  SpatialCoordinate::SpatialCoordinate (SpatialCoordinate const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  SpatialCoordinate::~SpatialCoordinate ()
  {
    destroy();
  }
  
  void SpatialCoordinate::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  SpatialCoordinate& SpatialCoordinate::operator= (SpatialCoordinate const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void SpatialCoordinate::copy (SpatialCoordinate const &other)
  {
    type_p           = other.type_p;
    nofAxes_p        = nofAxes_p;
    nofCoordinates_p = other.nofCoordinates_p;
    directionCoord_p = other.directionCoord_p;
    linearCoord_p    = other.linearCoord_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void SpatialCoordinate::summary (std::ostream &os)
  {
    os << "[SpatialCoordinate::summary]" << std::endl;
    os << "-- Type of spatial coordinate = " << type_p           << std::endl;
    os << "-- nof. coordinate axes       = " << nofAxes()        << std::endl;
    os << "-- nof. coordinate objects    = " << nofCoordinates() << std::endl;
    os << "-- World axis names           = " << worldAxisNames() << std::endl;
    os << "-- World axis units           = " << worldAxisUnits() << std::endl;
    os << "-- Reference pixel    (CRPIX) = " << referencePixel() << std::endl;
    os << "-- Increment          (CDELT) = " << increment()      << std::endl;
    os << "-- Reference value    (CRVAL) = " << referenceValue() << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  void SpatialCoordinate::init (CoordinateType::Types const &coordType)
  {
    type_p = coordType;

    switch (coordType) {
    case CoordinateType::DirectionRadius:
      {
	// book-keeping
	nofAxes_p        = 3;
	nofCoordinates_p = 2;
	// set up the coordinate objects
	directionCoord_p = CoordinateType::createDirectionCoordinate();
	casa::Vector<casa::String> names (1,"Length");
	casa::Vector<casa::String> units (1,"m");
	linearCoord_p    = CoordinateType::createLinearCoordinate(1,
								  names,
								  units);
      }
      break;
    case CoordinateType::Cartesian:
      {
	// book-keeping
	nofAxes_p        = 3;
	nofCoordinates_p = 1;
	// set up the coordinate object
	casa::Vector<casa::String> names (nofAxes_p,"Length");
	casa::Vector<casa::String> units (nofAxes_p,"m");
	linearCoord_p    = CoordinateType::createLinearCoordinate(nofAxes_p,
								  names,
								  units);
      }
      break;
    case CoordinateType::Spherical:
      {
	// book-keeping
	nofAxes_p        = 3;
	nofCoordinates_p = 1;
	// set up the coordinate object
	casa::Vector<casa::String> names (nofAxes_p);
	casa::Vector<casa::String> units (nofAxes_p);
	names(0) = "Length";
	names(1) = "Direction";
	names(2) = "Direction";
	units(0) = "m";
	units(1) = "rad";
	units(2) = "rad";
	linearCoord_p    = CoordinateType::createLinearCoordinate(nofAxes_p,
								  names,
								  units);
      }
      break;
    case CoordinateType::Cylindrical:
      {
	nofAxes_p        = 3;
	nofCoordinates_p = 1;
	// set up the coordinate object
	casa::Vector<casa::String> names (nofAxes_p);
	casa::Vector<casa::String> units (nofAxes_p);
	names(0) = "Length";
	names(1) = "Direction";
	names(2) = "Length";
	units(0) = "m";
	units(1) = "rad";
	units(2) = "m";
	linearCoord_p    = CoordinateType::createLinearCoordinate(nofAxes_p,
								  names,
								  units);
      }
      break;
    default:
      init (CoordinateType::DirectionRadius);
      break;
    }
  }
  
  // ------------------------------------------------------------- worldAxisNames

  Vector<String> SpatialCoordinate::worldAxisNames()
  {
    Vector<String> names (nofAxes_p);

    switch (type_p) {
    case CoordinateType::Direction:
      names = directionCoord_p.worldAxisNames();
      break;
    case CoordinateType::DirectionRadius:
      {
	Vector<String> namesDirection = directionCoord_p.worldAxisNames();
	Vector<String> linearNames    = linearCoord_p.worldAxisNames();
	//
	names(0) = namesDirection(0);
	names(1) = namesDirection(1);
	names(2) = linearNames(0);
      }
      break;
    default:
      names = linearCoord_p.worldAxisNames();
      break;
    }

    return names;
  }

  // ------------------------------------------------------------- worldAxisUnits

  Vector<String> SpatialCoordinate::worldAxisUnits()
  {
    Vector<String> units (nofAxes_p);

    switch (type_p) {
    case CoordinateType::Direction:
      units = directionCoord_p.worldAxisUnits();
      break;
    case CoordinateType::DirectionRadius:
      {
	Vector<String> directionUnits = directionCoord_p.worldAxisUnits();
	Vector<String> linearUnits    = linearCoord_p.worldAxisUnits();
	//
	units(0) = directionUnits(0);
	units(1) = directionUnits(1);
	units(2) = linearUnits(0);
      }
      break;
    default:
      units = linearCoord_p.worldAxisUnits();
      break;
    }

    return units;
  }

  // ------------------------------------------------------------- referencePixel

  Vector<double> SpatialCoordinate::referencePixel()
  {
    Vector<double> refPixel (nofAxes_p);

    switch (type_p) {
    case CoordinateType::Direction:
      refPixel = directionCoord_p.referencePixel();
      break;
    case CoordinateType::DirectionRadius:
      {
	Vector<double> refPixelDirection = directionCoord_p.referencePixel();
	Vector<double> refPixelLinear    = linearCoord_p.referencePixel();
	//
	refPixel(0) = refPixelDirection(0);
	refPixel(1) = refPixelDirection(1);
	refPixel(2) = refPixelLinear(0);
      }
      break;
    default:
      refPixel = linearCoord_p.referencePixel();
      break;
    }

    return refPixel;
  }

  // ------------------------------------------------------------------ increment

  Vector<double> SpatialCoordinate::increment()
  {
    Vector<double> increment (nofAxes_p);

    switch (type_p) {
    case CoordinateType::Direction:
      increment = directionCoord_p.increment();
      break;
    case CoordinateType::DirectionRadius:
      {
	Vector<double> incrementDirection = directionCoord_p.increment();
	Vector<double> incrementLinear    = linearCoord_p.increment();
	//
	increment(0) = incrementDirection(0);
	increment(1) = incrementDirection(1);
	increment(2) = incrementLinear(0);
      }
      break;
    default:
      increment = linearCoord_p.increment();
      break;
    }

    return increment;
  }

  // ------------------------------------------------------------- referenceValue

  Vector<double> SpatialCoordinate::referenceValue()
  {
    Vector<double> refValue (nofAxes_p);

    switch (type_p) {
    case CoordinateType::Direction:
      refValue = directionCoord_p.referenceValue();
      break;
    case CoordinateType::DirectionRadius:
      {
	Vector<double> refValueDirection = directionCoord_p.referenceValue();
	Vector<double> refValueLinear    = linearCoord_p.referenceValue();
	//
	refValue(0) = refValueDirection(0);
	refValue(1) = refValueDirection(1);
	refValue(2) = refValueLinear(0);
      }
      break;
    default:
      refValue = linearCoord_p.referenceValue();
      break;
    }

    return refValue;
  }

  // -------------------------------------------------------------------- toWorld

  void SpatialCoordinate::toWorld (Vector<double> &world,
				   const Vector<double> &pixel)
  {
    switch (type_p) {
      case CoordinateType::Direction:
	directionCoord_p.toWorld (world,pixel);
	break;
    case CoordinateType::DirectionRadius:
      {
	Vector<casa::Double> worldDirection (2);
	Vector<casa::Double> worldLinear (2);
	Vector<casa::Double> pixelDirection (2);
	Vector<casa::Double> pixelLinear (1);
	pixelDirection(0) = pixel(0);
	pixelDirection(1) = pixel(1);
	pixelLinear(0)    = pixel(2);
	// conversion
	directionCoord_p.toWorld (worldDirection,pixelDirection);
	linearCoord_p.toWorld (worldLinear,pixelLinear);
	// copy to output
	world(0) = worldDirection(0);
	world(1) = worldDirection(0);
	world(2) = worldLinear(0);
      }
      break;
    default:
      linearCoord_p.toWorld (world,pixel);
      break;
    }
  }
  
} // Namespace CR -- end
