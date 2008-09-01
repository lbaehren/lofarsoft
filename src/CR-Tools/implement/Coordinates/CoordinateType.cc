/*-------------------------------------------------------------------------*
 | $Id:: NewClass.cc 1159 2007-12-21 15:40:14Z baehren                   $ |
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

#include <Coordinates/CoordinateType.h>

namespace CR { // Namespace CR -- begin

  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  CoordinateType::CoordinateType (CoordinateType const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  void CoordinateType::copy (CoordinateType const &other)
  {
    type_p = other.type_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void CoordinateType::summary (std::ostream &os)
  {
    os << "[CoordinateType] Summary of object properties." << std::endl;
    os << "-- Coordinate type = " << type_p          << std::endl;
    os << "-- has projection  = " << hasProjection() << std::endl;
    os << "-- is cartesian    = " << isCartesian()   << std::endl;
    os << "-- is spherical    = " << isSpherical()   << std::endl;
    os << "-- is cylindrical  = " << isCylindrical() << std::endl;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // -------------------------------------------------------------- hasProjection
  
  bool CoordinateType::hasProjection (CoordinateType::Types const &coord)
  {
    switch (coord) {
    case CoordinateType::Direction:
    case CoordinateType::DirectionRadius:
      return true;
      break;
    default:
      return false;
      break;
    }
  }
  
  // ---------------------------------------------------------------- isCartesian
  
  bool CoordinateType::isCartesian (CoordinateType::Types const &coord)
  {
    switch (coord) {
    case CoordinateType::Cartesian:
      return true;
      break;
    default:
      return false;
      break;
    }
  }
  
  // ---------------------------------------------------------------- isSpherical
  
  bool CoordinateType::isSpherical (CoordinateType::Types const &coord)
  {
    switch (coord) {
    case CoordinateType::Spherical:
      return true;
      break;
    default:
      return false;
      break;
    }
  }

  // -------------------------------------------------------------- isCylindrical
  
  bool CoordinateType::isCylindrical (CoordinateType::Types const &coord)
  {
    switch (coord) {
    case CoordinateType::Cylindrical:
      return true;
      break;
    default:
      return false;
      break;
    }
  }

  // -------------------------------------------------- createDirectionCoordinate

  DirectionCoordinate CoordinateType::createDirectionCoordinate ()
  {
    return createDirectionCoordinate (casa::MDirection::AZEL,
				      casa::Projection::STG);
  }
  
  // -------------------------------------------------- createDirectionCoordinate
  
  DirectionCoordinate
  CoordinateType::createDirectionCoordinate (casa::MDirection::Types const &directionType,
					     casa::Projection::Type const &projectionType)
  {
    casa::Quantum<casa::Double> refLon (   0, "deg");
    casa::Quantum<casa::Double> refLat (90.0, "deg");
    casa::Quantum<casa::Double> incLon (-1.0, "deg");
    casa::Quantum<casa::Double> incLat ( 1.0, "deg");
    casa::Matrix<casa::Double> xform(2,2);
    
    xform            = 0.0;
    xform.diagonal() = 1.0;

    return casa::DirectionCoordinate (directionType,
				      casa::Projection(projectionType),
				      refLon,
				      refLat,
				      incLon,
				      incLat,
				      xform,
				      0,
				      0);
  }

  // ----------------------------------------------------- createLinearCoordinate

  LinearCoordinate
  CoordinateType::createLinearCoordinate (unsigned int const &nofAxes)
  {
    Vector<casa::String> names (nofAxes,"");
    Vector<casa::String> units (nofAxes,"");

    return createLinearCoordinate (nofAxes,
				   names,
				   units);
  }
  
  // ----------------------------------------------------- createLinearCoordinate

  LinearCoordinate
  CoordinateType::createLinearCoordinate (unsigned int const &nofAxes,
					  Vector<casa::String> const &names,
					  Vector<casa::String> const &units)
  {
    Vector<casa::Double> crpix(nofAxes,0.0);
    Vector<casa::Double> crval(nofAxes,0.0);
    Vector<casa::Double> cdelt(nofAxes,1.0);
    Matrix<casa::Double> pc(nofAxes,nofAxes,0.0);
    
    pc.diagonal() = 1.0;
    
    if (nofAxes == names.nelements() && nofAxes == units.nelements()) {
      return LinearCoordinate (names,
			       units,
			       crval,
			       cdelt,
			       pc,
			       crpix);
    } else {
      std::cerr << "[CoordinateType::createLinearCoordinate] " 
		<< "Mismatch in rank of parameter vectors!" << std::endl;
      std::cerr << "-- nofAxes = " << nofAxes << std::endl;
      std::cerr << "-- names   = " << names   << std::endl;
      std::cerr << "-- units   = " << units   << std::endl;
      return LinearCoordinate (Vector<casa::String>(nofAxes,names(0)),
			       Vector<casa::String>(nofAxes,units(0)),
			       crval,
			       cdelt,
			       pc,
			       crpix);
    }
  }

  // -------------------------------------------------------------------- summary

  void CoordinateType::summary (std::ostream &os,
				casa::Coordinate const &coord)
  {
    os << "-- Coord. type           = " << coord.type()           << std::endl;
    os << "-- Coord. name           = " << coord.showType()       << std::endl;
    os << "-- nof. pixel axes       = " << coord.nPixelAxes()     << std::endl;
    os << "-- nof. world axes       = " << coord.nWorldAxes()     << std::endl;
    os << "-- World axis names      = " << coord.worldAxisNames() << std::endl;
    os << "-- World axis units      = " << coord.worldAxisUnits() << std::endl;
    os << "-- Reference value       = " << coord.referenceValue() << std::endl;
    os << "-- Reference pixel       = " << coord.referencePixel() << std::endl;
    os << "-- Increment             = " << coord.increment()      << std::endl;
  }

} // Namespace CR -- end
