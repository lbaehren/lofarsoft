/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <casa/Quanta/MVDirection.h>
#include <measures/Measures/MeasConvert.h>
#include <measures/Measures/MeasRef.h>

#include <Imaging/SkymapCoordinates.h>
#include <Math/VectorConversion.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ---------------------------------------------------------- SkymapCoordinates
  
  SkymapCoordinates::SkymapCoordinates ()
  {
    Bool status (true);
    
    status = init (TimeFreq(),
		   ObservationData (),
		   1);
    
    if (!status) {
      std::cerr << "[SkymapCoordinates] Error initializing object!"
		<< endl;
    }
  }
  
  // ---------------------------------------------------------- SkymapCoordinates
  
  SkymapCoordinates::SkymapCoordinates (TimeFreq const &timeFreq,
					ObservationData const &obsData)
  {
    Bool status (true);
    uint nofBlocks (1);
    
    status = init (timeFreq,
		   obsData,
		   nofBlocks);
    
    if (!status) {
      std::cerr << "[SkymapCoordinates] Error initializing object!"
		<< endl;
    }
  }

  // ---------------------------------------------------------- SkymapCoordinates
  
  SkymapCoordinates::SkymapCoordinates (TimeFreq const &timeFreq,
					ObservationData const &obsData,
					SkymapCoordinates::MapOrientation mapOrientation,
					BeamType beamType)
  {
    Bool status (true);
    uint nofBlocks (1);
    
    status = init (timeFreq,
		   obsData,
		   nofBlocks,
		   mapOrientation,
		   beamType);
    
    if (!status) {
      std::cerr << "[SkymapCoordinates] Error initializing object!"
		<< endl;
    }
  }
  
  // ---------------------------------------------------------- SkymapCoordinates
  
  SkymapCoordinates::SkymapCoordinates (TimeFreq const &timeFreq,
					ObservationData const &obsData,
					uint const &nofBlocks,
					SkymapCoordinates::MapOrientation mapOrientation,
					BeamType beamType)
  {
    Bool status (true);
    
    status = init (timeFreq,
		   obsData,
		   nofBlocks,
		   mapOrientation,
		   beamType);
    
    if (!status) {
      std::cerr << "[SkymapCoordinates] Error initializing object!"
		<< endl;
    }
  }
  
  // ---------------------------------------------------------- SkymapCoordinates
  
  SkymapCoordinates::SkymapCoordinates (TimeFreq const &timeFreq,
					ObservationData const &obsData,
					String const &refcode,
					String const &projection,
					Vector<double> const &refValue,
					Vector<double> const &increment,
					IPosition const &pixels,
					bool const &anglesInDegrees)
  {
    Bool status (true);
    uint nofBlocks (1);
    
    status = init (timeFreq,
		   obsData,
		   nofBlocks);
    
    if (status) {
      status = setDirectionAxis (refcode,
				 projection,
				 refValue,
				 increment,
				 pixels,
				 anglesInDegrees);
      if (!status) {
	std::cerr << "[SkymapCoordinates] Error assigning DirectionCoordinate!"
		  << endl;
      }
    } else {
      std::cerr << "[SkymapCoordinates] Error initializing object!"
		<< endl;
    }
  }
  
  // ---------------------------------------------------------- SkymapCoordinates
  
  SkymapCoordinates::SkymapCoordinates (SkymapCoordinates const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  SkymapCoordinates::~SkymapCoordinates ()
  {
    destroy();
  }
  
  void SkymapCoordinates::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================

  // ------------------------------------------------------------------ operator=
  
  SkymapCoordinates& SkymapCoordinates::operator= (SkymapCoordinates const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }

  // ----------------------------------------------------------------------- copy
  
  void SkymapCoordinates::copy (SkymapCoordinates const &other)
  {
    timeFreq_p       = other.timeFreq_p;
    obsData_p        = other.obsData_p;
    nofBlocks_p      = other.nofBlocks_p;
    spatialType_p    = other.spatialType_p;
    mapOrientation_p = other.mapOrientation_p;
    beamType_p       = other.beamType_p;
    shape_p          = other.shape_p;
    csys_p           = other.csys_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  // ----------------------------------------------------------------------- init
  
  bool SkymapCoordinates::init (TimeFreq const &timeFreq,
				ObservationData const &obsData,
				uint const &nofBlocks)
  {
    return init (timeFreq,
		 obsData,
		 nofBlocks,
		 SkymapCoordinates::NORTH_EAST,
		 FREQ_POWER);
  }
  
  // ----------------------------------------------------------------------- init
  
  bool SkymapCoordinates::init (TimeFreq const &timeFreq,
				ObservationData const &obsData,
				uint const &nofBlocks,
				SkymapCoordinates::MapOrientation mapOrientation,
				BeamType beamType)
  {
    timeFreq_p       = timeFreq;
    obsData_p        = obsData;
    nofBlocks_p      = nofBlocks;
    mapOrientation_p = mapOrientation;
    beamType_p       = beamType;
    shape_p          = IPosition (5,120,120,1,1,1);
    
    return defaultCoordinateSystem ();
  }

  // ------------------------------------------------------------- mapOrientation

  bool SkymapCoordinates::mapOrientation (std::string &top,
					  std::string &right)
  {
    bool ok (true);
    switch (mapOrientation_p) {
    case NORTH_EAST:
      top   = "North";
      right = "East";
      break;
    case NORTH_WEST:
      top   = "North";
      right = "West";
      break;
    case SOUTH_EAST:
      top   = "South";
      right = "East";
      break;
    case SOUTH_WEST:
      top   = "South";
      right = "West";
      break;
    }
    try {
      
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::mapOrientation] " << message << endl;
      ok = false;
    }

    return ok;
  }
  
  // ---------------------------------------------------------- setMapOrientation

  bool
  SkymapCoordinates::setMapOrientation (SkymapCoordinates::MapOrientation const &mapOrientation)
  {
    bool ok (True);
    
    try {
      mapOrientation_p = mapOrientation;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setMapOrientation] " << message << endl;
      ok = false;
    }
    
    return ok;
  }

  // ---------------------------------------------------------------- beamType

  bool SkymapCoordinates::beamType (std::string &domain,
				    std::string &quantity)
  {
    return Beamformer::beamType (domain,
				 quantity,
				 beamType_p);
  }
  
  // ------------------------------------------------------------- setMapQuantity

  bool
  SkymapCoordinates::setBeamType (BeamType const &beamType)
  {
    bool ok (True);

    try {
      beamType_p = beamType;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setBeamType] " << message << endl;
      ok = false;
    }

    if (ok) {
      ok = setShape ();
    }

    return ok;
  }

  // ------------------------------------------------------------- setMapQuantity

  bool SkymapCoordinates::setBeamType (std::string const &domain,
				       std::string const &quantity)
  {
    bool ok (True);
    
    /*
      The basic functionality for conversion is implemented in BeamType;
      therefore we forward the request and deal with the result later on.
    */
    ok = Beamformer::beamType (beamType_p,
			       domain,
			       quantity);
    
    if (ok) {
      ok = setShape ();
    }

    return ok;
  }

  
  // ============================================================================
  //
  //  Direction coordinates (celestial position & distance)
  //
  // ============================================================================
  
  bool SkymapCoordinates::setSpatialCoordinates (SpatialCoordinate const &coord)
  {
    bool status (true);

    switch (coord.type()) {
    case CoordinateType::Direction:
      /* For the forseeable future we will be keeping three axes, even if the
	 radial axis is degenerate.
      */
      std::cout << "[SkymapCoordinates::setSpatialCoordinates] "
		<< "Usage coordinate type Direction not (yet) supported."
		<< std::endl;
      status = false;
      break;
    case CoordinateType::DirectionRadius:
      spatialType_p = CoordinateType (coord.type());
      break;
    default:
      spatialType_p = CoordinateType (coord.type());
      break;
    }

    return status;
  }
  
  // ------------------------------------------------------------- MDirectionType
  
  MDirection::Types SkymapCoordinates::MDirectionType (std::string const &refcode)
  {
    Bool ok (True);
    MDirection md;
    MDirection::Types tp;

    try {
      ok = md.setRefString (refcode);
      ok = md.getType(tp,refcode);
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::MDirectionType] " << message << endl;
    }

    if (!ok) {
      std::cerr << "[SkymapCoordinates::MDirectionType] Wrong reference code!\n"
		<< " String reads " <<  md.getRefString()
		<< " but should be " << refcode
		<< endl;
    }
    
    return tp;
  }
  
  // ------------------------------------------------------------- ProjectionType
  
  Projection::Type SkymapCoordinates::ProjectionType (std::string const &refcode)
  {
    Projection prj;
    Projection::Type tp (Projection::STG);
    
    try {
      tp = prj.type(refcode);
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::ProjectionType] " << message << endl;
    }
    
    return tp;
  }

  // ----------------------------------------------------------- setDirectionAxis

  bool SkymapCoordinates::setDirectionAxis (String const &refcode,
					    String const &projection,
					    Vector<double> const &refValue,
					    Vector<double> const &increment,
					    IPosition const &pixels,
					    bool const &anglesInDegrees)
  {
    bool status (true);
    
    // Create a new DirectionCoordinate object from the provided parameters
    DirectionCoordinate coord = directionCoordinate (refcode,
						     projection,
						     refValue,
						     increment,
						     pixels,
						     anglesInDegrees);

    // store the shape information
    status = setDirectionShape (pixels(0),pixels(1));

    if (!status) {
      cerr << "[SkymapCoordinates::setDirectionAxis] "
	   << "There was an error storing the shape information!" << endl;
      return false;
    }
    
    /*
      Replace the current DirectionCoordinate with a new one created from the
      input parameters
    */
    return setDirectionAxis(coord);
  }
  
  // -------------------------------------------------------- directionCoordinate
  
  DirectionCoordinate
  SkymapCoordinates::directionCoordinate (String const &refcode,
					  String const &projection,
					  Vector<double> const &refValue,
					  Vector<double> const &increment,
					  IPosition const &pixels,
					  bool const &anglesInDegrees)
  {
    uint nofAxes (2);
    Vector<double> crval (nofAxes);
    Vector<double> crpix (nofAxes);
    Vector<double> cdelt (nofAxes);
    Matrix<double> xform(nofAxes,nofAxes,0.0);

    // Setting for field of view centered on a viewing direction
    crpix(0) = 0.5*pixels(0);
    crpix(1) = 0.5*pixels(1);

    xform.diagonal() = 1.0;

    if (anglesInDegrees) {
      crval(0) = deg2rad(refValue(0));
      crval(1) = deg2rad(refValue(1));
      cdelt(0) = deg2rad(increment(0));
      cdelt(1) = deg2rad(increment(1));
    } else {
      crval = refValue;
      cdelt = increment;
    }
    
    DirectionCoordinate coord (MDirectionType(refcode),
			       casa::Projection(ProjectionType(projection)),
			       crval(0),
			       crval(1),
			       cdelt(0),
			       cdelt(1),
			       xform,
			       crpix(0),
			       crpix(1));
    
    return coord;
  }

  // -------------------------------------------------------- directionAxisValues

  bool SkymapCoordinates::directionAxisValues (casa::String const &refcode,
					       Matrix<double> &directions,
					       Matrix<bool> &mask,
					       bool const &anglesInDegrees)
  {
    /*
      Translate the reference code string into a MDirectionType, such that we can
      simply forward the function call
    */
    MDirection::Types directionType = MDirectionType (refcode);
    /*
      Carry out the extraction of the direction axis values
    */
    return directionAxisValues (directionType,
				directions,
				mask,
				anglesInDegrees);
  }
  
  // -------------------------------------------------------- directionAxisValues
  
  bool SkymapCoordinates::directionAxisValues (MDirection::Types const &refType,
					       Matrix<double> &directions,
					       Matrix<bool> &mask,
					       bool const &anglesInDegrees)
  {
    bool status (true);

    // local variables
    int lon (0);
    int lat (0);
    int nValue (0);
    Vector<double> pixel (2);
    Vector<double> world (2);
    /*
      [1] adjust the shapes of the provided arrays
    */
    try {
      directions.resize (shape_p(0)*shape_p(1),2);
      mask.resize (shape_p(0),shape_p(1));
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::directionAxisValues]" << message 
		<< std::endl;
    }
    
    /*
      [2] The conversion from pixel to world coordinates is done using the
      DirectionCoordinate extracted from the CoordinateSystem.
    */
    DirectionCoordinate dc = directionCoordinate();
    for (lon=0, pixel(0)=0.0; lon<shape_p(0); lon++, pixel(0)++) {
      for (lat=0, pixel(1)=0.0; lat<shape_p(1); lat++, pixel(1)++) {
	// perform the actual conversion from pixel to world coordinate
	mask(lon,lat) = dc.toWorld (world,pixel);
	// stored the retrieved value into the returned array
	directions.row(nValue) = world;
	nValue++;
      }
    }

    /*
      If the coordinate values are requested for a different reference frame as 
      internal to the image (e.g. because we need AZEL coordinate for the
      beamforming while the map is presented in J2000), an additional conversion
      step is required.
    */
    MDirection::Types myDirectionType = dc.directionType();

    if (myDirectionType != refType) {
      /*
	Create a frame from the time and location of the observation; the
	required information are stored within the ObservationData object
      */
      casa::MeasFrame frame (obsData_p.epoch(),
			     obsData_p.observatoryPosition());
      /*
	Set up the reference frame information required for creating a conversion
	engine
      */
      MDirection::Ref refFrom (myDirectionType);
      MDirection::Ref refTo (refType,frame);
      MDirection::Convert engine (refFrom,refTo);
      // check if the conversion engine is operational
      if (engine.isNOP()) {
	cerr << "--> Conversion engine not operational!" << endl;
	status = false;
      } else {
	casa::MVDirection MVDirectionFROM;
	Vector<casa::Quantity> QDirectionTO (2);
	// go through the direction coordinate values and perform the conversion
	for (int n(0); n<nValue; n++) {
	  MVDirectionFROM = MVDirection (Quantity(directions(n,0),"rad"),
					 Quantity(directions(n,1),"rad"));
	  // conversion step
	  QDirectionTO = engine (MVDirectionFROM).getValue().getRecordValue();
	  // store the obtained values
	  if (anglesInDegrees) {
	    directions(n,0) = QDirectionTO(0).getValue("deg");
	    directions(n,1) = QDirectionTO(1).getValue("deg");
	  } else {
	    directions(n,0) = QDirectionTO(0).getValue("rad");
	    directions(n,1) = QDirectionTO(1).getValue("rad");
	  }
	}
      }  
    } else {
      /*
	Even in the case where no conversion between reference frames is
	required, we still need to check if the returned values of the 
	direction angles are supposed to be given in degrees.
      */
      if (anglesInDegrees) {
	directions *= 180/CR::pi;
      }
    }

    return status;
  }
  
  // ------------------------------------------------------------ setDistanceAxis

  bool SkymapCoordinates::setDistanceAxis (double const &refPixel,
					   double const &refValue,
					   double const &increment)
  {
    bool ok (true);
    uint nofErrors (0);
    Vector<double> crpix (1,refPixel);
    Vector<double> crval (1,refValue);
    Vector<double> cdelt (1,increment);
    
    LinearCoordinate axis = distanceAxis ();

    if (!axis.setReferencePixel (crpix)) { nofErrors++; }
    if (!axis.setReferenceValue (crval)) { nofErrors++; }
    if (!axis.setIncrement(cdelt))       { nofErrors++; }

    if (nofErrors == 0) {
      try {
	ok = csys_p.replaceCoordinate (axis,SkymapCoordinates::Distance);
      } catch (std::string message) {
	cerr << "[SkymapCoordinates::setDistanceAxis] " << message << endl;
	ok = false;
      }
    }

    return ok;
  }
  
  // --------------------------------------------------------- distanceAxisValues

  Vector<double> SkymapCoordinates::distanceAxisValues ()
  {
    Vector<double> crpix = distanceAxis().referencePixel();
    Vector<double> pixelValues (shape_p(2));

    for (int pixel(0); pixel<shape_p(2); pixel++) {
      pixelValues(pixel) = crpix(0)+pixel;
    }

    return distanceAxisValues (pixelValues);
  }
  
  // --------------------------------------------------------- distanceAxisValues

  Vector<double>
  SkymapCoordinates::distanceAxisValues (Vector<double> const &pixelValues)
  {
    uint nofPixels (pixelValues.nelements());
    Vector<double> worldValues (nofPixels);
    Vector<double> valPixel (1);
    Vector<double> valWorld (1);
    LinearCoordinate axis = distanceAxis ();
    
    for (uint pixel(0); pixel<nofPixels; pixel++) {
      valPixel(0) = pixelValues(pixel);
      axis.toWorld (valWorld,valPixel);
      worldValues(pixel) = valWorld(0);
   }

    return worldValues;
  }
  
  // ============================================================================
  //
  //  Observation Data (epoch, location, etc.)
  //
  // ============================================================================

  // ------------------------------------------------------------ ObservationData

  bool SkymapCoordinates::setObservationData (ObservationData const &obsData)
  {
    bool status (true);
    
    try {
      obsData_p = obsData;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setObservationData] Error storing new value!"
		<< endl;
      status = false;
    }
    
    return status;
  }

  // ============================================================================
  //
  //  Time & Frequency coordinates
  //
  // ============================================================================
  
  // --------------------------------------------------------------- setNofBlocks

  bool SkymapCoordinates::setNofBlocks (uint const &nofBlocks)
  {
    // store the new value ...
    nofBlocks_p = nofBlocks;
    // ... and update the resulting shape of the image pixel array
    return setShape();
  }
  
  // ---------------------------------------------------------------- setTimeFreq

  bool SkymapCoordinates::setTimeFreq (TimeFreq const &timeFreq)
  {
    bool status (true);
    
    try {
      timeFreq_p = timeFreq;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setTimeFreq] Error storing new value!"
		<< endl;
      status = false;
    }
    
    return status;
  }
  
  // ---------------------------------------------------------------- setTimeFreq

  bool SkymapCoordinates::setTimeFreq (DataReader const &dr)
  {
    bool status (true);

    try {
      timeFreq_p = dr;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setTimeFreq] "
	"Error extracting TimeFreq from DataReader!"
		<< endl;
      status = false;
    }
    
    return status;
  }

  // ---------------------------------------------------------------- setTimeFreq

  bool SkymapCoordinates::setTimeFreq (uint const &blocksize,
				       double const &sampleFrequency,
				       uint const &nyquistZone,
				       double const &referenceTime)
  {
    TimeFreq timeFreq (blocksize,
		       sampleFrequency,
		       nyquistZone,
		       referenceTime);
    
    return setTimeFreq (timeFreq);
  }
  
  // ------------------------------------------------------------------- timeAxis

  bool SkymapCoordinates::setTimeAxis ()
  {
    bool status (true);
    uint axisNumber (3);
    double crval (0.0);
    double increment (0.0);
    
    switch (beamType_p) {
    case TIME_FIELD:
    case TIME_POWER:
    case TIME_CC:
    case TIME_P:
    case TIME_X:
      increment = timeFreq_p.sampleInterval();
      break;
    case FREQ_POWER:
    case FREQ_FIELD:
      increment = timeFreq_p.blocksize()/timeFreq_p.sampleFrequency();
      break;
    }

    // Update the time axis of the coordinate system
    try {
      LinearCoordinate axis = timeFreq_p.timeAxis (crval,
						   increment,
						   0);
      
      // write the modified coordinate axis back to the coordinate system
      return csys_p.replaceCoordinate(axis,axisNumber);
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setTimeAxis] " << message << endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------------------------- timeAxisValues

  Vector<double> SkymapCoordinates::timeAxisValues ()
  {
    unsigned int nofValues = shape_p(3);
    Vector<double> pixel (nofValues);

    for (unsigned int n(0); n<nofValues; n++) {
      pixel(n) = double(n);
    }

    return timeAxisValues (pixel);
  }

  // ------------------------------------------------------------- timeAxisValues

  Vector<double>
  SkymapCoordinates::timeAxisValues (Vector<double> const &pixelValues)
  {
    uint nofPixels (pixelValues.nelements());
    Vector<double> worldValues (nofPixels);
    Vector<double> valPixel (1);
    Vector<double> valWorld (1);
    LinearCoordinate axis = timeAxis ();
    
    for (uint pixel(0); pixel<nofPixels; pixel++) {
      valPixel(0) = pixelValues(pixel);
      axis.toWorld (valWorld,valPixel);
      worldValues(pixel) = valWorld(0);
   }

    return worldValues;
  }

  // ------------------------------------------------------------- timeAxisStride

  int SkymapCoordinates::timeAxisStride ()
  {
    int stride (1);
    
    switch (beamType_p) {
    case TIME_FIELD:
    case TIME_POWER:
    case TIME_CC:
    case TIME_P:
    case TIME_X:
      stride = timeFreq_p.blocksize();
      break;
    case FREQ_POWER:
    case FREQ_FIELD:
      stride = 1;
      break;
    }

    return stride;
  }
  
  // -------------------------------------------------------- frequencyAxisValues

  Vector<double> SkymapCoordinates::frequencyAxisValues ()
  {
    unsigned int nofValues = shape_p(4);
    Vector<double> pixel (nofValues);

    for (unsigned int n(0); n<nofValues; n++) {
      pixel(n) = double(n);
    }

    return frequencyAxisValues (pixel);
  }

  // -------------------------------------------------------- frequencyAxisValues

  Vector<double>
  SkymapCoordinates::frequencyAxisValues (Vector<double> const &pixelValues)
  {
    uint nofPixels (pixelValues.nelements());
    Vector<double> worldValues (nofPixels);
    Vector<double> valPixel (1);
    Vector<double> valWorld (1);
    SpectralCoordinate axis = frequencyAxis ();
    
    for (uint pixel(0); pixel<nofPixels; pixel++) {
      valPixel(0) = pixelValues(pixel);
      axis.toWorld (valWorld,valPixel);
      worldValues(pixel) = valWorld(0);
   }

    return worldValues;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // -------------------------------------------------------------------- summary
  
  void SkymapCoordinates::summary (std::ostream &os)
  {
    bool status (true);
    std::string domain;
    std::string quantity;
    std::string top;
    std::string right;
    DirectionCoordinate dc (directionCoordinate());

    status = beamType (domain,quantity);
    status = mapOrientation (top,right);
    
    os << "-- TimeFreq object:" << endl;
    os << " Blocksize      [samples] = " << timeFreq_p.blocksize()
       << endl;
    os << " Sample frequency    [Hz] = " << timeFreq_p.sampleFrequency()
       << endl;
    os << " Nyquist zone             = " << timeFreq_p.nyquistZone()
       << endl;
    os << " Reference time     [sec] = " << timeFreq_p.referenceTime()
       << endl;
    os << " FFT length    [channels] = " << timeFreq_p.fftLength()
       << endl;
    os << " Sample interval      [s] = " << timeFreq_p.sampleInterval()
       << endl;
    os << " Frequency increment [Hz] = " << timeFreq_p.increment()(1)
       << endl;
    os << " Frequency band      [Hz] = " << timeFreq_p.frequencyRange()
       << endl;
    os << "-- ObservationData object:" << endl;
    os << " Description              = " << obsData_p.description()
       << endl;
    os << " Observer                 = " << obsData_p.observer()
       << endl;
    os << " Epoch                    = " << obsData_p.epoch()
       << endl;
    os << " Observatory              = " << obsData_p.observatory()
       << endl;
    os << " Observatory position     = " << obsData_p.observatoryPosition()
       << endl;
    os << " nof. antennas            = " << obsData_p.nofAntennas()
       << endl;
    os << " antenna positions        = " << obsData_p.antennaPositions().shape()
       << endl;

    os << "-- Image properties:" << endl;
    os << " nof. processed blocks    = " << nofBlocks_p             << endl;
    os << " Projection               = " << dc.projection().name()  << endl;
    os << " Skymap orientation       = " << mapOrientation_p
       << " [" << top << "," << right << "]"                        << endl;
    os << " Skymap quantity          = " << beamType_p           
       << " [" << domain << "," << quantity << "]"                  << endl;
    os << " Spatial coordinate type  = " << spatialType_p.type()    << endl;
    os << " Number of coordinates    = " << csys_p.nCoordinates()   << endl;
    os << " Shape of the pixel array = " << shape_p                 << endl;
    os << " World axis names         = " << csys_p.worldAxisNames() << endl;
    os << " World axis units         = " << csys_p.worldAxisUnits() << endl;
    os << " Reference pixel  (CRPIX) = " << csys_p.referencePixel() << endl;
    os << " Reference value  (CRVAL) = " << csys_p.referenceValue() << endl;
    os << " Increment        (CDELT) = " << csys_p.increment()      << endl;
  }

  // ============================================================================
  //
  //  Default settings for coordinate axes and the coordinate system
  //
  // ============================================================================

  // ---------------------------------------------------- defaultCoordinateSystem
  
  bool SkymapCoordinates::defaultCoordinateSystem ()
  {
    bool status (true);
    CoordinateSystem cs;

    // Set the correct shape of the image
    if (!setShape()) {
      std::cerr << "[SkymapCoordinates::defaultCoordsys] "
		<< " Error setting up image shape " << endl;
      status = false;
    }

    // Assign observation information 
    try {
      cs.setObsInfo(obsData_p.obsInfo());
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::defaultCoordsys] " << message << endl;
      status = false;
    }
    
    // Assign coordinate axes
    try {
      cs.addCoordinate(defaultDirectionAxis());
      cs.addCoordinate(defaultDistanceAxis());
      cs.addCoordinate(defaultTimeAxis());
      cs.addCoordinate(defaultFrequencyAxis());
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::defaultCoordsys] " << message << endl;
      status = false;
    }

    // Store the newly created coordinate system
    try {
      csys_p = cs;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::defaultCoordsys] " << message << endl;
      status = false;
    }
    
    return status;
  }

  // ------------------------------------------------------- defaultDirectionAxis

  DirectionCoordinate SkymapCoordinates::defaultDirectionAxis ()
  {
    uint nofAxes (2);
    std::string refcode ("AZEL");
    std::string projection ("STG");
    Vector<double> refValue (nofAxes);
    Vector<double> increment (nofAxes);
    IPosition shape (nofAxes);
    bool anglesInDegree (true);

    refValue(0) = 0.0;
    refValue(1) = 90.0;

    increment(0) = -2.0;
    increment(1) = +2.0;

    shape(0) = shape_p(0);
    shape(1) = shape_p(1);

    return directionCoordinate (refcode,
				projection,
				refValue,
				increment,
				shape,
				anglesInDegree);
    
  }
  
  // -------------------------------------------------------- defaultDistanceAxis
  
  LinearCoordinate SkymapCoordinates::defaultDistanceAxis ()
  {
    uint nofAxes (1);
    Vector<String> names (nofAxes,"Distance");
    Vector<String> units (nofAxes,"m");
    Vector<double> refVal (nofAxes,-1.0);
    Vector<double> inc (nofAxes,0.0);
    Matrix<double> pc (nofAxes,nofAxes);
    Vector<double> refPix (nofAxes,0.0);

    pc            = 0.0;
    pc.diagonal() = 1.0;
    
    return LinearCoordinate (names,
			     units,
			     refVal,
			     inc,
			     pc,
			     refPix);
  }
  
  // ------------------------------------------------------- defaultFrequencyAxis

  SpectralCoordinate SkymapCoordinates::defaultFrequencyAxis ()
  {
    return timeFreq_p.frequencyAxis();
  }

  // ------------------------------------------------------------ defaultTimeAxis

  LinearCoordinate SkymapCoordinates::defaultTimeAxis ()
  {
    return timeFreq_p.timeAxis();
  }

  // --------------------------------------------------------------- defaultShape
  
  bool SkymapCoordinates::setShape ()
  {
    bool status (true);
    IPosition shape (shape_p);
    int strideTimeAxis (timeAxisStride());

    /*
      organization of the axes: [lon,lat,dist,time,freq]
                                [ 0 , 1 , 2  ,3   , 4  ]

      organization of the coordinates: [Direction,Linear,Linear,Spectral]
                                       [ 0       , 1    , 2    , 3      ]
    */

    try {
      // Time-axis
      shape(3) = strideTimeAxis*nofBlocks_p;
      switch (beamType_p) {
      case TIME_FIELD:
      case TIME_POWER:
      case TIME_CC:
      case TIME_P:
      case TIME_X:
	// frequency
	shape(4) = 1;
	break;
      case FREQ_POWER:
      case FREQ_FIELD:
	// frequency
	shape(4) = timeFreq_p.fftLength();
	break;
      }
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setShape] " << message << endl;
      status = false;
    }

    if (status) {
      shape_p = shape;
    }

    return status;
  }
  
} // Namespace CR -- end
