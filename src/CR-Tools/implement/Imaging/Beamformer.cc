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

  //_____________________________________________________________________________
  //                                                                   Beamformer
  
  Beamformer::Beamformer ()
    : GeomWeight()
  {
    init ();
  }
  
  //_____________________________________________________________________________
  //                                                                   Beamformer
  
  Beamformer::Beamformer (GeomWeight const &weights)
    : GeomWeight(weights)
  {
    init();
  }
  
  //_____________________________________________________________________________
  //                                                                   Beamformer
  
  Beamformer::Beamformer (GeomPhase const &phases,
			  bool const &bufferWeights)
    : GeomWeight (phases,
		  bufferWeights)
  {
    init ();
  }
  
  //_____________________________________________________________________________
  //                                                                   Beamformer
  
  Beamformer::Beamformer (GeomDelay const &geomDelay,
			  Vector<double> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeomWeight(geomDelay,
		 frequencies,
		 bufferPhases,
		 bufferWeights)
  {
    init();
  }
  
  //_____________________________________________________________________________
  //                                                                   Beamformer
  
  Beamformer::Beamformer (GeomDelay const &geomDelay,
			  Vector<MVFrequency> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeomWeight(geomDelay,
		 frequencies,
		 bufferPhases,
		 bufferWeights)
  {
    init();
  }
  
  //_____________________________________________________________________________
  //                                                                   Beamformer
  
  Beamformer::Beamformer (Matrix<double> const &antPositions,
			  CoordinateType::Types const &antCoord,
			  Matrix<double> const &skyPositions,
			  CoordinateType::Types const &skyCoord,
			  bool const &anglesInDegrees,
			  bool const &farField,
			  bool const &bufferDelays,
			  Vector<double> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeomWeight(antPositions,
		 antCoord,
		 skyPositions,
		 skyCoord,
		 anglesInDegrees,
		 farField,
		 bufferDelays,
		 frequencies,
		 bufferPhases,
		 bufferWeights)
  {
    init ();
  }
  
  //_____________________________________________________________________________
  //                                                                   Beamformer
  
  Beamformer::Beamformer (Vector<MVPosition> const &antPositions,
			  Vector<MVPosition> const &skyPositions,
			  bool const &farField,
			  bool const &bufferDelays,
			  Vector<MVFrequency> const &frequencies,
			  bool const &bufferPhases,
			  bool const &bufferWeights)
    : GeomWeight(antPositions,
		 skyPositions,
		 farField,
		 bufferDelays,
		 frequencies,
		 bufferPhases,
		 bufferWeights)
  {
    init ();
  }

  //_____________________________________________________________________________
  //                                                                   Beamformer
  
  Beamformer::Beamformer (Beamformer const &other)
    : GeomWeight (other)
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
    GeomWeight::operator= (other);

    skymapType_p = other.skymapType_p;
    norm_p       = other.norm_p;

    shapeBeam_p.resize(other.shapeBeam_p.nelements());
    shapeBeam_p = other.shapeBeam_p;

    bfWeights_p.resize(other.bfWeights_p.shape());
    bfWeights_p = other.bfWeights_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init

  void Beamformer::init ()
  {
    // default setting for the used beamforming method
    setSkymapType (SkymapQuantity::FREQ_POWER);
    /* set the weights used in the beamforming */
    setWeights ();
  }
  
  // ---------------------------------------------------------------- setBeamType
  
  bool Beamformer::setSkymapType (SkymapQuantity const &skymapType)
  {
    bool status (true);

    IPosition wShape = GeomWeight::shape();  //  [freq,ant,sky]
    int blocksize    = (wShape(0)-1)*2;

    switch (skymapType.type()) {
    case SkymapQuantity::FREQ_FIELD:
      std::cerr << "[Beamformer::setSkymapType]" << std::endl;
      std::cerr << " Beam type FREQ_FIELD not yet supported!" << std::endl;
      status = false;
//       beamType_p    = beam;
//       processData_p = &Beamformer::freq_field;
//       shapeBeam_p   = IPosition(2,wShape(0),wShape(2));
//       norm_p        = 1.0/wShape(1);
      break;
    case SkymapQuantity::FREQ_POWER:
      skymapType_p  = skymapType;
      processData_p = &Beamformer::freq_power;
      shapeBeam_p   = IPosition(2,wShape(0),wShape(2));
      norm_p        = 1.0/(wShape(1)*wShape(1));
      break;
    case SkymapQuantity::TIME_FIELD:
      skymapType_p  = skymapType;
      processData_p = &Beamformer::time_field;
      shapeBeam_p   = IPosition(2,blocksize,wShape(2));
      norm_p        = 1.0;
      break;
    case SkymapQuantity::TIME_POWER:
      skymapType_p  = skymapType;
      processData_p = &Beamformer::time_power;
      shapeBeam_p   = IPosition(2,blocksize,wShape(2));
      norm_p        = 1.0;
      break;
    case SkymapQuantity::TIME_CC:
      skymapType_p  = skymapType;
      processData_p = &Beamformer::time_cc;
      shapeBeam_p   = IPosition(2,blocksize,wShape(2));
      norm_p        = 1.0;
      break;
    case SkymapQuantity::TIME_P:
      skymapType_p  = skymapType;
      processData_p = &Beamformer::time_p;
      shapeBeam_p   = IPosition(2,blocksize,wShape(2));
      norm_p        = 1.0;
      break;
    case SkymapQuantity::TIME_X:
      skymapType_p  = skymapType;
      processData_p = &Beamformer::time_x;
      shapeBeam_p   = IPosition(2,blocksize,wShape(2));
      norm_p        = 1.0;
      break;
    default:
      std::cerr << "[Beamformer::setSkymapType]"
		<< " Unsupported skymap quantity or type of beam!"
		<< std::endl;
      break;
    }
    
    return status;
  }

  // -------------------------------------------------------------------- summary

  void Beamformer::summary (std::ostream &os)
  {
    os << "[Beamformer] Summary of object"                         << std::endl;
    os << "-- Near-field beamforming = " << GeomDelay::nearField() << std::endl;
    os << "-- Beam domain type/name  = " << domainType() << " / " << domainName() 
       << std::endl;
    os << "-- Beamforming method     = " << skymapType_p.name()    << std::endl;
    os << "-- buffer delays          = " << bufferDelays_p         << std::endl;
    os << "-- buffer phases          = " << bufferPhases_p         << std::endl;
    os << "-- buffer weights         = " << bufferWeights_p        << std::endl;
    os << "-- Geometrical delays     = " << GeomDelay::shape()     << std::endl;
    os << "                          = " << delays_p.shape()       << std::endl;
    os << "-- Geometrical phases     = " << GeomPhase::shape()     << std::endl;
    os << "                          = " << phases_p.shape()       << std::endl;
    os << "-- Geometrical weights    = " << GeomWeight::shape()    << std::endl;
    os << "                          = " << weights_p.shape()      << std::endl;
    os << "-- Beamformer weights     = " << shapeWeights()         << std::endl;
    os << "-- beamformed data shape  = " << shapeBeam()            << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                    setPhases

  void Beamformer::setDelays ()
  {
    GeomPhase::setDelays();
    setWeights();
  }
  
  //_____________________________________________________________________________
  //                                                                    setPhases

  void Beamformer::setPhases ()
  {
    GeomPhase::setPhases();
    setWeights();
  }
  
  //_____________________________________________________________________________
  //                                                                   setWeights

  /*!
    Due to the way the Beamformer class is derived on the underlying
    base-classes, the most memory efficient manner to compute the weights is to
    not store the underlying quantities but do directly compute everything from
    ground up.
  */
  void Beamformer::setWeights ()
  {
    /*
      Forward the function call to the method of the base class; required
      recursive calls are implemented through GeomWeight::setWeights().
    */
    GeomWeight::setWeights();
    
    /*
      In case the geometrical weights are buffered, we simply set up a reference
      to the array storing the values. If no buffering of the geometrical weights
      is done, we retrieve the values and store them here.
    */
    
    //________________________________________________________________
    //                                                  -- / -- / TRUE
    if (bufferWeights_p) {
      bfWeights_p.reference(weights_p);
    }
    //________________________________________________________________
    //                                                 -- / -- / FALSE
    else {
      /*
       * Adjust the size of the array storing the weights
       */
      IPosition nelem = GeomWeight::shape();
      bfWeights_p.resize(nelem);
      //______________________________________________________________
      //                                             -- / TRUE / FALSE
      if (bufferPhases_p) {
	std::cout << "[Beamformer::setWeights] -- / TRUE / FALSE" << std::endl;
	/*
	 * Phases are buffered, so we compute the weights from them.
	 *  shape(phases)  = [freq,ant,sky]
	 *  shape(weights) = [freq,ant,sky]
	 */
	int freq;
	int ant;
	int k;
	// compute the weights
	for (k=0; k<nelem(2); k++) {
	  for (ant=0; ant<nelem(1); ant++) {
	    for (freq=0; freq<nelem(0); freq++) {
	      bfWeights_p(freq,ant,k) = DComplex(cos(phases_p(freq,ant,k)),sin(phases_p(freq,ant,k)));
	    } // END : freq 
	  } // END : ant
	} // END : k
      }
      //______________________________________________________________
      //                                            -- / FALSE / FALSE
      else {
	Matrix<double> delays;
	//____________________________________________________________
	//                                        TRUE / FALSE / FALSE
	if (bufferDelays_p) {
#ifdef DEBUGGING_MESSAGES
	  std::cout << "[Beamformer::setWeights] TRUE / FALSE / FALSE" << std::endl;
#endif
	  delays.reference(delays_p);
	}
	//____________________________________________________________
	//                                       FALSE / FALSE / FALSE
	else {
#ifdef DEBUGGING_MESSAGES
 	  std::cout << "[Beamformer::setWeights] FALSE / FALSE / FALSE" << std::endl;
#endif
	  GeomDelay::delays(delays);
	}
	/*
	 * Since the delays depend on whether beamforming is performed in
	 * the near-field or the far-field, we leave the first step up to
	 * the GeomDelay class; everything from there on we compute here.
	 *  shape(delays)  = [ant,sky]
	 *  shape(phases)  = [freq,ant,sky]
	 *  shape(weights) = [freq,ant,sky]
	 */
	int freq;
	int ant;
	int sky;
	double phase;
	for (sky=0; sky<nelem(2); sky++) {
	  for (ant=0; ant<nelem(1); ant++) {
	    for (freq=0; freq<nelem(0); freq++) {
	      phase = CR::_2pi*frequencies_p(freq)*delays(ant,sky);
	      bfWeights_p(freq,ant,sky) = DComplex(cos(phase),sin(phase));
	    } // END : freq 
	  } // END : ant
	} // END : k
	
      }
    }

#ifdef DEBUGGING_MESSAGES
    std::cout << "-- nof. antennas  = " << GeomDelay::nofAntPositions() << std::endl;
    std::cout << "-- nof. pointings = " << GeomDelay::nofSkyPositions() << std::endl;
    std::cout << "-- shape(delays)  = " << GeomDelay::shape()           << std::endl;
    std::cout << "-- shape(phases)  = " << GeomPhase::shape()           << std::endl;
    std::cout << "-- shape(weights) = " << bfWeights_p.shape()          << std::endl;
    std::cout << "-- weights[,0,]   = " << bfWeights_p.xzPlane(0)       << std::endl;
#endif

    /*
     *  Update the shape information on the array returning the beamformed data
     */
    setSkymapType(skymapType_p);
    
  }
  
  //_____________________________________________________________________________
  //                                                                    checkData

  template <class T>
  bool Beamformer::checkData (casa::Matrix<T> &beam,
			      casa::Array<DComplex> const &data)
  {
    uint nofFailedChecks (0);
    IPosition shapeBeam    = beam.shape();
    IPosition shapeData    = data.shape();
    IPosition shapeWeights = bfWeights_p.shape();
    
    /*
      delays  = [antenna,direction]
      phases  = [channel,antenna,direction]
      weights = [channel,antenna,direction]
      data    = [channel,antenna,(antenna)]
      beam    = [channel,direction]
    */

    // Check the number of frequency channels in the data array

    if (shapeData(0) != shapeWeights(0)) {
      std::cerr << "[Beamformer::checkData] Mismatch in number of frequencies"
		<< std::endl;
      std::cerr << "-- shape(data)    = " << shapeData    << std::endl;
      std::cerr << "-- shape(weights) = " << shapeWeights << std::endl;
      // increment error counter
      nofFailedChecks++;
    }

    /*
      Check the number of antennas in the data array; be aware that we might
      need to do some additional checking in case we are working with ACM data
    */

    if (shapeData(1) != shapeWeights(1)) {
      std::cerr << "[Beamformer::checkData] Mismatch in number of antennas"
		<< std::endl;
      std::cerr << "-- data    : " << shapeData(1)    << std::endl;
      std::cerr << "-- weights : " << shapeWeights(1) << std::endl;
      // increment error counter
      nofFailedChecks++;
    }

    if (nofFailedChecks) {
      return false;
    } else {
      return true;
    }
  }
  
  //_____________________________________________________________________________
  //                                                                    beam_freq

  void Beamformer::beam_freq (casa::Vector<DComplex> &beamFreq,
			      casa::Array<DComplex> const &data,
			      uint const &direction,
			      bool const &normalize)
  {
    IPosition shape = data.shape();    // [freq,antenna,(antenna)]
    IPosition pos (shape.nelements()); // [freq,antenna,(antenna)]

    // loop over antennas
    for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
      // loop over frequency channels
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	beamFreq(pos(0)) = data(pos)*bfWeights_p(pos(1),direction,pos(0));
      }
    }

    /*
      (Optional) normalization step
    */
    if (normalize) {
      beamFreq /= DComplex(pos(1));
    }
  }
  
  //_____________________________________________________________________________
  //                                                                    beam_time

  /*!
    \f[ S (\vec\rho,t)
        = \mathcal{F}^{-1} \Bigl\{ \widetilde S (\vec\rho,\nu) \Bigr\}
    \f]
    
    \retval beamTime -- [sample] Vector with the time-series samples for a
            single beam towards the sky position \f$ \vec\rho \f$.
    \param data      -- 
    \param direction -- 
    \param normalize -- Normalize the values by the number of combined data
           channels? If set to <tt>true</tt> an additional factor of \f$1/N\f$
	   will be applied to the beamformed data.
  */
  void Beamformer::beam_time (casa::Vector<double> &beamTime,
			      casa::Array<DComplex> const &data,
			      uint const &direction,
			      bool const &normalize)
  {
    IPosition shape = data.shape();    // [freq,antenna,(antenna)]
    int blocksize   = 2*(shape(0)-1);
    casa::Vector<DComplex> beamFreq (shape(0));
    
    /* get the beam in the frequency domain */
    beam_freq (beamFreq,
	       data,
	       direction,
	       normalize);
    
    /* inverse Fourier transform to obtain the beam in the time domain */
    FFTServer<double,std::complex<double> > server(IPosition(1,blocksize),
						   casa::FFTEnums::REALTOCOMPLEX);
    server.fft (beamTime,beamFreq);
  }
  
  //_____________________________________________________________________________
  //                                                                   freq_field

  bool Beamformer::freq_field (casa::Matrix<DComplex> &beam,
			       const casa::Array<DComplex> &data)
  {
    bool status (true);
    // Shape of the array with the input data,         [freq,ant]
    IPosition index (data.shape());
    // Shape of the array with the beamformer weights, [freq,ant,sky]
    IPosition shape = bfWeights_p.shape();

    try {
      // Temporary storage of multiplication result
      casa::DComplex tmp;
      
      for (int sky(0); sky<shape(2); sky++) {                 /* loop: sky positions */
	for (index(0)=0; index(0)<shape(0); index(0)++) {     /* loop: frequencies   */
	  tmp = 0;
	  /*
	   * Sum up the complex-valued contributions from the individual antennas
	   */
	  for (index(1)=0; index(1)<shape(1); index(1)++) {
	    tmp += data(index)*bfWeights_p(index(0),index(1),sky);
	  }
	  /*
	   * Assign the pixel value: square of absolute value of the previously 
	   * computed sum.
	   */
	  beam(index(0),sky) = norm_p*tmp;
	} // END : frequency
      } // END : sky position
      
    } catch (std::string message) {
      std::cerr << "[Beamformer::freq_power] " << message << std::endl;
      status = false;
    }
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                                   freq_power

  /*!
    \retval beam -- [frequency,position] Beam formed from the provided input data.
    \param  data -- [frequency,antenna] Input data which will be processed to
            form a given type of beam; array containing the input data is
	    organized according to what is provided by the DataReader framework
    
    \return status   -- Status of the operation; returns <i>false</i> if an
            an error was encountered
  */
  bool Beamformer::freq_power (casa::Matrix<double> &beam,
			       const casa::Array<DComplex> &data)
  {
    bool status (true);
    // Shape of the array with the input data,         [freq,ant]
    IPosition index (data.shape());
    // Shape of the array with the beamformer weights, [freq,ant,sky]
    IPosition shape = bfWeights_p.shape();
    // Temporary storage of multiplication result
    casa::DComplex tmp;

    try {
      /* BEGIN: sky positions */
      for (int sky(0); sky<shape(2); sky++) {
	/* BEGIN: frequencies   */
	for (index(0)=0; index(0)<shape(0); index(0)++) {
	  tmp = 0;
	  /*
	   * Sum up the complex-valued contributions from the individual antennas
	   */
	  for (index(1)=0; index(1)<shape(1); index(1)++) {
	    tmp += data(index)*bfWeights_p(index(0),index(1),sky);
	  }
	  /*
	   * Assign the pixel value: square of absolute value of the previously 
	   * computed sum.
	   */
	  beam(index(0),sky) = norm_p*real(tmp*conj(tmp));
	} // END : frequency
      } // END : sky position
      
    } catch (std::string message) {
      std::cerr << "[Beamformer::freq_power] " << message << std::endl;
      status = false;
    }
    
#ifdef DEBUGGING_MESSAGES
    std::cout << "[Beamformer::freq_power]"                         << std::endl;
    std::cout << "-- IPosition(index) = " << index                  << std::endl;
    std::cout << "-- IPosition(shape) = " << shape                  << std::endl;
    std::cout << "-- shape(data)      = " << data.shape()           << std::endl;
    std::cout << "-- shape(weights)   = " << bfWeights_p.shape()    << std::endl;
    std::cout << "-- shape(beam)      = " << beam.shape()           << std::endl;
    std::cout << "-- Normalization    = " << norm                   << std::endl;
    std::cout << "-- weights [1,,]    = " << bfWeights_p.yzPlane(1) << std::endl;
    std::cout << "-- beam      [,1]   = " << beam.column(1)         << std::endl;
    std::cout << "-- beam      [1,]   = " << beam.row(1)            << std::endl;
#endif
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                                   time_field

  bool Beamformer::time_field (casa::Matrix<double> &beam,
			       const casa::Array<DComplex> &data)
  {
    bool status (true);

    std::cerr << "[Beamformer::time_field] Method not yet implemented!"
	      << std::endl;
    std::cout << "-- shape(data) = " << data.shape() << std::endl;
    std::cout << "-- shape(beam) = " << beam.shape() << std::endl;
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                                   time_power

  /*!
    \retval beam -- [frequency,position] Beam formed from the provided input data.
    \param  data -- [frequency,antenna] Input data which will be processed to
            form a given type of beam; array containing the input data is
	    organized according to what is provided by the DataReader framework
    
    \return status   -- Status of the operation; returns <i>false</i> if an
            an error was encountered
  */
  bool Beamformer::time_power (casa::Matrix<double> &beam,
			       const casa::Array<DComplex> &data)
  {
    bool status (true);

    std::cerr << "[Beamformer::time_power] Method not yet implemented!"
	      << std::endl;
    std::cout << "-- shape(data) = " << data.shape() << std::endl;
    std::cout << "-- shape(beam) = " << beam.shape() << std::endl;
    
    return status;
  }

  //_____________________________________________________________________________
  //                                                                      time_cc

  /*!
    \retval beam -- [frequency,position] Beam formed from the provided input data.
    \param  data -- [frequency,antenna] Input data which will be processed to
            form a given type of beam; array containing the input data is
	    organized according to what is provided by the DataReader framework
    
    \return status   -- Status of the operation; returns <i>false</i> if an
            an error was encountered
  */
  bool Beamformer::time_cc (casa::Matrix<double> &beam,
			    const casa::Array<DComplex> &data)
  {
    bool status (true);
    
    try {
      
      casa::IPosition shape = bfWeights_p.shape();  //  [freq,antenna,direction]
      double nofBaselines   = GeomDelay::nofBaselines();
      int direction (0);
      int ant2 (0);
      casa::IPosition pos (2);
      casa::Vector<DComplex> tmpFreq (shape(0),0.0);
      casa::Vector<double>   tmpTime (shapeBeam_p(0),0.0);
      casa::Matrix<double>   tmpData (shape(1),shapeBeam_p(0));
      
      /*
	Set up the casa::FFTServer which is to handle the inverse Fourier
	transform taking place before the summation step.
      */
      FFTServer<double,std::complex<double> > server(IPosition(1,shapeBeam_p(0)),
						     casa::FFTEnums::REALTOCOMPLEX);
      // resize array returning the beamformed data
      beam.resize (shapeBeam_p(0),shape(2),0.0);
      
      for (direction=0; direction<shape(2); direction++) {
	// Precompute the shifted time series for a given sky position
	for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	  // --- Apply the beamformer weights to the Fourier-transformed data
	  for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	    tmpFreq(pos(0)) = data(pos)*bfWeights_p(pos(0),pos(1),direction);
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
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                                       time_p

  bool Beamformer::time_p (casa::Matrix<double> &beam,
			   const casa::Array<DComplex> &data)
  {
    bool status (true);

    if (checkData (beam,data)) {
      
      int direction(0);
      casa::IPosition pos(2);                       // [freq,antenna]
      casa::IPosition shape = bfWeights_p.shape();  // [freq,antenna,direction]
      casa::Vector<DComplex> tmpFreq (shape(0));
      casa::Vector<double> tmpTime (shapeBeam_p(0));
      casa::Vector<double> sum (shapeBeam_p(0));
      FFTServer<double,DComplex> server(casa::IPosition(1,shapeBeam_p(0)),
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
	    tmpFreq(pos(0)) = data(pos)*bfWeights_p(pos(0),pos(1),direction);
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
  
  //_____________________________________________________________________________
  //                                                                       time_x

  bool Beamformer::time_x (casa::Matrix<double> &beam,
			   const casa::Array<DComplex> &data)
  {
    bool status (true);

    std::cerr << "[Beamformer::time_x] Method not yet implemented!" << std::endl;
    std::cout << "-- shape(data) = " << data.shape() << std::endl;
    std::cout << "-- shape(beam) = " << beam.shape() << std::endl;
    
    return status;
  }
  
} // Namespace CR -- end
