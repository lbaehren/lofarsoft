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

/* $Id: SkymapCoordinates.cc,v 1.12 2007/04/19 14:22:46 horneff Exp $*/

#include <lopes/Skymap/SkymapCoordinates.h>

namespace LOPES { // Namespace LOPES -- begin
  
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
		<< std::endl;
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
		<< std::endl;
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
		<< std::endl;
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
		<< std::endl;
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
  {;}

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
		 NORTH_EAST,
		 FREQ_POWER);
  }
  
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

  // ---------------------------------------------------------- setMapOrientation

  bool
  SkymapCoordinates::setMapOrientation (SkymapCoordinates::MapOrientation const &mapOrientation)
  {
    bool ok (True);
    
    try {
      mapOrientation_p = mapOrientation;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::setMapOrientation] " << message << std::endl;
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
      std::cerr << "[SkymapCoordinates::mapQuantity] " << message << std::endl;
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
      std::cerr << "[SkymapCoordinates::setMapQuantity] " << message << std::endl;
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
		  << quantity << std::endl;
	ok = false;
      }
    } else if (domain == "freq" || domain == "Freq" || domain == "FREQ") {
      if (quantity == "field" || quantity == "Field" || quantity == "FIELD") {
	mapQuantity_p = FREQ_FIELD;
      } else if (quantity == "power" || quantity == "Power" || quantity == "POWER") {
	mapQuantity_p = FREQ_POWER;
      } else {
	std::cerr << "[SkymapCoordinates::setMapQuantity] Unknown signal quantity "
		  << quantity << std::endl;
	ok = false;
      }
    } else {
      std::cerr << "[SkymapCoordinates::setMapQuantity] Unknown signal domain "
		<< domain << std::endl;
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
      std::cerr << "[SkymapCoordinates::MDirectionType] " << message << std::endl;
    }

    if (!ok) {
      std::cerr << "[SkymapCoordinates::MDirectionType] Wrong reference code!\n"
		<< " String reads " <<  md.getRefString()
		<< " but should be " << refcode
		<< std::endl;
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
      std::cerr << "[SkymapCoordinates::ProjectionType] " << message << std::endl;
    }
    
    return tp;
  }

  // -------------------------------------------------------- directionCoordinate

  DirectionCoordinate
  SkymapCoordinates::directionCoordinate (const String& refcode,
					  const String& projection,
					  const Vector<Double>& refValue,
					  const Vector<Double>& increment,
					  const IPosition& pixels)
  {
    uint nofAxes (2);
    Vector<Double> refPixel (nofAxes);
    Matrix<Double> xform(nofAxes,nofAxes,0.0);

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
    
//     coord.setReferencePixel(refPixel);
    
    return coord;
  }

  // ------------------------------------------------------------ setDistanceAxis

  bool SkymapCoordinates::setDistanceAxis (Double const &refPixel,
					   Double const &refValue,
					   Double const &increment)
  {
    bool ok (true);
    uint nofErrors (0);
    Vector<Double> crpix (1,refPixel);
    Vector<Double> crval (1,refValue);
    Vector<Double> cdelt (1,increment);
    
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

  Vector<Double> SkymapCoordinates::distanceAxisValues ()
  {
    Vector<Double> crpix = distanceAxis().referencePixel();
    Vector<Double> pixelValues (shape_p(2));

    for (int pixel(0); pixel<shape_p(2); pixel++) {
      pixelValues(pixel) = crpix(0)+pixel;
    }

    return distanceAxisValues (pixelValues);
  }
  
  // --------------------------------------------------------- distanceAxisValues

  Vector<Double>
  SkymapCoordinates::distanceAxisValues (Vector<Double> const &pixelValues)
  {
    uint nofPixels (pixelValues.nelements());
    Vector<Double> worldValues (nofPixels);
    Vector<Double> valPixel (1);
    Vector<Double> valWorld (1);
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
		<< std::endl;
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
		<< std::endl;
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
		<< std::endl;
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
      std::cerr << "[SkymapCoordinates::setTimeAxis] " << message << std::endl;
      status = false;
    }

    return status;
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
    vector<double> frequencyBand (timeFreq_p.frequencyBand());

    status = mapQuantity (domain,quantity);
    
    os << "-- TimeFreq object:" << std::endl;
    os << " Blocksize      [samples] = " << timeFreq_p.blocksize()
       << std::endl;
    os << " Sample frequency    [Hz] = " << timeFreq_p.sampleFrequency()
       << std::endl;
    os << " Nyquist zone             = " << timeFreq_p.nyquistZone()
       << std::endl;
    os << " Reference time     [sec] = " << timeFreq_p.referenceTime()
       << std::endl;
    os << " FFT length    [channels] = " << timeFreq_p.fftLength()
       << std::endl;
    os << " Sample interval      [s] = " << timeFreq_p.sampleInterval()
       << std::endl;
    os << " Frequency increment [Hz] = " << timeFreq_p.frequencyIncrement()
       << std::endl;
    os << " Frequency band      [Hz] = " << frequencyBand[0]
       << " .. " << frequencyBand[1] << std::endl;
    
    os << "-- ObservationData object:" << std::endl;
    os << " Description              = " << obsData_p.description()
       << std::endl;
    os << " Observer                 = " << obsData_p.observer()
       << std::endl;
    os << " Epoch                    = " << obsData_p.epoch()
       << std::endl;
    os << " Observatory              = " << obsData_p.observatory()
       << std::endl;
    os << " Observatory position     = " << obsData_p.observatoryPosition()
       << std::endl;
    os << " nof. antennas            = " << obsData_p.nofAntennas()
       << std::endl;
    os << " antenna positions        = " << obsData_p.antennaPositions().shape()
       << std::endl;

    os << "-- Image properties:" << std::endl;
    os << " nof. processed blocks    = " << nofBlocks_p             << std::endl;
    os << " Skymap orientation       = " << mapOrientation_p        << std::endl;
    os << " Skymap quantity          = " << mapQuantity_p           
       << " [" << domain << "," << quantity << "]"                  << std::endl;
    os << " Number of coordinates    = " << csys_p.nCoordinates()   << std::endl;
    os << " Shape of the pixel array = " << shape_p                 << std::endl;
    os << " World axis names         = " << csys_p.worldAxisNames() << std::endl;
    os << " World axis units         = " << csys_p.worldAxisUnits() << std::endl;
    os << " Reference pixel  (CRPIX) = " << csys_p.referencePixel() << std::endl;
    os << " Reference value  (CRVAL) = " << csys_p.referenceValue() << std::endl;
    os << " Increment        (CDELT) = " << csys_p.increment()      << std::endl;
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
		<< " Error setting up image shape " << std::endl;
      status = false;
    }

    // Assign observation information 
    try {
      cs.setObsInfo(obsData_p.obsInfo());
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::defaultCoordsys] " << message << std::endl;
      status = false;
    }
    
    // Assign coordinate axes
    try {
      cs.addCoordinate(defaultDirectionAxis());
      cs.addCoordinate(defaultDistanceAxis());
      cs.addCoordinate(defaultTimeAxis());
      cs.addCoordinate(defaultFrequencyAxis());
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::defaultCoordsys] " << message << std::endl;
      status = false;
    }

    // Store the newly created coordinate system
    try {
      csys_p = cs;
    } catch (std::string message) {
      std::cerr << "[SkymapCoordinates::defaultCoordsys] " << message << std::endl;
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
    Vector<Double> refValue (nofAxes);
    Vector<Double> increment (nofAxes);
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
    Vector<Double> refVal (nofAxes,-1.0);
    Vector<Double> inc (nofAxes,0.0);
    Matrix<Double> pc (nofAxes,nofAxes);
    Vector<Double> refPix (nofAxes,0.0);

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
      std::cerr << "[SkymapCoordinates::setShape] " << message << std::endl;
      status = false;
    }

    if (status) {
      shape_p = shape;
    }

    return status;
  }
  
} // Namespace LOPES -- end
