/*-------------------------------------------------------------------------*
 | $Id:: NewClass.cc 1964 2008-09-06 17:52:38Z baehren                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#include <Coordinates/SkymapCoordinate.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //____________________________________________________________ SkymapCoordinate

  SkymapCoordinate::SkymapCoordinate ()
  {
    init();
  }
  
  //____________________________________________________________ SkymapCoordinate

  SkymapCoordinate::SkymapCoordinate (ObservationData const &obsData,
				      TimeFreqCoordinate const &timeFreqCoord,
				      SkymapQuantity::Type const &quantity)
  {
    SpatialCoordinate spatialCoord = SpatialCoordinate();

    init (obsData,
	  spatialCoord,
	  timeFreqCoord,
	  quantity);
  }

  //____________________________________________________________ SkymapCoordinate
  
  SkymapCoordinate::SkymapCoordinate (ObservationData const &obsData,
				      SpatialCoordinate const &spatialCoord,
				      TimeFreqCoordinate const &timeFreqCoord,
				      SkymapQuantity::Type const &quantity)
  {
    init (obsData,
	  spatialCoord,
	  timeFreqCoord,
	  quantity);
  }
  
  //____________________________________________________________ SkymapCoordinate

  SkymapCoordinate::SkymapCoordinate (SkymapCoordinate const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  SkymapCoordinate::~SkymapCoordinate ()
  {
    destroy();
  }
  
  void SkymapCoordinate::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  SkymapCoordinate& SkymapCoordinate::operator= (SkymapCoordinate const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  //________________________________________________________________________ copy
  
  void SkymapCoordinate::copy (SkymapCoordinate const &other)
  {
    obsData_p        = other.obsData_p;
    skymapQuantity_p = other.skymapQuantity_p;
    spatialCoord_p   = other.spatialCoord_p;
    timeFreqCoord_p  = other.timeFreqCoord_p;
    csys_p           = other.csys_p;
    nofAxes_p        = other.nofAxes_p;
    shape_p          = other.shape_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  //__________________________________________________________ setObservationData

  bool SkymapCoordinate::setObservationData (ObservationData const &obsData)
  {
    bool status (true);

    // store the input data
    obsData_p = obsData;

    // update coordinate system object 
    setCoordinateSystem ();

    return status;
  }

  //___________________________________________________________ setSkymapQuantity

  bool SkymapCoordinate::setSkymapQuantity (SkymapQuantity const &skymapQuantity,
					    bool const &adjustSettings)
  {
    bool status (true);

    status = timeFreqCoord_p.setCoordType(skymapQuantity.coordinateType(),
					  adjustSettings);

    if (status) {
      skymapQuantity_p = skymapQuantity;
    } else {
      std::cerr << "[SkymapCoordinate::setSkymapQuantity]"
		<< " New skymap quantity rejected due to error in"
		<< " time-frequency coordinate!"
		<< std::endl;
    }

    // update coordinate system object 
    setCoordinateSystem ();
    
    return status;
  }
  
  //________________________________________________________ setSpatialCoordinate
  
  bool SkymapCoordinate::setSpatialCoordinate (SpatialCoordinate const &coord)
  {
    bool status (true);

    // store the input data
    spatialCoord_p = coord;

    // update coordinate system object 
    setCoordinateSystem ();

    return status;
  }

  //_______________________________________________________ setTimeFreqCoordinate
  
  bool SkymapCoordinate::setTimeFreqCoordinate (TimeFreqCoordinate const &coord)
  {
    bool status (true);

    // store the input data
    timeFreqCoord_p = coord;
    
    // update coordinate system object 
    setCoordinateSystem ();

    return status;
  }

  //_______________________________________________________________________ shape

  void SkymapCoordinate::setShape ()
  {
    uint counter (0);
    casa::IPosition shapeSkymap (nofAxes());
    casa::IPosition shapeSpatial  = spatialCoord_p.shape();
    casa::IPosition shapeTimeFreq = timeFreqCoord_p.shape();

    for (uint n(0); n<shapeSpatial.nelements(); n++) {
      shapeSkymap(counter) = shapeSpatial(n);
      counter++;
    }

    for (uint n(0); n<shapeTimeFreq.nelements(); n++) {
      shapeSkymap(counter) = shapeTimeFreq(n);
      counter++;
    }

    shape_p = shapeSkymap;
  }

  //_____________________________________________________________________ summary
  
  void SkymapCoordinate::summary (std::ostream &os)
  {
    os << "[SkymapCoordinate] Summary of internal parameters." << std::endl;
    os << "-- Skymap quantity name    = " << skymapQuantity_p.name() << std::endl;
    os << "-- nof. coordinate objects = " << nofCoordinates()        << std::endl;
    os << "-- nof. coordinate axes    = " << nofAxes()               << std::endl;
    os << "-- Shape of the axes       = " << shape()                 << std::endl;
    os << "-- World axis names        = " << csys_p.worldAxisNames() << std::endl;
    os << "-- World axis units        = " << csys_p.worldAxisUnits() << std::endl;
    os << "-- Reference pixel (CRPIX) = " << csys_p.referencePixel() << std::endl;
    os << "-- Increment       (CDELT) = " << csys_p.increment()      << std::endl;
    os << "-- Reference value (CRVAL) = " << csys_p.referenceValue() << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //________________________________________________________________________ init
  
  void SkymapCoordinate::init ()
  {
    ObservationData obsData          = ObservationData();
    SpatialCoordinate spatialCoord   = SpatialCoordinate ();
    TimeFreqCoordinate timeFreqCoord = TimeFreqCoordinate();
    
    init (obsData,
	  spatialCoord,
	  timeFreqCoord,
	  SkymapQuantity::FREQ_POWER);
  }
  
  //________________________________________________________________________ init
  
  void SkymapCoordinate::init (ObservationData const &obsData,
			       SpatialCoordinate const &spatialCoord,
			       TimeFreqCoordinate const &timeFreqCoord,
			       SkymapQuantity::Type const &quantity)
  {
    obsData_p        = obsData;
    spatialCoord_p   = spatialCoord;
    timeFreqCoord_p  = timeFreqCoord;
    
    setSkymapQuantity(quantity);

    setCoordinateSystem ();
  }
  
  //_________________________________________________________ setCoordinateSystem
  
  void SkymapCoordinate::setCoordinateSystem ()
  {
    casa::CoordinateSystem csys;
    
    csys.setObsInfo (obsData_p.obsInfo());

    /* Check the ordering of time and frequency axis */
    if (!timeFreqCoord_p.timeAxisLast()) {
      timeFreqCoord_p.setTimeAxisLast(true);
    }

    spatialCoord_p.toCoordinateSystem(csys);
    timeFreqCoord_p.toCoordinateSystem(csys);
    
    csys_p = csys;

    /* Set additional internal variables which might need updating */

    nofAxes_p = spatialCoord_p.nofAxes()+timeFreqCoord_p.nofAxes();

    setShape();
  }

  //_____________________________________________________________ worldAxisValues

  casa::Matrix<double> SkymapCoordinate::worldAxisValues (bool const &fastedAxisFirst)
  {
    // determine the number of positions
    uint nofPoints (1);
    for (uint n(0); n<nofAxes_p; n++) {
      nofPoints *= shape_p(n);
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
    
    /* Set up the arrays for the conversion and the returned values */
    casa::Matrix<double> worldValues (nofPoints,nofAxes_p);
    casa::IPosition pos(nofAxes_p,0);
    casa::Vector<double> pixel (nofAxes_p,0);
    casa::Vector<double> world (nofAxes_p,0);
    uint npos (0);
    
    if (nofAxes_p>5) {
      std::cerr << "[SkymapCoordinate::worldAxisValues]"
		<< " Exceeded number of supported coordinate axes!"
		<< std::endl;
    } else {
      for (pos(4)=0; pos(4)<shape_p(axis(4)); pos(4)++) {
	pixel(axis(4)) = pos(4);
	for (pos(3)=0; pos(3)<shape_p(axis(3)); pos(3)++) {
	  pixel(axis(3)) = pos(3);
	  for (pos(2)=0; pos(2)<shape_p(axis(2)); pos(2)++) {
	    pixel(axis(2)) = pos(2);
	    for (pos(1)=0; pos(1)<shape_p(axis(1)); pos(1)++) {
	      pixel(axis(1)) = pos(1);
	      for (pos(0)=0; pos(0)<shape_p(axis(0)); pos(0)++) {
		pixel(axis(0)) = pos(0);
		// conversion from pixel to world coordinates
		csys_p.toWorld (world,pixel);
		// copy result to returned array
		worldValues.row(npos) = world;
		// increment counter
		npos++;
	      }
	    }
	  }
	}
      }
    }
    
    return worldValues;
  }

} // Namespace CR -- end
