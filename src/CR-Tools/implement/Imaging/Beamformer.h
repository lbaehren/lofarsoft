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
#include <Imaging/AntennaGain.h>
#include <Imaging/GeomWeight.h>
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
      <li>GeomDelay
      <li>GeomPhase
      <li>GeomWeight
      <li>SkymapQuantity
      <li>AntennaGain
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

    In order to consistently implement the functionality for a new beamforming
    method a number of changes are required (here using the cross-correlation
    beam as an example):
    <ol>
      <li>Add an entry to the CR::SkymapQuantity class, as this will be used
      internally for managing function calls and redirection from the generic
      interface.
      \code
      typedef enum {
        // Definition of the various other methods
	// [...]
        // Cross-correlation beam (cc-beam)
        TIME_CC
      } Type;
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
	  case SkymapQuantity::TIME_CC:
	    // Register the type of the skymap quantity
	    skymapType_p  = skymapType;
	    // Register the shape of the array with the beamformed data
	    shapeBeam_p   = IPosition(2,blocksize,shape_of_weights(2));
	    // Direct the data processing function to the actual implementation
	    processData_p = &Beamformer::time_cc;
	    break;
	}

	return status;
      }
      \endcode
    </ol>

    <h3>Example(s)</h3>

    <ol>
      <li>A new Beamformer object can be created based on object of any of its
      base classes; the least number of new argumentes is required when starting
      out with geometrical weights:
      \code
      // Creation of object to handle the geometrical weights
      GeomWeight w (antPositions,
                    CR::CoordinateType::Cartesian,
		    skyPositions,
		    CR::CoordinateType::Cartesian,
		    anglesInDegrees,
		    farField,
		    bufferDelays,
		    frequencies,
		    bufferPhases,
		    bufferWeights);

      // Create Beamformer
      Beamformer bf (w);
      \endcode
      <li>In order to avoid unnecessary checking and resizing of arrays, the
      array returning the beamformed data needs to be properly dimensioned before
      calling Beamformer::processData
      \code
      Array<double> beam;
      // Select the type of beam to compute from the data
      bf.setSkymapType(SkymapQuantity::FREQ_POWER);
      // Bring the array to hold the beamformed up to the correct shape
      beam.resize(bf.shapeBeam());
      \endcode
    </ol>    
  */  
  class Beamformer : public GeomWeight {
    
    //! Type of beamforming method used in data processing
    SkymapQuantity skymapType_p;
    //! Shape of the array with the beamformed data
    IPosition shapeBeam_p;
    //! The weights applied by the Beamformer
    casa::Cube<DComplex> bfWeights_p;
    //! Complex-valued antenna gains per antenna
    AntennaGain antennaGains_p;
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
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    Beamformer ();
    
    /*!
      \brief Argumented constructor

      \param weights -- Object encapulating the geometrical weights applied by
             the Beamformer.
    */
    Beamformer (GeomWeight const &weights);
    
    /*!
      \brief Argumented constructor

      \param phases        -- Object encapsulating the geometrical phases from
             which the geometrical weights applied by the Beamformer are derived.
      \param bufferWeights -- Buffer the values of the geometrical weights?
    */
    Beamformer (GeomPhase const &phases,
		bool const &bufferWeights=true);
    
    /*!
      \brief Argumented constructor

      \param geomDelay     -- Object encapsulating the geometrical delays from
             which the geoometrical weights applied by the Beamformer are derived.
      \param frequencies   --
      \param bufferPhases  -- 
      \param bufferWeights -- Buffer the values of the weights?
    */
    Beamformer (GeomDelay const &geomDelay,
		Vector<double> const &frequencies,
		bool const &bufferPhases=false,
		bool const &bufferWeights=false);
    
    /*!
      \brief Argumented constructor
      
      \param geomDelay     -- 
      \param frequencies   --
      \param bufferPhases  -- 
      \param bufferWeights -- Buffer the values of the weights?
    */
    Beamformer (GeomDelay const &geomDelay,
		Vector<MVFrequency> const &frequencies,
		bool const &bufferPhases=false,
		bool const &bufferWeights=false);
    
    /*!
      \brief Argumented constructor

      \param antPositions -- [antenna,3], positions of the antennas
      \param antCoord     -- 
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param skyCoord     --
      \param anglesInDegrees -- If the coordinates of the antenna positions
             contain an angular component, is this component given in degrees?
	     If set <tt>false</tt> the angular components are considered to be
	     given in radians.
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
      \param frequencies  -- The frequencies, [Hz], for which the phases are
             being computed.
      \param bufferPhases -- Buffer the values of the phases?
      \param bufferWeights -- Buffer the values of the weights?
    */
    Beamformer (Matrix<double> const &antPositions,
		CoordinateType::Types const &antCoord,
		Matrix<double> const &skyPositions,
		CoordinateType::Types const &skyCoord,
		bool const &anglesInDegrees,
		bool const &farField,
		bool const &bufferDelays,
		Vector<double> const &frequencies,
		bool const &bufferPhases,
		bool const &bufferWeights=false);
    
    /*!
      \brief Argumented constructor

      \param antPositions -- Positions of the antennas
      \param skyPositions -- Positions towards which the beams are formed
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
      \param frequencies  -- The frequencies for which the phases are being
             computed.
      \param bufferPhases -- Buffer the values of the phases?
      \param bufferWeights -- Buffer the values of the weights?
    */
    Beamformer (Vector<MVPosition> const &antPositions,
		Vector<MVPosition> const &skyPositions,
		bool const &farField,
		bool const &bufferDelays,
		Vector<MVFrequency> const &frequencies,
		bool const &bufferPhases,
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

      \param os -- Output stream to which the summary is being written.
    */
    void summary (std::ostream &os);
    
    // ------------------------------------------------------------------ Methods

    /*!
      \brief Get the shape of the array holding the Beamformer weights

      \return shape --
    */
    inline IPosition shapeWeights () const {
      return bfWeights_p.shape();
    }

    /*!
      \brief Get the shape of the array with the beamformed data
      
      \return shape -- Shape of the array with the beamformed data
    */
    inline IPosition shapeBeam () const {
      return shapeBeam_p;
    }
    
    /*!
      \brief Get the complex-valued antenn gains

      \return gains -- Container for the complex-values antennas gains,
              \f$ w_{i,\rm Gain} (\vec \rho, \nu) \f$, as function of pointing 
	      direction and frequency.
    */
    inline AntennaGain antennaGain () const {
      return antennaGains_p;
    }

    /*!
      \brief Set the complex-valued antenn gains

      \param gains -- Container for the complex-values antennas gains,
             \f$ w_{i,\rm Gain} (\vec \rho, \nu) \f$, as function of pointing 
	     direction and frequency.
    */
    bool setAntennaGain (AntennaGain const &gains);
    
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
			const casa::Array<DComplex> &data)
      {
	if (checkData(beam,data)) {
	  return (this->*processData_p) (beam,data);
	} else {
	  return false;
	}
      }
    
  protected:
    
    //! Compute and set the values of the geometrical delays
    void setDelays ();
    
    //! Compute and set the values of the geometrical phases
    void setPhases ();
    
    //! Compute and set the values of the geometrical weights
    void setWeights ();
    
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

    //! Initialize internal settings of the Beamformer
    void init ();

    /*
      \brief Set the values of the Beamformer weights

      \param geomWeights -- [freq,antenna,direction] Array with the complex
             geometrical weights.
      \param gainWeights -- [freq,antenna,direction] Array with the complex
             antenna gains, \f$ w_{j,\rm gain} (\vec\rho,\nu) \f$, describing
	     the beam pattern of the antennas as fucntions of direction and
	     frequency.
    */
    void setWeights (casa::Cube<DComplex> const &geomWeights,
		     casa::Cube<DComplex> const &gainWeights);

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
  
