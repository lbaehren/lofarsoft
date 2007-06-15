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

#include <Imaging/GeometricalDelay.h>

const double lightspeed = 2.99795e08;

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ----------------------------------------------------------- GeometricalDelay
  
#ifdef HAVE_CASA
  GeometricalDelay::GeometricalDelay ()
    : bufferDelays_p (false)
  {
    casa::IPosition shape(2,1,3);
    antPositions_p.resize(shape);
    skyPositions_p.resize(shape);
    
    antPositions_p = 0.0;

    skyPositions_p      = 0.0;
    skyPositions_p(0,2) = 1.0;

    setDelays();
  }
#else 
#ifdef HAVE_BLITZ
  GeometricalDelay::GeometricalDelay ()
    : bufferDelays_p (false)
  {
    antPositions_p.resize(1,3);
    skyPositions_p.resize(1,3);
    
    antPositions_p = 0.0, 0.0, 0.0;
    skyPositions_p = 0.0, 0.0, 1.0;

    setDelays();
  }
#endif
#endif
  
  // ----------------------------------------------------------- GeometricalDelay
  
#ifdef HAVE_CASA
  GeometricalDelay::GeometricalDelay (casa::Matrix<double> const &antPositions,
				      casa::Matrix<double> const &skyPositions,
				      bool const &bufferDelays)
    : bufferDelays_p (false)
  {
    if (!setAntPositions (antPositions,false)) {
      std::cerr << "-- There was an error setting the ant positions" << std::endl;
      // use defaults
      antPositions_p.resize(1,3);
      antPositions_p = 0.0;
    }
    
    if (!setSkyPositions (skyPositions,false)) {
      std::cerr << "-- There was an error setting the sky positions" << std::endl;
      // use defaults
      skyPositions_p.resize(1,3);
      skyPositions_p      = 0.0;
      skyPositions_p(0,2) = 1.0;
    }
  
    // once all the input parameters have been stored we can compute the delays
    bufferDelays_p = bufferDelays;
    if (bufferDelays_p) {
      std::cout << "-- buffering geometrical delays..." << std::endl;
      setDelays();
    }
  }
#else
#ifdef HAVE_BLITZ
  GeometricalDelay::GeometricalDelay (blitz::Array<double,2> const &antPositions,
				      blitz::Array<double,2> const &skyPositions,
				      bool const &bufferDelays)
    : bufferDelays_p (false)
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
    bufferDelays_p = bufferDelays;
    if (bufferDelays_p) {
      std::cout << "-- buffering geometrical delays..." << std::endl;
      setDelays();
    }
  }
#endif
#endif
  
  // ----------------------------------------------------------- GeometricalDelay
  
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
  
  // ------------------------------------------------------------------ operator=

  GeometricalDelay& GeometricalDelay::operator= (GeometricalDelay const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy

  void GeometricalDelay::copy (GeometricalDelay const &other)
  {
    antPositions_p.resize (other.antPositions_p.shape());
    antPositions_p = other.antPositions_p;

    skyPositions_p.resize (other.skyPositions_p.shape());
    skyPositions_p = other.skyPositions_p;

    bufferDelays_p  = other.bufferDelays_p;

    if (bufferDelays_p) {
      delays_p.resize(other.delays_p.shape());
      delays_p        = other.delays_p;
    }
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  // ------------------------------------------------------------ setAntPositions
  
#ifdef HAVE_CASA
  bool GeometricalDelay::setAntPositions (const casa::Matrix<double> &antPositions,
					  const bool &bufferDelays)
  {
    bool status (true);

    // check the shape of the array
    if (antPositions.ncolumn() == 3) {
      // store the array
      antPositions_p.resize (antPositions.shape());
      antPositions_p = antPositions;
      // update the values of the geometrical delays
      if (bufferDelays) {
	bufferDelays_p = bufferDelays;
	setDelays();
      }
    } else {
      std::cerr << "Wrong number of array colums; must be 3!" << std::endl;
      status  = false;
    }
    
    return status;
  }
#else 
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setAntPositions (const blitz::Array<double,2> &antPositions,
					  const bool &bufferDelays)
  {
    bool status (true);

    // check the shape of the array
    if (antPositions.cols() == 3) {
      // store the array
      antPositions_p.resize (antPositions.shape());
      antPositions_p = antPositions;
      // update the values of the geometrical delays
      if (bufferDelays) {
	bufferDelays_p = bufferDelays;
	setDelays();
      }
    } else {
      std::cerr << "Wrong number of array colums; must be 3!" << std::endl;
      status  = false;
    }
    
    return status;
  }
#endif
#endif
  
  // ------------------------------------------------------------ setSkyPositions

#ifdef HAVE_CASA
  bool GeometricalDelay::setSkyPositions (const casa::Matrix<double> &skyPositions,
					  const bool &bufferDelays)
  {
    bool status (true);
    
    // check the shape of the array
    if (skyPositions.ncolumn() == 3) {
      // store the array
      skyPositions_p.resize (skyPositions.shape());
      skyPositions_p = skyPositions;
      // update the values of the geometrical delays
      if (bufferDelays) {
	bufferDelays_p = bufferDelays;
	setDelays();
      }
    } else {
      std::cerr << "Wrong number of array colums; must be 3!" << std::endl;
      status  = false;
    }
    
    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setSkyPositions (const blitz::Array<double,2> &skyPositions,
					  const bool &bufferDelays)
  {
    bool status (true);

    // check the shape of the array
    if (skyPositions.cols() == 3) {
      // store the array
      skyPositions_p.resize (skyPositions.shape());
      skyPositions_p = skyPositions;
      // update the values of the geometrical delays
      if (bufferDelays) {
	bufferDelays_p = bufferDelays;
	setDelays();
      }
    } else {
      std::cerr << "Wrong number of array colums; must be 3!" << std::endl;
      status  = false;
    }
    
    return status;
  }
#endif
#endif

  // ------------------------------------------------------------ setSkyPositions
  
#ifdef HAVE_CASA
  bool GeometricalDelay::setSkyPositions (casa::Matrix<double> const &skyPositions,
					  CR::CoordinateTypes const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    bool status (true);

    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setSkyPositions (blitz::Array<double,2> const &skyPositions,
					  CR::CoordinateTypes const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    bool status (true);
    
    return status;
  }
#endif
#endif
  
  // ------------------------------------------------------------ setSkyPositions
  
#ifdef HAVE_CASA
  bool GeometricalDelay::setSkyPositions (casa::Vector<double> const &xValues,
					  casa::Vector<double> const &yValues,
					  casa::Vector<double> const zValues,
					  CR::CoordinateTypes const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    bool status (true);

    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setSkyPositions (blitz::Array><double,1> const &xValues,
					  blitz::Array><double,1> const &yValues,
					  blitz::Array><double,1> const zValues,
					  CR::CoordinateTypes const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    bool status (true);

    return status;
  }
#endif
#endif

  // ---------------------------------------------------------------------- delay

#ifdef HAVE_CASA
  casa::Matrix<double> GeometricalDelay::delays ()
  {
    // if the delays are buffered internally, we just need to return the array
    if (bufferDelays_p) {
      std::cout << "-- returning buffered geometrical delays..." << std::endl;
      return delays_p;
    } else {
      return calcDelays();
    }
  }
#else
#ifdef HAVE_BLITZ
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
#endif
#endif

  // ------------------------------------------------------------------ setDelays

  void GeometricalDelay::setDelays ()
  {
    if (bufferDelays_p) {
      delays_p.resize (nofAntennaPositions(),nofSkyPositions());
      delays_p = calcDelays();
    }
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

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

  // ------------------------------------------------------------------ calcDelay

#ifdef HAVE_CASA
  double GeometricalDelay::calcDelay (double const &xSky,
				      double const &ySky,
				      double const &zSky,
				      double const &xAntenna,
				      double const &yAntenna,
				      double const &zAntenna)
  {
    double delay (0.0);
    casa::Vector<double> skyPosition(3);
    casa::Vector<double> diff (3);

    skyPosition(0) = xSky;
    skyPosition(1) = ySky;
    skyPosition(2) = zSky;

    diff(0) = xSky-xAntenna;
    diff(1) = ySky-yAntenna;
    diff(2) = zSky-zAntenna;
    
    delay = (CR::L2Norm(diff)-CR::L2Norm(skyPosition))/lightspeed;
    
    return delay;
  }
#else
#ifdef HAVE_BLITZ
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
    
    delay = (CR::L2Norm(diff)-CR::L2Norm(skyPosition))/lightspeed;
    
    return delay;
  }
#endif
#endif

  // ----------------------------------------------------------------- calcDelays

#ifdef HAVE_CASA
  casa::Matrix<double> GeometricalDelay::calcDelays ()
  {
    int nAnt (0);
    int nSky (0);
    int nofAnt (nofAntennaPositions());
    int nofSky (nofSkyPositions());
    casa::Vector<double> skyPos (3);
    casa::Vector<double> diff (3);
    casa::Matrix<double> delays (nofAnt,nofSky);

    // computation of the geometrical delays
    for (nSky=0; nSky<nofSky; nSky++) {
      skyPos = skyPositions_p.row(nSky);
      for (nAnt=0; nAnt<nofAnt; nAnt++) {
 	diff = skyPos - antPositions_p.row(nAnt);
 	delays(nAnt,nSky) = (L2Norm(diff)-L2Norm(skyPos))/lightspeed;
      }
    }
    
    return delays;
  }
#else
#ifdef HAVE_BLITZ
  blitz::Array<double,2> GeometricalDelay::calcDelays ()
  {
    int nAnt (0);
    int nSky (0);
    int nofAnt (nofAntennaPositions());
    int nofSky (nofSkyPositions());
    blitz::Array<double,1> skyPos (3);
    blitz::Array<double,1> diff (3);
    blitz::Array<double,2> delays (nofAnt,nofSky);

    // computation of the geometrical delays
    for (nSky=0; nSky<nofSky; nSky++) {
      skyPos = skyPositions_p(nSky,blitz::Range(blitz::Range::all()));
      for (nAnt=0; nAnt<nofAnt; nAnt++) {
	diff = skyPos-antPositions_p(nAnt,blitz::Range(blitz::Range::all()));
	delays(nAnt,nSky) = (CR::L2Norm(diff)-CR::L2Norm(skyPos))/lightspeed;
      }
    }
    
    return delays;
  }
#endif
#endif
  
} // NAMESPACE CR -- END
