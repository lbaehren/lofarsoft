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

    //! Buffer the beamformer weights?
    bool bufferWeights_p;

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
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna and
	     sky positions and afterwards kept in memory; if set <i>no</i> only
	     the input parameters are stored an no further action is taken.
      \param bufferPhases -- Buffer the values of the phases?
      \param bufferWeights -- Buffer the values of the geometrical weights?
    */
    BeamformerWeights(casa::Matrix<double> const &antPositions,
		      CR::CoordinateType const &antCoordType,
		      casa::Matrix<double> const &skyPositions,
		      CR::CoordinateType const &skyCoordType,
		      casa::Vector<double> const &frequencies,
		      bool const &bufferDelays=false,
		      bool const &bufferPhases=false,
		      bool const &bufferWeights=false);

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
    
    
    
  private:
    
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
  
