/*-------------------------------------------------------------------------*
 | $Id: template-class.cc,v 1.13 2007/06/13 09:41:37 bahren Exp          $ |
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

#include <Imaging/Beamformer.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ----------------------------------------------------------------- Beamformer
  
  Beamformer::Beamformer ()
    : GeometricalWeight()
  {
    init ();
  }
  
  // ----------------------------------------------------------------- Beamformer
  
#ifdef HAVE_CASA
  Beamformer::Beamformer (casa::Matrix<double> const &antPositions,
			  casa::Matrix<double> const &skyPositions,
			  casa::Vector<double> const &frequencies,
			  bool const &bufferDelays,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeometricalWeight(antPositions,
			skyPositions,
			frequencies,
			bufferDelays,
			bufferPhases,
			bufferWeights)
  {
    init ();
  }
#else
#ifdef HAVE_BLITZ
  Beamformer::Beamformer (const blitz::Array<double,2> &antPositions,
			  const blitz::Array<double,2> &skyPositions,
			  blitz::Array<double,1> const &frequencies,
			  bool const &bufferDelays,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeometricalWeight(antPositions,
			skyPositions,
			frequencies,
			bufferDelays,
			bufferPhases,
			bufferWeights)
  {
    init ();
  }
#endif
#endif
  
  Beamformer::Beamformer (Beamformer const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  Beamformer::~Beamformer ()
  {
    destroy();
  }
  
  void Beamformer::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  Beamformer& Beamformer::operator= (Beamformer const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void Beamformer::copy (Beamformer const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init

  void Beamformer::init ()
  {
    bool status (true);
    // Activate the buffering of the weighting factors
    bufferWeights_p = true;
    // default setting for the used beamforming method
    status = setBeamType (Beamformer::ADDANTENNAS);
  }
  
  // --------------------------------------------------------------- beamTypeName

  std::string Beamformer::beamTypeName ()
  {
    std::string name;
    
    switch (beamType_p) {
    case ADDANTENNAS:
      name = "AddAntennas";
      break;
    case ADDBASELINES:
      name = "AddBaselines";
      break;
    case CCBEAM:
      name = "ccBeam";
      break;
    case XBEAM:
      name = "xBeam";
      break;
    }

    return name;
  }

  // ---------------------------------------------------------------- setBeamType

  bool Beamformer::setBeamType (Beamformer::BeamType const &beam)
  {
    bool status (true);
    
    switch (beam) {
    case ADDANTENNAS:
      beamType_p = beam;
      processData_p = &Beamformer::add_signals_per_antenna;
      break;
    case ADDBASELINES:
      beamType_p = beam;
      break;
    case CCBEAM:
      beamType_p = beam;
      processData_p = &Beamformer::cc_beam;
      break;
    case XBEAM:
      beamType_p = beam;
      processData_p = &Beamformer::x_beam;
      break;
    }
    
    return status;
  }

  void Beamformer::summary (std::ostream &os)
  {
    os << "[Beamformer] Summary of object"                     << std::endl;
    os << "-- Sky positions      : " << skyPositions_p.shape() << std::endl;
    os << "-- Antenna positions  : " << antPositions_p.shape() << std::endl;
    os << "-- Frequency values   : " << frequencies_p.shape()  << std::endl;
    os << "-- Buffer delays?     : " << bufferDelays_p         << std::endl;
    os << "-- Buffer phases?     : " << bufferPhases_p         << std::endl;
    os << "-- Buffer weights?    : " << bufferWeights_p        << std::endl;
    os << "-- Beamforming method : " << beamType_p
       << " / " << beamTypeName() << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ---------------------------------------------------- add_signals_per_antenna

#ifdef HAVE_CASA
  bool Beamformer::add_signals_per_antenna (casa::Matrix<double> &beam,
					    const casa::Matrix<DComplex> &data)
  {
    bool status (true);
    int nofSkyPositions (skyPositions_p.nrow());
    int nofAntennas (antPositions_p.nrow());
    int nofFrequencies (frequencies_p.nelements());
    casa::IPosition shapeData (data.shape());

    /*
      Check if the shape of the array with the input data matched the internal
      parameters.
    */
    if (shapeData(0) == nofSkyPositions &&
	shapeData(1) == nofFrequencies) {
      // additional local variables
      int direction (0);
      int antenna (0);
      int freq (0);
      casa::DComplex tmp;
      // resize array returning the beamformed data
      beam.resize (nofSkyPositions,nofFrequencies,0.0);
      /*
	Compute the beams for all combinations of sky positions and frequency
	values.
      */
      for (direction=0; direction<nofSkyPositions; direction++) {
	for (antenna=0; antenna<nofAntennas; antenna++) {
	  for (freq=0; freq<nofFrequencies; freq++) {
	    tmp = data(antenna,freq)*weights_p(antenna,direction,freq);
	    beam(direction,freq) += real(tmp*conj(tmp));
	  }
	}
      }
    } else {
      std::cerr << "[Beamformer::add_signals_per_antenna]" << std::endl;
      std::cerr << "-- Wrong shape of array with input data!" << std::endl;
      status = false;
    }

    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::add_signals_per_antenna (blitz::Array<double,2> &beam,
					    const blitz::Array<complex<double>,2> &data)
  {
    bool status (true);
    
    return status;
  }
#endif
#endif  

  // -------------------------------------------------------------------- cc_beam

#ifdef HAVE_CASA
  bool Beamformer::cc_beam (casa::Matrix<double> &beam,
			    const casa::Matrix<DComplex> &data)
  {
    bool status (true);
    
    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::cc_beam (blitz::Array<double,2> &beam,
			    const blitz::Array<conplex<double>,2> &data)
  {
    bool status (true);
    
    return status;
  }
#endif
#endif
  
  // --------------------------------------------------------------------- x_beam

#ifdef HAVE_CASA
  bool Beamformer::x_beam (casa::Matrix<double> &beam,
			    const casa::Matrix<DComplex> &data)
  {
    bool status (true);
    
    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::x_beam (blitz::Array<double,2> &beam,
			    const blitz::Array<conplex<double>,2> &data)
  {
    bool status (true);
    
    return status;
  }
#endif
#endif
  
} // Namespace CR -- end
