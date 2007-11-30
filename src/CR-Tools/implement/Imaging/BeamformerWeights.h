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

#ifndef BEAMFORMERWEIGHTS_H
#define BEAMFORMERWEIGHTS_H

// Standard library header files
#include <iostream>
#include <string>

#include <Imaging/GeometricalDelay.h>
#include <Imaging/GeometricalPhase.h>
#include <Imaging/GeometricalWeight.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class BeamformerWeights
    
    \ingroup Imaging
    
    \brief Container for the weights applied by the Beamformer
    
    \author Lars B&auml;hren

    \date 2007/11/29

    \test tBeamformerWeights.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>GeometricalWeight -- class handling the computation of the 
      geometrical components of the Beamformer weights, as given by the antenna
      positions, pointing positions and frequency values.
      <li>Beamformer
    </ul>
    
    <h3>Synopsis</h3>

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

    <b>Implementation details.</b>
    In the default case here no antenna gains, \f$ w_{j,\rm gain} (\vec\rho,\nu) \f$,
    are provided, this class will do nothing else but buffer the calculated
    geometrical weights \f$ w_{\rm geom} (\vec x_j,\vec\rho,\nu) \f$.
    
    <h3>Example(s)</h3>
    
  */  
  class BeamformerWeights : public GeometricalWeight {

    //! Do we have antenna gains?
    bool haveAntennaGains_p;

  protected:

    //! The weights applied by the Beamformer
    casa::Cube<DComplex> bfWeights_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    BeamformerWeights ();
    
    /*!
      \brief Fully argumented constructor
      
      \param antPositions -- [nofAntennas,3] Antenna positions for which the
             delay is computed, given in Cartesian coordinates \f$ (x,y,z) \f$
      \param antCoordType -- CR::CoordinateType of the antenna position
             coordinates; if the coordinates are non-cartesian and thereby
	     include anglular components, the values must be provided in radians.
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
             which to point, given in the same reference frame as the antenna
	     positions, \f$ (x,y,z) \f$
      \param skyCoordType -- CR::CoordinateType of the sky position coordinates;
             if the coordinates are non-cartesian and thereby include anglular
	     components, the values must be provided in radians.
      \param frequencies  -- Frequencies for which the geometrical delays are
             converted into phases

      Internally assigned parameters:
      - bufferDelays  = false
      - bufferPhases  = false
      - bufferWeights = false
    */
    BeamformerWeights (casa::Matrix<double> const &antPositions,
		       CR::CoordinateType const &antCoordType,
		       casa::Matrix<double> const &skyPositions,
		       CR::CoordinateType const &skyCoordType,
		       casa::Vector<double> const &frequencies);
    
    /*!
      \brief Argumented constructor

      \param weights -- Object storing the geometrical weights which are required
             for the Beamformer
    */
    BeamformerWeights (GeometricalWeight const &weights);
    
    /*!
      \brief Argumented constructor using existing GeometricalPhase object
      
      \param phase -- GeometricalPhase object encapsulating the functionality
             on top of which this class builds.
    */
    BeamformerWeights (GeometricalPhase const &phase);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another BeamformerWeights object from which to create this new
      one.
    */
    BeamformerWeights (BeamformerWeights const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~BeamformerWeights ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another BeamformerWeights object from which to make a copy.
    */
    BeamformerWeights& operator= (BeamformerWeights const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Do we have values to describe the antenna gain patterns?

      \return haveAntennaGains -- Returns <tt>true</tt> if the we have a valid
              set of antenna gain values, describing the 
    */
    inline bool haveAntennaGains () {
      return haveAntennaGains_p;
  }

    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, BeamformerWeights.
    */
    std::string className () const {
      return "BeamformerWeights";
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
    
  private:

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
      \brief Unconditional copying
    */
    void copy (BeamformerWeights const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* BEAMFORMERWEIGHTS_H */
  
