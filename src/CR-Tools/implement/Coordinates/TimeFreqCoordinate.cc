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

  // ------------------------------------------------------------- TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate ()
    : TimeFreq()
  {
    uint blocksPerFrame = 1;
    uint nofFrames      = 1;

    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames);
  }
  
  // ------------------------------------------------------------- TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (uint const &blocksPerFrame,
					  uint const &nofFrames)
    : TimeFreq()
  {
    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames);
  }
  
  // ------------------------------------------------------------- TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (uint const &blocksize,
					  double const &sampleFrequency,
					  uint const &nyquistZone,
					  uint const &blocksPerFrame,
					  uint const &nofFrames)
    : TimeFreq(blocksize,
	       sampleFrequency,
	       nyquistZone)
  {
    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames);
  }
  
  // ------------------------------------------------------------- TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (uint const &blocksize,
					  casa::Quantity const &sampleFrequency,
					  uint const &nyquistZone,
					  uint const &blocksPerFrame,
					  uint const &nofFrames)
    : TimeFreq(blocksize,
	       sampleFrequency,
	       nyquistZone)
  {
    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames);
  }
  
  // ------------------------------------------------------------- TimeFreqCoordinate
  
  TimeFreqCoordinate::TimeFreqCoordinate (TimeFreq const &timeFreq,
					  uint const &blocksPerFrame,
					  uint const &nofFrames)
    : TimeFreq(timeFreq)
  {
    init (CoordinateType(CoordinateType::Frequency),
	  blocksPerFrame,
	  nofFrames);
  }
  
  // ------------------------------------------------------------- TimeFreqCoordinate
  
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
  
  void TimeFreqCoordinate::copy (TimeFreqCoordinate const &other)
  {
    TimeFreq::operator= (other);
    
    blocksPerFrame_p = other.blocksPerFrame_p;
    nofFrames_p      = other.nofFrames_p;
    coordType_p      = other.coordType_p;
    coordTime_p      = other.coordTime_p;
    coordFrequency_p = other.coordFrequency_p;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  //_______________________________________________________________ setBeamDomain
  
  bool TimeFreqCoordinate::setBeamCoordDomain (CR::CoordinateType::Types const &type)
  {
    return setBeamCoordDomain (CoordinateType(type));
  }
  
  //_______________________________________________________________ setBeamDomain
  
  bool TimeFreqCoordinate::setBeamCoordDomain (CR::CoordinateType const &type)
  {
    bool status (true);
    
    switch (type.type()) {
    case CoordinateType::Time:
      if (blocksPerFrame_p == 1) {
	coordType_p = type;
      } else {
	std::cerr << "[TimeFreqCoordinate::setBeamDomain] " 
		  << "Inconsistent combination of parameter values!" << std::endl;
	status = false;
      }
      break;
    case CoordinateType::Frequency:
      coordType_p = type;
      break;
    default:
      std::cerr << "[TimeFreqCoordinate::setBeamDomain] Invalid domain!" << std::endl;
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
      if (blocksPerFrame > 1 && adjustDomain) {
	blocksPerFrame_p = blocksPerFrame;
	coordType_p.setType (CoordinateType::Frequency);
      } else {
	blocksPerFrame_p = blocksPerFrame;
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
    os << "-- Shape of the axes    = " << shape()               << std::endl;
    os << "-- Reference pixel      = " << referencePixel()      << std::endl;
    os << "-- Reference value      = " << referenceValue()      << std::endl;
    os << "-- Axis value increment = " << increment()           << std::endl;
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
				 uint const &nofFrames)
  {
    coordType_p = coordType;
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
    
    return shape;
  }
  
  // ------------------------------------------------------------- setCoordinates

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
    
    coordTime_p      = CoordinateType::makeTimeCoordinate(crval(0),
							  cdelt(0));
    coordFrequency_p = CoordinateType::makeSpectralCoordinate (crval(1),
							       cdelt(1));
  }

  // ------------------------------------------------------------------ increment
  
  Vector<double> TimeFreqCoordinate::increment () const
  {
    Vector<double> cdelt (2);

    cdelt(0) = coordTime_p.increment()(0);
    cdelt(1) = coordFrequency_p.increment()(0);

    return cdelt;
  }

  // ------------------------------------------------------------- worldAxisNames
  
  Vector<casa::String> TimeFreqCoordinate::worldAxisNames () const
  {
    Vector<casa::String> names (2);

    names(0) = coordTime_p.worldAxisNames()(0);
    names(1) = coordFrequency_p.worldAxisNames()(0);

    return names;
  }

  // ------------------------------------------------------------- worldAxisUnits
  
  Vector<casa::String> TimeFreqCoordinate::worldAxisUnits () const
  {
    Vector<casa::String> units (2);

    units(0) = coordTime_p.worldAxisUnits()(0);
    units(1) = coordFrequency_p.worldAxisUnits()(0);

    return units;
  }

  // ------------------------------------------------------------- referencePixel
  
  Vector<double> TimeFreqCoordinate::referencePixel () const
  {
    Vector<double> refPixel (2);

    refPixel(0) = coordTime_p.referencePixel()(0);
    refPixel(1) = coordFrequency_p.referencePixel()(0);

    return refPixel;
  }

  // ------------------------------------------------------------- referenceValue

  Vector<double> TimeFreqCoordinate::referenceValue () const
  {
    Vector<double> refValue (2);

    refValue(0) = coordTime_p.referenceValue()(0);
    refValue(1) = coordFrequency_p.referenceValue()(0);

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

    csys.addCoordinate(coordTime_p);
    csys.addCoordinate(coordFrequency_p);
  }
  
} // Namespace CR -- end
