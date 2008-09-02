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

#include <casa/Arrays/Array.h>

#include <Imaging/GeometricalWeight.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \brief List of implemented and supported beam types.
  */
  typedef enum {
    /*!
      Electric field strength as function of frequency,
      \f[ \widetilde S (\vec\rho,\nu) = \frac{1}{N} \sum_{j=1}^{N}
      w_{j}(\vec \rho,\nu) \widetilde{s_{j}}(\nu) \f]
    */
    FREQ_FIELD,
    /*!
      <b>Directed spectral power</b> (power of the electric field as function
      of frequency) <br>
      \f[ \widetilde P (\vec\rho,\nu) = \overline{\widetilde S (\vec\rho,\nu)}
      \cdot \widetilde S (\vec\rho,\nu) \f]
      where 
      \f[ \widetilde S (\vec\rho,\nu) = \frac{1}{N_{\rm Ant}}
      \sum_{j=1}^{N_{\rm Ant}} \widetilde S_{j} (\vec\rho,\nu) =
      \frac{1}{N_{\rm Ant}} \sum_{j=1}^{N_{\rm Ant}} w (\vec x_j, \vec \rho, \nu)
      \widetilde s_{j} (\nu) \f]
      in which \f$ w \f$ is the weighting factor for each combination of antenna,
      pointing direction and frequency and \f$ \widetilde s_j \f$ is the Fourier
      transform of the data from antenna \f$ j \f$.
      
      While in the above step the beam beam is computed by summation over all 
      selected antennas, there also is the option to sum over antenna pairs,
      i.e. baselines
      \f[ \widetilde P (\vec\rho,\nu) = \left( \frac{N^2-N}{2} \right)^{-1}
      \sum_{j=1}^{N} \sum_{k>j}^{N} \overline{w_{j}(\vec\rho,\nu)
      \widetilde s_{j}(\nu)} w_{k}(\vec\rho,\nu) \widetilde s_{k}(\nu) \f]

      Instead of computing the beam from the spectra \f$ \widetilde s_j (\nu) \f$
      of the individual antennas, the directed power also can be computed from
      the array correlation matrix (ACM), \f$ C_{jk}(\nu) =
      \overline{\widetilde s_j(\nu)} \widetilde s_k(\nu) \f$.
      \f[ \widetilde P (\vec\rho,\nu) = \left( \frac{N^2-N}{2} \right)^{-1}
      \sum_{j=1}^{N} \sum_{k>j}^{N} \overline{w_{j}(\vec\rho,\nu)}
      w_{k}(\vec\rho,\nu) \, C_{jk}(\nu) \f]
    */
    FREQ_POWER,
    /*!
      Electric field strength as function of time (sample resolution)

      \f[ S (\vec\rho,t) = \mathcal{F}^{-1} \left\{ \widetilde S(\vec\rho,\nu)
      \right\} \f]
    */
    TIME_FIELD,
    /*!
      Power of the electric field as function of time

      \f[ P (\vec\rho,\nu) = \left| S (\vec\rho,t) \right|^{2} \f]
    */
    TIME_POWER,
    /*!
      <b>Cross-correlation beam</b> (cc-beam)<br>
      The data from each unique pair of antennas is multiplied, the resulting
      values are averaged, and then the square root is taken while preserving
      the sign.
      \f[
      cc(\vec \rho)[t] = \, ^+_- \sqrt{\left| \frac{1}{N_{Pairs}} \sum^{N-1}_{i=1}
      \sum^{N}_{j>i} s_i(\vec\rho)[t] s_j(\vec\rho)[t] \right|}
      \f]
      where 
      \f[
      s_j(\vec\rho)[t]
      = \mathcal{F}^{-1} \left\{ \tilde s_j(\vec\rho)[k] \right\}
      = \mathcal{F}^{-1} \Bigl\{ w_j(\vec\rho)[k] \cdot \tilde s_j[k] \Bigr\}
      \f]
      is the time shifted field strength of the single antennas for a direction
      \f$\vec \rho \f$. \f$ N \f$ is the number of antennas, \f$t\f$ the time or
      pixel index and \f$N_{Pairs}\f$ the number of unique pairs of antennas.
      The negative sign is taken if the sum had a negative sign before taking the
      absolute values, and the positive sign otherwise.
      Computation - for each direction in the sky - is performed as follows:
      <ol>
      <li>Compute the shifted time-series for all antennas,
      \f$ s_j (\vec\rho,t) \f$, by first applying the weights to the Fourier
      transformed data and thereafter transforming back to time domain.
      <li>Sum over unique products from all antenna pairs and normalize by
      the number of such pairs.
      <li>Take the square root of the sum and multiply with the sign of the
      some.
      </ol>       */
    TIME_CC,
    /*!
      <b>Power-beam</b> (p-beam)
      \f[
      P (\vec\rho,\nu)
      \ = \ \sqrt{\frac{1}{N} \sum_{j=1}^{N} \left( s_{j}(\vec\rho,\nu) \right)^2}
      \ = \ \sqrt{\frac{1}{N} \sum_{j=1}^{N} \left( \mathcal{F}^{-1} \Bigl\{
      \widetilde s_{j}(\vec\rho,\nu) \Bigr\} \right)^{2}} 
      \ = \ \sqrt{\frac{1}{N} \sum_{j=1}^{N} \left( \mathcal{F}^{-1} \Bigl\{
      w_{j}(\vec\rho,\nu) \widetilde s_{j}(\nu) \Bigr\} \right)^{2}} 
      \f]
    */
    TIME_P,
    /*!
      <b>Excess-beam</b> (x-beam)
      
      \f[
      P_{x}(\vec\rho,t) = P_{cc}(\vec\rho,t) \cdot \left| \frac{\langle
      P_{cc}(\vec\rho,t) \rangle}{\langle P(\vec\rho,t) \rangle} \right|
      \f]
    */
    TIME_X
  } BeamType;

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
      <li>Add an entry to the CR::BeamType enumeration, as this will be used
      internally for managing function calls and redirection from the generic
      interface.
      \code
      typedef enum {
        // some entries
	TIME_CC,
	// some more entries
      } BeamType;
      \endcode
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
      <li><i>(Optional, but recommended)</i> Register the new BeamType with the
      Beamformer::beamTypeName function; this will enable retriving the name
      of newly implemented beamforming method instead only the number from the
      CR::BeamType enumeration.
      \code
      static std::string beamTypeName (BeamType const &beamType)
      {
        std::string name;
      
	switch (beamType) {
	  case TIME_CC:
	  name = "TIME_CC";
	  break;
	}

	return name;
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
    BeamType beamType_p;
    
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
    
    /*!
      \brief Default constructor
    */
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
    inline BeamType beamType () {
      return beamType_p;
    }

    /*!
      \brief Get the name of the beam type to be used at processing

      \return beamType -- The type of beam to be used at data processing
     */
    inline std::string beamTypeName () {
      return beamTypeName(beamType_p);
    }
    
    /*!
      \brief Get the name of the beam type to be used at processing
      
      \param beamType -- The type of beam to be used at data processing
      
      \return name -- The name of the beam type to be used at data processing
    */
    static std::string beamTypeName (BeamType const &beamType);
    
    /*!
      \brief Set the type of beam to be used at data processing
      
      \param beam -- The BeamType to be used at data processing
      
      \return status -- Status of the operation; returns <i>false</i> if an
              an error was encountered
    */
    bool setBeamType (BeamType const &beam);
    
    /*!
      \brief Convert combination of domain and quantity label to BeamType
      
      \retval beamType -- BeamType identifier
      \param domain -- Domain (time, freq) in which the beamformed data will
             reside.
      \param quantity -- (EM) Quanity the beamformed data are going to represent

      \return status -- Status of the operation; returns <i>false</i> if an
              an error was encountered
    */
    static bool beamType (BeamType &beamType,
			  string const &domain,
			  string const &quantity);
    
    /*!
      \brief Convert BeamType to combination of domain and quantity label
      
      \retval domain   -- 
      \retval quantity -- 
      \param beamType  -- 

      \return status -- Status of the operation; returns <i>false</i> if an
              an error was encountered
    */
    static bool beamType (std::string &domain,
			  std::string &quantity,
			  BeamType const &beamType)
      {
	bool ok (true);
	
	try {
	  switch (beamType) {
	  case TIME_FIELD:
	    domain   = "TIME";
	    quantity = "FIELD";
	    break;
	  case TIME_POWER:
	    domain   = "TIME";
	    quantity = "POWER";
	    break;
	  case TIME_CC:
	    domain   = "TIME";
	    quantity = "CC";
	    break;
	  case TIME_P:
	    domain   = "TIME";
	    quantity = "P";
	    break;
	  case TIME_X:
	    domain   = "TIME";
	    quantity = "X";
	    break;
	  case FREQ_POWER:
	    domain   = "FREQ";
	    quantity = "POWER";
	    break;
	  case FREQ_FIELD:
	    domain   = "FREQ";
	    quantity = "FIELD";
	    break;
	  }
	} catch (std::string message) {
	  std::cerr << "[Beamformer::beamType] " << message << std::endl;
	  ok = false;
	}
	
	return ok;
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
  
