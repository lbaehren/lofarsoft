/***************************************************************************
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

// $Id$

#include <Imaging/SkymapCoordinates.h>

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
					ObservationData const &obsData,
					uint const &nofBlocks)
  {
    Bool status (true);
    
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
					SkymapCoordinates::MapQuantity mapQuantity)
  {
    Bool status (true);
    uint nofBlocks (1);
    
    status = init (timeFreq,
		   obsData,
		   nofBlocks,
		   mapOrientation,
		   mapQuantity);
    
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
					SkymapCoordinates::MapQuantity mapQuantity)
  {
    Bool status (true);
    
    status = init (timeFreq,
		   obsData,
		   nofBlocks,
		   mapOrientation,
		   mapQuantity);
    
    if (!status) {
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
  
  SkymapCoordinates& SkymapCoordinates::operator= (SkymapCoordinates const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void SkymapCoordinates::copy (SkymapCoordinates const &other)
  {
    timeFreq_p       = other.timeFreq_p;
    obsData_p        = other.obsData_p;
    nofBlocks_p      = other.nofBlocks_p;
    mapOrientation_p = other.mapOrientation_p;
    mapQuantity_p    = other.mapQuantity_p;
    shape_p          = other.shape_p;
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
		 SkymapCoordinates::FREQ_POWER);
  }
  
  // ----------------------------------------------------------------------- init
  
  bool SkymapCoordinates::init (TimeFreq const &timeFreq,
				ObservationData const &obsData,
				uint const &nofBlocks,
				SkymapCoordinates::MapOrientation mapOrientation,
				SkymapCoordinates::MapQuantity mapQuantity)
  {
    timeFreq_p       = timeFreq;
    obsData_p        = obsData;
    nofBlocks_p      = nofBlocks;
    mapOrientation_p = mapOrientation;
    mapQuantity_p    = mapQuantity;
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

  // ---------------------------------------------------------------- mapQuantity

  bool SkymapCoordinates::mapQuantity (std::string &domain,
				       std::string &quantity)
  {
    bool ok (true);

    try {
      switch (mapQuantity_p) {
      case TIME_FIELD:
	domain   = "TIME";
	quantity = "FIELD";
	break;
      case TIME_POWER:
	domain   = "TIME";
	quantity = "POWER";
	break;
      case TIME_CC:
	domain   = "TIME";
	quantity = "CC";
	break;
      case TIME_X:
	domain   = "TIME";
	quantity = "X";
	break;
      case FREQ_POWER:
	domain   = "FREQ";
	quantity = "POWER";
	break;
      case FREQ_FIELD:
	domain   = "FREQ";
	quantity = "FIELD";
	break;
      }
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::mapQuantity] " << message << endl;
      ok = false;
    }
    
    return ok;
  }
  
  // ------------------------------------------------------------- setMapQuantity

  bool
  SkymapCoordinates::setMapQuantity (SkymapCoordinates::MapQuantity const &mapQuantity)
  {
    bool ok (True);

    try {
      mapQuantity_p = mapQuantity;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setMapQuantity] " << message << endl;
      ok = false;
    }

    if (ok) {
      ok = setShape ();
    }

    return ok;
  }

  // ------------------------------------------------------------- setMapQuantity

  bool SkymapCoordinates::setMapQuantity (std::string const &domain,
					  std::string const &quantity)
  {
    bool ok (True);
    
    if (domain == "time" || domain == "Time" || domain == "TIME") {
      if (quantity == "field" || quantity == "Field" || quantity == "FIELD") {
	mapQuantity_p = TIME_FIELD;
      } else if (quantity == "power" || quantity == "Power" || quantity == "POWER") {
	mapQuantity_p = TIME_POWER;
      } else if (quantity == "cc" || quantity == "CC") {
	mapQuantity_p = TIME_CC;
      } else if (quantity == "x" || quantity == "X") {
	mapQuantity_p = TIME_X;
      } else {
	std::cerr << "[SkymapCoordinates::setMapQuantity] Unknown signal quantity "
		  << quantity << endl;
	ok = false;
      }
    } else if (domain == "freq" || domain == "Freq" || domain == "FREQ") {
      if (quantity == "field" || quantity == "Field" || quantity == "FIELD") {
	mapQuantity_p = FREQ_FIELD;
      } else if (quantity == "power" || quantity == "Power" || quantity == "POWER") {
	mapQuantity_p = FREQ_POWER;
      } else {
	std::cerr << "[SkymapCoordinates::setMapQuantity] Unknown signal quantity "
		  << quantity << endl;
	ok = false;
      }
    } else {
      std::cerr << "[SkymapCoordinates::setMapQuantity] Unknown signal domain "
		<< domain << endl;
      ok = false;
    }

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

  // -------------------------------------------------------- directionCoordinate

  DirectionCoordinate
  SkymapCoordinates::directionCoordinate (const String& refcode,
					  const String& projection,
					  const Vector<double>& refValue,
					  const Vector<double>& increment,
					  const IPosition& pixels)
  {
    uint nofAxes (2);
    Vector<double> refPixel (nofAxes);
    Matrix<double> xform(nofAxes,nofAxes,0.0);

    // Setting for field of view centered on a viewing direction
    refPixel(0) = 0.5*pixels(0);
    refPixel(1) = 0.5*pixels(1);

    xform.diagonal() = 1.0;
    
    DirectionCoordinate coord (MDirectionType(refcode),
			       casa::Projection(ProjectionType(projection)),
			       refValue(0),
			       refValue(1),
			       increment(0),
			       increment(1),
			       xform,
			       refPixel(0),
			       refPixel(1));
    
    return coord;
  }

  // -------------------------------------------------------- directionAxisValues
  
  Matrix<double>
  SkymapCoordinates::directionAxisValues (bool const &anglesInDegrees)
  {
    bool verbose (false);
    // Extract the direction axis from the coordinate system object
    DirectionCoordinate coord = directionAxis();
    // Extract the MDirection type
    casa::MDirection::Types type = coord.directionType();
    casa::String directionRefString;
    
    // Feedback on the obtained result
    if (verbose) {
      MDirection direction (type);
      unsigned int refType = direction.getRef().getType();
      cout << "[SkymapCoordinates::directionAxisValues]" << endl;
      cout << "-- Reference string = " << direction.getRefString() << endl;
      cout << "-- Direction type   = " << direction.type()         << endl;
      cout << "-- Reference type   = " << refType                  << endl;
    }
    
    return directionAxisValues (type);
  }

  // -------------------------------------------------------- directionAxisValues

  Matrix<double>
  SkymapCoordinates::directionAxisValues (casa::String const &refcode)
  {
    // Convert reference code strin to reference type
    return directionAxisValues (MDirectionType(refcode));
  }

  // -------------------------------------------------------- directionAxisValues

  Matrix<double>
  SkymapCoordinates::directionAxisValues (casa::MDirection::Types const &type)
  {
    int lon (0);
    int lat (0);
    int nValue (0);
    IPosition shape (2,shape_p(0)*shape_p(1),2);
    Vector<double> pixel (2);
    Vector<double> world (2);
    Matrix<double> values (shape);
    bool status (true);

    /*
      Extract the direction axis from the coordinate system object and adjust
      the reference frame used for the world coordinates
    */

    cout << "--> extracting the direction axis ..." << endl;
    DirectionCoordinate dc = directionAxis();
//     cout << "--> adjusting reference frame conversion ..." << endl;
//     dc.setReferenceConversion(type);
    cout << "--> converting coordinate values ..." << endl;
    for (lon=0, pixel(0)=0.0; lon<shape_p(0); lon++, pixel(0)++) {
      for (lat=0, pixel(1)=0.0; lat<shape_p(1); lat++, pixel(1)++) {
	// perform the actual conversion from pixel to world coordinate
	status = dc.toWorld (world,pixel);
	// stored the retrieved value into the returned array
	values.row(nValue) = world;
	nValue++;
      }
    }

    return values;
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
    
    switch (mapQuantity_p) {
    case TIME_FIELD:
    case TIME_POWER:
    case TIME_CC:
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
    unsigned int nofValues = shape_p(SkymapCoordinates::Time);
    Vector<double> pixel (nofValues);

    for (unsigned int n(0); n<nofValues; n++) {
      pixel(n) = double(n);
    }

    return timeAxisValues (pixel);
  }

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

  // -------------------------------------------------------- frequencyAxisValues

  Vector<double> SkymapCoordinates::frequencyAxisValues ()
  {
    unsigned int nofValues = shape_p(SkymapCoordinates::Frequency);
    Vector<double> pixel (nofValues);

    for (unsigned int n(0); n<nofValues; n++) {
      pixel(n) = double(n);
    }

    return frequencyAxisValues (pixel);
  }

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
    vector<double> frequencyBand (timeFreq_p.frequencyBand());
    DirectionCoordinate dc (directionAxis());

    status = mapQuantity (domain,quantity);
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
    os << " Frequency increment [Hz] = " << timeFreq_p.frequencyIncrement()
       << endl;
    os << " Frequency band      [Hz] = " << frequencyBand[0]
       << " .. " << frequencyBand[1] << endl;
    
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
    os << " Skymap quantity          = " << mapQuantity_p           
       << " [" << domain << "," << quantity << "]"                  << endl;
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

    refValue(0) = 0.0*casa::C::pi/180.0;
    refValue(1) = 90.0*casa::C::pi/180.0;

    increment(0) = -2.0*casa::C::pi/180.0;
    increment(1) = +2.0*casa::C::pi/180.0;

    shape(0) = shape_p(0);
    shape(1) = shape_p(1);

    return directionCoordinate (refcode,
				projection,
				refValue,
				increment,
				shape);
    
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

    /*
      organization of the axes: [lon,lat,dist,time,freq]
                                [ 0 , 1 , 2  ,3   , 4  ]

      organization of the coordinates: [Direction,Linear,Linear,Spectral]
                                       [ 0       , 1    , 2    , 3      ]
    */

    try {
      switch (mapQuantity_p) {
      case TIME_FIELD:
      case TIME_POWER:
      case TIME_CC:
      case TIME_X:
	// time
	shape(3) = timeFreq_p.blocksize()*nofBlocks_p;
	// frequency
	shape(4) = 1;
	break;
      case FREQ_POWER:
      case FREQ_FIELD:
	// time
	shape(3) = nofBlocks_p;
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
