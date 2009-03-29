/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#include <Imaging/GeomDelay.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction / Destruction
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                    GeomDelay
  
  GeomDelay::GeomDelay (Matrix<double> const &antPositions,
			Matrix<double> const &skyPositions,
			bool const &anglesInDegrees,
			bool const &farField,
			bool const &bufferDelays)
  {
    init (antPositions,
	  skyPositions,
	  anglesInDegrees,
	  farField,
	  bufferDelays);
  }
  
  //_____________________________________________________________________________
  //                                                                    GeomDelay
  
  GeomDelay::GeomDelay (Matrix<double> const &antPositions,
			CoordinateType::Types const &antCoord,
			Matrix<double> const &skyPositions,
			CoordinateType::Types const &skyCoord,
			bool const &anglesInDegrees,
			bool const &farField,
			bool const &bufferDelays)
  {
    init (antPositions,
	  antCoord,
	  skyPositions,
	  skyCoord,
	  anglesInDegrees,
	  farField,
	  bufferDelays);
  }
  
  //_____________________________________________________________________________
  //                                                                    GeomDelay
  
  GeomDelay::GeomDelay (Vector<MVPosition> const &antPositions,
			Vector<MVPosition> const &skyPositions,
			bool const &farField,
			bool const &bufferDelays)
  {
    init (antPositions,
	  skyPositions,
	  farField,
	  bufferDelays);
  }
  
  //_____________________________________________________________________________
  //                                                                    GeomDelay
  
  GeomDelay::GeomDelay (GeomDelay const &other)
  {
    copy (other);
  }
  
  //_____________________________________________________________________________
  //                                                                   ~GeomDelay
  
  GeomDelay::~GeomDelay ()
  {
    destroy();
  }
  
  //_____________________________________________________________________________
  //                                                                      destroy
  
  void GeomDelay::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                    operator=

  GeomDelay& GeomDelay::operator= (GeomDelay const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  //_____________________________________________________________________________
  //                                                                         copy

  void GeomDelay::copy (GeomDelay const &other)
  {
    farField_p     = other.farField_p;
    bufferDelays_p = other.bufferDelays_p;

    antPositions_p.resize(other.antPositions_p.shape());
    antPositions_p = other.antPositions_p;

    skyPositions_p.resize(other.skyPositions_p.shape());
    skyPositions_p = other.skyPositions_p;
    
    if (bufferDelays_p) {
      delays_p.resize(other.delays_p.shape());
      delays_p = other.delays_p;
    }
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                         init

  void GeomDelay::init ()
  {
    Matrix<double> antPos (1,3,0.0);
    Matrix<double> skyPos (1,3,0.0);

    /* Default sky position: [0,0,1] */
    skyPos(0,2) = 1.0;
    
    init (antPos,
	  skyPos,
	  false,
	  false);
  }
  
  //_____________________________________________________________________________
  //                                                                         init
  
  void GeomDelay::init (Matrix<double> const &antPositions,
			Matrix<double> const &skyPositions,
			bool const &anglesInDegrees,
			bool const &farField,
			bool const &bufferDelays)
  {
    init (antPositions,
	  CoordinateType::Cartesian,
	  skyPositions,
	  CoordinateType::Cartesian,
	  anglesInDegrees,
	  farField,
	  bufferDelays);
  }
  
  //_____________________________________________________________________________
  //                                                                         init
  
  void GeomDelay::init (Matrix<double> const &antPositions,
			CoordinateType::Types const &antCoord,
			Matrix<double> const &skyPositions,
			CoordinateType::Types const &skyCoord,
			bool const &anglesInDegrees,
			bool const &farField,
			bool const &bufferDelays)
  {
    // switch off buffering
    bufferDelays_p = false;

    // store antenna and sky positions
    setAntPositions (antPositions,
		     antCoord,
		     anglesInDegrees);
    setSkyPositions (skyPositions,
		     skyCoord,
		     anglesInDegrees);

    // set control parameters
    farField_p     = farField;
    bufferDelays_p = bufferDelays;

    setDelays();
  }
  
  //_____________________________________________________________________________
  //                                                                         init
  
  void GeomDelay::init (Vector<MVPosition> const &antPositions,
			Vector<MVPosition> const &skyPositions,
			bool const &farField,
			bool const &bufferDelays)
  {
    // switch off buffering
    bufferDelays_p = false;

    // store antenna and sky positions
    setAntPositions (antPositions);
    setSkyPositions (skyPositions);

    // set control parameters
    farField_p     = farField;
    bufferDelays_p = bufferDelays;

    setDelays();
  }

  //_____________________________________________________________________________
  //                                                                 bufferDelays
  
  void GeomDelay::bufferDelays (bool const &bufferDelays)
  {
    // case 1: buffering previously disabled, not switched on
    if (!bufferDelays_p && bufferDelays) {
      bufferDelays_p = bufferDelays;
      setDelays();
    }
    // case 2: buffering of values switched off after previous enabled
    else if (bufferDelays_p && !bufferDelays) {
      bufferDelays_p = bufferDelays;
      Matrix<double> mat;
      delays_p.resize(mat.shape());
      delays_p = mat;
    }
  }
  
  //_____________________________________________________________________________
  //                                                              setAntPositions
  
  bool GeomDelay::setAntPositions (Matrix<double> const &antPositions,
				   CoordinateType::Types const &type,
				   bool const &anglesInDegrees)
  {
    bool status     = true;
    IPosition antShape = antPositions.shape();
    
    antPositions_p.resize (antShape);
    
    switch (type) {
    case CoordinateType::Cartesian:
      antPositions_p = antPositions;
      break;
    default:
      {
	Vector<double> out (3);
	for (int n(0); n<antShape(0); n++) {
	  status = PositionVector::convert(out,
					   CoordinateType::Cartesian,
					   antPositions.row(n),
					   type,
					   anglesInDegrees);
	  antPositions_p.row(n) = out;
	}
      }
      break;
    };
    
    // Update the values of the delays
    setDelays();

    return status;
  }

  //_____________________________________________________________________________
  //                                                              setAntPositions

  bool GeomDelay::setAntPositions (Vector<MVPosition> const &antPositions)
  {
    bool status (true);
    uint nelem = antPositions.nelements();

    antPositions_p.resize (nelem,3);

    for (uint n(0); n<nelem; n++) {
      antPositions_p.row(n) = antPositions(n).getValue();
    }

    // Update the values of the delays
    setDelays();

    return status;
  }

  //_____________________________________________________________________________
  //                                                              setSkyPositions
  
  bool GeomDelay::setSkyPositions (Matrix<double> const &skyPositions,
				   CoordinateType::Types const &type,
				   bool const &anglesInDegrees)
  {
    bool status     = true;
    IPosition antShape = skyPositions.shape();

    /* Check the shape of the provided input array */
    if (antShape(1) != 3) {
      std::cerr << "[GeomDelay::setSkyPositions] Incorrect shape of input array: "
		<< antShape
		<< "Expected array of shape [nofAnt,3]"
		<< std::endl;
      return false;
    }
    
    skyPositions_p.resize (antShape);
    
    switch (type) {
    case CoordinateType::Cartesian:
      skyPositions_p = skyPositions;
      break;
    default:
      {
	Vector<double> out (3);
	for (int n(0); n<antShape(0); n++) {
	  status = PositionVector::convert(out,
					   CoordinateType::Cartesian,
					   skyPositions.row(n),
					   type,
					   anglesInDegrees);
	  skyPositions_p.row(n) = out;
	}
      }
      break;
    };
    
    // Update the values of the delays
    setDelays();

    return status;
  }

  //_____________________________________________________________________________
  //                                                              setSkyPositions

  bool GeomDelay::setSkyPositions (Vector<double> const &skyPositions,
				   CoordinateType::Types const &type,
				   bool const &anglesInDegrees)
  {
    if (skyPositions.nelements() == 3) {
      Matrix<double> mat (1,3);
      mat.row(0) = skyPositions;
      // forward the function call
      return setSkyPositions (mat,
			      type,
			      anglesInDegrees);
    } else {
      std::cerr << "[GeomDelay::setSkyPositions] Incorrect shape of input vector: "
		<< skyPositions.shape()
		<< std::endl;
      return false;
    }
  }
  
  //_____________________________________________________________________________
  //                                                              setSkyPositions

  bool GeomDelay::setSkyPositions (Vector<MVPosition> const &skyPositions)
  {
    bool status (true);
    uint nelem = skyPositions.nelements();

    skyPositions_p.resize (nelem,3);

    for (uint n(0); n<nelem; n++) {
      skyPositions_p.row(n) = skyPositions(n).getValue();
    }

    // Update the values of the delays
    setDelays();

    return status;
  }

  //_____________________________________________________________________________
  //                                                                     farField
  
  void GeomDelay::farField (bool const &farField)
  {
    // check if we need to adjust the buffer for the values of the delays
    if (farField_p != farField) {
      setDelays();
    }
    
    // store the value of the parameter
    farField_p = farField;
  }
  
  //_____________________________________________________________________________
  //                                                                      summary
  
  void GeomDelay::summary (std::ostream &os)
  {
    os << "[GeomDelay] Summary of internal parameters." << std::endl;
    os << "-- Far-field delay        = " << farField()         << std::endl;
    os << "-- Near-field delay       = " << nearField()        << std::endl;
    os << "-- Buffer delay values    = " << bufferDelays_p     << std::endl;
    os << "-- nof. antenna positions = " << nofAntPositions()  << std::endl;
    os << "-- nof. sky positions     = " << nofSkyPositions()  << std::endl;
    os << "-- Shape of delays array  = " << shape()            << std::endl;
    os << "                          = " << delays_p.shape()   << std::endl;
  }
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                    setDelays
  
  void GeomDelay::setDelays ()
  {
    /* Only recompute and set values if buffering is enabled */
    if (bufferDelays_p) {
      casa::IPosition itsShape = shape();
      delays_p.resize(itsShape);
      
      delays (delays_p,
	      antPositions_p,
	      skyPositions_p,
	      farField_p);
    }
  }
  
  //_____________________________________________________________________________
  //                                                                       delays
  
  Matrix<double> GeomDelay::delays ()
  {
    if (bufferDelays_p) {
      return delays_p;
    } else {
      return delays (antPositions_p,
		     skyPositions_p,
		     farField_p);
    }
  }
  
  //_____________________________________________________________________________
  //                                                                       delays
  
  void GeomDelay::delays (Matrix<double> &d)
  {
    if (bufferDelays_p) {
      d.resize(delays_p.shape());
      d = delays_p;
    } else {
      delays (d,
	      antPositions_p,
	      skyPositions_p,
	      farField_p);
    }
  }
  
  //_____________________________________________________________________________
  //                                                                        delay
  
  double GeomDelay::delays (std::vector<double> const &antPosition,
			    std::vector<double> const &skyPosition,
			    bool const &farField)
  {
    double delay = 0;
    
    if (farField) {
      std::vector<double> direction (3);
      CR::normalize (direction,skyPosition);
      delay = -(direction[0]*antPosition[0]
		+direction[1]*antPosition[1]
		+direction[2]*antPosition[2])/lightspeed;
    }
    else {
      std::vector<double> diff (3);
      diff[0] = skyPosition[0]-antPosition[0];
      diff[1] = skyPosition[1]-antPosition[1];
      diff[2] = skyPosition[2]-antPosition[2];
      delay = (CR::L2Norm(diff)-CR::L2Norm(skyPosition))/lightspeed;
    }
    
    return delay;
  }

  //_____________________________________________________________________________
  //                                                                        delay
  
  double GeomDelay::delays (casa::Vector<double> const &antPosition,
			    casa::Vector<double> const &skyPosition,
			    bool const &farField)  
  {
    double delay = 0;
    
    if (farField) {
      casa::Vector<double> direction (3);
      CR::normalize (direction,skyPosition);
      delay = -(direction(0)*antPosition(0)
		+direction(1)*antPosition(1)
		+direction(2)*antPosition(2))/lightspeed;
    }
    else {
      casa::Vector<double> diff = skyPosition-antPosition;
      delay = (CR::L2Norm(diff)-CR::L2Norm(skyPosition))/lightspeed;
    }
    
    return delay;
  }

  //_____________________________________________________________________________
  //                                                                        delay
  
  Matrix<double> GeomDelay::delays (casa::Matrix<double> const &antPositions,
				    casa::Matrix<double> const &skyPositions,
				    bool const &farField)
  {
    unsigned int nofAnt = antPositions.nrow();
    unsigned int nofSky = skyPositions.nrow();
    Matrix<double> d (nofAnt,nofSky);
    
    for (unsigned int ant(0); ant<nofAnt; ant++) {
      for (unsigned int sky(0); sky<nofSky; sky++) {
	d(ant,sky) = delays (antPositions.row(ant),
			     skyPositions.row(sky),
			     farField);
      }
    }
    
    return d;
  }

  //_____________________________________________________________________________
  //                                                                        delay
  
  void GeomDelay::delays (Matrix<double> &d,
			  casa::Matrix<double> const &antPositions,
			  casa::Matrix<double> const &skyPositions,
			  bool const &farField)
  {
    unsigned int nofAnt = antPositions.nrow();
    unsigned int nofSky = skyPositions.nrow();
    
    d.resize(nofAnt,nofSky);
    
    for (unsigned int ant(0); ant<nofAnt; ant++) {
      for (unsigned int sky(0); sky<nofSky; sky++) {
	d(ant,sky) = delays (antPositions.row(ant),
			     skyPositions.row(sky),
			     farField);
      }
    }
  }
  
} // Namespace CR -- end
