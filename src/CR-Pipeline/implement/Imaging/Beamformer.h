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
      CCBEAM,
      XBEAM
    } BeamType;
    
  private:
    
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
				       const casa::Matrix<double> &data);
#else
#ifdef HAVE_BLITZ
    bool (Beamformer::*processData_p) (casa::Matrix<double> &beam,
				       const casa::Matrix<double> &data);
#endif
#endif
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Beamformer ();
    
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
      \brief Set the type of beam to be used at processing

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
		      const casa::Matrix<double> &data) {
      return (this->*processData_p) (beam,data);
    }
#else
#ifdef HAVE_BLITZ
    bool processData (blitz::Array<double,2> &beam,
		      const blitz::Array<double,2> &data) {
      return (this->*processData_p) (beam,data);
    }
#endif
#endif
    
    /*!
      \brief Form beam by adding up Fourier-transformed data

      \f[ S (\vec\rho,\nu) = \sum_{j=1}^{N_{\rm Ant}} \widetilde s_{j}
      (\vec\rho,\nu) \f]

      \retval beam -- [nofSkyPosition,nofChannels] Beam formed from the provided
                      input data.
      \param  data -- [nofDatasets,nofChannels] Input data which will be
                      processed to form a given type of beam.

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
#ifdef HAVE_CASA
    bool add_signals_per_antenna (casa::Matrix<double> &beam,
				  const casa::Matrix<double> &data);
#else
#ifdef HAVE_BLITZ
    bool add_signals_per_antenna (blitz::Array<double,2> &beam,
				  const blitz::Array<double,2> &data);
#endif
#endif

#ifdef HAVE_CASA
    bool cc_beam (casa::Matrix<double> &beam,
		  const casa::Matrix<double> &data);
#else
#ifdef HAVE_BLITZ
    bool cc_beam (blitz::Array<double,2> &beam,
		  const blitz::Array<double,2> &data);
#endif
#endif
    
#ifdef HAVE_CASA
    bool x_beam (casa::Matrix<double> &beam,
		 const casa::Matrix<double> &data);
#else
#ifdef HAVE_BLITZ
    bool x_beam (blitz::Array<double,2> &beam,
		 const blitz::Array<double,2> &data);
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
    
  };
  
} // Namespace CR -- end

#endif /* BEAMFORMER_H */
  
