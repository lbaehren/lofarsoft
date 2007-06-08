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

#include <Imaging/GeometricalWeight.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  GeometricalWeight::GeometricalWeight ()
    : GeometricalPhase ()
  {;}
  
  GeometricalWeight::GeometricalWeight (GeometricalWeight const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  GeometricalWeight::~GeometricalWeight ()
  {
    destroy();
  }
  
  void GeometricalWeight::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  GeometricalWeight& GeometricalWeight::operator= (GeometricalWeight const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void GeometricalWeight::copy (GeometricalWeight const &other)
  {
    // copy the underlying base object
    GeometricalPhase::operator= (other);

    bufferWeights_p = other.bufferWeights_p;

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
  
#ifdef HAVE_CASA
  casa::Cube<DComplex> GeometricalWeight::weights ()
  {
    if (bufferWeights_p) {
      std::cout << "-- Returning buffered weights..." << std::endl;
      return weights_p;
    } else {
      std::cout << "-- Returning recomputed weights..." << std::endl;
      return calcWeights();
    }
  }
#else
#ifdef HAVE_BLITZ
  blitz::Array<complex<double>,3> GeometricalWeight::weights ()
  {
    if (bufferWeights_p) {
      std::cout << "-- Returning buffered weights..." << std::endl;
      return weights_p;
    } else {
      std::cout << "-- Returning recomputed weights..." << std::endl;
      return calcWeights();
    }
  }
#endif
#endif  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
#ifdef HAVE_CASA
  casa::Cube<DComplex> GeometricalWeight::calcWeights ()
  {
    // get the geometrical phases
    casa::Cube<double> phases = GeometricalPhase::phases();
    casa::IPosition shape     = phases.shape();
    // array to store the computed weights
    casa::Cube<DComplex> weights (shape);
    
    return weights;
  }
#else
#ifdef HAVE_BLITZ
  blitz::Array<complex<double>,3> GeometricalWeight::calcWeights ()
  {
    // get the geometrical phases
    blitz::Array<double,3> phases = GeometricalPhase::phases();
    //     casa::IPosition shape     = phases.shape();
    // array to store the computed weights
    blitz::Array<complex<double>,3> weights (phases.shape());

    return weights;
  }
#endif
#endif
  
  
} // NAMESPACE CR -- END
