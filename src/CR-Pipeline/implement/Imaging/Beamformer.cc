/*-------------------------------------------------------------------------*
 | $Id$ |
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

// CASA header files
#ifdef HAVE_CASA
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
using casa::IPosition;
using casa::FFTServer;
using casa::Matrix;
using casa::Slicer;
using casa::Vector;
#endif
// Custom header files
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
  Beamformer::Beamformer (Matrix<double> const &antPositions,
			  Matrix<double> const &skyPositions,
			  Vector<double> const &frequencies)
    : GeometricalWeight(antPositions,
			skyPositions,
			frequencies,
			false,
			false,
			true)
  {
    init ();
  }
#else
#ifdef HAVE_BLITZ
  Beamformer::Beamformer (const blitz::Array<double,2> &antPositions,
			  const blitz::Array<double,2> &skyPositions,
			  blitz::Array<double,1> const &frequencies)
    : GeometricalWeight(antPositions,
			skyPositions,
			frequencies,
			false,
			false,
			true)
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

  // ------------------------------------------------------------------ operator=
  
  Beamformer& Beamformer::operator= (Beamformer const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }

  // ----------------------------------------------------------------------- copy
  
  void Beamformer::copy (Beamformer const &other)
  {
    // copy the underlying base object
    GeometricalWeight::operator= (other);

    // copy settings to handle the type of beam
    setBeamType (other.beamType_p);
  }

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
    status = setBeamType (FREQ_POWER);
  }
  
  // ---------------------------------------------------------------- setBeamType

  bool Beamformer::setBeamType (BeamType const &beam)
  {
    bool status (true);
    
    switch (beam) {
    case FREQ_FIELD:
      std::cerr << "[Beamformer::setBeamType] FREQ_FIELD not yet supported!"
		<< std::endl;
      status = false;
      break;
    case FREQ_POWER:
      beamType_p    = beam;
      processData_p = &Beamformer::freq_power;
      break;
    case TIME_FIELD:
      std::cerr << "[Beamformer::setBeamType] TIME_FIELD not yet supported!"
		<< std::endl;
      status = false;
      break;
    case TIME_POWER:
      beamType_p    = beam;
      processData_p = &Beamformer::time_power;
      break;
    case TIME_CC:
      beamType_p    = beam;
      processData_p = &Beamformer::time_cc;
      break;
    case TIME_P:
      beamType_p    = beam;
      processData_p = &Beamformer::time_p;
      break;
    case TIME_X:
      beamType_p    = beam;
      processData_p = &Beamformer::time_x;
      break;
    }
    
    return status;
  }

  // --------------------------------------------------------------- beamTypeName

  std::string Beamformer::beamTypeName ()
  {
    return beamTypeName(beamType_p);
  }

  // ------------------------------------------------------------------- beamType

  bool Beamformer::beamType (BeamType &beamType,
			     string const &domain,
			     string const &quantity)
  {
    bool ok (true);

    if (domain == "time" || domain == "Time" || domain == "TIME") {
      if (quantity == "field" || quantity == "Field" || quantity == "FIELD") {
	beamType = TIME_FIELD;
      } else if (quantity == "power" || quantity == "Power" || quantity == "POWER") {
	beamType = TIME_POWER;
      } else if (quantity == "cc" || quantity == "CC") {
	beamType = TIME_CC;
      } else if (quantity == "p" || quantity == "P") {
	beamType = TIME_P;
      } else if (quantity == "x" || quantity == "X") {
	beamType = TIME_X;
      } else {
	std::cerr << "[Beamformer::beamType] Unknown signal quantity "
		  << quantity << std::endl;
	ok = false;
      }
    } else if (domain == "freq" || domain == "Freq" || domain == "FREQ") {
      if (quantity == "field" || quantity == "Field" || quantity == "FIELD") {
	beamType = FREQ_FIELD;
      } else if (quantity == "power" || quantity == "Power" || quantity == "POWER") {
	beamType = FREQ_POWER;
      } else {
	std::cerr << "[SkymapCoordinates::setMapQuantity] Unknown signal quantity "
		  << quantity << std::endl;
	ok = false;
      }
    } else {
      std::cerr << "[SkymapCoordinates::setMapQuantity] Unknown signal domain "
		<< domain << std::endl;
      ok = false;
    }

    return ok;
  }

  // -------------------------------------------------------------------- summary

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

  // ------------------------------------------------------------------ checkData

#ifdef HAVE_CASA
  bool Beamformer::checkData (casa::Matrix<double> &beam,
			      const casa::Matrix<DComplex> &data)
  {
    bool ok (true);
    
    int nofFrequencies (frequencies_p.nelements());
    IPosition shapeData (data.shape());

    /*
      shape(data) = [antenna,channel]
      shape(beam) = [position,channel]
    */
    
    if (shapeData(0) == nofAntennas_p &&
	shapeData(1) == nofFrequencies) {
      ok = true;
    } else {
      std::cerr << "[Beamformer::checkData]" << std::endl;
      std::cerr << "-- Wrong shape of array with input data!"    << std::endl;
      std::cerr << "--> shape(data)    [antennas,channels]            = "
		<< data.shape()
		<< std::endl;
      std::cerr << "--> shape(weights) [antennas,directions,channels] = "
		<< weights_p.shape()
		<< std::endl;
      ok = false;
    }
    
    return ok;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::checkData (blitz::Array<double,2> &beam,
			      const blitz::Array<complex<double>,2> &data)
  {
    bool ok (true);
    
    return ok;
  }
#endif
#endif
  
  // ---------------------------------------------------------------- processData
  
#ifdef HAVE_CASA
  bool Beamformer::processData (casa::Matrix<double> &beam,
				const casa::Matrix<DComplex> &data)
  {
    if (checkData(beam,data)) {
      return (this->*processData_p) (beam,data);
    } else {
      return false;
    }
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::processData (blitz::Array<double,2> &beam,
				const blitz::Array<complex<double>,2> &data)
  {
    if (checkData(beam,data)) {
      return (this->*processData_p) (beam,data);
    } else {
      return false;
    }
  }
#endif
#endif
  
  // ----------------------------------------------------------------- freq_field
  
#ifdef HAVE_CASA
  bool Beamformer::freq_field (Matrix<DComplex> &beam,
			       const Matrix<DComplex> &data)
  {
    bool status (true);
    int nofSkyPositions (skyPositions_p.nrow());
    int nofFrequencies (frequencies_p.nelements());
    IPosition shapeData (data.shape());

    /*
      Check if the shape of the array with the input data matched the internal
      parameters.
    */
    if (shapeData(0) == nofSkyPositions &&
	shapeData(1) == nofFrequencies) {
      // additional local variables
      int direction (0);
      uint antenna (0);
      int freq (0);
      // resize array returning the beamformed data
      beam.resize (nofSkyPositions,nofFrequencies,0.0);
      /*
	Compute the beams for all combinations of sky positions and frequency
	values. We unfortunately need the innermost loop over the frequencies,
	since the CASA Array module appears not to support the "+=" operator
	for sub-arrays (e.g. to process all values along the frequency axis).
      */
      for (direction=0; direction<nofSkyPositions; direction++) {
	for (antenna=0; antenna<nofAntennas_p; antenna++) {
	  for (freq=0; freq<nofFrequencies; freq++) {
	    beam(direction,freq) = data(antenna,freq)*weights_p(antenna,direction,freq);
	  }
	}
      }
    } else {
      std::cerr << "[Beamformer::freq_field]" << std::endl;
      std::cerr << "-- Wrong shape of array with input data!" << std::endl;
      std::cerr << "--> shape(data)    = " << data.shape()      << std::endl;
      std::cerr << "--> shape(weights) = " << weights_p.shape() << std::endl;
      status = false;
    }

    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::freq_field (blitz::Array<complex<double>,2> &beam,
			       const blitz::Array<complex<double>,2> &data)
  {
    bool status (true);

    return status;
  }
#endif
#endif
  
  // ----------------------------------------------------------------- freq_power

#ifdef HAVE_CASA
  bool Beamformer::freq_power (Matrix<double> &beam,
			       const Matrix<DComplex> &data)
  {
    bool status (true);
    int nofSkyPositions (skyPositions_p.nrow());
    int nofFrequencies (frequencies_p.nelements());
    int direction (0);
    uint antenna (0);
    int freq (0);
    casa::DComplex tmp;
    
    try {
      // Resize the array returning the beamformed data
      beam.resize (nofSkyPositions,nofFrequencies,0.0);
      
      std::cout << "[Beamformer::freq_power] Processing data..." << std::endl;
      
      // Iteration over the set of directions in the sky
      for (direction=0; direction<nofSkyPositions; direction++) {
	for (antenna=0; antenna<nofAntennas_p; antenna++) {
	  for (freq=0; freq<nofFrequencies; freq++) {
	    tmp = data(antenna,freq)*weights_p(antenna,direction,freq);
	    beam(direction,freq) += real(tmp*conj(tmp));
	  }
	}
      }
    } catch (std::string message) {
      std::cerr << "[Beamformer::freq_power] " << message << std::endl;
      status = false;
    }

    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::freq_power (blitz::Array<double,2> &beam,
			       const blitz::Array<complex<double>,2> &data)
  {
    bool status (true);
    
    return status;
  }
#endif
#endif
  
  // ----------------------------------------------------------------- time_power

#ifdef HAVE_CASA
  bool Beamformer::time_power (Matrix<double> &beam,
			       const Matrix<DComplex> &data)
  {
    bool status (true);

    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::time_power (blitz::Array<double,2> &beam,
			       const blitz::Array<complex<double>,2> &data)
  {
    bool status (true);
    int nofSkyPositions (skyPositions_p.nrow());
    int nofFrequencies (frequencies_p.nelements());
    IPosition shapeData (data.shape());

    /*
      Check if the shape of the array with the input data matched the internal
      parameters.
    */
    if (shapeData(0) == nofSkyPositions &&
	shapeData(1) == nofFrequencies) {
      // additional local variables
      int direction (0);
      uint antenna (0);
      int freq (0);
      int blocksize (2*(nofFrequencies-1));
      Vector<DComplex> vectFreq (nofFrequencies);

      std::cout << "[Beamformer::time_power] Processing data..." << std::endl;

      for (direction=0; direction<nofSkyPositions; direction++) {
	for (antenna=0; antenna<nofAntennas_p; antenna++) {
	  // (1) Assemble the beamformed spectrum
	  for (freq=0; freq<nofFrequencies; freq++) {
	    vectFreq(freq) = data(antenna,freq)*weights_p(antenna,direction,freq);
	  }
	  // (2) Inverse Fourier transform back to the time domain
	  // (3) Assemble power time-series
	}
      }

    } else {
      std::cerr << "[Beamformer::time_power]" << std::endl;
      std::cerr << "-- Wrong shape of array with input data!" << std::endl;
      std::cerr << "--> shape(data)    = " << data.shape()      << std::endl;
      std::cerr << "--> shape(weights) = " << weights_p.shape() << std::endl;
      status = false;
    }

    return status;
  }
#endif
#endif

  // -------------------------------------------------------------------- time_cc

#ifdef HAVE_CASA
  bool Beamformer::time_cc (Matrix<double> &beam,
			    const Matrix<DComplex> &data)
  {
    bool status (true);
    int nofSkyPositions (skyPositions_p.nrow());
    int nofFrequencies (frequencies_p.nelements());
    IPosition shapeData (data.shape());
    
    /*
      Check if the shape of the array with the input data matched the internal
      parameters.
    */
    if (shapeData(0) == nofSkyPositions &&
	shapeData(1) == nofFrequencies) {
      // additional local variables
      double nofBaselines (GeometricalDelay::nofBaselines());
      int blocksize (2*(nofFrequencies-1));
      int direction (0);
      uint ant1 (0);
      uint ant2 (0);
      int freq (0);
      Vector<DComplex> tmpFreq (nofFrequencies,0.0);
      Vector<double>   tmpTime (blocksize,0.0);
      Matrix<double>   tmpData (nofAntennas_p,blocksize);

      /*
	Set up the casa::FFTServer which is t handle the inverse Fourier
	transform taking place before the summation step.
      */
      FFTServer<double,std::complex<double> > server(IPosition(1,blocksize),
						     casa::FFTEnums::REALTOCOMPLEX);
      // resize array returning the beamformed data
      beam.resize (nofSkyPositions,blocksize,0.0);

      std::cout << "[Beamformer::time_cc] Processing data..." << std::endl;

      for (direction=0; direction<nofSkyPositions; direction++) {
	// Precompute the shifted time series for a given sky position
	for (ant1=0; ant1<nofAntennas_p; ant1++) {
	  // --- Apply the beamformer weights to the Fourier-transformed data
	  for (freq=0; freq<nofFrequencies; freq++) {
	    tmpFreq(freq) = data(ant1,freq)*weights_p(ant1,direction,freq);
	  }
	  // --- Inverse Fourier transform back to time domain
	  server.fft (tmpTime,tmpFreq);
	  tmpData.row(ant1) = tmpTime;
	}
	/*
	  Once we have precomputed all the shifted time-series, we go through
	  all baselines and add up antenna combinations.
	*/
	for (ant1=0; ant1<nofAntennas_p; ant1++) {
	  for (ant2=ant1+1; ant2<nofAntennas_p; ant2++) {
	    tmpTime += tmpData.row(ant1)*tmpData.row(ant2);
	  }  // end loop: antenn2
	}  // end loop: ant1
	// normalization w.r.t. the number of baselines
	tmpTime /= nofBaselines;
	//
	beam.row(direction) = CR::sign(tmpTime)*sqrt(abs(tmpTime));
      }
    } else {
      std::cerr << "[Beamformer::time_cc]"                    << std::endl;
      std::cerr << "-- Wrong shape of array with input data!" << std::endl;
      status = false;
    }

    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::time_cc (blitz::Array<double,2> &beam,
			    const blitz::Array<conplex<double>,2> &data)
  {
    bool status (true);
    
    return status;
  }
#endif
#endif
  
  // --------------------------------------------------------------------- time_p
  
#ifdef HAVE_CASA
  bool Beamformer::time_p (Matrix<double> &beam,
			   const Matrix<DComplex> &data)
  {
    bool status (true);
    
    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::time_p (blitz::Array<double,2> &beam,
			   const blitz::Array<conplex<double>,2> &data)
  {
    bool status (true);
    
    return status;
  }
#endif
#endif
  
  // --------------------------------------------------------------------- time_x
  
#ifdef HAVE_CASA
  bool Beamformer::time_x (Matrix<double> &beam,
			   const Matrix<DComplex> &data)
  {
    bool status (true);
    
    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool Beamformer::time_x (blitz::Array<double,2> &beam,
			   const blitz::Array<conplex<double>,2> &data)
  {
    bool status (true);
    
    return status;
  }
#endif
#endif
  
} // Namespace CR -- end
