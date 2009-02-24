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

#include <Coordinates/TimeFreqCoordinate.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  //__________________________________________________________ TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (bool const &timeAxisLast)
    : TimeFreq()
  {
    uint blocksPerFrame = 1;
    uint nofFrames      = 1;

    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames,
	  timeAxisLast);
  }

  //__________________________________________________________ TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (uint const &blocksize,
					  uint const &blocksPerFrame,
					  uint const &nofFrames,
					  bool const &timeAxisLast)
    : TimeFreq(blocksize)
  {
    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames,
	  timeAxisLast);
  }
  
  //__________________________________________________________ TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (uint const &blocksize,
					  double const &sampleFrequency,
					  uint const &nyquistZone,
					  uint const &blocksPerFrame,
					  uint const &nofFrames,
					  bool const &timeAxisLast)
    : TimeFreq(blocksize,
	       sampleFrequency,
	       nyquistZone)
  {
    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames,
	  timeAxisLast);
  }
  
  //__________________________________________________________ TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (uint const &blocksize,
					  casa::Quantity const &sampleFrequency,
					  uint const &nyquistZone,
					  uint const &blocksPerFrame,
					  uint const &nofFrames,
					  bool const &timeAxisLast)
    : TimeFreq(blocksize,
	       sampleFrequency,
	       nyquistZone)
  {
    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames,
	  timeAxisLast);
  }
  
  //__________________________________________________________ TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (TimeFreq const &timeFreq,
					  uint const &blocksPerFrame,
					  uint const &nofFrames,
					  bool const &timeAxisLast)
    : TimeFreq(timeFreq)
  {
    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames,
	  timeAxisLast);
  }
  
  //__________________________________________________________ TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (TimeFreqCoordinate const &other)
    : TimeFreq(other),
      coordType_p (CoordinateType(CoordinateType::Frequency))
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  TimeFreqCoordinate::~TimeFreqCoordinate ()
  {
    destroy();
  }
  
  void TimeFreqCoordinate::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  TimeFreqCoordinate& TimeFreqCoordinate::operator= (TimeFreqCoordinate const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }

  //________________________________________________________________________ copy
  
  void TimeFreqCoordinate::copy (TimeFreqCoordinate const &other)
  {
    TimeFreq::operator= (other);
    
    blocksPerFrame_p = other.blocksPerFrame_p;
    nofFrames_p      = other.nofFrames_p;
    nofAxes_p        = other.nofAxes_p;
    timeAxisLast_p   = other.timeAxisLast_p;
    coordType_p      = other.coordType_p;
    coordTime_p      = other.coordTime_p;
    coordFrequency_p = other.coordFrequency_p;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  //________________________________________________________________ setCoordType
  
  bool TimeFreqCoordinate::setCoordType (CR::CoordinateType::Types const &type,
					 bool const &adjust)
  {
    return setCoordType (CoordinateType(type),
			 adjust);
  }
  
  //________________________________________________________________ setCoordType
  
  bool TimeFreqCoordinate::setCoordType (CR::CoordinateType const &type,
					 bool const &adjust)
  {
    bool status (true);
    
    switch (type.type()) {
    case CoordinateType::Time:
      /*
       *  If only a single data block is written into a time-frame, not further
       *  checking is required; otherwise we can a) try to adjust the internal 
       *  settings or b) reject the type adjustment and throw an error message.
       */
      if (blocksPerFrame_p == 1) {
	coordType_p = type;
      } else {
	if (adjust) {
	  coordType_p      = type;
	  blocksPerFrame_p = 1;
	} else {
	  std::cerr << "[TimeFreqCoordinate::setCoordType] " 
		    << "Inconsistent combination of parameter values!" << std::endl;
	  status = false;
	}
      }
      break;
    case CoordinateType::Frequency:
      coordType_p = type;
      break;
    default:
      std::cerr << "[TimeFreqCoordinate::setCoordType] Invalid domain!" << std::endl;
      status = false;
      break;
    }
    
    // update the coordinate objects
    setCoordinates();
    
    return status;
  }
  
  //___________________________________________________________ setBlocksPerFrame
  
  bool TimeFreqCoordinate::setBlocksPerFrame (uint const &blocksPerFrame,
					      bool const &adjustDomain)
  {
    bool status (true);
    
    switch (coordType_p.type()) {
    case CoordinateType::Time:
      {
	if (blocksPerFrame == 1) {
	  blocksPerFrame_p = blocksPerFrame;
	} else {
	  if (adjustDomain) {
	    // throw error message
	    std::cerr << "[TimeFreqCoordinate::setBlocksPerFrame]"
		      << " Adjusting target coordinate domain to frequency."
		      << std::endl;
	    // set internal parameters
	    blocksPerFrame_p = blocksPerFrame;
	    coordType_p.setType(CoordinateType::Frequency);
	  } else {
	    // throw error message
	    std::cerr << "[TimeFreqCoordinate::setBlocksPerFrame]"
		      << " New value rejected, because of wrong target domain."
		      << std::endl;
	    status = false;
	  }
	}
      }
      break;
    default:
      blocksPerFrame_p = blocksPerFrame;
      break;
    }
    
    // update the coordinate objects
    setCoordinates();
    
    return status;
  }

  //________________________________________________________________ setNofFrames

  void TimeFreqCoordinate::setNofFrames (uint const &nofFrames)
  {
    // store input value
    nofFrames_p = nofFrames;
    // update the coordinate objects
    setCoordinates();
  }
  
  //_____________________________________________________________________ summary
  
  void TimeFreqCoordinate::summary (std::ostream &os)
  {
    os << "[TimeFreqCoordinate] Summary of internal parameters."    << std::endl;
    os << "-- Blocksize            = " << blocksize_p           << std::endl;
    os << "-- Sample frequency     = " << sampleFrequency_p     << std::endl;
    os << "-- Nyquist zone         = " << nyquistZone_p         << std::endl;
    os << "-- Reference time       = " << referenceTime_p       << std::endl;
    os << "-- Target domain        = " << coordType_p.name()    << std::endl;
    os << "-- Blocks per frame     = " << blocksPerFrame_p      << std::endl;
    os << "-- nof. frames          = " << nofFrames_p           << std::endl;
    os << "-- Time axis last?      = " << timeAxisLast_p        << std::endl;
    os << "-- Shape of the axes    = " << shape()               << std::endl;
    os << "-- Reference pixel      = " << referencePixel()      << std::endl;
    os << "-- Reference value      = " << referenceValue()      << std::endl;
    os << "-- Axis value increment = " << increment()           << std::endl;
    os << "-- World axis names     = " << worldAxisNames()      << std::endl;
  }

  // ============================================================================
  //
  //  Overloading of methods inherited from TimeFreq
  //
  // ============================================================================
  
  void TimeFreqCoordinate::setBlocksize (uint const &blocksize)
  {
    // call the corresponding method of the base class
    TimeFreq::setBlocksize(blocksize);
    // update the coordinate objects
    setCoordinates();
  }
  
  void TimeFreqCoordinate::setSampleFrequency (double const &sampleFrequency)
  {
    // call the corresponding method of the base class
    TimeFreq::setSampleFrequency(sampleFrequency);
    // update the coordinate objects
    setCoordinates();
  }
  
  void TimeFreqCoordinate::setNyquistZone (uint const &nyquistZone)
  {
    // call the corresponding method of the base class
    TimeFreq::setNyquistZone(nyquistZone);
    // update the coordinate objects
    setCoordinates();
  }
  
  void TimeFreqCoordinate::setReferenceTime (double const &referenceTime)
  {
    // call the corresponding method of the base class
    TimeFreq::setReferenceTime(referenceTime);
    // update the coordinate objects
    setCoordinates();
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  void TimeFreqCoordinate::init (CoordinateType const &coordType,
				 uint const &blocksPerFrame,
				 uint const &nofFrames,
				 bool const &timeAxisLast)
  {
    nofAxes_p      = 2;
    coordType_p    = coordType;
    timeAxisLast_p = timeAxisLast;
    setBlocksPerFrame (blocksPerFrame);
    setNofFrames (nofFrames);
  }
  
  // ---------------------------------------------------------------------- shape
  
  casa::IPosition TimeFreqCoordinate::shape () const
  {
    casa::IPosition shape(2);
    
    switch (coordType_p.type()) {
    case CoordinateType::Time:
      shape(0) = blocksize_p*nofFrames_p;
      shape(1) = 1;
      break;
    case CoordinateType::Frequency: {
      shape(0) = nofFrames_p;
      shape(1) = fftLength_p;
    }
      break;
    default:
      std::cerr << "[TimeFreqCoordinate::shape] Invalid domain!" << std::endl;
      break;
    };

    /* Axis ordering */
    if (timeAxisLast_p) {
      casa::IPosition tmp(2);
      tmp(0) = shape(1);
      tmp(1) = shape(0);
      return tmp;
    } else {
      return shape;
    }
    
  }

  //__________________________________________________________________ timeValues

  Vector<double> TimeFreqCoordinate::timeValues ()
  {
    int nofFrames;;

    if (timeAxisLast_p) {
      nofFrames = shape()(1);
    } else {
      nofFrames = shape()(0);
    }

    Vector<double> pixel (1);
    Vector<double> world (1);
    Vector<double> values (nofFrames);
    
    for (int n(0); n<nofFrames; n++) {
      pixel(0) = n;
      coordTime_p.toWorld(world,pixel);
      values(n) = world(0);
    }

    return values;
  }
  
  //_____________________________________________________________ frequencyValues

  Vector<double> TimeFreqCoordinate::frequencyValues ()
  {
    int nofChannels;
    
    if (timeAxisLast_p) {
      nofChannels = shape()(0);
    } else {
      nofChannels = shape()(1);
    }

    Vector<double> pixel (1);
    Vector<double> world (1);
    Vector<double> values (nofChannels);
    
    for (int n(0); n<nofChannels; n++) {
      pixel(0) = n;
      coordFrequency_p.toWorld(world,pixel);
      values(n) = world(0);
    }

    return values;
  }

  //_____________________________________________________________ worldAxisValues

  Matrix<double> TimeFreqCoordinate::worldAxisValues (bool const &fastedAxisFirst)
  {
    // determine the number of positions
    uint nofPositions (1);
    casa::IPosition axisShape = shape();
    for (uint n(0); n<nofAxes_p; n++) {
      nofPositions *= axisShape(n);
    }
    
    // set up in which order to iterate through the axes
    casa::IPosition axis (nofAxes_p);
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
       efficient than using the TimeFreqCoordinate::toWorld method, since the 
       latter has to take care of the various ways in which the spatial
       coordinate can be composed out of the basic coordinate objects.
    */
    casa::CoordinateSystem csys;
    toCoordinateSystem (csys);

        /* Set up the arrays for the conversion and the returned values */
    casa::Matrix<double> worldValues (nofPositions,nofAxes_p);
    casa::IPosition pos(nofAxes_p,0);
    casa::Vector<double> pixel (nofAxes_p,0);
    casa::Vector<double> world (nofAxes_p,0);
    uint npos (0);
    
    for (pos(1)=0; pos(1)<axisShape(axis(1)); pos(1)++) {
      pixel(axis(1)) = pos(1);
      for (pos(0)=0; pos(0)<axisShape(axis(0)); pos(0)++) {
	pixel(axis(0)) = pos(0);
	// conversion from pixel to world coordinates
	csys.toWorld (world,pixel);
	// copy result to returned array
	worldValues.row(npos) = world;
	// increment counter
	npos++;
      }
    }
    
    return worldValues;
  }

  //______________________________________________________________ setCoordinates

  void TimeFreqCoordinate::setCoordinates (int const &nFrame)
  {
    Vector<double> crval (2);
    Vector<double> cdelt (2);

    //______________________________________________________
    // Compute the increment along the coordinate axis

    switch (coordType_p.type()) {
    case CoordinateType::Time:
      cdelt(0) = 1/sampleFrequency_p;
      cdelt(1) = sampleFrequency_p/2;
      break;
    case CoordinateType::Frequency:
      cdelt(0) = blocksize_p*blocksPerFrame_p*nofFrames_p/sampleFrequency_p;
      cdelt(1) = sampleFrequency_p/blocksize_p;
      break;
    default:
      std::cerr << "[TimeFreqCoordinate::increment] Invalid domain!" << std::endl;
      break;
    };

    //______________________________________________________
    // Compute the reference value of the coordinate axis
    
    switch (coordType_p.type()) {
    case CoordinateType::Time:
      crval(0) = referenceTime() + blocksize_p*nFrame/sampleFrequency_p;
      crval(1) = sampleFrequency_p*(2*nyquistZone_p-1)/4;
      break;
    case CoordinateType::Frequency: {
      crval(0) = referenceTime() + blocksize_p*blocksPerFrame_p*nFrame/sampleFrequency_p;
      crval(1) = sampleFrequency_p*(nyquistZone_p-1)/2;
    }
      break;
    default:
      std::cerr << "[TimeFreqCoordinate::shape] Invalid domain!" << std::endl;
      break;
    };
    
    //______________________________________________________
    // Set up the internal coordinate objects
    
    if (timeAxisLast_p) {
      coordFrequency_p = CoordinateType::makeSpectralCoordinate (crval(0),
								 cdelt(0));
      coordTime_p      = CoordinateType::makeTimeCoordinate(crval(1),
							    cdelt(1));
    } else {
      coordTime_p      = CoordinateType::makeTimeCoordinate(crval(0),
							    cdelt(0));
      coordFrequency_p = CoordinateType::makeSpectralCoordinate (crval(1),
								 cdelt(1));
    }
  }

  // ------------------------------------------------------------------ increment
  
  Vector<double> TimeFreqCoordinate::increment () const
  {
    Vector<double> cdelt (2);

    if (timeAxisLast_p) {
      cdelt(0) = coordFrequency_p.increment()(0);
      cdelt(1) = coordTime_p.increment()(0);
    } else {
      cdelt(0) = coordTime_p.increment()(0);
      cdelt(1) = coordFrequency_p.increment()(0);
    }
    
    return cdelt;
  }

  // ------------------------------------------------------------- worldAxisNames
  
  Vector<casa::String> TimeFreqCoordinate::worldAxisNames () const
  {
    Vector<casa::String> names (2);
    
    if (timeAxisLast_p) {
      names(0) = coordFrequency_p.worldAxisNames()(0);
      names(1) = coordTime_p.worldAxisNames()(0);
    } else {
      names(0) = coordTime_p.worldAxisNames()(0);
      names(1) = coordFrequency_p.worldAxisNames()(0);
    } 
    
    return names;
  }

  // ------------------------------------------------------------- worldAxisUnits
  
  Vector<casa::String> TimeFreqCoordinate::worldAxisUnits () const
  {
    Vector<casa::String> units (2);

    if (timeAxisLast_p) {
      units(0) = coordFrequency_p.worldAxisUnits()(0);
      units(1) = coordTime_p.worldAxisUnits()(0);
    } else {
      units(0) = coordTime_p.worldAxisUnits()(0);
      units(1) = coordFrequency_p.worldAxisUnits()(0);
    }

    return units;
  }

  // ------------------------------------------------------------- referencePixel
  
  Vector<double> TimeFreqCoordinate::referencePixel () const
  {
    Vector<double> refPixel (2);

    if (timeAxisLast_p) {
      refPixel(0) = coordFrequency_p.referencePixel()(0);
      refPixel(1) = coordTime_p.referencePixel()(0);
    } else {
      refPixel(0) = coordTime_p.referencePixel()(0);
      refPixel(1) = coordFrequency_p.referencePixel()(0);
    }

    return refPixel;
  }

  // ------------------------------------------------------------- referenceValue

  Vector<double> TimeFreqCoordinate::referenceValue () const
  {
    Vector<double> refValue (2);
    
    if (timeAxisLast_p) {
      refValue(0) = coordFrequency_p.referenceValue()(0);
      refValue(1) = coordTime_p.referenceValue()(0);
    } else {
      refValue(0) = coordTime_p.referenceValue()(0);
      refValue(1) = coordFrequency_p.referenceValue()(0);
    }

    return refValue;
  }

  //_____________________________________________________________________ toWorld
  
  void TimeFreqCoordinate::toWorld (Vector<double> &world,
				    Vector<double> const &pixel)
  {
    Vector<double> in (1);
    Vector<double> out (1);

    // Time axis
    in = pixel(0);
    coordTime_p.toWorld (out,in);
    world(0) = out(0);

    // Frequency axis
    in = pixel(1);
    coordFrequency_p.toWorld (out,in);
    world(1) = out(0);
  }
  
  //_____________________________________________________________________ toPixel
  
  void TimeFreqCoordinate::toPixel (Vector<double> &pixel,
				    Vector<double> const &world)

  {
    Vector<double> in (1);
    Vector<double> out (1);

    // Time axis
    in = world(0);
    coordTime_p.toPixel (out,in);
    pixel(0) = out(0);

    // Frequency axis
    in = world(1);
    coordFrequency_p.toPixel (out,in);
    pixel(1) = out(0);
  }

  //__________________________________________________________ toCoordinateSystem

  void TimeFreqCoordinate::toCoordinateSystem (casa::CoordinateSystem &csys,
					       bool const &append)
  {
    if (!append) {
      csys = casa::CoordinateSystem();
    }
    
    if (timeAxisLast_p) {
      csys.addCoordinate(coordFrequency_p);
      csys.addCoordinate(coordTime_p);
    } else {
      csys.addCoordinate(coordTime_p);
      csys.addCoordinate(coordFrequency_p);
    }
  }
  
} // Namespace CR -- end
