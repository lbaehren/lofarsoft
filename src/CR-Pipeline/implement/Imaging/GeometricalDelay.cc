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

/*!
  \class GeometricalDelay
*/

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  GeometricalDelay::GeometricalDelay ()
  {
    blitz::Array<double,2> tmp (1,3);
    blitz::Array<double,2> delay (1,1);

    tmp   = 0.0;
    delay = 0.0;

    antPositions_p = tmp;
    skyPositions_p = tmp;
    delays_p        = delay;
    bufferDelays_p  = true;
  }
  
  GeometricalDelay::GeometricalDelay (const blitz::Array<double,2> &antPositions,
				      const blitz::Array<double,2> &skyPositions,
				      const bool &bufferDelay)
  {
    antPositions_p = antPositions;
    skyPositions_p = skyPositions;
    // once all the input parameters have been stored we can compute the delays
    if (bufferDelays_p) {
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
    skyPositions_p = other.skyPositions_p;
    antPositions_p = other.antPositions_p;
    delays_p        = other.delays_p;
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

  blitz::Array<double,2> GeometricalDelay::delay ()
  {
    // if the delays are buffered internally, we just need to return the array
    if (bufferDelays_p) {
      return delays_p;
    } else {
      int nAnt (0);
      int nSky (0);
      int nofAnt (nofAntennaPositions());
      int nofSky (nofSkyPositions());
      blitz::Array<double,2> delays (nofAnt,nofSky);

      for (nAnt=0; nAnt<nofAnt; nAnt++) {
	for (nSky=0; nSky<nofSky; nSky++) {
	  delays(nAnt,nSky) = antPositions_p(0)*skyPositions_p(0);
	}
      }

      return delays;
    }
  }

  void GeometricalDelay::setDelay ()
  {
    delays_p = delay();
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  void GeometricalDelay::summary ()
  {
    std::cout << "Sky positions     : " << skyPositions_p.shape() << std::endl;
    std::cout << "Antenna positions : " << antPositions_p.shape() << std::endl;
    std::cout << "nof. Baselines    : " << nofBaselines()         << std::endl;
  }

} // NAMESPACE CR -- END
