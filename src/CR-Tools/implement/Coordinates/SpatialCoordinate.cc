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
  
  SpatialCoordinate::SpatialCoordinate (CoordinateType::Types const &coordType,
					casa::String const &refcode,
					casa::String const &projection)
  {
    init (coordType,
	  refcode,
	  projection);
  }
  
  // ---------------------------------------------------------- SpatialCoordinate
  
  SpatialCoordinate::SpatialCoordinate (casa::DirectionCoordinate const &direction,
					casa::LinearCoordinate const &linear)
  {
    type_p           = CoordinateType::DirectionRadius;
    nofAxes_p        = 3;
    nofCoordinates_p = 2;
    
    directionCoord_p = direction;
    linearCoord_p    = linear;
  }
  
  // ---------------------------------------------------------- SpatialCoordinate

  SpatialCoordinate::SpatialCoordinate (casa::LinearCoordinate const &linear,
					CoordinateType::Types const &coordType)
  {
    if (linear.nPixelAxes() == 3) {
      switch (coordType) {
      case CoordinateType::Cartesian:
      case CoordinateType::Spherical:
      case CoordinateType::Cylindrical:
	type_p           = coordType;
	nofAxes_p        = 3;
	nofCoordinates_p = 1;
	linearCoord_p    = linear;
	break;
      default:
	std::cerr << "[SpatialCoordinat::SpatialCoordinate]"
		  << " Provided coordinate type does not match coordinate object!"
		  << std::endl;
	init (coordType);
	break;
      };
    } else {
      init (coordType);
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
    os << "-- Type of spatial coordinate = " << type_p             << std::endl;
    os << "-- nof. coordinate axes       = " << nofAxes()          << std::endl;
    os << "-- nof. coordinate objects    = " << nofCoordinates()   << std::endl;
    os << "-- World axis names           = " << worldAxisNames()   << std::endl;
    os << "-- World axis units           = " << worldAxisUnits()   << std::endl;
    os << "-- Reference pixel    (CRPIX) = " << referencePixel()   << std::endl;
    os << "-- Increment          (CDELT) = " << increment()        << std::endl;
    os << "-- Reference value    (CRVAL) = " << referenceValue()   << std::endl;
    os << "-- Spherical map projection   = " << projection()       << std::endl;
    os << "-- Direction reference code   = " << directionRefcode() << std::endl;
  }

  // ----------------------------------------------------- setDirectionCoordinate
  
  bool SpatialCoordinate::setDirectionCoordinate (DirectionCoordinate const &direction)
  {
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      directionCoord_p = direction;
      return true;
      break;
    default:
      std::cerr << "[SpatialCoordinate::setDirectionCoordinate] "
		<< "No direction component for this spatial coordinate!"
		<< std::endl;
      return false;
      break;
    };
  }

  // ----------------------------------------------------------- directionRefcode

  String SpatialCoordinate::directionRefcode ()
  {
    switch (type_p) {
    case CoordinateType::Direction:
    case CoordinateType::DirectionRadius:
      {
	casa::MDirection md;
	return md.showType(directionCoord_p.directionType());
      }
      break;
    default:
      return "NONE";
      break;
    };
  }
  
  // ----------------------------------------------------------------- projection

  String SpatialCoordinate::projection ()
  {
    switch (type_p) {
    case CoordinateType::Direction:
    case CoordinateType::DirectionRadius:
      return directionCoord_p.projection().name();
      break;
    default:
      return "NONE";
      break;
    };
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  void SpatialCoordinate::init (CoordinateType::Types const &coordType,
				casa::String const &refcode,
				casa::String const &projection)
  {
    type_p = coordType;

    switch (coordType) {
    case CoordinateType::DirectionRadius:
      {
	// book-keeping
	nofAxes_p        = 3;
	nofCoordinates_p = 2;
	// set up the coordinate objects
	directionCoord_p = CoordinateType::makeDirectionCoordinate(refcode,
								   projection);
	casa::Vector<casa::String> names (1,"Length");
	casa::Vector<casa::String> units (1,"m");
	linearCoord_p    = CoordinateType::makeLinearCoordinate(1,
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
	linearCoord_p    = CoordinateType::makeLinearCoordinate(nofAxes_p,
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
	linearCoord_p    = CoordinateType::makeLinearCoordinate(nofAxes_p,
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
	linearCoord_p    = CoordinateType::makeLinearCoordinate(nofAxes_p,
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

  // ---------------------------------------------------------- setWorldAxisNames
  
  bool SpatialCoordinate::setWorldAxisNames (Vector<String> const &names)
  {
    bool status (true);

    // Check the input
    if (names.nelements() != nofAxes()) {
      std::cerr << "[SpatialCoordinate::setWorldAxisNames]"
		<< " Number of worldAxisNames values does not match the number"
		<< " of coordinate axes!"
		<< std::endl;
      return false;
    }
    
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      {
	Vector<String> namesDir (2);
	Vector<String> namesLin (1);
	//
	namesDir(0) = names(0);
	namesDir(1) = names(1);
	namesLin(0) = names(2);
	//
	directionCoord_p.setWorldAxisNames (namesDir);
	linearCoord_p.setWorldAxisNames (namesLin);
      }
      break;
    default:
      linearCoord_p.setWorldAxisNames (names);
      break;
    };

    return status;
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

  // ---------------------------------------------------------- setWorldAxisUnits
  
  bool SpatialCoordinate::setWorldAxisUnits (Vector<String> const &units)
  {
    bool status (true);

    // Check the input
    if (units.nelements() != nofAxes()) {
      std::cerr << "[SpatialCoordinate::setWorldAxisUnits]"
		<< " Number of worldAxisUnits values does not match the number"
		<< " of coordinate axes!"
		<< std::endl;
      return false;
    }
    
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      {
	Vector<String> unitsDir (2);
	Vector<String> unitsLin (1);
	//
	unitsDir(0) = units(0);
	unitsDir(1) = units(1);
	unitsLin(0) = units(2);
	//
	directionCoord_p.setWorldAxisUnits (unitsDir);
	linearCoord_p.setWorldAxisUnits (unitsLin);
      }
      break;
    default:
      linearCoord_p.setWorldAxisUnits (units);
      break;
    };

    return status;
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

  // ---------------------------------------------------------- setReferencePixel

  bool SpatialCoordinate::setReferencePixel (Vector<double> const &refPixel)
  {
    bool status (true);

    // Check the input
    if (refPixel.nelements() != nofAxes()) {
      std::cerr << "[SpatialCoordinate::setReferencePixel]"
		<< " Number of reference pixel values does not match the number"
		<< " of coordinate axes!"
		<< std::endl;
      return false;
    }
    
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      {
	Vector<double> crpixDir (2);
	Vector<double> crpixLin (1);
	//
	crpixDir(0) = refPixel(0);
	crpixDir(1) = refPixel(1);
	crpixLin(0) = refPixel(2);
	//
	directionCoord_p.setReferencePixel (crpixDir);
	linearCoord_p.setReferencePixel (crpixLin);
      }
      break;
    default:
      linearCoord_p.setReferencePixel (refPixel);
      break;
    };

    return status;
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

  // --------------------------------------------------------------- setIncrement

  bool SpatialCoordinate::setIncrement (Vector<double> const &incr)
  {
    bool status (true);

    // Check the input
    if (incr.nelements() != nofAxes()) {
      std::cerr << "[SpatialCoordinate::setIncrement]"
		<< " Number of increment values does not match the number"
		<< " of coordinate axes!"
		<< std::endl;
      return false;
    }
    
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      {
	Vector<double> cdeltDir (2);
	Vector<double> cdeltLin (1);
	//
	cdeltDir(0) = incr(0);
	cdeltDir(1) = incr(1);
	cdeltLin(0) = incr(2);
	//
	directionCoord_p.setIncrement (cdeltDir);
	linearCoord_p.setIncrement (cdeltLin);
      }
      break;
    default:
      linearCoord_p.setIncrement (incr);
      break;
    };

    return status;
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

  // ---------------------------------------------------------- setReferenceValue

  bool SpatialCoordinate::setReferenceValue (Vector<double> const &refValue)
  {
    bool status (true);

    // Check the input
    if (refValue.nelements() != nofAxes()) {
      std::cerr << "[SpatialCoordinate::setReferenceValue]"
		<< " Number of reference value values does not match the number"
		<< " of coordinate axes!"
		<< std::endl;
      return false;
    }
    
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      {
	Vector<double> crvalDir (2);
	Vector<double> crvalLin (1);
	//
	crvalDir(0) = refValue(0);
	crvalDir(1) = refValue(1);
	crvalLin(0) = refValue(2);
	//
	directionCoord_p.setReferenceValue (crvalDir);
	linearCoord_p.setReferenceValue (crvalLin);
      }
      break;
    default:
      linearCoord_p.setReferenceValue (refValue);
      break;
    };

    return status;
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
	Vector<casa::Double> dirPixel (2);
	Vector<casa::Double> dirWorld (2);
	Vector<casa::Double> linPixel (1);
	Vector<casa::Double> linWorld (2);
	dirPixel(0) = pixel(0);
	dirPixel(1) = pixel(1);
	linPixel(0) = pixel(2);
	// conversion
	directionCoord_p.toWorld (dirWorld,dirPixel);
	linearCoord_p.toWorld (linWorld,linPixel);
	// copy to output
	world(0) = dirWorld(0);
	world(1) = dirWorld(1);
	world(2) = linWorld(0);
      }
      break;
    default:
      linearCoord_p.toWorld (world,pixel);
      break;
    }
  }
  
  // -------------------------------------------------------------------- toPixel

  void SpatialCoordinate::toPixel (Vector<double> &pixel,
				   const Vector<double> &world)
  {
    switch (type_p) {
      case CoordinateType::Direction:
	directionCoord_p.toPixel (pixel,world);
	break;
    case CoordinateType::DirectionRadius:
      {
	Vector<casa::Double> dirPixel (2);
	Vector<casa::Double> linPixel (2);
	Vector<casa::Double> dirWorld (2);
	Vector<casa::Double> linWorld (1);
	dirWorld(0) = world(0);
	dirWorld(1) = world(1);
	linWorld(0)    = world(2);
	// conversion
	directionCoord_p.toPixel (dirPixel,dirWorld);
	linearCoord_p.toPixel (linPixel,linWorld);
	// copy to output
	pixel(0) = dirPixel(0);
	pixel(1) = dirPixel(1);
	pixel(2) = linPixel(0);
      }
      break;
    default:
      linearCoord_p.toPixel (pixel,world);
      break;
    }
  }

  // --------------------------------------------------------- toCoordinateSystem

  void SpatialCoordinate::toCoordinateSystem (casa::CoordinateSystem &csys,
					      bool const &append)
  {
    if (!append) {
      std::cerr << "[SpatialCoordinate::toCoordinateSystem]"
		<< " Option append=false not yet implemented!"
		<< std::endl;
    }
    
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      csys.addCoordinate (directionCoord_p);
      csys.addCoordinate (linearCoord_p);
      break;
    default:
      csys.addCoordinate (linearCoord_p);
      break;
    };
  }
  
} // Namespace CR -- end
