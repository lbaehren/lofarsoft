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

#ifndef CR_BEAMFORMER_H
#define CR_BEAMFORMER_H

// Standard library header files
#include <iostream>
#include <string>

// casacore header files
#include <casa/Arrays/Array.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>

// CR-Tools header files
#include <Imaging/GeometricalWeight.h>
#include <Imaging/SkymapQuantity.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \brief Correlation types for polarized data
    
    Since by default we are dealing with dual-polarization data, we have a number
    of choices on how to combine the individual signals.
  */
  typedef enum {
    NONE,
    I,
    Q,
    U,
    V,
    RR,
    RL,
    LR,
    LL,
    XX,
    XY,
    YX,
    YY
  } CorrelationTypes;
  
  /*!
    \class Beamformer
    
    \ingroup CR_Imaging
    
    \brief Brief description for class Beamformer
    
    \author Lars B&auml;hren
    
    \date 2007/06/13
    
    \test tBeamformer.cc
    
    <h3>Prerequisite</h3>

    <ul type="square">
      <li>GeometricalDelay
      <li>GeometricalPhase
      <li>GeometricalWeight
      <li>SkymapQuantity
    </ul>
    
    <h3>Synopsis</h3>

    <ul type="square">
      <li><b>Beamformer weights</b>
      In order to phase up the signal \f$ s_j(t) \f$ from antenna \f$j\f$ located
      at position \f$ \vec x_j \f$, we multiply its Fourier transform 
      \f$ \widetilde s_j (\nu) \f$ with a complex weight factor
      \f$ w_j (\vec\rho,\nu) \f$ (corresponding to a shift in the time domain):
      \f[ w_{j} (\vec\rho,\nu) = w_{\rm geom} (\vec x_j,\vec\rho,\nu) \cdot
      w_{j,\rm gain} (\vec\rho,\nu) \f]
      In this \f$ w_{\rm geom} (\vec x_j,\vec\rho,\nu) \f$ is the component which
      is defined completely by the geometrical setup -- antenna position and pointing
      position -- and the observation frequency. The additional factor
      \f$ w_{j,\rm gain} (\vec\rho,\nu) \f$ accounts for the direction- and
      frequency-dependent reception pattern of the antena, which is either known
      EM simulation of the antenna or calibration measurements.
      
      In the default case -- where no antenna gains,
      \f$ w_{j,\rm gain} (\vec\rho,\nu) \f$, are provided -- this class will do
      nothing else but buffer the calculated geometrical weights
      \f$ w_{\rm geom} (\vec x_j,\vec\rho,\nu) \f$.
    </ul>

    <h3>Adding new beamforming methods</h3>

    Implementation of a new beamforming methods requires three entries within
    this class:
    <ol>
      <li>Add an entry to the CR::SkymapQuantity class, as this will be used
      internally for managing function calls and redirection from the generic
      interface.
      <li>The function implementing the new beam type, using the interface
      of Beamformer::processData.
      \code
      bool time_cc (casa::Matrix<double> &beam,
                    const casa::Array<DComplex> &data);
      \endcode
      <li>Add entry to Beamformer::setBeamType in order to be able to selected
      the new beam type.
      \code
      bool Beamformer::setBeamType (BeamType const &beam)
      {
        bool status (true);
	
	switch (beam) {
	case TIME_CC:
	  beamType_p    = beam;
	  processData_p = &Beamformer::time_cc;
	  break;
	}

	return status;
      }
      \endcode
    </ol>

    <h3>Example(s)</h3>

    <ol>
      <li>Use and existing object handling the geometrical weights in order
      to set up a new Beamformer:
      \code
      // [1] create an object handling the geometrical weights
      GeometricalWeight weight (get_antennaPositions(),
                                get_skyPositions(),
				get_frequencies());

      // [2] create new Beamformer from the previously created GeometricalWeights
      Beamformer bf (weight);
      \endcode
    </ol>    
  */  
  class Beamformer : public GeometricalWeight {
    
    //! Type of beamforming method used in data processing
    SkymapQuantity skymapType_p;
    
    /*!
      \brief Pointer to the function performing the beamforming
     
      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
              an error was encountered
    */
    bool (Beamformer::*processData_p) (casa::Matrix<double> &beam,
				       const casa::Array<DComplex> &data);
    
    //! The weights applied by the Beamformer
    casa::Cube<DComplex> bfWeights_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    Beamformer ();
    
    /*!
      \brief Argumented constructor

      \param weights -- Object storing the geometrical weights which are required
             for the Beamformer
    */
    Beamformer (GeometricalWeight const &weights);
    
    /*!
      \brief Argumented constructor using existing GeometricalPhase object
      
      \param phase -- GeometricalPhase object encapsulating the functionality
             on top of which this class builds.
      \param bufferWeights -- Buffer the values of the geometrical weights?
    */
    Beamformer (GeometricalPhase const &phase,
		bool const &bufferWeights=false);
    
    /*!
      \brief Argumented constructor using existing GeometricalDelay object
      
      \param delay -- GeometricalDelay object encapsulating the functionality
             on top of which this class builds.
      \param frequencies  -- Frequencies for which the geometrical delays are
             converted into phases, [Hz]
      \param bufferPhases -- Buffer the values of the phases?
      \param bufferWeights -- Buffer the values of the geometrical weights?
    */
    Beamformer (GeometricalDelay const &delay,
		casa::Vector<double> const &frequencies,
		bool const &bufferPhases=false,
		bool const &bufferWeights=false);
    
    /*!
      \brief Argumented constructor
      
      \param antPositions -- [nofAntennas,3] Antenna positions for which the
             delay is computed, given in Cartesian coordinates \f$ (x,y,z) \f$
      \param antCoordType -- CR::CoordinateType::Types of the antenna position
             coordinates; if the coordinates are non-cartesian and thereby
	     include anglular components, the values must be provided in radians.
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
             which to point, given in the same reference frame as the antenna
	     positions, \f$ (x,y,z) \f$
      \param skyCoordType -- CR::CoordinateType::Types of the sky position coordinates;
             if the coordinates are non-cartesian and thereby include anglular
	     components, the values must be provided in radians.
      \param frequencies  -- Frequencies for which the geometrical delays are
             converted into phases
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna and
	     sky positions and afterwards kept in memory; if set <i>no</i> only
	     the input parameters are stored an no further action is taken.
      \param bufferPhases -- Buffer the values of the phases?
      \param bufferWeights -- Buffer the values of the geometrical weights?
    */
    Beamformer (casa::Matrix<double> const &antPositions,
		CR::CoordinateType::Types const &antCoordType,
		casa::Matrix<double> const &skyPositions,
		CR::CoordinateType::Types const &skyCoordType,
		casa::Vector<double> const &frequencies,
		bool const &bufferDelays=false,
		bool const &bufferPhases=false,
		bool const &bufferWeights=false);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Beamformer object from which to create this new
      one.
    */
    Beamformer (Beamformer const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~Beamformer ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Beamformer object from which to make a copy.
    */
    Beamformer& operator= (Beamformer const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the type of beam to be used at processing

      \return beamType -- The type of beam to be used at data processing
     */
    inline SkymapQuantity skymapType () const {
      return skymapType_p;
    }

    /*!
      \brief Get the type of the beam type to be used at processing

      \return beamType -- The type of beam to be used at data processing
     */
    inline CoordinateType::Types domainType () {
      return skymapType_p.domainType();
    }
    
    /*!
      \brief Get the name of the beam type to be used at processing

      \return beamType -- The type of beam to be used at data processing
     */
    inline std::string domainName () {
      return skymapType_p.domainName();
    }
    
    /*!
      \brief Set the type of beam to be used at data processing
      
      \param beam -- The BeamType to be used at data processing
      
      \return status -- Status of the operation; returns <i>false</i> if an
              an error was encountered
    */
    bool setSkymapType (SkymapQuantity const &skymapType);
    
    /*!
      \brief Set the type of beam to be used at data processing
      
      \param beam -- The BeamType to be used at data processing
      
      \return status -- Status of the operation; returns <i>false</i> if an
              an error was encountered
    */
    inline bool setSkymapType (SkymapQuantity::Type const &skymapType) {
      return setSkymapType (SkymapQuantity(skymapType));
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Beamformer.
    */
    std::string className () const {
      return "Beamformer";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);
    
    // ------------------------------------------------------------------ Methods

    /*!
      \brief Set values for the antenna gains, \f$ w_{j,\rm gain} (\vec\rho,\nu) \f$

      \param gains -- [freq,antenna,direction] Array with the complex antenna
             gains, \f$ w_{j,\rm gain} (\vec\rho,\nu) \f$, describing the beam
	     pattern of the antennas as fucntions of direction and frequency
    */
    bool setAntennaGains (casa::Cube<DComplex> const &gains);
    
    /*!
      \brief Unset the values for the antenna gains
      
      Calling this function will remove the effect of the antenna gain calibration,
      i.e. restricting the beamformer weights to the geometrical weights only.
    */
    bool unsetAntennaGains ();    
    
    /*!
      \brief Beamforming of the data, returning real-valued result

      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    template <class T>
    bool processData (casa::Matrix<T> &beam,
		      const casa::Array<DComplex> &data);
    
    /*!
      \brief Directed field as function of frequency, \f$ \widetilde S (\vec\rho,\nu) \f$

      <b>Note:</b> This function cannot be called via the <tt>processData</tt>
      method, since the returned array consists of complex values.
      
      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    bool freq_field (casa::Matrix<DComplex> &beam,
		     const casa::Array<DComplex> &data);
    
    /*!
      \brief Form a cross-correlation beam
      
      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    bool time_cc (casa::Matrix<double> &beam,
		  const casa::Array<DComplex> &data);
    
    /*!
      \brief Form a power-beam (p-beam)

      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    bool time_p (casa::Matrix<double> &beam,
		 const casa::Array<DComplex> &data);
    
    /*!
      \brief Form an excess-beam (x-beam)

      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    bool time_x (casa::Matrix<double> &beam,
		 const casa::Array<DComplex> &data);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (Beamformer const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Initialize internal settings of the Beamformer
    */
    void init ();

    /*
      \brief Set the values of the Beamformer weights
    */
    bool setBeamformerWeights ();
    
    /*
      \brief Set the values of the Beamformer weights

      \param gains -- [freq,antenna,direction] Array with the complex antenna
             gains, \f$ w_{j,\rm gain} (\vec\rho,\nu) \f$, describing the beam
	     pattern of the antennas as fucntions of direction and frequency
    */
    bool setBeamformerWeights (casa::Cube<DComplex> const &gains);

    /*!
      \brief Check if the input data are consistent with the internal settings

      Currently the checking is restricted to the data array; to also check the 
      shape of the array returning the beamformed data, we need to take into
      consideration the beamforming method, because depending on whether the 
      result is in the frequency or time domain, the length of the first axis
      will vary.

      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    template <class T>
    bool checkData (casa::Matrix<T> &beam,
		    casa::Array<DComplex> const &data);

    /*!
      \brief Compute single beam in frequency space

      \f[
      \widetilde S (\vec\rho,\nu)
      = \frac{1}{N} \sum_{j=1}^{N} \widetilde s_{j}(\vec\rho,\nu)
      = \frac{1}{N} \sum_{j=1}^{N} w_{j}(\vec\rho,\nu) \widetilde s_{j}(\nu)
      \f]

      \retval beamFreq -- [freq] Vector with the frequency channels for a single
              beam towards the sky position \f$ \vec\rho \f$.
      \param data      -- 
      \param direction -- 
      \param normalize -- Normalize the values by the number of combined data
             channels? If set to <tt>true</tt> an additional factor of \f$1/N\f$
	     will be applied to the beamformed data.
    */
    void beam_freq (casa::Vector<DComplex> &beamFreq,
		    casa::Array<DComplex> const &data,
		    uint const &direction,
		    bool const &normalize=true);
    
    /*!
      \brief Compute single beam in the time domain

      \f[
      S (\vec\rho,t)
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
    void beam_time (casa::Vector<double> &beamTime,
		    casa::Array<DComplex> const &data,
		    uint const &direction,
		    bool const &normalize=true);
    
    /*!
      \brief Directed spectral power, \f$ \widetilde P (\vec\rho,\nu) \f$

      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    bool freq_power (casa::Matrix<double> &beam,
		     const casa::Array<DComplex> &data);

    /*!
      \brief Directed field strength time-series, \f$ S (\vec\rho,t) \f$
    */
    bool time_field (casa::Matrix<double> &beam,
		     const casa::Array<DComplex> &data);
    
    /*!
      \brief Directed power time series, \f$ S (\vec\rho,t) \f$

      \retval beam -- [frequency,position] Beam formed from the provided input data.
      \param  data -- [frequency,antenna] Input data which will be processed to
              form a given type of beam; array containing the input data is
	      organized according to what is provided by the DataReader framework

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    bool time_power (casa::Matrix<double> &beam,
		     const casa::Array<DComplex> &data);
    
  };
  
} // Namespace CR -- end

#endif /* BEAMFORMER_H */
  
