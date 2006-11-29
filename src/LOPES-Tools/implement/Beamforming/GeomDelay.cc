/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

/* $Id: GeomDelay.cc,v 1.1 2006/10/31 21:07:59 bahren Exp $*/

#include <Beamforming/GeomDelay.h>

/*!
  \class GeomDelay
*/

namespace LOPES {
  
  double GeomDelay::pi = 3.1415926535897932384626433832795;
  double GeomDelay::lightspeed = 299792458.0;
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ GeomDelay
  
  GeomDelay::GeomDelay ()
  {
    init ();
  }
  
  // ------------------------------------------------------------------ GeomDelay
  
  GeomDelay::GeomDelay (const Vector<double> &source,
			const Vector<double> &antenna)
  {
    init (source,
	  antenna);
  }
  
  // ------------------------------------------------------------------ GeomDelay
  
  GeomDelay::GeomDelay (const Vector<double> &source,
			const Vector<double> &antenna1,
			const Vector<double> &antenna2)
  {
    init (source,
	  antenna1,
	  antenna2);
  }
  
  // ------------------------------------------------------------------ GeomDelay
  
  GeomDelay::GeomDelay (GeomDelay const &other)
  {
    copy (other);
  }
  
  // ----------------------------------------------------------------------- init
  
  void GeomDelay::init ()
  {
    uint ndim (3);
    Vector<double> source   (ndim,0.0);
    Vector<double> antenna1 (ndim,0.0);
    Vector<double> antenna2 (ndim,0.0);
    
    source_p   = source;
    antenna1_p = antenna1;
    antenna2_p = antenna2;
    delay_p    = 0.0;
  }
  
  // ----------------------------------------------------------------------- init
  
  void GeomDelay::init (const Vector<double> &source,
			const Vector<double> &antenna)
  {
    uint ndim (3);
    Vector<double> antenna1 (ndim,0.0);
    
    init (source,
	  antenna1,
	  antenna);
  }
  
  // ----------------------------------------------------------------------- init
  
  void GeomDelay::init (const Vector<double> &source,
			const Vector<double> &antenna1,
			const Vector<double> &antenna2)
  {
    init ();
    
    setSource (source);
    setAntenna1 (antenna1);
    setAntenna2 (antenna2);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  GeomDelay::~GeomDelay ()
  {
    destroy();
  }
  
  void GeomDelay::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  GeomDelay& GeomDelay::operator= (GeomDelay const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void GeomDelay::copy (GeomDelay const &other)
  {
    source_p.resize(other.source_p.shape());
    antenna1_p.resize(other.antenna1_p.shape());
    antenna2_p.resize(other.antenna2_p.shape());
    
    source_p   = other.source_p;
    antenna1_p = other.antenna1_p;
    antenna2_p = other.antenna2_p;
    delay_p    = other.delay_p;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ setSource
  
  void GeomDelay::setSource (const Vector<double> &source)
  {
    if (source.nelements() == 3) {
      source_p.resize(source.shape());
      source_p = source;
      // recompute delay
      setDelay();
    } else {
      std::cerr << "[GeomDelay::setSource] Illegal length of vector"
		<< std::endl;
    }
  }
  
  // ---------------------------------------------------------------- setAntenna1
  
  void GeomDelay::setAntenna1 (const Vector<double> &antenna1)
  {
    if (antenna1.nelements() == 3) {
      antenna1_p.resize(antenna1.shape());
      antenna1_p = antenna1;
      // recompute delay
      setDelay();
    } else {
      std::cerr << "[GeomDelay::setAntenna1] Illegal length of vector"
		<< std::endl;
    }
  }
  
  // ---------------------------------------------------------------- setAntenna2
  
  void GeomDelay::setAntenna2 (const Vector<double> &antenna2)
  {
    if (antenna2.nelements() == 3) {
      antenna2_p.resize(antenna2.shape());
      antenna2_p = antenna2;
      // recompute delay
      setDelay();
    } else {
      std::cerr << "[GeomDelay::setAntenna2] Illegal length of vector"
		<< std::endl;
    }
  }
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ---------------------------------------------------------------------- delay
  
  double GeomDelay::delay (const Vector<double> &source,
			   const Vector<double> &antenna1,
			   const Vector<double> &antenna2)
  {
    Vector<double> baseline (antenna1.shape());
    Vector<double> xi       (antenna1.shape());
    Vector<double> eta      (antenna1.shape());
    
    baseline = antenna2 - antenna1;
    xi       = source - antenna1;
    eta      = xi - baseline;
    
    if (sum(baseline) == 0.0) {
      std::cerr << "[GeomDelay::delay] Baseline = " << baseline << std::endl;
      return 0.0;
    }
    else {
      return ((sqrt(sum(eta*eta)) - sqrt(sum(xi*xi)))/GeomDelay::lightspeed);
    }
  }
  
  // ------------------------------------------------------------------- setDelay
  
  bool GeomDelay::setDelay ()
  {
    bool status (true);
    
    try {
      delay_p = delay (source_p,
		       antenna1_p,
		       antenna2_p);
    } catch (AipsError x) {
      std::cerr << "[GeomDelay::setDelay] " << x.getMesg() << std::endl;
      delay_p = 0.0;
      status = false;
    }
    
    return status;
  }
  
}
