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

  std::string CoordinateType::name ()
  {
    return getName (type_p);
  }
  
  //_____________________________________________________________________ getName

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
    unsigned int nofAxes (2);
    Vector<Quantum<double> > refValue (nofAxes);
    Vector<Quantum<double> > increment (nofAxes);
    Vector<double> refPixel (nofAxes);

    refValue(0)  = Quantum<double> (0,"deg");
    refValue(1)  = Quantum<double> (90,"deg");
    increment(0) = Quantum<double> (-1,"deg");
    increment(1) = Quantum<double> (1,"deg");
    refPixel(0)  = 0;
    refPixel(1)  = 0;
    
    return makeDirectionCoordinate (directionType,
				    projectionType,
				    refValue,
				    increment,
				    refPixel);
  }
  
  // ---------------------------------------------------- makeDirectionCoordinate
  
  casa::DirectionCoordinate
  CoordinateType::makeDirectionCoordinate (casa::MDirection::Types const &directionType,
					   casa::Projection::Type const &projectionType,
					   Vector<Quantum<double> > const &refValue,
					   Vector<Quantum<double> > const &increment,
					   Vector<double> const &refPixel)
  {
    unsigned int nofAxes (2);
    casa::Matrix<casa::Double> xform(nofAxes,nofAxes);
    
    xform            = 0.0;
    xform.diagonal() = 1.0;

    /* Check the dimensions of the vectors with the input parameters */
    
    if (refValue.nelements() == nofAxes
	&& increment.nelements() == nofAxes
	&& refPixel.nelements() == nofAxes) {
      return casa::DirectionCoordinate (directionType,
					casa::Projection(projectionType),
					refValue(0),
					refValue(1),
					increment(0),
					increment(1),
					xform,
					refPixel(0),
					refPixel(1));
    } else {
      /* Cast error message*/
      std::cerr << "[CoordinateType::makeDirectionCoordinate]" << std::endl;
      std::cerr << "" << std::endl;
      /* Make direction coordinate with minimal consistent set of parameters */
      return makeDirectionCoordinate (directionType,
				      projectionType);
    }
  }
  
  //_____________________________________________________________________________
  //                                                         makeLinearCoordinate
  
  LinearCoordinate
  CoordinateType::makeLinearCoordinate (unsigned int const &nofAxes,
					casa::String const &name,
					casa::String const &unit,
					double const &refValue,
					double const &increment,
					double const &refPixel)
  {
    Vector<casa::String> names (nofAxes,name);
    Vector<casa::String> units (nofAxes,unit);
    Vector<double> crval (nofAxes,refValue);
    Vector<double> cdelt (nofAxes,increment);
    Vector<double> crpix (nofAxes,refPixel);
    
    return makeLinearCoordinate (names,
				 units,
				 crval,
				 cdelt,
				 crpix);
  }
  
  //_____________________________________________________________________________
  //                                                         makeLinearCoordinate
  
  LinearCoordinate
  CoordinateType::makeLinearCoordinate (unsigned int const &nofAxes,
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
      std::cerr << "[CoordinateType::makeLinearCoordinate] " 
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
  
  //_____________________________________________________________________________
  //                                                         makeLinearCoordinate
  
  LinearCoordinate
  CoordinateType::makeLinearCoordinate (Vector<casa::String> const &names,
					Vector<casa::String> const &units,
					Vector<double> const &refValue,
					Vector<double> const &increment,
					Vector<double> const &refPixel)
  {
    uint nelem = refValue.nelements();
    Matrix<casa::Double> pc(nelem,nelem);
    
    pc = 0.0;
    pc.diagonal(1);
    
    LinearCoordinate coord (names,
			    units,
			    refValue,
			    increment,
			    pc,
			    refPixel);
    
    return coord;
  }
  
  
  //_____________________________________________________________________________
  //                                                       makeSpectralCoordinate
  
  SpectralCoordinate
  CoordinateType::makeSpectralCoordinate (double const &refValue,
					  double const &increment,
					  double const &refPixel)
  {
    Vector<String> names (1,"Frequency");
    
    casa::SpectralCoordinate coord (casa::MFrequency::TOPO,
				    refValue,
				    increment,
				    refPixel);
    
    coord.setWorldAxisNames(names);
    
    return coord;
  }

  //_____________________________________________________________________________
  //                                                         makeStokesCoordinate
  
  casa::StokesCoordinate CoordinateType::makeStokesCoordinate ()
  {
    Vector<casa::Int> iquv(4);
    
    iquv(0) = casa::Stokes::I;
    iquv(1) = casa::Stokes::Q;
    iquv(2) = casa::Stokes::U;
    iquv(3) = casa::Stokes::V;
    
    return casa::StokesCoordinate (iquv);
  }
  
  //_____________________________________________________________________________
  //                                                           makeTimeCoordinate
  
  LinearCoordinate
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
    
    return LinearCoordinate (names,
			     units,
			     crval,
			     inc,
			     pc,
			     crpix);
  }
  
  // ------------------------------------------------------------------------------
  
  LinearCoordinate
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
