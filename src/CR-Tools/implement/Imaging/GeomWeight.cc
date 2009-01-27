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
      std::cout << "[GeomWeight::bufferWeights] Switching buffering OFF -> ON"
		<< std::endl;
      bufferWeights_p = bufferWeights;
      setWeights ();
    }
    // case 2: buffering of values switched off after previous enabled
    else if (bufferWeights_p && !bufferWeights) {
      std::cout << "[GeomWeight::bufferWeights] Switching buffering ON -> OFF"
		<< std::endl;
      bufferWeights_p = bufferWeights;
      Matrix<DComplex> mat;
      weights_p.resize(mat.shape());
      weights_p = mat;
    }
  }

  //_____________________________________________________________________________
  // weights

  Cube<DComplex> GeomWeight::weights ()
  {
    if (bufferWeights_p) {
      return weights_p;
    } else {
      if (bufferPhases_p) {
	return calcWeights (phases_p);
      } else {
	Cube<double> geomPhases = GeomPhase::phases();
	return calcWeights (geomPhases);
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
      /* Adjust the shape of the array storing the weights */
      casa::IPosition itsShape = shape();
      weights_p.resize(itsShape);
      /* Retrieve the values of the geometrical phases */
      if (bufferPhases_p) {
	weights_p = calcWeights(phases_p);
      } else {
	casa::Cube<double> geomPhases = GeomPhase::phases();
	weights_p = calcWeights(geomPhases);
      }
    }
  }
  
  //_____________________________________________________________________________
  //                                                                  calcWeights

  Cube<DComplex> GeomWeight::calcWeights (Matrix<double> const &delays,
					  Vector<double> const &frequencies)
  {
    Cube<double> p = GeomPhase::calcPhases(delays,frequencies);

    IPosition nelem = p.shape();
    Cube<DComplex> w (nelem);
    int i,j,k;
    
    for (k=0; k<nelem(2); k++) {
      for (j=0; j<nelem(1); j++) {
	for (i=0; i<nelem(0); i++) {
	  w(i,j,k) = DComplex(cos(p(i,j,k)),sin(p(i,j,k)));
	} 
      } 
    } 
    
    return w;
  }
  
  //_____________________________________________________________________________
  //                                                                  calcWeights

  Cube<DComplex> GeomWeight::calcWeights (Cube<double> const &phases)
  {
    IPosition nelem = phases.shape();
    Cube<DComplex> w (nelem);
    int i,j,k;
    
    for (k=0; k<nelem(2); k++) {
      for (j=0; j<nelem(1); j++) {
	for (i=0; i<nelem(0); i++) {
	  w(i,j,k) = DComplex(cos(phases(i,j,k)),sin(phases(i,j,k)));
	} 
      } 
    } 
    
    return w;
  }
  
  
} // Namespace CR -- end
