/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include <Imaging/GeomWeight.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                   GeomWeight

  GeomWeight::GeomWeight (bool const &bufferWeights)
    : GeomPhase()
  {
    bufferWeights_p = bufferWeights;
    setWeights();
  }
  
  //_____________________________________________________________________________
  //                                                                   GeomWeight

  GeomWeight::GeomWeight (GeomPhase const &geomPhase,
			  bool const &bufferWeights)
    : GeomPhase(geomPhase)
  {
    bufferWeights_p = bufferWeights;
    setWeights();
  }

  //_____________________________________________________________________________
  //                                                                   GeomWeight

  GeomWeight::GeomWeight (GeomDelay const &geomDelay,
			  Vector<double> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeomPhase(geomDelay,
		frequencies,
		bufferPhases)
  {
    bufferWeights_p = bufferWeights;
    setWeights();
  }
  
  //_____________________________________________________________________________
  //                                                                   GeomWeight

  GeomWeight::GeomWeight (GeomDelay const &geomDelay,
			  Vector<MVFrequency> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeomPhase(geomDelay,
		frequencies,
		bufferPhases)
  {
    bufferWeights_p = bufferWeights;
    setWeights();
  }
  
  //_____________________________________________________________________________
  //                                                                   GeomWeight

  GeomWeight::GeomWeight (Matrix<double> const &antPositions,
			  CoordinateType::Types const &antCoord,
			  Matrix<double> const &skyPositions,
			  CoordinateType::Types const &skyCoord,
			  bool const &anglesInDegrees,
			  bool const &farField,
			  bool const &bufferDelays,
			  Vector<double> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeomPhase(antPositions,
		antCoord,
		skyPositions,
		skyCoord,
		anglesInDegrees,
		farField,
		bufferDelays,
		frequencies,
		bufferPhases)
  {
    bufferWeights_p = bufferWeights;
    setWeights();
  }

  //_____________________________________________________________________________
  //                                                                   GeomWeight

  GeomWeight::GeomWeight (Vector<MVPosition> const &antPositions,
			  Vector<MVPosition> const &skyPositions,
			  bool const &farField,
			  bool const &bufferDelays,
			  Vector<MVFrequency> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeomPhase(antPositions,
		skyPositions,
		farField,
		bufferDelays,
		frequencies,
		bufferPhases)
  {
    bufferWeights_p = bufferWeights;
    setWeights();
  }
  
  //_____________________________________________________________________________
  //                                                                   GeomWeight

  GeomWeight::GeomWeight (GeomWeight const &other)
    : GeomPhase(other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  GeomWeight::~GeomWeight ()
  {
    destroy();
  }
  
  void GeomWeight::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                    operator=

  GeomWeight& GeomWeight::operator= (GeomWeight const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  //_____________________________________________________________________________
  //                                                                         copy
  
  void GeomWeight::copy (GeomWeight const &other)
  {
    // copy the underlying base object
    GeomPhase::operator= (other);

    bufferWeights_p = other.bufferWeights_p;

    /* Only try copying the values of the geometrical weights in case buffering
       is selected.
    */
    if (bufferWeights_p) {
      weights_p.resize(other.weights_p.shape());
      weights_p = other.weights_p;
    }
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                bufferWeights

  void GeomWeight::bufferWeights (bool const &bufferWeights)
  {
    // case 1: buffering OFF -> ON
    if (!bufferWeights_p && bufferWeights) {
      bufferWeights_p = bufferWeights;
      setWeights ();
    }
    // case 2: buffering of values switched off after previous enabled
    else if (bufferWeights_p && !bufferWeights) {
      bufferWeights_p = bufferWeights;
      Matrix<DComplex> mat;
      weights_p.resize(mat.shape());
      weights_p = mat;
    }
  }

  //_____________________________________________________________________________
  //                                                                      weights

  Cube<DComplex> GeomWeight::weights ()
  {
    Cube<DComplex> w;

    weights (w);

    return w;
  }

  //_____________________________________________________________________________
  //                                                                      weights

  void GeomWeight::weights (Cube<DComplex> &w)
  {
    if (bufferWeights_p) {
      w.resize(weights_p.shape());
      w = weights_p;
    }
    else {
      if (bufferPhases_p) {
	calcWeights (w,phases_p);
      }
      else {
	Cube<double> p;
	GeomPhase::phases(p);
	calcWeights (w,p);
      } 
    }
  }
  
  //_____________________________________________________________________________
  //                                                                      summary
  
  void GeomWeight::summary (std::ostream &os)
  {
    os << "[GeomWeight] Summary of internal parameters." << std::endl;
    os << "-- Far-field delay        = " << GeomDelay::farField()  << std::endl;
    os << "-- Near-field delay       = " << GeomDelay::nearField() << std::endl;
    os << "-- Buffer delay values    = " << bufferDelays_p         << std::endl;
    os << "-- Shape of delays array  = " << GeomDelay::shape()     << std::endl;
    os << "-- Buffer phases values   = " << bufferPhases_p         << std::endl;
    os << "-- Shape of phases array  = " << GeomPhase::shape()     << std::endl;
    os << "                          = " << phases_p.shape()       << std::endl;
    os << "-- Buffer weights values  = " << bufferWeights_p        << std::endl;
    os << "-- Shape of weights array = " << shape()                << std::endl;
    os << "                          = " << weights_p.shape()      << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                    setPhases

  void GeomWeight::setDelays ()
  {
    GeomPhase::setDelays();
    setWeights();
  }
  
  //_____________________________________________________________________________
  //                                                                    setPhases

  void GeomWeight::setPhases ()
  {
    GeomPhase::setPhases();
    setWeights();
  }
  
  //_____________________________________________________________________________
  //                                                                   setWeights

  void GeomWeight::setWeights ()
  {
    /* Only recompute and set values if buffering is enabled */
    if (bufferWeights_p) {
      if (bufferPhases_p) {
	calcWeights(weights_p,
		    phases_p);
      } else {
	casa::Cube<double> geomPhases = GeomPhase::phases();
	calcWeights(weights_p,
		    geomPhases);
      }
    }
  }
  
  //_____________________________________________________________________________
  //                                                                  calcWeights

  Cube<DComplex> GeomWeight::calcWeights (Matrix<double> const &delays,
					  Vector<double> const &frequencies)
  {
    Cube<double> p;
    Cube<DComplex> w;

    GeomPhase::phases(p,delays,frequencies);
    calcWeights (w,p);
    
    return w;
  }
  
  //_____________________________________________________________________________
  //                                                                  calcWeights

  Cube<DComplex> GeomWeight::calcWeights (Cube<double> const &phases)
  {
    Cube<DComplex> w;

    calcWeights (w,phases);
    
    return w;
  }
  
  //_____________________________________________________________________________
  //                                                                  calcWeights

  void GeomWeight::calcWeights (Cube<DComplex> &w,
				Cube<double> const &phases)
  {
    IPosition nelem = phases.shape();
    int freq,ant,k;

    w.resize(nelem);
    
    for (k=0; k<nelem(2); k++) {
      for (ant=0; ant<nelem(1); ant++) {
	for (freq=0; freq<nelem(0); freq++) {
	  w(freq,ant,k) = DComplex(cos(phases(freq,ant,k)),sin(phases(freq,ant,k)));
	} 
      } 
    } 
  }
  
} // Namespace CR -- end
