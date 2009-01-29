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

#include <Imaging/GeomPhase.h>

namespace CR { // Namespace CR -- begin
  
  //_____________________________________________________________________________
  //                                                                    GeomPhase
  
  GeomPhase::GeomPhase ()
    : GeomDelay()
  {
    Vector<double> freq (1,0.0);
    
    init (freq,
	  false);
  }
  
  //_____________________________________________________________________________
  //                                                                    GeomPhase

  GeomPhase::GeomPhase (Vector<double> const &frequencies,
			bool const &bufferPhases)
    : GeomDelay()
  {
    init (frequencies,
	  bufferPhases);
  }
  
  GeomPhase::GeomPhase (Vector<MVFrequency> const &frequencies,
			bool const &bufferPhases)
    : GeomDelay()
  {
    init (frequencies,
	  bufferPhases);
  }
  
  //_____________________________________________________________________________
  //                                                                    GeomPhase
  
  GeomPhase::GeomPhase (GeomDelay const &geomDelay,
			Vector<double> const &frequencies,
			bool const &bufferPhases)
    : GeomDelay(geomDelay)
  {
    init (frequencies,
	  bufferPhases);
  }
  
  GeomPhase::GeomPhase (GeomDelay const &geomDelay,
			Vector<MVFrequency> const &frequencies,
			bool const &bufferPhases)
    : GeomDelay(geomDelay)
  {
    init (frequencies,
	  bufferPhases);
  }
  
  //_____________________________________________________________________________
  //                                                                    GeomPhase

  GeomPhase::GeomPhase (Matrix<double> const &antPositions,
			CoordinateType::Types const &antCoord,
			Matrix<double> const &skyPositions,
			CoordinateType::Types const &skyCoord,
			bool const &anglesInDegrees,
			bool const &farField,
			bool const &bufferDelays,
			Vector<double> const &frequencies,
			bool const &bufferPhases)
    : GeomDelay (antPositions,
		 antCoord,
		 skyPositions,
		 skyCoord,
		 anglesInDegrees,
		 farField,
		 bufferDelays)
  {
    init (frequencies,
	  bufferPhases);
  }
  
  GeomPhase::GeomPhase (Vector<MVPosition> const &antPositions,
			Vector<MVPosition> const &skyPositions,
			bool const &farField,
			bool const &bufferDelays,
			Vector<MVFrequency> const &frequencies,
			bool const &bufferPhases)
    : GeomDelay (antPositions,
		 skyPositions,
		 farField,
		 bufferDelays)
  {
    init (frequencies,
	  bufferPhases);
  }
  
  //_____________________________________________________________________________
  //                                                                    GeomPhase
  
  GeomPhase::GeomPhase (GeomPhase const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  GeomPhase::~GeomPhase ()
  {
    destroy();
  }
  
  void GeomPhase::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  GeomPhase& GeomPhase::operator= (GeomPhase const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }

  //_____________________________________________________________________________
  //                                                                         copy
  
  void GeomPhase::copy (GeomPhase const &other)
  {
    // copy the underlying base object
    GeomDelay::operator= (other);

    bufferPhases_p = other.bufferPhases_p;

    frequencies_p.resize(other.frequencies_p.shape());
    frequencies_p = other.frequencies_p;
    
    /* Only try copying the values of the geometrical phases in case buffering
       is selected.
    */
    if (bufferPhases_p) {
      phases_p.resize(other.phases_p.shape());
      phases_p = other.phases_p;
    }
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                 bufferPhases

  void GeomPhase::bufferPhases (bool const &bufferPhases)
  {
    // case 1: buffering OFF -> ON
    if (!bufferPhases_p && bufferPhases) {
      bufferPhases_p = bufferPhases;
      setPhases ();
    }
    // case 2: buffering of values switched off after previous enabled
    else if (bufferPhases_p && !bufferPhases) {
      bufferPhases_p = bufferPhases;
      Matrix<double> mat;
      phases_p.resize(mat.shape());
      phases_p = mat;
    }
  }

  //_____________________________________________________________________________
  //                                                               setFrequencies

  bool GeomPhase::setFrequencies (Vector<double> const &frequencies)
  {
    bool status (true);
    
    try {
      frequencies_p.resize(frequencies.shape());
      frequencies_p = frequencies;
    } catch (std::string message) {
      std::cerr << "[GeomPhase::setFrequencies] " << message << std::endl;
      status = false;
    }

    setPhases();

    return status;
  }

  bool GeomPhase::setFrequencies (Vector<MVFrequency> const &frequencies)
  {
    bool status (true);
    uint nelem (frequencies.nelements());
    
    try {
      frequencies_p.resize(nelem);
      for (uint n(0); n<nelem; n++) {
	frequencies_p(n) = frequencies(n).getValue();
      }
    } catch (std::string message) {
      std::cerr << "[GeomPhase::setFrequencies] " << message << std::endl;
      status = false;
    }
    
    setPhases();
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                                      summary

  void GeomPhase::summary (std::ostream &os)
  {
    os << "[GeomPhase] Summary of internal parameters." << std::endl;
    os << "-- Far-field delay        = " << GeomDelay::farField()  << std::endl;
    os << "-- Near-field delay       = " << GeomDelay::nearField() << std::endl;
    os << "-- Buffer delay values    = " << bufferDelays_p         << std::endl;
    os << "-- Shape of delays array  = " << GeomDelay::shape()     << std::endl;
    os << "-- Buffer phases values   = " << bufferPhases_p         << std::endl;
    os << "-- Shape of phases array  = " << shape()                << std::endl;
    os << "                          = " << phases_p.shape()       << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                         init

  void GeomPhase::init (Vector<double> const &frequencies,
			bool const &bufferPhases)
  {
    bufferPhases_p = bufferPhases;
    
    setFrequencies(frequencies);
  }

  //_____________________________________________________________________________
  //                                                                         init

  void GeomPhase::init (Vector<MVFrequency> const &frequencies,
			bool const &bufferPhases)
  {
    bufferPhases_p = bufferPhases;

    setFrequencies(frequencies);
  }

  //_____________________________________________________________________________
  //                                                                        shape
  
  casa::IPosition GeomPhase::shape()
  {
    casa::IPosition shapeDelays = GeomDelay::shape();
    
    return casa::IPosition (3,
			    frequencies_p.nelements(),
			    shapeDelays(0),
			    shapeDelays(1));
  }

  //_____________________________________________________________________________
  //                                                                        phase
  
  casa::Cube<double> GeomPhase::calcPhases (Matrix<double> const &delay,
					    Vector<double> const &freq)
  {
    uint nofFreq = freq.nelements();
    uint nofAnt  = delay.nrow();
    uint nofSky  = delay.ncolumn();
    
    Cube<double> values (nofFreq,
			 nofAnt,
			 nofSky);
    
#ifdef HAVE_CASACORE
    for (uint n(0); n<nofFreq; n++) {
      values.yzPlane(n) = calcPhases(delay,freq(n));
    }
#else
    uint nFreq (0);
    uint nAnt (0);
    uint nSky(0);

    for (nSky=0; nSky<nofSky; nSky++) {
      for (nAnt=0; nAnt<nofAnt; nAnt++) {
	for (nFreq=0; nFreq<nofFreq; nFreq++) {
	  values(nFreq,nAnt,nSky) = calcPhases(delay(nAnt,nSky),freq(nFreq));
	}
      }
    }
#endif
    
    return values;
  }
  
  //_____________________________________________________________________________
  //                                                                       phases
  
  casa::Cube<double> GeomPhase::phases ()
  {
    if (bufferPhases_p) {
      /* If the values of the geometrical phases are buffered, all we need to
	 do is return the array string them. */
      return phases_p;
    } else {
      /* If we need to compute the geometrical phases, check if at least the
	 values of the delays are buffered. */
      if (bufferDelays_p) {
	return phases(delays_p);
      } else {
	casa::Matrix<double> geomDelays = GeomDelay::delays();
	return phases(geomDelays);
      }
    }
  }

  //_____________________________________________________________________________
  //                                                                       phases

  casa::Cube<double> GeomPhase::phases (Matrix<double> const &delays)
  {
    casa::IPosition phasesShape = shape();
    casa::Cube<double> values (phasesShape);
    
#ifdef HAVE_CASACORE
    for (int freq(0); freq<phasesShape(0); freq++) {
      values.yzPlane (freq) = calcPhases (delays,
					  frequencies_p(freq));
    }
#else
    uint nofAnt = delay.nrow();
    uint nofSky = delay.ncolumn();
    uint nFreq  = 0;
    uint nAnt   = 0;
    uint nSky   = 0;

    for (nSky=0; nSky<nofSky; nSky++) {
      for (nAnt=0; nAnt<nofAnt; nAnt++) {
	for (nFreq=0; nFreq<phasesShape(0); nFreq++) {
	  values(nFreq,nAnt,nSky) = calcPhases(delay(nAnt,nSky),freq(nFreq));
	}
      }
    }
#endif
    
    return values;
  }

  //_____________________________________________________________________________
  //                                                                    setDelays

  void GeomPhase::setDelays ()
  {
    GeomDelay::setDelays();
    setPhases();
  }
  
  //_____________________________________________________________________________
  //                                                                    setPhases

  void GeomPhase::setPhases ()
  {
    /* Only recompute and set values if buffering is enabled */
    if (bufferPhases_p) {
      casa::IPosition itsShape = shape();
      phases_p.resize();
      if (bufferDelays_p) {
	phases_p = phases(delays_p);
      } else {
	casa::Matrix<double> geomDelays = GeomDelay::delays();
	phases_p = phases(geomDelays);
      }
    }
  }
  
} // Namespace CR -- end
