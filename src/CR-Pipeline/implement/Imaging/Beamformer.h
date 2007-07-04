/*-------------------------------------------------------------------------*
 | $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
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

#ifndef BEAMFORMER_H
#define BEAMFORMER_H

// Standard library header files
#include <iostream>
#include <string>

#include <Imaging/GeometricalWeight.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Beamformer

    \ingroup Imaging
    
    \brief Brief description for class Beamformer
    
    \author Lars B&auml;hren

    \date 2007/06/13

    \test tBeamformer.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Familiarity of beamforming with phased arrays.
    </ul>
    
    <h3>Synopsis</h3>

    Implementation of a new beamforming methods requires three entries within
    this class:
    <ol>
      <li>Add an entry to the enumeration of the beam-types
      <li>The function implementing the new beam type, using the interface
          of Beamformer::processData.
      <li>Add entry to Beamformer::setBeamType in order to be able to selected
          the new beam type.
    </ol>
    
    <h3>Example(s)</h3>
    
  */  
  class Beamformer : public GeometricalWeight {
    
  public:
    
    /*!
      \brief List of implemented and supported beam types.
    */
    typedef enum {
      /*!
	Add up the Fourier-transformed antennas signals
      */
      ADDANTENNAS,
      /*!
	Add up the Fourier-transformed antenna signals after forming of baselines
      */
      ADDBASELINES,
      /*!
	Cross-correlation beam
      */
      CCBEAM,
      XBEAM
    } BeamType;
    
  private:

    //! Type of beamforming method used in data processing
    Beamformer::BeamType beamType_p;
    
    /*!
      \brief Pointer to the function performing the beamforming
     
      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool (Beamformer::*processData_p) (casa::Matrix<double> &beam,
				       const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool (Beamformer::*processData_p) (blitz:Array<double,2> &beam,
				       const blitz:Array<complex<double>,2> &data);
#endif
#endif
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Beamformer ();
    
    /*!
      \brief Argumented constructor
      
      \param antPositions  -- [nofAntennas,3] Antenna positions for which the
                              delay is computed, \f$ (x,y,z) \f$
      \param skyPositions  -- [nofSkyPositions,3] Positions in the sky towards
                              which to point, given in the same reference frame
			      as the antenna positions, \f$ (x,y,z) \f$
      \param frequencies   -- Frequencies for which the geometrical delays are
                              converted into phases
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
                              <i>yes</i> the delays will be computed from the 
			      provided antenna and sky positions and afterwards
			      kept in memory; if set <i>no</i> only the input 
			      parameters are stored an no further action is taken.
      \param bufferPhases  -- Buffer the values of the phases?
      \param bufferWeights -- Buffer the values of the geometrical weights?
    */
#ifdef HAVE_CASA
    Beamformer (casa::Matrix<double> const &antPositions,
		casa::Matrix<double> const &skyPositions,
		casa::Vector<double> const &frequencies,
		bool const &bufferDelays=false,
		bool const &bufferPhases=false,
		bool const &bufferWeights=false);
#else
#ifdef HAVE_BLITZ
    Beamformer (const blitz::Array<double,2> &antPositions,
		const blitz::Array<double,2> &skyPositions,
		blitz::Array<double,1> const &frequencies,
		bool const &bufferDelays=false,
		bool const &bufferPhases=false,
		bool const &bufferWeights=false);
#endif
#endif
    
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
    inline Beamformer::BeamType beamType () {
      return beamType_p;
    }

    /*!
      \brief Get the name of the beam type to be used at processing

      \return beamType -- The type of beam to be used at data processing
     */
    std::string beamTypeName ();

    /*!
      \brief Set the type of beam to be used at data processing

      \param beam    -- 

      \return status -- 
     */
    bool setBeamType (Beamformer::BeamType const &beam);
    
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
      \brief Beamforming of the data, returning real-valued result

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool processData (casa::Matrix<double> &beam,
		      const casa::Matrix<DComplex> &data) {
      return (this->*processData_p) (beam,data);
    }
#else
#ifdef HAVE_BLITZ
    bool processData (blitz::Array<double,2> &beam,
		      const blitz::Array<complex<double>,2> &data) {
      return (this->*processData_p) (beam,data);
    }
#endif
#endif
    
    /*!
      \brief Form beam by adding up Fourier-transformed data

      \f[ S (\vec\rho,\nu) = \sum_{j=1}^{N_{\rm Ant}} \widetilde s_{j}
      (\vec\rho,\nu) \f]
      From the above we get the (directed) power spectrum via
      \f[ \widetilde P (\vec\rho,\nu) = \overline{\widetilde S (\vec\rho,\nu)}
      \cdot \widetilde S (\vec\rho,\nu) \f]

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool add_signals_per_antenna (casa::Matrix<double> &beam,
				  const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool add_signals_per_antenna (blitz::Array<double,2> &beam,
				  const blitz::Array<complex<double>,2> &data);
#endif
#endif
    
    /*!
      \brief Form a cross-correlation beam
      
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
      = \mathcal{F}^{-1} \left\{ w_j(\vec\rho)[k] \cdot \tilde s_j[k]\right\}
      \f]
      is the time shifted field strength of the single antennas for a direction
      \f$\vec \rho \f$. \f$ N \f$ is the number of antennas, \f$t\f$ the time or
      pixel index and \f$N_{Pairs}\f$ the number of unique pairs of antennas.
      The negative sign is taken if the sum had a negative sign before taking the
      absolute values, and the positive sign otherwise.

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool cc_beam (casa::Matrix<double> &beam,
		  const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool cc_beam (blitz::Array<double,2> &beam,
		  const blitz::Array<conplex<double>,2> &data);
#endif
#endif
    
    /*!
      \brief Form an axcess-beam (x-beam)

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool x_beam (casa::Matrix<double> &beam,
		 const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool x_beam (blitz::Array<double,2> &beam,
		 const blitz::Array<complex<double>,2> &data);
#endif
#endif
    
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
    
  };
  
} // Namespace CR -- end

#endif /* BEAMFORMER_H */
  
