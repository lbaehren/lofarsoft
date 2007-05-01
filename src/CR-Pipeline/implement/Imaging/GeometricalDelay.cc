/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

/* $Id: template-class.cc,v 1.10 2006/10/31 19:19:57 bahren Exp $*/

#include <Imaging/GeometricalDelay.h>

const double lightspeed = 2.99795e08;

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  GeometricalDelay::GeometricalDelay ()
  {
    antPositions_p.resize(1,3);
    skyPositions_p.resize(1,3);
    
    antPositions_p = 0.0, 0.0, 0.0;
    skyPositions_p = 0.0, 0.0, 1.0;
    bufferDelays_p  = true;

    setDelay();
  }
  
  GeometricalDelay::GeometricalDelay (const blitz::Array<double,2> &antPositions,
				      const blitz::Array<double,2> &skyPositions,
				      const bool &bufferDelay)
  {
    if (!setAntPositions (antPositions,false)) {
      std::cerr << "-- There was an error setting the ant positions" << std::endl;
      // use defaults
      antPositions_p.resize(1,3);
      antPositions_p = 0.0, 0.0, 0.0;
    }
    
    if (!setSkyPositions (skyPositions,false)) {
      std::cerr << "-- There was an error setting the sky positions" << std::endl;
      // use defaults
      skyPositions_p.resize(1,3);
      skyPositions_p = 0.0, 0.0, 1.0;
    }
  
    // once all the input parameters have been stored we can compute the delays
    bufferDelays_p = bufferDelay;
    if (bufferDelays_p) {
      std::cout << "-- buffering geometrical delays..." << std::endl;
      setDelay();
    }
  }
  
  GeometricalDelay::GeometricalDelay (GeometricalDelay const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  GeometricalDelay::~GeometricalDelay ()
  {
    destroy();
  }
  
  void GeometricalDelay::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  GeometricalDelay& GeometricalDelay::operator= (GeometricalDelay const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void GeometricalDelay::copy (GeometricalDelay const &other)
  {
    antPositions_p.resize (other.antPositions_p.shape());
    antPositions_p = other.antPositions_p;

    skyPositions_p.resize (other.skyPositions_p.shape());
    skyPositions_p = other.skyPositions_p;

    delays_p.resize(other.delays_p.shape());
    delays_p        = other.delays_p;

    bufferDelays_p  = other.bufferDelays_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  // ------------------------------------------------------------ setAntPositions

  bool GeometricalDelay::setAntPositions (const blitz::Array<double,2> &antPositions,
					  const bool &bufferDelay)
  {
    bool status (true);

    // check the shape of the array
    if (antPositions.cols() == 3) {
      // store the array
      antPositions_p.resize (antPositions.shape());
      antPositions_p = antPositions;
      // update the values of the geometrical delays
      if (bufferDelay) {
	setDelay();
      }
    } else {
      std::cerr << "Wrong number of array colums; must be 3!" << std::endl;
      status  = false;
    }
    
    return status;
  }
  
  // ------------------------------------------------------------ setSkyPositions

  bool GeometricalDelay::setSkyPositions (const blitz::Array<double,2> &skyPositions,
					  const bool &bufferDelay)
  {
    bool status (true);

    // check the shape of the array
    if (skyPositions.cols() == 3) {
      // store the array
      skyPositions_p.resize (skyPositions.shape());
      skyPositions_p = skyPositions;
      // update the values of the geometrical delays
      if (bufferDelay) {
	setDelay();
      }
    } else {
      std::cerr << "Wrong number of array colums; must be 3!" << std::endl;
      status  = false;
    }
    
    return status;
  }
  
  // ---------------------------------------------------------------------- delay

  blitz::Array<double,2> GeometricalDelay::delays ()
  {
    // if the delays are buffered internally, we just need to return the array
    if (bufferDelays_p) {
      std::cout << "-- returning buffered geometrical delays..." << std::endl;
      return delays_p;
    } else {
      return calcDelays();
    }
  }

  // ------------------------------------------------------------------- setDelay

  void GeometricalDelay::setDelay ()
  {
    delays_p.resize (nofAntennaPositions(),nofSkyPositions());
    delays_p = calcDelays();
  }

  // ------------------------------------------------------------------ calcDelay

  double GeometricalDelay::calcDelay (double const &xSky,
				      double const &ySky,
				      double const &zSky,
				      double const &xAntenna,
				      double const &yAntenna,
				      double const &zAntenna)
  {
    double delay (0.0);
    blitz::Array<double,1> skyPosition(3);
    blitz::Array<double,1> diff (3);

    skyPosition = xSky,ySky,zSky;
    diff        = xSky-xAntenna,ySky-yAntenna,zSky-zAntenna;
    
    delay = (L2Norm(diff)-L2Norm(skyPosition))/lightspeed;
    
    return delay;
  }

  // ----------------------------------------------------------------- calcDelays

  blitz::Array<double,2> GeometricalDelay::calcDelays ()
  {
    int nAnt (0);
    int nSky (0);
    int nofAnt (nofAntennaPositions());
    int nofSky (nofSkyPositions());
    blitz::Array<double,1> skyPos (3);
    blitz::Array<double,1> diff (3);
    blitz::Array<double,2> delays (nofAnt,nofSky);

    // control feedback
//     std::cout << "# antennas    = " << nofAnt << std::endl;
//     std::cout << "# positions   = " << nofSky << std::endl;
//     std::cout << "shape(delays) = " << delays.shape() << std::endl;

    // computation of the geometrical delays
    for (nSky=0; nSky<nofSky; nSky++) {
      skyPos = skyPositions_p(nSky,blitz::Range(blitz::Range::all()));
      for (nAnt=0; nAnt<nofAnt; nAnt++) {
	diff = skyPos-antPositions_p(nAnt,blitz::Range(blitz::Range::all()));
	delays(nAnt,nSky) = (L2Norm(diff)-L2Norm(skyPos))/lightspeed;
      }
    }
    
    return delays;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  void GeometricalDelay::summary ()
  {
    summary (std::cout);
  }

  void GeometricalDelay::summary (std::ostream &os)
  {
    os << "[GeometricalDelay] Summary of object" << std::endl;
    os << "-- Sky positions     : " << skyPositions_p.shape() << std::endl;
    os << "-- Antenna positions : " << antPositions_p.shape() << std::endl;
    os << "-- nof. Baselines    : " << nofBaselines()         << std::endl;
    os << "-- buffer delays     : " << bufferDelays_p         << std::endl;
    
    if (bufferDelays_p) {
      os << "-- Delays            : " << delays_p.shape()     << std::endl;
    }
  }

} // NAMESPACE CR -- END
