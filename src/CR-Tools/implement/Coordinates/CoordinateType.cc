/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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
#include <dal/CoordinateGenerator.h>

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
    os << "-- is spatial      = " << isSpatial()     << std::endl;
    os << "-- is temporal     = " << isTemporal()    << std::endl;
    os << "-- is spectral     = " << isSpectral()    << std::endl;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //________________________________________________________________________ name

  /*!
    \return name -- The name of the coordinate type
  */
  std::string CoordinateType::name ()
  {
    return getName (type_p);
  }
  
  //_____________________________________________________________________ getName

  /*!
    \param type  -- 
    \return name -- The name of the coordinate type
  */
  std::string CoordinateType::getName (CoordinateType::Types const &type)
  {
    std::string name;

    switch (type) {
    case CoordinateType::Direction:
      name="Direction";
      break;
    case CoordinateType::DirectionRadius:
      name="DirectionRadius";
      break;
    case CoordinateType::Cartesian:
      name="Cartesian";
      break;
    case CoordinateType::Spherical:
      name="Spherical";
      break;
    case CoordinateType::Cylindrical:
      name="Cylindrical";
      break;
    case CoordinateType::AzElHeight:
      name="AzElHeight";
      break;
    case CoordinateType::AzElRadius:
      name="AzElRadius";
      break;
    case CoordinateType::NorthEastHeight:
      name="NorthEastHeight";
      break;
    case CoordinateType::LongLatRadius:
      name="LongLatRadius";
      break;
    case CoordinateType::Time:
      name="Time";
      break;
    case CoordinateType::Frequency:
      name="Frequency";
      break;
    };

    return name;
  }
  
  //_______________________________________________________________ hasProjection
  
  /*!
    \param coord -- Coordinate type to check
    
    \return hasProjection -- <tt>true</tt> if the coordinate type contains a
            spherical map projection; this is the case if the coordinate is
	    composed of at least one object of type casa::DirectionCoordinate.
  */
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

  // ------------------------------------------------------------------ isSpatial
  
  bool CoordinateType::isSpatial (CoordinateType::Types const &coord)
  {
    switch (coord) {
    case CoordinateType::Time:
    case CoordinateType::Frequency:
      return false;
      break;
    default:
      return true;
      break;
    }
  }

  // ----------------------------------------------------------------- isTemporal
  
  bool CoordinateType::isTemporal (CoordinateType::Types const &coord)
  {
    switch (coord) {
    case CoordinateType::Time:
      return true;
      break;
    default:
      return false;
      break;
    }
  }

  // ----------------------------------------------------------------- isSpectral
  
  bool CoordinateType::isSpectral (CoordinateType::Types const &coord)
  {
    switch (coord) {
    case CoordinateType::Frequency:
      return true;
      break;
    default:
      return false;
      break;
    }
  }

  // ---------------------------------------------------- makeDirectionCoordinate

  casa::DirectionCoordinate CoordinateType::makeDirectionCoordinate ()
  {
    return makeDirectionCoordinate (casa::MDirection::AZEL,
				    casa::Projection::STG);
  }
  
  // ---------------------------------------------------- makeDirectionCoordinate
  
  casa::DirectionCoordinate
  CoordinateType::makeDirectionCoordinate (casa::String const &refcode,
					   casa::String const &projection)
  {
    return makeDirectionCoordinate(MDirectionType(refcode),
				   ProjectionType(projection));
  }
  
  // ---------------------------------------------------- makeDirectionCoordinate
  
  casa::DirectionCoordinate
  CoordinateType::makeDirectionCoordinate (casa::MDirection::Types const &directionType,
					   casa::Projection::Type const &projectionType)
  {
    casa::DirectionCoordinate coord;
    unsigned int nofAxes (2);
    Vector<casa::Quantum<double> > refValue (nofAxes);
    Vector<casa::Quantum<double> > increment (nofAxes);
    Vector<double> refPixel (nofAxes);

    refValue(0)  = casa::Quantum<double> (0,"deg");
    refValue(1)  = casa::Quantum<double> (90,"deg");
    increment(0) = casa::Quantum<double> (-1,"deg");
    increment(1) = casa::Quantum<double> (1,"deg");
    refPixel(0)  = 0;
    refPixel(1)  = 0;
    
    DAL::CoordinateGenerator::makeCoordinate (coord,
					      directionType,
					      projectionType,
					      refValue,
					      increment,
					      refPixel);
    
    return coord;
  }
  
  //_____________________________________________________________________________
  //                                                           makeTimeCoordinate
  
  casa::LinearCoordinate
  CoordinateType::makeTimeCoordinate (double const &refValue,
				      double const &increment,
				      double const &refPixel)
  {
    Vector<String> names (1,"Time");
    Vector<String> units (1,"s");
    Vector<double> crval (1,refValue);
    Vector<double> inc   (1,increment);
    Matrix<double> pc    (1,1,1.0);
    Vector<double> crpix (1,refPixel);
    
    return casa::LinearCoordinate (names,
				   units,
				   crval,
				   inc,
				   pc,
				   crpix);
  }
  
  // ------------------------------------------------------------------------------
  
  casa::LinearCoordinate
  CoordinateType::makeFaradayCoordinate (double const &refValue,
					 double const &increment,
					 double const &refPixel)
  {
    Vector<String> names (1,"Faraday rotation");
    Vector<String> units (1,"rad/(m)2");
    Vector<double> refVal (1,refValue);
    Vector<double> inc (1,increment);
    Matrix<double> pc (1,1,1.0);
    Vector<double> crpix (1,refPixel);
    
    return casa::LinearCoordinate (names,
				   units,
				   refVal,
				   inc,
				   pc,
				   crpix);
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
