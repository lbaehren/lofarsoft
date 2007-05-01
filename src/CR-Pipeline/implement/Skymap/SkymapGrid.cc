/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: SkymapGrid.cc,v 1.6 2006/08/09 10:04:22 bahren Exp $ */


#include <Skymap/SkymapGrid.h>

// =============================================================================
//
//  Construction / Destruction
//
// =============================================================================

SkymapGrid::SkymapGrid ()
{
  init_p = True;
  //
  SkymapGrid::init();
}

SkymapGrid::SkymapGrid (const ObsInfo& obsInfo)
{
  // Inhibit completion of initialization, to avoid overwriting the provided
  // observation information with default values
  init_p = False;
  // Store the observation information
  SkymapGrid::setObservationInfo (obsInfo);
  // Initialize the remaining grid parameters
  SkymapGrid::init();
}

SkymapGrid::SkymapGrid (const String refcode,
			const String projection,
			const IPosition& shape,
			const Vector<Double>& center,
			const Vector<Double>& increment)
{
  SkymapGrid::init (SkymapGrid::defaultObservationInfo(),
		    refcode,
		    projection,
		    shape,
		    center,
		    increment);
}

SkymapGrid::SkymapGrid (const ObsInfo& obsInfo,
			const String refcode,
			const String projection,
			const IPosition& shape,
			const Vector<Double>& center,
			const Vector<Double>& increment)
{
  SkymapGrid::init (obsInfo,
		    refcode,
		    projection,
		    shape,
		    center,
		    increment);
}

SkymapGrid::~SkymapGrid () {;}

// =============================================================================
//
//  Initialization of internal data
//
// =============================================================================

void SkymapGrid::init ()
{
  Int nofAxes (2);
  String refcode ("AZEL");
  String projection ("STG");
  IPosition shape (nofAxes);
  Vector<Double> center (nofAxes);
  Vector<Double> increment (nofAxes);
  ObsInfo obsInfo (SkymapGrid::defaultObservationInfo());

  shape = 120;       // default grid : 120x120 pixels

  center(0) = 0.0;   // default coordinate of the map center:
  center(1) = 90.0;  //  local zenith

  increment = 2.0;   // increment chosen such to cover the whole hemisphere

  // Check wether or not to overwrite the cuurent observation information
  // stored with the coordinate system tool.
  if (!init_p) {
    obsInfo = csys_p.obsInfo();
  }
  
  SkymapGrid::init (obsInfo,
		    refcode,
		    projection,
		    shape,
		    center,
		    increment);
}

void SkymapGrid::init (const ObsInfo& obsInfo,
		       const String refcode,
		       const String projection,
		       const IPosition& shape,
		       const Vector<Double>& center,
		       const Vector<Double>& increment)
{
  verbose_p = False;
  init_p = False;
  //
  SkymapGrid::setObservationInfo (obsInfo);
  SkymapGrid::setReferencecode (refcode);
  SkymapGrid::setProjection (projection);
  SkymapGrid::setShape (shape);
  SkymapGrid::setCenter (center);
  SkymapGrid::setIncrement (increment);
  SkymapGrid::setNorthUp (True);
  SkymapGrid::setEastLeft (True);
  //
  SkymapGrid::setElevationRange (SkymapGrid::defaultElevation());
  //
  init_p = True;
  //
  SkymapGrid::setGrid ();
}

// =============================================================================
//
//  Operators
//
// =============================================================================

SkymapGrid& SkymapGrid::operator= (const SkymapGrid& other) 
{
  SkymapGrid::copy_other (other);
  return *this;
}

void SkymapGrid::copy_other (const SkymapGrid& other)
{
  if (this != &other) {
    try {
      init_p    = other.init_p;
      verbose_p = other.verbose_p;
    } catch (AipsError x) {
      cerr << "[SkymapGrid::copy_other] [1] " << x.getMesg() << endl;
    }
    //
    try {
      refcode_p = other.refcode_p;
      projection_p = other.projection_p;
      eastIsLeft_p = other.eastIsLeft_p;
      northIsUp_p  = other.northIsUp_p;
      shape_p      = other.shape_p;
      center_p     = other.center_p;
      increment_p  = other.increment_p;
    } catch (AipsError x) {
      cerr << "[SkymapGrid::copy_other] [2] " << x.getMesg() << endl;
    }
    //
    try {
      csys_p = other.csys_p;
    } catch (AipsError x) {
      cerr << "[SkymapGrid::copy_other] [3] " << x.getMesg() << endl;
    }
    //
    try {
      azel_p.resize(other.azel_p.shape());
      azel_p = other.azel_p;
      //
      elevation_p.resize(other.elevation_p.shape());
      elevation_p = other.elevation_p;
      //
      mask_p.resize(other.mask_p.shape());
      mask_p = other.mask_p;
    } catch (AipsError x) {
      cerr << "[SkymapGrid::copy_other] [4] " << x.getMesg() << endl;
    }
  }
}

// =============================================================================
//
//  WCS settings
//
// =============================================================================

ObsInfo SkymapGrid::observationInfo ()
{
  return csys_p.obsInfo();
}

ObsInfo SkymapGrid::defaultObservationInfo ()
{
  ObsInfo obsInfo;
  Time startTime;
  Quantity epoch (startTime.modifiedJulianDay(), "d");
  //
  obsInfo.setTelescope ("WSRT");
  obsInfo.setObserver ("LOPES-Tools User");
  obsInfo.setObsDate (epoch);
  //
  return obsInfo;
}

void SkymapGrid::setObservationInfo (const ObsInfo& obsInfo)
{
  csys_p.setObsInfo (obsInfo);
  //
  if (init_p) {
    SkymapGrid::setGrid ();
  }
}

void SkymapGrid::setReferencecode (const String refcode)
{
  refcode_p = refcode;
  //
  if (init_p) {
//     SkymapGrid::setCoordinateSystem ();
    SkymapGrid::setGrid ();
  }
}

void SkymapGrid::setProjection (const String projection)
{
  projection_p = projection;
  //
  if (init_p) {
//     SkymapGrid::setCoordinateSystem ();
    SkymapGrid::setGrid ();
  }
}

DirectionCoordinate SkymapGrid::directionCoordinate ()
{
  Int coordinateAxis (0);
  Coordinate::Type type;
  DirectionCoordinate dc;
  
  type = dc.type();
  coordinateAxis = csys_p.findCoordinate (type, -1);
  
  return csys_p.directionCoordinate(coordinateAxis);
}

// =============================================================================
//
//  Access to the grid parameters
//
// =============================================================================

Int SkymapGrid::nelements ()
{
  Int nofAxes (shape_p.nelements());
  Int nofPoints (1);
  //
  for (int n=0; n<nofAxes; n++) {
    nofPoints *= shape_p(n);
  }
  //
  return nofPoints;
}

Int SkymapGrid::nelements (const Bool which)
{
  Int nofPixels (mask_p.nelements());
  Int nofElements (0);
  //
  for (int n=0; n<nofPixels; n++) {
    if (mask_p(n) == which) {
      nofElements++;
    }
  }
  //
  return nofElements;
}

// Grid shape

IPosition SkymapGrid::shape () {
  return shape_p;
}

void SkymapGrid::setShape (const IPosition& shape) {
  shape_p = shape;
  //
  if (init_p) {
    SkymapGrid::setGrid ();
  }
}

void SkymapGrid::setShape (const Vector<Int>& shape) {
  shape_p = shape;
  //
  if (init_p) {
    SkymapGrid::setGrid ();
  }
}

Vector<Double> SkymapGrid::centerPixel()
{
  Int nofAxes (shape_p.nelements());
  Vector<Double> centerPixel (nofAxes);

  for (int n=0; n<nofAxes; n++) {
    centerPixel(n) = shape_p(n)/2.0+1;
  }

  return centerPixel;
}

void SkymapGrid::setCenter (const Vector<Double>& center)
{
  IPosition shape (center.shape());
  //
  center_p.resize(shape);
  center_p = center;
  //
  if (init_p) {
    SkymapGrid::setGrid ();
  }
}

void SkymapGrid::setIncrement (const Vector<Double>& increment)
{
  IPosition shape (increment.shape());
  //
  increment_p.resize(shape);
  increment_p = increment;
  //
  if (init_p) {
    SkymapGrid::setGrid ();
  }
}


// =============================================================================
//
//  Manipulation of the coordinate system tool
//
// =============================================================================

MDirection::Types SkymapGrid::MDirectionType (const String refcode)
{
  Bool ok (True);
  MDirection md;
  MDirection::Types tp;
  //
  try {
    ok = md.getType(tp,refcode);
  } catch (AipsError x) {
    cerr << "[SkymapGrid::MDirectionType] " << x.getMesg() << endl;
  }
  //
  return tp;
}

void SkymapGrid::setCoordinateSystem ()
{
  Vector<Double> refValue (center_p);                  // Reference value
  Vector<Double> refPixel (SkymapGrid::centerPixel()); // Reference pixel
  Matrix<Double> xform(2,2);                           // Linear transform
  MDirection::Types typeMDirection (SkymapGrid::MDirectionType(refcode_p));
  Projection::Type typeProjection (Projection::STG);

  xform            = 0.0;
  xform.diagonal() = 1.0;

  // For the map projection obtain (a) the internal Type and (b) the wether
  // or not is it a zenithal projection
  try {
    Projection prj;
    typeProjection = prj.type(projection_p);
    //
    if (prj.isZenithal(typeProjection) && center_p(1) == 90) {
      refValue(0) = 0.0;
      refValue(1) = 90.0;
    }
  } catch (AipsError x) {
    cerr << "[SkymapGrid::setCoordinateSystem] " << x.getMesg() << endl;
  }

  // Create the DirectionCoordinate to be added to the CoordinateSystem
  DirectionCoordinate dir (typeMDirection,                // Direction
			   Projection(typeProjection),    // Projection
			   refValue(0)*C::pi/180.0,       // CRVAL(0)
			   refValue(1)*C::pi/180.0,       // CRVAL(1)
			   increment_p(0)*C::pi/180.0,    // CDELT(0)
			   increment_p(1)*C::pi/180.0,    // CDELT(1)
			   xform,                         // PC(i,j)
			   refPixel(0),                   // CRPIX(0)
			   refPixel(1));                  // CRPIX(1)

  // Set up the internal coordinate system
  try {
    CoordinateSystem cs;
    //
    cs.setObsInfo (csys_p.obsInfo());
    cs.addCoordinate (dir);
    //
    csys_p = cs;
  } catch (AipsError x) {
    cerr << "[SkymapGrid::setCoordinateSystem] " << x.getMesg() << endl;
  }

}

void SkymapGrid::adjustReferencePixel ()
{ 
  // Set up the initial CoordinateSystem object
  SkymapGrid::setCoordinateSystem ();

  // Adjust the sign of the coordinate increment based on the combination of
  // map projection and map orientation.
  SkymapGrid::adjustIncrement();

  {
    Bool ok (True);
    Vector<Double> world (center_p.shape());
    Vector<Double> pixel (center_p.shape());
    Vector<Double> centerPixel (SkymapGrid::centerPixel());
    Vector<Double> offset (center_p.shape());
    //
    world = center_p;
    world *= C::pi/180;
    offset = 0.0;

    // To what pixel coordinates transforms the intended center position of 
    // the map?
    ok = csys_p.toPixel(pixel,world);

    // Compute the offset between the above value and the center pixel
    offset = pixel - centerPixel;
    //
    pixel -= 2.0*offset;
    //
    csys_p.setReferencePixel (pixel);
    //
    pixel = SkymapGrid::centerPixel();
    pixel *= C::pi/180;
    ok = csys_p.toWorld (world,pixel);
    //
    if (verbose_p) {
      cout << "\n[SkymapGrid::adjustReferencePixel]" << endl;
      cout << " - World axis units ........... : " << csys_p.worldAxisUnits()
	   << endl;
      cout << " - Reference value .... (CRVAL) : " << csys_p.referenceValue()
	   << endl;
      cout << " - Reference pixel .... (CRPIX) : " << csys_p.referencePixel()
	   << endl;
      cout << " - Coordinate increment (CDELT) : " << csys_p.increment()
	   << endl;
      cout << " - Center pixel @ pixel ....... : " << pixel << endl;
      cout << " - Center pixel @ world ....... : " << world << endl;
      cout << " - Center pixel ............... : " << centerPixel << " = "
	   << center_p << endl;
    }
  }
}

void SkymapGrid::adjustIncrement () 
{
  DirectionCoordinate dc (SkymapGrid::directionCoordinate());
  Vector<Double> increment (2);

  // (1) Get the projection type
  Projection projection (dc.projection());
  Projection::Type projectionType (projection.type());
  Bool isZenithal (projection.isZenithal(projectionType));

  // (2) Set the CDELT correction for the chosen map orientation
  if (northIsUp_p && eastIsLeft_p) {
    if (isZenithal) {
      increment(0) = 1;  // Longitude, [0,360]
      increment(1) = 1;  // Latitude   [0,90]
    } else {
      increment = 1;
    }
  } else if (northIsUp_p && !eastIsLeft_p) {
    if (isZenithal) {
      increment(0) = -1;
      increment(1) = 1;
    } else {
      increment(0) = -1;
      increment(1) = 1;
    }
  } else if (!northIsUp_p && eastIsLeft_p) {
    if (isZenithal) {
      increment(0) = 1;
      increment(1) = -1;
    } else {
      increment(0) = 1;
      increment(1) = -1;
    }
  } else {
    if (isZenithal) {
      increment = -1;
    } else {
      increment = -1;
    }
  }

  // (3) Set the coordinate increment
  increment(0) *= increment_p(0)*C::pi/180.0;
  increment(1) *= increment_p(1)*C::pi/180.0;
  
  csys_p.setIncrement (increment);

  if (verbose_p) {
    cout << "[SkymapGrid::adjustIncrement]" << endl;
    cout << " - Map projection ............ : " << projection.name(projectionType)
	 << endl;
    cout << " - Projection is zenithal .... : " << isZenithal << endl;
    cout << " - Coordinate increment ...... : " << increment << endl;
    cout << " - Stored coordinate increment : " << csys_p.increment() << endl;
  }

}

// =============================================================================
//
//  Orientation of the skymap
//
// =============================================================================

void SkymapGrid::setOrientation (const Vector<String>& directions)
{
  Int nelem (directions.nelements());
  Bool eastIsLeft (True);
  Bool northIsUp (True);

  if (nelem < 2) {
    cerr << "[SkymapGrid::setOrientation] Too few parameters." << endl;
  } else {
    // check latitude
    if (directions(0) != "East") {
      eastIsLeft = False;
    }
    // check longitude
    if (directions(1) != "North") {
      northIsUp = False;
    }
    // set internal values
    SkymapGrid::setEastLeft (eastIsLeft);
    SkymapGrid::setNorthUp (northIsUp);
  }
}

Vector<String> SkymapGrid::orientation () 
{
  Vector<String> orientation (2);

  if (eastIsLeft_p) {
    orientation(0) = "East";
  } else {
    orientation(0) = "West";
  }
  
  if (northIsUp_p) {
    orientation(1) = "North";
  } else {
    orientation(1) = "South";
  }

  return orientation;
}

Bool SkymapGrid::isNorthUp () {
  return northIsUp_p;
}

void SkymapGrid::setNorthUp (const Bool northIsUp)
{
  Bool recomputeGrid (True);
  //
  if (northIsUp_p == northIsUp) {
    recomputeGrid = False;
  }
  //
  northIsUp_p = northIsUp;
  //
  if (init_p && recomputeGrid) {
      SkymapGrid::setGrid ();
  }
}

Bool SkymapGrid::isEastLeft () {
  return eastIsLeft_p;
}

void SkymapGrid::setEastLeft (const Bool eastIsLeft)
{
  Bool recomputeGrid (True);
  //
  if (eastIsLeft_p == eastIsLeft) {
    recomputeGrid = False;
  }
  //
  eastIsLeft_p = eastIsLeft;
  //
  if (init_p && recomputeGrid) {
    SkymapGrid::setGrid ();
  }
}


// =============================================================================
//
//  Elevation range in the local (AZEL) reference frame
//
// =============================================================================

void SkymapGrid::setElevationRange (const Vector<Double>& elevation)
{
  elevation_p.resize(elevation.shape());
  elevation_p = elevation;
  //
  if (init_p) {
    SkymapGrid::setGrid ();
  }  
}

Vector<Double> SkymapGrid::defaultElevation ()
{
  Vector<Double> elevation(2);
  //
  elevation(0) = 0.0;
  elevation(1) = 90.0;
  // 
  return elevation;
}

// =============================================================================
//
//  Access to the coordinate grid and its mask
//
// =============================================================================

void SkymapGrid::setGrid ()
{
  // Determine the value of the reference pixel
  SkymapGrid::adjustReferencePixel ();

  // Set up a conversion engine to convert the coordinates in the celestial
  // frame to local ones.

  ObservationData obsData (csys_p.obsInfo());
  MDirection::Convert engine = obsData.conversionEngine (refcode_p,
							 True);

  // -----------------------------------------------------------------
  // Coordinate conversion

  try {
    Int nofAxes (shape_p.nelements());
    Int nofPixels (SkymapGrid::nelements());
    Int numPixel (0);
    IPosition pixel (nofAxes);
    Vector<Double> world (nofAxes);
    MVDirection MVDirectionFROM;
    Vector<Quantity> QDirectionTO(nofAxes);
    //
    azel_p.resize (nofPixels,nofAxes);
    mask_p.resize (nofPixels);
    mask_p = True;
    //
    for (pixel(0)=1; pixel(0)<=shape_p(0); pixel(0)++) {
      for (pixel(1)=1; pixel(1)<=shape_p(1); pixel(1)++) {
	// conversion: pixel -> world
	mask_p(numPixel) = csys_p.toWorld(world,pixel);
	// conversion: world -> AZEL
	MVDirectionFROM = MVDirection (Quantity(world(0),"rad"),
				       Quantity(world(1),"rad"));
	QDirectionTO = engine(MVDirectionFROM).getValue().getRecordValue();
	azel_p(numPixel,0) = QDirectionTO(0).getValue("deg");
	azel_p(numPixel,1) = QDirectionTO(1).getValue("deg");
	// check if the elevation position is within the allowed range
	if (azel_p(numPixel,1) < elevation_p(0)) {
	  mask_p(numPixel) = False;
	}
	if (azel_p(numPixel,1) > elevation_p(1)) {
	  mask_p(numPixel) = False;
	}
	// increment pixel number counter
	numPixel++;
      }
    }
  } catch (AipsError x) {
    cerr << "[SkymapGrid::setGrid] " << x.getMesg() << endl;
  }
  // -----------------------------------------------------------------//
}

Matrix<Double> SkymapGrid::grid ()
{
  Bool ok (True);
  Int nofAxes (shape_p.nelements());
  Int nofPixels (SkymapGrid::nelements());
  Int numPixel (0);
  IPosition pixel (2);
  Vector<Double> world (2);
  Matrix<Double> grid;
  
  try {
    grid.resize (nofPixels,nofAxes);
  } catch (AipsError x) {
    cerr << "[SkymapGrid::grid] " << x.getMesg() << endl;
  }

  for (pixel(0)=0; pixel(0)<shape_p(0); pixel(0)++) {
    for (pixel(1)=0; pixel(1)<shape_p(1); pixel(1)++) {
      ok = csys_p.toWorld(world,pixel);
      // store the world values
      grid(numPixel,0) = world(0);
      grid(numPixel,1) = world(1);
      // increment pixel number counter
      numPixel++;
    }
  }

  // convert values from radian to degrees
  grid /= C::pi/180.0;

  return grid;
}

Matrix<Double> SkymapGrid::grid (const Bool which)
{
  Int nofAxes (shape_p.nelements());
  Int nofPixels (mask_p.nelements());
  Int numPixel (0);
  Int nofSelection (0);

  // -----------------------------------------------------------------
  // Determine the number of grid nodes matching the selection
  // criterium

  for (int n=0; n<nofPixels; n++) {
    nofSelection += int(mask_p(n));
  }
  //
  if (!which) {
    nofSelection = nofPixels-nofSelection;
  }

  // -----------------------------------------------------------------
  // Extract the grid elements matching the selection criterium

  Matrix<Double> gridAll (SkymapGrid::grid ());
  Matrix<Double> grid (nofSelection,nofAxes);

  for (int n=0; n<nofPixels; n++) {
    if (mask_p(n) == which) {
      grid(numPixel,0) = gridAll(n,0);
      grid(numPixel,1) = gridAll(n,1);
      //
      numPixel++;
    }
  }

  return grid;
}

void SkymapGrid::grid (Matrix<Double>& grid, 
		       Vector<Bool>& mask)
{
  grid = SkymapGrid::grid ();
  //
  mask.resize (mask_p.shape());
  mask = mask_p;
}

// =============================================================================
//
//  Grid node positions in the local reference frame
//
// =============================================================================

Matrix<Double> SkymapGrid::azel (const Bool which) 
{
  Int nofAxes (shape_p.nelements());
  Int nofPixels (mask_p.nelements());
  Int numPixel (0);
  Int nofSelection (0);

  // -----------------------------------------------------------------
  // Determine the number of grid nodes matching the selection
  // criterium

  for (int n=0; n<nofPixels; n++) {
    nofSelection += int(mask_p(n));
  }
  //
  if (!which) {
    nofSelection = nofPixels-nofSelection;
  }

  // -----------------------------------------------------------------
  // Extract the grid elements matching the selection criterium

  Matrix<Double> azel (nofSelection,nofAxes);

  for (int n=0; n<nofPixels; n++) {
    if (mask_p(n) == which) {
      azel(numPixel,0) = azel_p(n,0);
      azel(numPixel,1) = azel_p(n,1);
      //
      numPixel++;
    }
  }

  return azel;
}

// =============================================================================
//
//  
//
// =============================================================================

void SkymapGrid::show (std::ostream& os)
{
  os << "\n[SkymapGrid]\n" << endl;
  
  ObsInfo obsInfo (csys_p.obsInfo());
  
  os << " # Observation" << endl;
  os << " -- Observation epoch ........... : " << obsInfo.obsDate() << endl;
  os << " -- Observation location ........ : " << obsInfo.telescope() << endl;
  os << " -- Observer .................... : " << obsInfo.observer() << endl;

  os << " # Skymap grid" << endl;
  os << " -- Reference code .............. : " << refcode_p << endl;
  os << " -- Projection .................. : " << projection_p << endl;
  os << " -- Grid shape .................. : " << shape_p << endl;
  os << " -- Grid center coordinates ..... : " << center_p << endl;
  os << " -- Map orientation [East,North]  : [" << eastIsLeft_p << ","
     << northIsUp_p << "]" << endl;
  os << " -- Allowed range in elevation .. : " << elevation_p << endl;

  Bool ok (True);
  Vector<Double> world (center_p.shape());
  Vector<Double> pixel (center_p.shape());
  
  pixel = csys_p.referencePixel();
  ok = csys_p.toWorld (world,pixel);

  os << " # Coordinate system" << endl;
  os << " -- Reference value (crval) ..... : " << csys_p.referenceValue() << endl;
  os << " -- Reference pixel (crpix) ..... : " << csys_p.referencePixel() << endl;
  os << " -- Coordinate increment (cdelt)  : " << csys_p.increment() << endl;
  os << " -- World axis units ............ : " << csys_p.worldAxisUnits() << endl;
  os << " -- Conversion of reference pixel : " << pixel << " -> " << world << endl;
}
