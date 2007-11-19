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

#include <Imaging/GeometricalWeight.h>
#include <Utilities/ProgressBar.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ---------------------------------------------------------- GeometricalWeight
  
  GeometricalWeight::GeometricalWeight ()
    : GeometricalPhase ()
  {
    bufferWeights_p = false;
  }
  
  // ---------------------------------------------------------- GeometricalWeight
  
  GeometricalWeight::GeometricalWeight (casa::Matrix<double> const &antPositions,
					casa::Matrix<double> const &skyPositions,
					casa::Vector<double> const &frequencies,
					bool const &bufferDelays,
					bool const &bufferPhases,
					bool const &bufferWeights)
    : GeometricalPhase (antPositions,
			skyPositions,
			frequencies,
			bufferDelays,
			bufferPhases),
      bufferWeights_p (bufferWeights)
  {
    if (bufferWeights) {
      setWeights();
    }
  }

  // ---------------------------------------------------------- GeometricalWeight
  
  GeometricalWeight::GeometricalWeight (casa::Matrix<double> const &antPositions,
					CR::CoordinateType const &antCoordType,
					casa::Matrix<double> const &skyPositions,
					CR::CoordinateType const &skyCoordType,
					casa::Vector<double> const &frequencies,
					bool const &bufferDelays,
					bool const &bufferPhases,
					bool const &bufferWeights)
    : GeometricalPhase (antPositions,
			antCoordType,
			skyPositions,
			skyCoordType,
			frequencies,
			bufferDelays,
			bufferPhases),
      bufferWeights_p (bufferWeights)
  {
  }
  
  // ---------------------------------------------------------- GeometricalWeight
  
  GeometricalWeight::GeometricalWeight (GeometricalDelay const &delay,
					casa::Vector<double> const &frequencies,
					bool const &bufferPhases,
					bool const &bufferWeights)
    : GeometricalPhase (delay,
			frequencies,
			bufferPhases)
  {
    bufferWeights_p = bufferWeights;
  }
  
  // ---------------------------------------------------------- GeometricalWeight
  
  GeometricalWeight::GeometricalWeight (GeometricalPhase const &phase,
					bool const &bufferWeights)
    : GeometricalPhase (phase)
  {
    bufferWeights_p = bufferWeights;
  }
  
  // ---------------------------------------------------------- GeometricalWeight
  
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
  
  void GeometricalWeight::summary (std::ostream &os)
  {
    os << "[GeometricalWeight] Summary of object"             << std::endl;
    os << "-- Sky positions     : " << skyPositions_p.shape() << std::endl;
    os << "-- Antenna positions : " << antPositions_p.shape() << std::endl;
    os << "-- Frequency values  : " << frequencies_p.shape()  << std::endl;
    os << "-- buffer delays     : " << bufferDelays_p         << std::endl;
    os << "-- buffer phases     : " << bufferPhases_p         << std::endl;
    os << "-- buffer weights    : " << bufferWeights_p        << std::endl;
    os << "-- show progress     : " << showProgress_p         << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods reimplemented from base classes
  //
  // ============================================================================

  // ------------------------------------------------------------- setFrequencies

  bool GeometricalWeight::setFrequencies (const casa::Vector<double> &frequencies,
					  bool const &bufferPhases)
  {
    bool ok (true);
    
    // forward the input parameters to the base classes
    GeometricalPhase::setFrequencies (frequencies,
				      bufferPhases);
    
    // if necessary updated the buffered values for the weights
    setWeights ();
    
    return ok;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ----------------------------------------------------------------- setWeights

  void GeometricalWeight::setWeights ()
  {
    if (bufferWeights_p) {
      IPosition shape (3);

      // Array axes: [frequency,antenna,skyPosition]
      shape(0) = GeometricalPhase::nofFrequencies();
      shape(1) = GeometricalDelay::nofAntennaPositions();
      shape(2) = GeometricalDelay::nofSkyPositions();

      // adjust the shape of the array storing the weights
      weights_p.resize (shape);
      // compute and assign the new values
      weights_p = calcWeights ();
    }
  }

  // ---------------------------------------------------------------- calcWeights

  casa::Cube<DComplex> GeometricalWeight::calcWeights ()
  {
    int freq (0);
    int ant (0);
    int sky (0);
    // get the geometrical phases, [frequency,antenna,position]
    casa::Cube<double> phases = GeometricalPhase::phases();
    casa::IPosition shape     = phases.shape();
    // array to store the computed weights
    casa::Cube<DComplex> weights (shape);
    // progress bar
    CR::ProgressBar progress (shape(2)-1);

    std::cout << "[GeometricalWeight::calcWeights]" << std::endl;

//     weights = casa::Cube<DComplex> (cos(phases),sin(phases));

    for (sky=0; sky<shape(2); sky++) {
      for (ant=0; ant<shape(1); ant++) {
	for (freq=0; freq<shape(0); freq++) {
	  weights(freq,ant,sky) = DComplex(cos(phases(freq,ant,sky)),
					   sin(phases(freq,ant,sky)));
 	}  // end loop: ant
      }  // end loop: sky
      if (showProgress_p) {
	progress.update(freq);
      }
    }  // end loop: freq
    
    std::cout << " --> Computation of weights completed." << std::endl;
    
    return weights;
  }
  
} // NAMESPACE CR -- END
