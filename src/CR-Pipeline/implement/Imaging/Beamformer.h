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

#ifndef BEAMFORMER_H
#define BEAMFORMER_H

// Standard library header files
#include <iostream>
#include <string>

#include <Imaging/GeometricalWeight.h>

namespace CR { // Namespace CR -- begin
  
    /*!
      \brief List of implemented and supported beam types.
    */
    typedef enum {
      //! Electric field strength as function of frequency
      FREQ_FIELD,
      //! Power of the electric field as function of frequency
      FREQ_POWER,
      //! Electric field strength as function of time (sample resolution)
      TIME_FIELD,
      //! Power of the electric field as function of time
      TIME_POWER,
      //! Cross-correlation beam (cc-beam)
      TIME_CC,
      //! Power-beam (p-beam)
      TIME_P,
      //! Excess-beam (x-beam)
      TIME_X
    } BeamType;
    
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
    
    //! Type of beamforming method used in data processing
    BeamType beamType_p;
    
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
    */
#ifdef HAVE_CASA
    Beamformer (casa::Matrix<double> const &antPositions,
		casa::Matrix<double> const &skyPositions,
		casa::Vector<double> const &frequencies);
#else
#ifdef HAVE_BLITZ
    Beamformer (const blitz::Array<double,2> &antPositions,
		const blitz::Array<double,2> &skyPositions,
		blitz::Array<double,1> const &frequencies);
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
    inline BeamType beamType () {
      return beamType_p;
    }

    /*!
      \brief Get the name of the beam type to be used at processing

      \return beamType -- The type of beam to be used at data processing
     */
    std::string beamTypeName ();
    
    /*!
      \brief Get the name of the beam type to be used at processing
      
      \param beamType -- The type of beam to be used at data processing
      
      \return name -- The name of the beam type to be used at data processing
    */
    static std::string beamTypeName (BeamType const &beamType) {
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
    
    /*!
      \brief Set the type of beam to be used at data processing
      
      \param beam    -- 
      
      \return status -- 
    */
    bool setBeamType (BeamType const &beam);
    
    /*!
      \brief Convert combination of domain and quantity label to BeamType
      
      \retval beamType -- 
      \param domain    -- 
      \param quantity  --
    */
    static bool beamType (BeamType &beamType,
			  string const &domain,
			  string const &quantity);
    
    /*!
      \brief Convert BeamType to combination of domain and quantity label
      
      \retval domain   -- 
      \retval quantity -- 
      \param beamType  -- 
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
      \brief Directed field as function of frequency, \f$ \widetilde S (\vec\rho,\nu) \f$

      <b>Note:</b> This function cannot be called via the <tt>processData</tt>
      method, since the returned array consists of complex values.
      
      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool freq_field (casa::Matrix<DComplex> &beam,
		     const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool freq_field (blitz::Array<complex<double>,2> &beam,
		     const blitz::Array<complex<double>,2> &data);
#endif
#endif
    
    /*!
      \brief Directed spectral power, \f$ \widetilde P (\vec\rho,\nu) \f$

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

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool freq_power (casa::Matrix<double> &beam,
		     const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool freq_power (blitz::Array<double,2> &beam,
		     const blitz::Array<complex<double>,2> &data);
#endif
#endif
    
    /*!
      \brief Directed power time series, \f$ S (\vec\rho,t) \f$

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool time_power (casa::Matrix<double> &beam,
		     const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool time_power (blitz::Array<double,2> &beam,
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

      Computation - for each direction in the sky - is performed as follows:
      <ol>
        <li>Compute the shifted time-series for all antennas,
	\f$ s_j (\vec\rho,t) \f$, by first applying the weights to the Fourier
	transformed data and thereafter transforming back to time domain.
	<li>Sum over unique products from all antenna pairs and normalize by
	the number of such pairs.
	<li>Take the square root of the sum and multiply with the sign of the
	some.
      </ol>

      \retval beam -- [nofSkyPosition,blocksize] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool time_cc (casa::Matrix<double> &beam,
		  const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool time_cc (blitz::Array<double,2> &beam,
		  const blitz::Array<conplex<double>,2> &data);
#endif
#endif
    
    /*!
      \brief Form a power-beam (p-beam)

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool time_p (casa::Matrix<double> &beam,
		 const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool time_p (blitz::Array<double,2> &beam,
		 const blitz::Array<complex<double>,2> &data);
#endif
#endif
    
    /*!
      \brief Form an excess-beam (x-beam)

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool time_x (casa::Matrix<double> &beam,
		 const casa::Matrix<DComplex> &data);
#else
#ifdef HAVE_BLITZ
    bool time_x (blitz::Array<double,2> &beam,
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
  
