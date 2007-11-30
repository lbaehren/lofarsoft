/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (<mail>)                                                 *
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

#include <Imaging/BeamformerWeights.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ---------------------------------------------------------- BeamformerWeights
  
  BeamformerWeights::BeamformerWeights ()
    : GeometricalWeight ()
  {
    bool status (true);
    status = setBeamformerWeights ();
  }

  // ---------------------------------------------------------- BeamformerWeights
  
  BeamformerWeights::BeamformerWeights (casa::Matrix<double> const &antPositions,
					CR::CoordinateType const &antCoordType,
					casa::Matrix<double> const &skyPositions,
					CR::CoordinateType const &skyCoordType,
					casa::Vector<double> const &frequencies)
    : GeometricalWeight (antPositions,
			 antCoordType,
			 skyPositions,
			 skyCoordType,
			 frequencies,
			 false,
			 false,
			 false)
  {
    bool status (true);
    status = setBeamformerWeights ();
  }
  
  // ---------------------------------------------------------- BeamformerWeights
  
  BeamformerWeights::BeamformerWeights (GeometricalPhase const &phase)
    : GeometricalWeight (phase,
			 false)
  {
    bool status (true);
    status = setBeamformerWeights ();
  }
  
  // ---------------------------------------------------------- BeamformerWeights
  
  BeamformerWeights::BeamformerWeights (GeometricalWeight const &weights)
    : GeometricalWeight (weights)
  {
    bool status (true);
    status = setBeamformerWeights ();
  }
  
  // ---------------------------------------------------------- BeamformerWeights
  
  BeamformerWeights::BeamformerWeights (BeamformerWeights const &other)
    : GeometricalWeight ()
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  BeamformerWeights::~BeamformerWeights ()
  {
    destroy();
  }
  
  void BeamformerWeights::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ operator=

  BeamformerWeights& BeamformerWeights::operator= (BeamformerWeights const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy

  void BeamformerWeights::copy (BeamformerWeights const &other)
  {
    // copy the underlying base object
    GeometricalWeight::operator= (other);
    
    bfWeights_p.resize(other.bfWeights_p.shape());
    bfWeights_p = other.bfWeights_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void BeamformerWeights::summary (std::ostream &os)
  {
    /* Summary of this class */
    std::cout << "[BeamformerWeights] Summary of object" << std:: endl;
    os << "-- Sky positions       : " << skyPositions_p.shape()     << std::endl;
    os << "-- Antenna positions   : " << antPositions_p.shape()     << std::endl;
    os << "-- Frequency values    : " << frequencies_p.shape()      << std::endl;
    os << "-- Geometrical weights : " << GeometricalWeight::shape() << std::endl;
    os << "-- buffer delays       : " << bufferDelays_p             << std::endl;
    os << "-- buffer phases       : " << bufferPhases_p             << std::endl;
    os << "-- buffer weights      : " << bufferWeights_p            << std::endl;
    os << "-- show progress       : " << showProgress_p             << std::endl;
    os << "-- Beamformer weights  : " << bfWeights_p.shape()        << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // ------------------------------------------------------------ setAntennaGains

  bool BeamformerWeights::setAntennaGains (casa::Cube<DComplex> const &gains)
  {
    bool status (true);

    /*
      Check if the provided array has the correct shape; for the time being we
      simply reject the provided data, if they to not match -- no interpolation
      is attempted for mitigation (this would require some additional information
      anyway, as we would need to know about the coordinate axes).
    */
    try {
      /* extract shape information first */
      casa::IPosition shapeGains       = gains.shape();
      casa::IPosition shapeGeomWeights = GeometricalWeight::shape();
      /* check if the shapes agree */
      if (shapeGains == shapeGeomWeights) {
	status = setBeamformerWeights (gains);
      } else {
	std::cerr << "[BeamformerWeights::setAntennaGains] "
		  << "Mismatching array shapes!" 
		  << std::endl;
	status = false;
      }
    } catch (std::string message) {
      std::cerr << "[BeamformerWeights::setAntennaGains] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------------------- setBeamformerWeights

  bool BeamformerWeights::setBeamformerWeights ()
  {
    bool status (true);
    
    /*
      In the simplest case no antenna gain corrections are applied, so the
       weights simply consist of the weights calculated from the geometrical 
       phases.
    */
    try {
      bfWeights_p.resize (GeometricalWeight::shape());
      bfWeights_p = GeometricalWeight::weights();
    } catch (std::string message) {
      std::cerr << "[BeamformerWeights::setBeamformerWeights] "
		<< message
		<< std::endl;
      status = false;
    }

    return status;
  }
  
  // ------------------------------------------------------- setBeamformerWeights
  
  bool BeamformerWeights::setBeamformerWeights (casa::Cube<DComplex> const &gains)
  {
    bool status (true);
    
    /*
      No array shape checking required at this point, this this is handled in
      the function accepting the antenna gains.
    */
    status = setBeamformerWeights();
    
    if (status) {
      bfWeights_p *= gains;
    }
    
    return status;
  }

  // ---------------------------------------------------------- unsetAntennaGains
  
  bool BeamformerWeights::unsetAntennaGains ()
  {
    bool status (true);

    try {
      status = setBeamformerWeights ();
    } catch (std::string message) {
      std::cerr << "[BeamformerWeights::unsetAntennaGains]"
		<< message
		<< std::endl;
      status = false;
    }
      
    return status;
  }

} // Namespace CR -- end
