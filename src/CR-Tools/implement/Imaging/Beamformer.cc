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

// CASA header files
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
// Custom header files
#include <Imaging/Beamformer.h>

using casa::IPosition;
using casa::FFTServer;
using casa::Slicer;

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
  
  Beamformer::Beamformer (GeometricalWeight const &weights)
    : GeometricalWeight (weights)
  {
    init ();
  }
  
  // ----------------------------------------------------------------- Beamformer
  
  Beamformer::Beamformer (GeometricalPhase const &phase,
			  bool const &bufferWeights)
    : GeometricalWeight (phase,
			 bufferWeights)
  {
    init ();
  }

  Beamformer::Beamformer (GeometricalDelay const &delay,
			  casa::Vector<double> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeometricalWeight (delay,
			 frequencies,
			 bufferPhases,
			 bufferWeights)
  {
    init ();
  }
  
  // ----------------------------------------------------------------- Beamformer
  
  Beamformer::Beamformer (casa::Matrix<double> const &antPositions,
			  CR::CoordinateType const &antCoordType,
			  casa::Matrix<double> const &skyPositions,
			  CR::CoordinateType const &skyCoordType,
			  casa::Vector<double> const &frequencies,
			  bool const &bufferDelays,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeometricalWeight (antPositions,
			 antCoordType,
			 skyPositions,
			 skyCoordType,
			 frequencies,
			 bufferDelays,
			 bufferPhases,
			 bufferWeights)
  {
    init ();
  }
  
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
    GeometricalWeight::bufferWeights (true);
    // default setting for the used beamforming method
    status = setBeamType (FREQ_POWER);
  }
  
  // ---------------------------------------------------------------- setBeamType

  bool Beamformer::setBeamType (BeamType const &beam)
  {
    bool status (true);
    
    switch (beam) {
    case FREQ_FIELD:
      std::cerr << "[Beamformer::setBeamType]" << std::endl;
      std::cerr << " Beam type FREQ_FIELD not yet supported!" << std::endl;
      status = false;
//       beamType_p    = beam;
//       processData_p = &Beamformer::freq_field;
      break;
    case FREQ_POWER:
      beamType_p    = beam;
      processData_p = &Beamformer::freq_power;
      break;
    case TIME_FIELD:
      beamType_p    = beam;
      processData_p = &Beamformer::time_field;
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
  
  std::string Beamformer::beamTypeName (BeamType const &beamType)
  {
    std::string name;
    
    switch (beamType) {
    case FREQ_FIELD:
      name = "FREQ_FIELD";
      break;
    case FREQ_POWER:
      name = "FREQ_POWER";
      break;
    case TIME_FIELD:
      name = "TIME_FIELD";
      break;
    case TIME_POWER:
      name = "TIME_POWER";
      break;
    case TIME_CC:
      name = "TIME_CC";
      break;
    case TIME_P:
      name = "TIME_P";
      break;
    case TIME_X:
      name = "TIME_X";
	break;
    }
    return name;
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

  template <class T>
  bool Beamformer::checkData (casa::Matrix<T> &beam,
			      casa::Array<DComplex> const &data)
  {
    uint nofFailedChecks (0);
    IPosition shape    = data.shape();
    int nofFrequencies = GeometricalPhase::nofFrequencies();
    int nofAntennas    = GeometricalDelay::nofAntennas();
    
    /*
      delays  = [antenna,direction]
      phases  = [channel,antenna,direction]
      weights = [channel,antenna,direction]
      data    = [channel,antenna,(antenna)]
      beam    = [channel,direction]
    */

    // Check the number of frequency channels in the data array

    if (shape(0) != nofFrequencies) {
      std::cerr << "[Beamformer::checkData] Mismatch in number of frequencies"
		<< std::endl;
      std::cerr << "-- data    : " << shape(0)       << std::endl;
      std::cerr << "-- weights : " << nofFrequencies << std::endl;
      // increment error counter
      nofFailedChecks++;
    }

    /*
      Check the number of antennas in the data array; be aware that we might
      need to do some additional checking in case we are working with ACM data
    */

    if (shape(1) != nofAntennas) {
      std::cerr << "[Beamformer::checkData] Mismatch in number of antennas"
		<< std::endl;
      std::cerr << "-- data    : " << shape(1)    << std::endl;
      std::cerr << "-- weights : " << nofAntennas << std::endl;
      // increment error counter
      nofFailedChecks++;
    }

    if (nofFailedChecks) {
      return false;
    } else {
      return true;
    }
  }
  
  // ---------------------------------------------------------------- processData
  
  template <class T>
  bool Beamformer::processData (casa::Matrix<T> &beam,
				const casa::Array<DComplex> &data)
  {
    if (checkData(beam,data)) {
      return (this->*processData_p) (beam,data);
    } else {
      return false;
    }
  }

  // ------------------------------------------------------------------ beam_freq
  
  void Beamformer::beam_freq (casa::Vector<DComplex> &beamFreq,
			      casa::Array<DComplex> const &data,
			      casa::Cube<DComplex> const &weights,
			      uint const &direction,
			      bool const &normalize)
  {
    IPosition shape = data.shape();    // [freq,antenna,(antenna)]
    IPosition pos (shape.nelements()); // [freq,antenna,(antenna)]

    // loop over antennas
    for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
      // loop over frequency channels
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	beamFreq(pos(0)) = data(pos)*weights(pos(1),direction,pos(0));
      }
    }

    /*
      (Optional) normalization step
    */
    if (normalize) {
      beamFreq /= DComplex(pos(1));
    }
  }
  
  // ------------------------------------------------------------------ beam_time
  
  void Beamformer::beam_time (casa::Vector<double> &beamTime,
			      casa::Array<DComplex> const &data,
			      casa::Cube<DComplex> const &weights,
			      uint const &direction,
			      bool const &normalize)
  {
    IPosition shape = data.shape();    // [freq,antenna,(antenna)]
    int blocksize   = 2*(shape(0)-1);
    casa::Vector<DComplex> beamFreq (shape(0));
    
    /* get the beam in the frequency domain */
    beam_freq (beamFreq,
	       data,
	       weights,
	       direction,
	       normalize);
    
    /* inverse Fourier transform to obtain the beam in the time domain */
    FFTServer<double,std::complex<double> > server(IPosition(1,blocksize),
						   casa::FFTEnums::REALTOCOMPLEX);
    server.fft (beamTime,beamFreq);
  }
  
  // ----------------------------------------------------------------- freq_field
  
  bool Beamformer::freq_field (casa::Matrix<DComplex> &beam,
			       const casa::Array<DComplex> &data)
  {
    bool status (true);

    if (checkData(beam,data)) {

      casa::Cube<DComplex> weights;
      
      if (bufferWeights_p) {
	weights.reference(weights_p);
      } else {
	weights = GeometricalWeight::weights();
      }

      uint direction(0);
      casa::IPosition pos(2);                   // [freq,antenna]
      casa::IPosition shape = weights.shape();  // [freq,antenna,direction]
      casa::Vector<DComplex> tmp (shape(0));

      for (direction=0; direction<shape(2); direction++) {
	beam_freq (tmp,
		   data,
		   weights,
		   direction,
		   true);
	beam.column(direction) = tmp;
      }

    } else {
      return false;
    }

    return status;
  }
  
  // ----------------------------------------------------------------- freq_power

  bool Beamformer::freq_power (casa::Matrix<double> &beam,
			       const casa::Array<DComplex> &data)
  {
    bool status (true);
    casa::Cube<DComplex> weights;
    uint freq (0);
    uint antenna (0);
    uint direction (0);
    casa::IPosition pos (2);
    casa::DComplex tmp;

    /*
      We need the weights in order to apply them to the data; if the weights
      are buffered, we set up a reference to the array storing them, otherwise
      we will have to retrieve them from the GeometricalWeights class first.
     */

    if (bufferWeights_p) {
      weights.reference(weights_p);
    } else {
      weights = GeometricalWeight::weights();
    }
    
    try {
      // Get the shape of the beamformer weights, [freq,antenna,sky]
      casa::IPosition shape = weights.shape();
      // Resize the array returning the beamformed data
      beam.resize (shape(0),shape(2),0.0);

      for (direction=0; direction<shape(2); direction++) {
	for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	  for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	    tmp = data(pos)*weights(pos(0),pos(1),direction);
	    beam(pos(0),direction) += real(tmp*conj(tmp));
	  }
	}
      }
      
    } catch (std::string message) {
      std::cerr << "[Beamformer::freq_power] " << message << std::endl;
      status = false;
    }

    return status;
  }
  
  // ----------------------------------------------------------------- time_field

  bool Beamformer::time_field (casa::Matrix<double> &beam,
			       const casa::Array<DComplex> &data)
  {
    bool status (true);

    std::cerr << "[Beamformer::time_field] Method not yet implemented!"
	      << std::endl;
    
    return status;
  }
  
  // ----------------------------------------------------------------- time_power

  bool Beamformer::time_power (casa::Matrix<double> &beam,
			       const casa::Array<DComplex> &data)
  {
    bool status (true);

    std::cerr << "[Beamformer::time_power] Method not yet implemented!"
	      << std::endl;
    
    return status;
  }

  // -------------------------------------------------------------------- time_cc

  bool Beamformer::time_cc (casa::Matrix<double> &beam,
			    const casa::Array<DComplex> &data)
  {
    bool status (true);

    if (checkData (beam,data)) {

      try {
	
	casa::Cube<DComplex> weights;
	if (bufferWeights_p) {
	  weights.reference(weights_p);
	} else {
	  weights = GeometricalWeight::weights();
	}
	
	casa::IPosition shape = weights.shape();  //  [freq,antenna,direction]
	int blocksize         = 2*(shape(0)-1);
	double nofBaselines   = GeometricalDelay::nofBaselines();
	int direction (0);
	uint ant2 (0);
	casa::IPosition pos (2);
	casa::Vector<DComplex> tmpFreq (shape(0),0.0);
	casa::Vector<double>   tmpTime (blocksize,0.0);
	casa::Matrix<double>   tmpData (shape(1),blocksize);

#ifdef DEBUGGING_MESSAGES
	std::cout << "-- shape(weights) = " << shape << std::endl;
	std::cout << "-- shape(tmpFreq) = " << tmpFreq.shape() << std::endl;
	std::cout << "-- shape(tmpTime) = " << tmpTime.shape() << std::endl;	
	std::cout << "-- shape(tmpData) = " << tmpData.shape() << std::endl;
#endif
	
	/*
	  Set up the casa::FFTServer which is to handle the inverse Fourier
	  transform taking place before the summation step.
	*/
	FFTServer<double,std::complex<double> > server(IPosition(1,blocksize),
						       casa::FFTEnums::REALTOCOMPLEX);
	// resize array returning the beamformed data
	beam.resize (blocksize,shape(2),0.0);
	
	for (direction=0; direction<shape(2); direction++) {
	  // Precompute the shifted time series for a given sky position
	  for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	    // --- Apply the beamformer weights to the Fourier-transformed data
	    for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	      tmpFreq(pos(0)) = data(pos)*weights(pos(0),pos(1),direction);
	    }
	    // --- Inverse Fourier transform back to time domain
	    server.fft (tmpTime,tmpFreq);
	    tmpData.row(pos(1)) = tmpTime;
	  }
	  /*
	    Once we have precomputed all the shifted time-series, we go through
	    all baselines and add up antenna combinations.
	  */
	  tmpTime = 0.0;
	  for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	    for (ant2=pos(1)+1; ant2<shape(1); ant2++) {
	      tmpTime += tmpData.row(pos(1))*tmpData.row(ant2);
	    }  // end loop: antenn2
	  }  // end loop: antenna 1
	  // normalization w.r.t. the number of baselines
	  tmpTime /= nofBaselines;
	  //
	  beam.column(direction) = CR::sign(tmpTime)*sqrt(abs(tmpTime));
	}
      } catch (std::string message) {
	std::cerr << "[Beamformer::time_cc] " << message << std::endl;
	status = false;
      } 
    }
    
    return status;
  }
  
  // --------------------------------------------------------------------- time_p
  
  bool Beamformer::time_p (casa::Matrix<double> &beam,
			   const casa::Array<DComplex> &data)
  {
    bool status (true);

    if (checkData (beam,data)) {
      
      casa::Cube<DComplex> weights;
      
      if (bufferWeights_p) {
	weights.reference(weights_p);
      } else {
	weights = GeometricalWeight::weights();
      }

      uint direction(0);
      casa::IPosition pos(2);                   // [freq,antenna]
      casa::IPosition shape = weights.shape();  // [freq,antenna,direction]
      int blocksize         = 2*(shape(0)-1);
      casa::Vector<DComplex> tmpFreq (shape(0));
      casa::Vector<double> tmpTime (blocksize);
      casa::Vector<double> sum (blocksize);
      FFTServer<double,DComplex> server(casa::IPosition(1,blocksize),
					casa::FFTEnums::REALTOCOMPLEX);

#ifdef DEBUGGING_MESSAGES
      std::cout << "[Beamformer::time_p]" << std::endl;
      std::cout << "- shape(data)    = " << data.shape()    << std::endl;
      std::cout << "- shape(beam)    = " << beam.shape()    << std::endl;
      std::cout << "- shape(weights) = " << shape           << std::endl;
      std::cout << "- shape(tmpFreq) = " << tmpFreq.shape() << std::endl;
      std::cout << "- shape(tmpTime) = " << tmpTime.shape() << std::endl;
      std::cout << "- shape(sum)     = " << sum.shape()     << std::endl;
#endif

      /* loop over sky positions */
      for (direction=0; direction<shape(2); direction++) {
	sum = 0.0;
	/* loop over antennas */
	for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	  /* loop over frequency channels */
	  for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	    tmpFreq(pos(0)) = data(pos)*weights(pos(0),pos(1),direction);
	  }
	  /* perform inverse Fourier transform on the beam to get back to time
	     domain */
	  server.fft (tmpTime,tmpFreq);
	  sum += tmpTime*tmpTime;
	} // end loop: antenna
	/* normalize with the number of antennas */
	beam.column(direction) = sqrt(sum/double(shape(1)));
      } // end loop: direction
    } else {
      status = false;
    }
        
    return status;
  }
  
  // --------------------------------------------------------------------- time_x
  
  bool Beamformer::time_x (casa::Matrix<double> &beam,
			   const casa::Array<DComplex> &data)
  {
    bool status (true);

    std::cerr << "[Beamformer::time_x] Method not yet implemented!" << std::endl;
    
    return status;
  }

  // ============================================================================
  //
  //  Template instantiation
  //
  // ============================================================================

  template bool Beamformer::processData (casa::Matrix<double> &beam,
					 const casa::Array<DComplex> &data);
//   template bool Beamformer::processData (casa::Matrix<DComplex> &beam,
// 					 const casa::Array<DComplex> &data);
//   template bool Beamformer::checkData (casa::Matrix<double> &beam,
// 				       casa::Array<double> const &data);
//   template bool Beamformer::checkData (casa::Matrix<double> &beam,
// 				       casa::Array<DComplex> const &data);
  
  
} // Namespace CR -- end
