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
  
  //_____________________________________________________________________________
  //                                                            SpatialCoordinate
  
  SpatialCoordinate::SpatialCoordinate ()
    : CoordinateBase()
  {
    init (CoordinateType::DirectionRadius,
	  "AZEL",
	  "STG");
  }
  
  //_____________________________________________________________________________
  //                                                            SpatialCoordinate
  
  SpatialCoordinate::SpatialCoordinate (casa::DirectionCoordinate const &direction,
					casa::LinearCoordinate const &linear)
    : CoordinateBase()
  {
    type_p           = CoordinateType::DirectionRadius;
    nofAxes_p        = 3;
    nofCoordinates_p = 2;
    shape_p          = IPosition (nofAxes_p,1);
    
    directionCoord_p = direction;
    linearCoord_p    = linear;
  }
  
  //_____________________________________________________________________________
  //                                                            SpatialCoordinate
  
  SpatialCoordinate::SpatialCoordinate (casa::LinearCoordinate const &linear,
					CoordinateType::Types const &coordType)
    : CoordinateBase()
  {
    if (linear.nPixelAxes() == 3) {
      switch (coordType) {
      case CoordinateType::Cartesian:
      case CoordinateType::Spherical:
      case CoordinateType::Cylindrical:
	type_p           = coordType;
	nofAxes_p        = 3;
	nofCoordinates_p = 1;
	shape_p          = IPosition(nofAxes_p,1);
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
  
  //_____________________________________________________________________________
  //                                                            SpatialCoordinate
  
  SpatialCoordinate::SpatialCoordinate (CoordinateType::Types const &coordType,
					casa::String const &refcode,
					casa::String const &projection,
					Vector<double> const &refPixel,
					Vector<Quantum<double> > const &refValue,
					Vector<Quantum<double> > const &increment,
					casa::IPosition const &shape)
    : CoordinateBase()
  {
    init (coordType,
	  refcode,
	  projection,
	  refPixel,
	  refValue,
	  increment,
	  shape);
  }
  
  //_____________________________________________________________________________
  //                                                            SpatialCoordinate
  
  SpatialCoordinate::SpatialCoordinate (SpatialCoordinate const &other)
    : CoordinateBase()
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

  //________________________________________________________________________ copy
  
  void SpatialCoordinate::copy (SpatialCoordinate const &other)
  {
    type_p           = other.type_p;
    nofAxes_p        = other.nofAxes_p;
    nofCoordinates_p = other.nofCoordinates_p;
    shape_p          = other.shape_p;
    directionCoord_p = other.directionCoord_p;
    linearCoord_p    = other.linearCoord_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                      summary
  
  void SpatialCoordinate::summary (std::ostream &os)
  {
    os << "[SpatialCoordinate::summary]" << std::endl;
    os << "-- Type of spatial coordinate   = " << type_p
       << " (" << CoordinateType::getName(type_p) << ")" 
       << std::endl;
    os << "-- nof. coordinate axes         = " << nofAxes()          << std::endl;
    os << "-- nof. coordinate objects      = " << nofCoordinates()   << std::endl;
    os << "-- nof. elements along the axes = " << shape()            << std::endl;
    os << "-- World axis names             = " << worldAxisNames()   << std::endl;
    os << "-- World axis units             = " << worldAxisUnits()   << std::endl;
    os << "-- Reference pixel      (CRPIX) = " << referencePixel()   << std::endl;
    os << "-- Increment            (CDELT) = " << increment()        << std::endl;
    os << "-- Reference value      (CRVAL) = " << referenceValue()   << std::endl;
    os << "-- Spherical map projection     = " << projection()       << std::endl;
    os << "-- Direction reference code     = " << directionRefcode() << std::endl;
  }

  //_____________________________________________________________________________
  //                                                       setDirectionCoordinate
  
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

  //_____________________________________________________________________________
  //                                                             directionRefcode

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
  
   //_____________________________________________________________________________
  //                                                                    projection

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

  //_____________________________________________________________________________
  //                                                                     setShape
  
  bool SpatialCoordinate::setShape (IPosition const &shape)
  {
    bool status (true);
    unsigned int nelem = shape.nelements();
    
    if (nelem == nofAxes_p) {
      shape_p = shape;
    } else {
      std::cerr << "[SpatialCoordinate::setShape]"
		<< " Provided shape information does not match the number of"
		<< " coordinate axes!"
		<< std::endl;
      status = false;
    }

    return status;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                         init
  
  void SpatialCoordinate::init (CoordinateType::Types const &coordType,
				casa::String const &refcode,
				casa::String const &projection)
  {
    type_p = coordType;
    
    casa::Vector<double> refPixel(3);
    Vector<Quantum<double> > refValue(3);
    Vector<Quantum<double> > increment(3);
    casa::IPosition nelem (3);

    switch (coordType) {
    case CoordinateType::DirectionRadius:
      {
	// Number of pixels along the coordinate axes
	nelem(0) = 240;
	nelem(1) = 240;
	nelem(2) = 1;
	// Reference pixel is center of map for directional components
	refPixel(0) = nelem(0)/2;
	refPixel(1) = nelem(1)/2;
	refPixel(2) = 0;
	// Reference value: local zenith
	refValue(0) = Quantum<double>(0,"deg");
	refValue(1) = Quantum<double>(90,"deg");
	refValue(2) = Quantum<double>(1,"m");
	// Coordinate increment
	increment(0) = Quantum<double>(-1,"deg");
	increment(1) = Quantum<double>(1,"deg");
	increment(2) = Quantum<double>(1,"m");
	
	init (coordType,
	      refcode,
	      projection,
	      refPixel,
	      refValue,
	      increment,
	      nelem);
      }
      break;
    case CoordinateType::Cartesian:
      {
	// Number of pixels along the coordinate axes
	nelem(0) = 1;
	nelem(1) = 1;
	nelem(2) = 1;
	// Reference pixel is center of map for directional components
	refPixel(0) = 0;
	refPixel(1) = 0;
	refPixel(2) = 0;
	// Reference value: local zenith
	refValue(0) = Quantum<double>(0,"m");
	refValue(1) = Quantum<double>(0,"m");
	refValue(2) = Quantum<double>(0,"m");
	// Coordinate increment
	increment(0) = Quantum<double>(1,"m");
	increment(1) = Quantum<double>(1,"m");
	increment(2) = Quantum<double>(1,"m");
	
	init (coordType,
	      refcode,
	      projection,
	      refPixel,
	      refValue,
	      increment,
	      nelem);
      }
      break;
    case CoordinateType::Spherical:
      {
	// Number of pixels along the coordinate axes
	nelem = 1;
	// Reference pixel is center of map for directional components
	refPixel = 0;
	// Reference value: coordinate origin
	refValue(0) = Quantum<double>(0,"m");
	refValue(1) = Quantum<double>(0,"deg");
	refValue(2) = Quantum<double>(0,"deg");
	// Coordinate increment
	increment(0) = Quantum<double>(1,"m");
	increment(1) = Quantum<double>(1,"deg");
	increment(2) = Quantum<double>(1,"deg");
	
	init (coordType,
	      refcode,
	      projection,
	      refPixel,
	      refValue,
	      increment,
	      nelem);
      }
      break;
    case CoordinateType::Cylindrical:
      {
	// Number of pixels along the coordinate axes
	nelem = 1;
	// Reference pixel is center of map for directional components
	refPixel = 0;
	// Reference value: coordinate origin
	refValue(0) = Quantum<double>(0,"m");
	refValue(1) = Quantum<double>(0,"deg");
	refValue(2) = Quantum<double>(0,"m");
	// Coordinate increment
	increment(0) = Quantum<double>(1,"m");
	increment(1) = Quantum<double>(1,"deg");
	increment(2) = Quantum<double>(1,"m");
	
	init (coordType,
	      refcode,
	      projection,
	      refPixel,
	      refValue,
	      increment,
	      nelem);
      }
      break;
    default:
      init (CoordinateType::DirectionRadius);
      break;
    }
  }

  //_____________________________________________________________________________
  //                                                                         init

  void SpatialCoordinate::init (CoordinateType::Types const &coordType,
				casa::String const &refcode,
				casa::String const &projection,
				casa::Vector<double> const &refPixel,
				Vector<Quantum<double> > const &refValue,
				Vector<Quantum<double> > const &increment,
				casa::IPosition const &shape)
  {
    type_p = coordType;

    switch (coordType) {
    case CoordinateType::DirectionRadius:
      {
	// book-keeping
	nofAxes_p        = 3;
	nofCoordinates_p = 2;
	setShape(shape);
	// set up DirectionCoordinate
	casa::MDirection::Types directionType  = MDirectionType(refcode);
	casa::Projection::Type  projectionType = ProjectionType(projection);
	casa::Matrix<double> xform(2,2);
	xform            = 0.0;
	xform.diagonal() = 1.0;
	directionCoord_p = casa::DirectionCoordinate (directionType,
						      casa::Projection(projectionType),
						      refValue(0),
						      refValue(1),
						      increment(0),
						      increment(1),
						      xform,
						      refPixel(0),
						      refPixel(1));
	// Set up LinearCoordinate
	casa::Vector<casa::String> names (1,"Length");
	Vector<Quantum<double> > crval (1);
	Vector<Quantum<double> > cdelt (1);
	Vector<double> crpix (1);
	Matrix<double> pc (1,1);
	crval(0)      = refValue(2);
	crpix(0)      = refPixel(2);
	cdelt(0)      = increment(2);
	pc            = 0.0;
	pc.diagonal() = 1.0;
	linearCoord_p    = casa::LinearCoordinate (names,
						   crval,
						   cdelt,
						   pc,
						   crpix);
      }
      break;
    case CoordinateType::Cartesian:
      {
	// book-keeping
	nofAxes_p        = 3;
	nofCoordinates_p = 1;
	shape_p          = IPosition(nofAxes_p,1);
	// Set up LinearCoordinate
	casa::Vector<casa::String> names (nofAxes_p,"Length");
	Matrix<double> pc (nofAxes_p,nofAxes_p);
	pc            = 0.0;
	pc.diagonal() = 1.0;
	linearCoord_p = casa::LinearCoordinate (names,
						refValue,
						increment,
						pc,
						refPixel);
      }
      break;
    case CoordinateType::Spherical:
      {
	// book-keeping
	nofAxes_p        = 3;
	nofCoordinates_p = 1;
	shape_p          = IPosition(nofAxes_p,1);
	// Set up LinearCoordinate
	casa::Vector<casa::String> names (nofAxes_p);
	Matrix<double> pc (nofAxes_p,nofAxes_p);
	names(0) = "Length";
	names(1) = "Direction";
	names(2) = "Direction";
	pc            = 0.0;
	pc.diagonal() = 1.0;
	linearCoord_p = casa::LinearCoordinate (names,
						refValue,
						increment,
						pc,
						refPixel);
      }
      break;
    case CoordinateType::Cylindrical:
      {
	// book-keeping
	nofAxes_p        = 3;
	nofCoordinates_p = 1;
	shape_p          = IPosition(nofAxes_p,1);
	// Set up LinearCoordinate
	casa::Vector<casa::String> names (nofAxes_p);
	Matrix<double> pc (nofAxes_p,nofAxes_p);
	names(0) = "Length";
	names(1) = "Direction";
	names(2) = "Length";
	pc            = 0.0;
	pc.diagonal() = 1.0;
	linearCoord_p = casa::LinearCoordinate (names,
						refValue,
						increment,
						pc,
						refPixel);
      }
      break;
    default:
      std::cerr << "[SpatialCoordinate::init]"
		<< " Sorry, coordinate type not yet supported!"
		<< std::endl;
      break;
    };
  }
  
  //_____________________________________________________________________________
  //                                                              worldAxisValues
  
  Matrix<double> SpatialCoordinate::worldAxisValues (bool const &fastedAxisFirst)
  {
    // determine the number of positions
    uint nofPositions (1);
    for (uint n(0); n<nofAxes_p; n++) {
      nofPositions *= shape_p(n);
    }

    // set up in which order to iterate through the axes
    IPosition axis (nofAxes_p);
    if (fastedAxisFirst) {
      for (uint n(0); n<nofAxes_p; n++) {
	axis(n) = n;
      }
    } else {
      for (uint n(0); n<nofAxes_p; n++) {
	axis(n) = nofAxes_p-1-n;
      }
    }

    /* Create coordinate system object to handle the conversion; this is more
       efficient than using the SpatialCoordinate::toWorld method, since the 
       latter has to take care of the various ways in which the spatial
       coordinate can be composed out of the basic coordinate objects.
    */
    casa::CoordinateSystem csys;
    toCoordinateSystem (csys);

#ifdef DEBUGGING_MESSAGES
    std::cout << "[SpatialCoordinate::worldAxisValues]" << std::endl;
    std::cout << "-- Shape            = " << shape_p      << std::endl;
    std::cout << "-- nof. positions   = " << nofPositions << std::endl;
    std::cout << "-- Axis ordering    = " << axis         << std::endl;
    std::cout << "-- World axis names = " << csys.worldAxisNames() << std::endl;
    std::cout << "-- World axis units = " << csys.worldAxisUnits() << std::endl;
    std::cout << "-- Reference pixel  = " << csys.referencePixel() << std::endl;
    std::cout << "-- Reference value  = " << csys.referenceValue() << std::endl;
    std::cout << "-- Increment        = " << csys.increment()      << std::endl;
#endif

    /* Set up the arrays for the conversion and the returned values */
    Matrix<double> positions (nofPositions,nofAxes_p);
    IPosition pos(nofAxes_p);
    Vector<double> pixel (nofAxes_p);
    Vector<double> world (nofAxes_p);
    uint npos (0);
    
    switch (type_p) {
    case CoordinateType::Direction:
      {
	for (pos(1)=0; pos(1)<shape_p(axis(1)); pos(1)++) {
	  pixel(axis(1)) = pos(1);
	  for (pos(0)=0; pos(0)<shape_p(axis(0)); pos(0)++) {
	    pixel(axis(0)) = pos(0);
	    // conversion from pixel to world coordinates
	    csys.toWorld (world,pixel);
	    // copy result to returned array
	    positions.row(npos) = world;
	    // increment counter
	    npos++;
	  }
	}
      }
      break;
    default:
      for (pos(2)=0; pos(2)<shape_p(axis(2)); pos(2)++) {
	pixel(axis(2)) = pos(2);
	for (pos(1)=0; pos(1)<shape_p(axis(1)); pos(1)++) {
	  pixel(axis(1)) = pos(1);
	  for (pos(0)=0; pos(0)<shape_p(axis(0)); pos(0)++) {
	    pixel(axis(0)) = pos(0);
	    // conversion from pixel to world coordinates
	    csys.toWorld (world,pixel);
	    // copy result to returned array
	    positions.row(npos) = world;
	    // increment counter
	    npos++;
	  }
	}
      }
      break;
    };
    
#ifdef DEBUGGING_MESSAGES
    std::cout << "[SpatialCoordinate::positionValues]"          << std::endl;
    std::cout << "-- nof. spatial positions = " << nofPositions << std::endl;
    std::cout << "-- axis iteration order   = " << axis         << std::endl;
    std::cout << "-- position values array  = " << positions.shape() << std::endl;
#endif

    return positions;
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

  bool SpatialCoordinate::setIncrement (Vector<double> const &incr,
					bool const &anglesInDegrees)
  {
    bool status (true);
    Vector<double> cdelt (incr);

    // Check the input
    if (incr.nelements() != nofAxes()) {
      std::cerr << "[SpatialCoordinate::setIncrement]"
		<< " Number of increment values does not match the number"
		<< " of coordinate axes!"
		<< std::endl;
      return false;
    }
    
    if (anglesInDegrees) {
      cdelt = CR::deg2rad(incr,type_p);
    }
    
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      {
	Vector<double> cdeltDir (2);
	Vector<double> cdeltLin (1);
	//
	cdeltDir(0) = cdelt(0);
	cdeltDir(1) = cdelt(1);
	cdeltLin(0) = cdelt(2);
	//
	directionCoord_p.setIncrement (cdeltDir);
	linearCoord_p.setIncrement (cdeltLin);
      }
      break;
    default:
      linearCoord_p.setIncrement (cdelt);
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

  bool SpatialCoordinate::setReferenceValue (Vector<double> const &refValue,
					     bool const &anglesInDegrees)
  {
    bool status (true);
    Vector<double> crval (refValue);

    // Check the input
    if (refValue.nelements() != nofAxes()) {
      std::cerr << "[SpatialCoordinate::setReferenceValue]"
		<< " Number of reference value values does not match the number"
		<< " of coordinate axes!"
		<< std::endl;
      return false;
    }

    if (anglesInDegrees) {
      crval = CR::deg2rad(refValue,type_p);
    }
    
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      {
	Vector<double> crvalDir (2);
	Vector<double> crvalLin (1);
	//
	crvalDir(0) = crval(0);
	crvalDir(1) = crval(1);
	crvalLin(0) = crval(2);
	//
	directionCoord_p.setReferenceValue (crvalDir);
	linearCoord_p.setReferenceValue (crvalLin);
      }
      break;
    default:
      linearCoord_p.setReferenceValue (crval);
      break;
    };

    return status;
  }

  //_____________________________________________________________ linearTransform

  Matrix<double> SpatialCoordinate::linearTransform()
  {
    switch (type_p) {
    case CoordinateType::DirectionRadius:
      {
	Matrix<double> xform (nofAxes_p,nofAxes_p,0.0);
	Matrix<double> xformDirection (directionCoord_p.linearTransform());
	Matrix<double> xformLinear (linearCoord_p.linearTransform());

	xform(0,0) = xformDirection(0,0);
	xform(0,1) = xformDirection(0,1);
	xform(1,0) = xformDirection(1,0);
	xform(1,1) = xformDirection(1,1);

	xform(2,2) = xformLinear(0,0);

	return xform;
      }
      break;
    case CoordinateType::Direction:
      return directionCoord_p.linearTransform();
      break;
    default:
      return linearCoord_p.linearTransform();
      break;
    }
  }

  //_____________________________________________________________________ toWorld

  void SpatialCoordinate::toWorld (Vector<double> &world,
				   const Vector<double> &pixel)
  {
    switch (type_p) {
      case CoordinateType::Direction:
	directionCoord_p.toWorld (world,pixel);
	break;
    case CoordinateType::DirectionRadius:
      {
	Vector<casa::Double> pixelDir (2);
	Vector<casa::Double> dirWorld (2);
	Vector<casa::Double> linPixel (1);
	Vector<casa::Double> linWorld (2);
	pixelDir(0) = pixel(0);
	pixelDir(1) = pixel(1);
	linPixel(0) = pixel(2);
	// conversion
	directionCoord_p.toWorld (dirWorld,pixelDir);
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

  //_____________________________________________________________________ toPixel

  void SpatialCoordinate::toPixel (Vector<double> &pixel,
				   const Vector<double> &world)
  {
    switch (type_p) {
      case CoordinateType::Direction:
	directionCoord_p.toPixel (pixel,world);
	break;
    case CoordinateType::DirectionRadius:
      {
	Vector<casa::Double> pixelDir (2);
	Vector<casa::Double> linPixel (2);
	Vector<casa::Double> dirWorld (2);
	Vector<casa::Double> linWorld (1);
	dirWorld(0) = world(0);
	dirWorld(1) = world(1);
	linWorld(0) = world(2);
	// conversion
	directionCoord_p.toPixel (pixelDir,dirWorld);
	linearCoord_p.toPixel (linPixel,linWorld);
	// copy to output
	pixel(0) = pixelDir(0);
	pixel(1) = pixelDir(1);
	pixel(2) = linPixel(0);
      }
      break;
    default:
      linearCoord_p.toPixel (pixel,world);
      break;
    }
  }

  //_____________________________________________________________________________
  //                                                           toCoordinateSystem

  void SpatialCoordinate::toCoordinateSystem (casa::CoordinateSystem &csys,
					      bool const &append)
  {
    /* If coordinates are not be simply be appended to an existing coordinate
     * system, we need to clean up first.
     */
    if (!append) {
      csys = casa::CoordinateSystem();
    }
    
    switch (type_p) {
    case CoordinateType::Direction:
      csys.addCoordinate (directionCoord_p);
      break;
    case CoordinateType::DirectionRadius:
      csys.addCoordinate (directionCoord_p);
      csys.addCoordinate (linearCoord_p);
      break;
    default:
      csys.addCoordinate (linearCoord_p);
      break;
    };
    
#ifdef DEBUGGING_MESSAGES
    std::cout << "[SpatialCoordinate::toCoordinateSystem]" << std::endl;
    std::cout << "-- World axis names = " << csys.worldAxisNames() << std::endl;
    std::cout << "-- World axis units = " << csys.worldAxisUnits() << std::endl;
    std::cout << "-- Reference pixel  = " << csys.referencePixel() << std::endl;
    std::cout << "-- Reference value  = " << csys.referenceValue() << std::endl;
    std::cout << "-- Increment        = " << csys.increment()      << std::endl;
#endif
  }
  
} // Namespace CR -- end
