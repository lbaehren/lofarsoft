/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: Skymapper.cc,v 1.11 2006/11/11 17:44:25 bahren Exp $*/

#include <iostream>
#include <casa/Containers/Record.h>
#include <casa/Exceptions/Error.h>
#include <lopes/Skymap/Skymapper.h>
#include <lopes/Skymap/SkymapperTools.h>
#include <lopes/Utilities/ProgressBar.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

// -------------------------------------------------------------------- Skymapper

Skymapper::Skymapper ()
  : verbose_p (False)
{
  DataReader *dr = new DataReader ();

  init(dr,
       ObservationData(),
       defaultShape());
}

// -------------------------------------------------------------------- Skymapper

Skymapper::Skymapper (DataReader *dr,
		      ObservationData const &obsData)
  : verbose_p (False)
{
  init(dr,
       obsData,
       defaultShape());
}

// -------------------------------------------------------------------- Skymapper

Skymapper::Skymapper (DataReader *dr,
		      ObservationData const &obsData,
		      const IPosition& shape,
		      const String& refcode,
		      const String& projection,
		      const Vector<Double>& center,
		      const Vector<Double>& resolution)
  : verbose_p (False)
{
  // [1] initialize the basic set of parameters
  init(dr,
       obsData,
       shape);
  // [2] update the settings for the direction axis
  isOperational_p = setDirectionAxis (refcode,
				      projection,
				      center,
				      resolution);
}

// -------------------------------------------------------------------- Skymapper

Skymapper::Skymapper (Skymapper const& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

Skymapper::~Skymapper ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

// -------------------------------------------------------------------- operator=

Skymapper &Skymapper::operator= (Skymapper const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

// ------------------------------------------------------------------------- copy

void Skymapper::copy (Skymapper const& other)
{
  verbose_p            = other.verbose_p;
  //
  filename_p           = other.filename_p;
  shape_p              = other.shape_p;
  csys_p               = other.csys_p;
  //
  blocksPerTimeframe_p = other.blocksPerTimeframe_p;
  //
  obsData_p    = other.obsData_p;
  skymap_p     = other.skymap_p;
}

// ---------------------------------------------------------------------- destroy

void Skymapper::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

// ------------------------------------------------------------------------- init

void Skymapper::init (IPosition const &shape,
		      CoordinateSystem const &csys)
{
  blocksPerTimeframe_p = 1;
  quantity_p           = SkymapQuantity (SkymapQuantity::FREQ_POWER);

  setFilename         ("skymap.img");
  setImageShape       (shape);
  setCoordinateSystem (csys);
}

// ------------------------------------------------------------------------- init

void Skymapper::init (DataReader *dr,
		      ObservationData const &obsData,
		      IPosition const &shape)
{
  blocksPerTimeframe_p = 1;
  quantity_p           = SkymapQuantity::FREQ_POWER;

  // handle the provided parameters
  isOperational_p = setDataReader (dr);
  setObservationData  (obsData);
  setImageShape       (shape);

  // set CoordinateSystem object from default/stored parameters
  setCoordinateSystem ();
  syncCoordinateSystem ();

  // Set the function which is used to read in the data during imaging
  pFunction = &Skymapper::getData;
}

// ---------------------------------------------------------------- setDataReader

Bool Skymapper::setDataReader (DataReader *dr)
{
  Bool status (True);

  try {
    dr_p = dr;
  } catch (AipsError x) {
    cerr << "[Skymapper::setDataReader] " << x.getMesg() << endl;
    status = False;
  }

  return status;
}

// ---------------------------------------------------------------- setDataReader

Bool Skymapper::setDataReader (GlishRecord &rec)
{
  Bool status (True);

  try {
    dr_p = DataReaderTools::assignFromRecord (rec,true);
  } catch (AipsError x) {
    cerr << "[Skymapper::setDataReader] " << x.getMesg() << endl;
    status = False;
  }
  
  return status;
}

// ----------------------------------------------------------- setObservationData

void Skymapper::setObservationData (ObservationData const &obsData,
				    Bool const &updateCsys)
{
  obsData_p = obsData;

  // update the ObsInfo object of the CoordinateSystem
  if (updateCsys) {
    syncCoordinateSystem (False);
  }
}

// ---------------------------------------------------------------- setImageShape

void Skymapper::setImageShape (IPosition const &shape,
			       Bool const &updateRefPixel)
{
  shape_p = shape;

  if (updateRefPixel) {
    std::cerr << "[Skymapper::setImageShape] Feature not yet implemented"
	      << std::endl;
  }
}

// ------------------------------------------------------------------ setQuantity

void Skymapper::setSkymapQuantity (SkymapQuantity::Type quantity)
{
  // Store the new parameter value
  quantity_p.setSkymapQuantity (quantity);

  // update the settings for the time axis
  try {
    uint axisNumber (3);
    int nofTimeframes (shape_p(axisNumber));
    int blocksPerTimeframe (blocksPerTimeframe_p);
    
    LinearCoordinate axis (csys_p.linearCoordinate(axisNumber));
    Vector<Double> crval (axis.referenceValue());
    
    setTimeAxis (crval(0),
		 nofTimeframes,
		 blocksPerTimeframe);
  } catch (AipsError x) {
    cerr << "[Skymapper::setSkymapQuantity] " << x.getMesg() << endl;
  }
}


// ==============================================================================
//
//  Coordinate system
//
// ==============================================================================

// ---------------------------------------------------------- setCoordinateSystem

void Skymapper::setCoordinateSystem ()
{
  CoordinateSystem cs;
  
  // assign observation information 
  try {
    cs.setObsInfo(obsData_p.obsInfo());
  } catch (AipsError x) {
    std::cerr << "[Skymapper::defaultCoordsys] " << x.getMesg() << std::endl;
  }
  
  // assign coordinate axes
  try {
    cs.addCoordinate(defaultDirectionAxis());
    cs.addCoordinate(defaultDistanceAxis());
    cs.addCoordinate(defaultTimeAxis());
    cs.addCoordinate(defaultFrequencyAxis());
  } catch (AipsError x) {
    cerr << "[Skymapper::defaultCoordsys] " << x.getMesg() << endl;
  }
  
  try {
    csys_p = cs;
  } catch (AipsError x) {
    cerr << "[Skymapper::defaultCoordsys] " << x.getMesg() << endl;
  }
  
}

// ---------------------------------------------------------- setCoordinateSystem

void Skymapper::setCoordinateSystem (CoordinateSystem const &csys)
{
  // The number of Coordinates that this CoordinateSystem contains.
  uint nofCoordinates (csys.nCoordinates());

  /*
    Keep in mind that the first axis is a DirectionCoordinate, thus the two
    axes belong to a single coordinate (therefore the "-1").
   */
  if (nofCoordinates == shape_p.nelements()-1) {
    csys_p = csys;
  } else {
    std::cerr << "[Skymapper::setCoordinateSystem] " 
	      << "Mismatch in number of coordinates"
	      << std::endl;
    std::cerr << " - nof. coordinates = " << nofCoordinates << std::endl;
    std::cerr << " - image shape      = " << shape_p        << std::endl;
  }
}

// ---------------------------------------------------------- setCoordinateSystem

bool Skymapper::setCoordinateSystem (const ObsInfo &obsinfo,
				     const String &refcode,
				     const String &projection,
				     const Vector<Double> &crpix,
				     const Vector<Double> &crval,
				     const Vector<Double> &cdelt)
{
  bool status (true);
  IPosition shape (1,5);

  /*!
    Test if the input vectors have the correct shape; all must have equal length,
    matching the number of image axes.
   */
  if (shape != crval.shape() ||
      shape != crval.shape() ||
      shape != cdelt.shape()) {
    return false;
  } else {
    shape = shape_p;

    CoordinateSystem cs = SkymapperTools::coordinateSystem (obsinfo,
							    refcode,
							    projection,
							    shape,
							    crpix,
							    crval,
							    cdelt);
    if (cs.nCoordinates() > 0) {
      setCoordinateSystem(cs);
    } else {
      return false;
    }
  }
  
  return status;
}

// ---------------------------------------------------------- setCoordinateSystem

void Skymapper::setCoordinateSystem (GlishRecord const &gRec)
{
  Record rec;
  gRec.toRecord(rec);
  CoordinateSystem *csys_tmp = CoordinateSystem::restore(rec, "");

  if (csys_tmp==0) {
    std::cerr << "[Skymapper::defaultCoordsys] "
	      << "Failed to create a CoordinateSystem from this record"
	      << std::endl;
  } else {
    CoordinateSystem csys = *csys_tmp;
    setCoordinateSystem (csys);
  }
  
  delete csys_tmp;
}

// --------------------------------------------------------- syncCoordinateSystem

void Skymapper::syncCoordinateSystem (Bool const &csysIsMaster)
{
  ObsInfo obsInfo;
  
  if (csysIsMaster) {
    // get the parameters from the source ...
    obsInfo = csys_p.obsInfo();
    // ... and copy it
    obsData_p.setObsInfo (obsInfo);
  } else {
    // get the parameters from the source
    obsInfo = obsData_p.obsInfo();
    // ... and copy it
    csys_p.setObsInfo (obsInfo);
  }
}

// --------------------------------------------------------- defaultDirectionAxis

DirectionCoordinate Skymapper::defaultDirectionAxis ()
{
  Matrix<Double> xform(2,2);

  xform = 0.0;
  xform.diagonal() = 1.0;

  DirectionCoordinate coord (MDirection::AZEL,
			     Projection(Projection::STG),
			     0.0*C::pi/180.0,
			     90.0*C::pi/180.0,
			     -2.0*C::pi/180.0,
			     2.0*C::pi/180.0,
			     xform,
			     0.5*shape_p(0),
			     0.5*shape_p(1));

  coord.setReferencePixel(Vector<Double>(2,0.0));
  
  return coord;
}

// ------------------------------------------------------------- setDirectionAxis

Bool Skymapper::setDirectionAxis (String const &refcode,
				  String const &projection,
				  const Vector<Double> &crval,
				  const Vector<Double> &cdelt)
{
  // Number of the DirectionAxis in the coordinate system
  uint axisNumber (0);

  // Extract the time axis from the coordinate system
  DirectionCoordinate axis (csys_p.directionCoordinate(axisNumber));

  DirectionCoordinate coord (SkymapperTools::MDirectionType(refcode),
			     Projection(SkymapperTools::ProjectionType(projection)),
			     crval(0)*C::pi/180.0,
			     crval(1)*C::pi/180.0,
			     cdelt(0)*C::pi/180.0,
			     cdelt(1)*C::pi/180.0,
			     axis.linearTransform(),
			     0.5*shape_p(0),
			     0.5*shape_p(1));

  // write the modified coordinate axis back to the coordinate system
  return csys_p.replaceCoordinate(coord,axisNumber);
}

// ---------------------------------------------------------- defaultDistanceAxis

LinearCoordinate Skymapper::defaultDistanceAxis ()
{
  Vector<String> names (1,"Distance");
  Vector<String> units (1,"m");
  Vector<Double> refVal (1,-1.0);
  Vector<Double> inc (1,0.0);
  Matrix<Double> pc (1,1,1.0);
  Vector<Double> refPix (1,0.0);
  
  return LinearCoordinate (names,
			   units,
			   refVal,
			   inc,
			   pc,
			   refPix);
}

// -------------------------------------------------------------- defaultTimeAxis

LinearCoordinate Skymapper::defaultTimeAxis ()
{
  Vector<String> names  (1,"Time");
  Vector<String> units  (1,"s");
  Vector<Double> refVal (1,0.0);
  Vector<Double> inc    (1,1.0);
  Matrix<Double> pc     (1,1,1.0);
  Vector<Double> refPix (1,0.0);
  
  return LinearCoordinate (names,
			   units,
			   refVal,
			   inc,
			   pc,
			   refPix);
}

// ------------------------------------------------------------ timeAxisIncrement

Double Skymapper::timeAxisIncrement ()
{
  Double increment (0);

  increment = 1.0/dr_p->samplingRate();

  switch (quantity_p.skymapQuantity()) {
  case SkymapQuantity::TIME_FIELD:
  case SkymapQuantity::TIME_POWER:
  case SkymapQuantity::TIME_CC:
  case SkymapQuantity::TIME_X:
    // nothing else to do at this point; time increment is (samplingRate)^{-1}
    break;
  case SkymapQuantity::FREQ_FIELD:
  case SkymapQuantity::FREQ_POWER:
    increment *= dr_p->blocksize()*blocksPerTimeframe_p;
    break;
  }

  return increment;
}

// ------------------------------------------------------------------ setTimeAxis

Bool Skymapper::setTimeAxis (const double &referenceValue,
			     const int& nofTimeframes,
			     const int& blocksPerTimeframe)
{
  Bool status (True);
  uint axisNumber (3);

  // Extract the time axis from the coordinate system ...
  LinearCoordinate axis (csys_p.linearCoordinate(axisNumber));
  // ... and get its current parameters
  Vector<Double> crval (axis.referenceValue());
  Vector<Double> cdelt (axis.increment());

  shape_p(axisNumber)  = nofTimeframes;
  blocksPerTimeframe_p = blocksPerTimeframe;

  crval(0) = referenceValue;
  cdelt(0) = timeAxisIncrement();

  // write the modified values back to the time axis
  status = axis.setReferenceValue (crval);
  status = axis.setIncrement      (cdelt);

  // write the modified coordinate axis back to the coordinate system
  return csys_p.replaceCoordinate(axis,axisNumber);
}

// --------------------------------------------------------- defaultFrequencyAxis

SpectralCoordinate Skymapper::defaultFrequencyAxis ()
{
  Vector<String> names  (1,"Frequency");
  SpectralCoordinate axis;

  axis.setWorldAxisNames(names);

  return axis;
}


// ==============================================================================
//
//  Processing
//
// ==============================================================================

// ---------------------------------------------------------------------- getData

void Skymapper::getData (Matrix<DComplex> &data)
{
  // get the calibrated frequency data
  data = dr_p->calfft();
  // advance position in the data stream by one block
  dr_p->nextBlock();
}

// ------------------------------------------------------------------ createImage

Bool Skymapper::createImage ()
{
  std::cout << "[Skymapper::createImage]" << std::endl;

  Bool status     (True);
  int radius      (0);
  int integration (0);
  uint block      (0);
  int nofLoops   (shape_p(2)*shape_p(3)*blocksPerTimeframe_p);
  Matrix<Double> antennaPositions (obsData_p.antennaPositions());
  Vector<Double> frequencies (dr_p->frequencyValues());
  Matrix<DComplex> data (dr_p->nofSelectedChannels(),
			 dr_p->nofSelectedAntennas());
  
  
  /*
    Adjust the settings of the embedded Skymap object
  */
  try {
    // Extract the direction axis from the coordinate system
    DirectionCoordinate axis (csys_p.directionCoordinate(0));
    Vector<Double> crval (axis.referenceValue());
    Vector<Double> cdelt (axis.increment());
    IPosition shape (2,shape_p(0),shape_p(1));

    MDirection direction = axis.directionType();
    String refcode       = direction.getRefString();
    String projection    = axis.projection().name();

    crval *= 1/(C::pi/180.0);
    cdelt *= 1/(C::pi/180.0);
    
    std::cout << " - Setting up the skymap grid ... "      << std::endl;
    std::cout << " -- coord. refcode    = " << refcode     << std::endl;
    std::cout << " -- coord. projection = " << projection  << std::endl;
    std::cout << " -- shape             = " << shape       << std::endl;
    std::cout << " -- reference value   = " << crval       << std::endl;
    std::cout << " -- coord. increment  = " << cdelt       << std::endl;
    skymap_p.setSkymapGrid (csys_p.obsInfo(),
			    refcode,
			    projection,
			    shape,
			    crval,
			    cdelt);

    skymap_p.setCoordinateSystem(csys_p);

    // Elevation range
    Vector<Double> elevation (2);
    elevation(0) = 00.0;
    elevation(1) = 90.0;
    skymap_p.setElevationRange (elevation);

    // Orientation of the sky map
    Vector<String> orientation (2);
    orientation(0) = "East";
    orientation(1) = "North";
    skymap_p.setOrientation (orientation);

    // Default value for the distance parameter
    skymap_p.setDistance(-1);

    // Set up the Skymap-internal function pointer to the function handling
    // the actual data processing
    skymap_p.setSkymapQuantity (quantity_p.skymapQuantity());
  } catch (AipsError x) {
    cerr << "[Skymapper::createImage] " << x.getMesg() << endl;
    return False;
  }
  
  /*
    Create paged image on disk, based on the information assembled to far

    The choice for "PagedImage<Double>" is problematic in the sense, that by
    this preselection we exclude computation of electric field as function of
    frequency (which has pixel data of complex type).
  */

  std::cout << " - Creating paged image on disk ... " << std::flush;
  
  TiledShape shape (shape_p);
  PagedImage<Double> image (shape,
			   csys_p,
			   filename_p);

  std::cout << "done." << std::endl;

  /*
    This is the core loop, generating the final skymap from a set of sub-images
  */

  uint numLoop (0);
  LOPES::ProgressBar bar (nofLoops);
  IPosition start  (shape_p.nelements(),0);
  IPosition stride (shape_p.nelements(),1);
  Cube<Double> pixels;
  Cube<Bool> imageMask;

  std::cout << " - computing image pixels ..." << std::endl;
  bar.update(numLoop);

  try {
    for (radius=0; radius<shape_p(2); radius++) {
      // 1. store the distance parameter
//       skymap_p.setDistance(-1.0);
      // 2. update pointer to position in image array
      start(2) = radius;
      // 3. update the beamformer weights for the current shell
      skymap_p.setPhaseGradients (frequencies,
				  antennaPositions);
      for (integration=0; integration<shape_p(3); integration++) {
	start(3) = integration;
	for (block=0; block<blocksPerTimeframe_p; block++) {
	  // 1. read in the data from disk
	  (this->*pFunction)(data);
	  // 2. process the data 
	  skymap_p.data2skymap(data);
	  // show the progress we are making on the image generation
	  numLoop++;
	  bar.update(numLoop);
	}
	// retrieve the Skymap data cube ...
	skymap_p.skymap (pixels,
			 imageMask,
			 Int(shape_p(4)));
	// ... and insert it at [az,el,RADIUS,INTEGRATION,freq]
	image.putSlice (pixels,start,stride);
      } // ------------------------------------------------------ end integration
      // rewind DataReader::position to initial data block
      dr_p->toStartBlock();
    } // ------------------------------------------------------------- end radius
    std::cout << std::endl;
    std::cout << " - all image pixels computed." << std::endl;
  } catch (AipsError x) {
    cerr << "[Skymapper::createImage] " << x.getMesg() << endl;
    return False;
  }
  
  /*
    Provide some summary of the control parameters used in the processing
   */
  std::cout << " - parameter summary:"
	    << std::endl;
  std::cout << " -- antenna positions array = " << antennaPositions.shape()
	    << std::endl;
  std::cout << " -- data array              = " << data.shape()
	    << std::endl;
  std::cout << " -- pixel array             = " << pixels.shape()
	    << std::endl;
  std::cout << " -- image shape             = " << shape_p
	    << std::endl;
  std::cout << " -- nof. computations loops = " << nofLoops
	    << std::endl;

  return status;
}


// ==============================================================================
//
//  Feedback 
//
// ==============================================================================

// ---------------------------------------------------------------------- summary

void Skymapper::summary ()
{
  summary (std::cout);
}

// ---------------------------------------------------------------------- summary

void Skymapper::summary (std::ostream &os)
{
  DirectionCoordinate axis (csys_p.directionCoordinate(0));
  MDirection direction = axis.directionType();
  String refcode       = direction.getRefString();
  String projection    = axis.projection().name();
  ObsInfo obsInfo      = csys_p.obsInfo();

  os << "[Skymapper] Summary of the internal parameters"     << std::endl;
  os << " - Observation ..........  " << std::endl;
  os << " -- observer             = " << obsInfo.observer()      << std::endl;
  os << " -- telescope            = " << obsInfo.telescope()     << std::endl;
  os << " -- observation date     = " << obsInfo.obsDate()       << std::endl;
  os << " -- observatory position = " << obsData_p.observatoryPosition()
     << std::endl;
  os << " - Data I/O ............   " << std::endl;
  os << " -- blocksize            = " << dr_p->blocksize()       << std::endl;
  os << " -- sampling rate [Hz]   = " << dr_p->samplingRate()    << std::endl;
  os << " -- Nyquist zone         = " << dr_p->nyquistZone()     << std::endl;
  os << " - Coordinates ..........  " << std::endl;
  os << " -- reference code       = " << refcode                 << std::endl;
  os << " -- projection           = " << projection              << std::endl;
  os << " -- nof. coordinates     = " << csys_p.nCoordinates()   << std::endl;
  os << " -- names                = " << csys_p.worldAxisNames() << std::endl;
  os << " -- units                = " << csys_p.worldAxisUnits() << std::endl;
  os << " -- ref. pixel           = " << csys_p.referencePixel() << std::endl;
  os << " -- ref. value           = " << csys_p.referenceValue() << std::endl;
  os << " -- coord. increment     = " << csys_p.increment()      << std::endl;
  os << " - Image ................  " << std::endl;
  os << " -- filename             = " << filename()              << std::endl;
  os << " -- image shape          = " << shape()                 << std::endl;
  os << " -- blocks per frame     = " << blocksPerTimeframe_p    << std::endl;
  os << " -- imaged quantity      = " << quantity_p.quantity()   << std::endl;
}
