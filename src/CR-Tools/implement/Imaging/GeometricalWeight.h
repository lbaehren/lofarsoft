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

#ifndef GEOMETRICALWEIGHT_H
#define GEOMETRICALWEIGHT_H

// Custom header files
#include <Imaging/GeometricalDelay.h>
#include <Imaging/GeometricalPhase.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class GeometricalWeight
    
    \ingroup Imaging
    
    \brief Brief description for class GeometricalWeight
    
    \author Lars B&auml;hren

    \date 2007/01/15

    \test tGeometricalWeight.cc

    <h3>Prerequisite</h3>

    <h3>Synopsis</h3>
    
    The basic equation, by which the geometrical weights for the beamforming
    are computed, is given by
    \f[ A_{\rm geom} = \exp \left( i \Phi \right) = \exp \left( i \frac{2\pi\nu}{c}
    \tau (R,L) \right) \f]
    where \f$ \nu \f$ is the observation frequency, \f$ c \f$ the speed of light,
    \f$ \mathbf{R} \f$ is a matrix collecting the positions of the \f$ 2p \f$
    array elements and \f$ L \f$ is a matrix collecting the \f$ q \f$ source
    positions. The geometrical delay \f$ \tau = \tau (R,L) \f$ is computed via the
    class GeometricalDelay, which also provides the mean to select between near-field
    and far-field geometry.
    
    <h3>Example(s)</h3>

    <ol>
      <li>Since most of the time you won't be dealing with the default constructor,
      but rather want to create a new object with some custom settings for the
      antenna positions, sky positions and frequency values, here is a simple
      example:
      \code
      // Get the various input parameters
      casa::Matrix<double> antennaPositions = get_antennaPositions();
      casa::Matrix<double> skyPositions     = get_skyPositions();
      casa::Vector<double> frequencies      = get_frequencies();

      // Create new object
      GeometricalWeight weight (antennaPositions,
                                skyPositions,
				frequencies);
      \endcode
      As pointed out in the API documentation below, the coordinate values in both
      <tt>antennaPositions</tt> and <tt>skyPositions</tt> need to be given in
      cartesian coordinates, \f$ (x,y,z) \f$, placing all positions withtin a
      common frame already.
      <li>Consider the following setup: using two antennas placed on a 200m
      baseline along the x-axis (located at \f$ (-100,0,0) \f$ and \f$ (100,0,0)
      \f$) you want to point towards the local zenith, i.e. \f$
      (0^{\circ},90^{\circ}) \f$ in Azimuth-Elevation coordinates.
      \code
      
      \endcode
    </ol>
    
  */  
  class GeometricalWeight : public GeometricalPhase {

  protected:

    //! Array storing the values of the geometrical weights, [freq,ant,sky]
    casa::Cube<DComplex> weights_p;
    //! Bufferthe values of the geometrical weights?
    bool bufferWeights_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    GeometricalWeight ();
    
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
    GeometricalWeight (casa::Matrix<double> const &antPositions,
		       casa::Matrix<double> const &skyPositions,
		       casa::Vector<double> const &frequencies,
		       bool const &bufferDelays=false,
		       bool const &bufferPhases=false,
		       bool const &bufferWeights=false);
    
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
    GeometricalWeight (casa::Matrix<double> const &antPositions,
		       CR::CoordinateType const &antCoordType,
		       casa::Matrix<double> const &skyPositions,
		       CR::CoordinateType const &skyCoordType,
		       casa::Vector<double> const &frequencies,
		       bool const &bufferDelays=false,
		       bool const &bufferPhases=false,
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
    GeometricalWeight (GeometricalDelay const &delay,
		       casa::Vector<double> const &frequencies,
		       bool const &bufferPhases=false,
		       bool const &bufferWeights=false);
    
    /*!
      \brief Argumented constructor using existing GeometricalPhase object
      
      \param phase -- GeometricalPhase object encapsulating the functionality
             on top of which this class builds.
      \param bufferWeights -- Buffer the values of the geometrical weights?
    */
    GeometricalWeight (GeometricalPhase const &phase,
		       bool const &bufferWeights=false);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another GeometricalWeight object from which to create this new
      one.
    */
    GeometricalWeight (GeometricalWeight const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~GeometricalWeight ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeometricalWeight object from which to make a copy.
    */
    GeometricalWeight& operator= (GeometricalWeight const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Are the values for the geometrical weights buffered?

      \return bufferWeights -- Returns <i>true</i> if the values for the weights
                               are buffered.
    */
    inline bool bufferWeights () {
      return bufferWeights_p;
    }

    /*!
      \brief Enable/disable buffering of the values for the geometrical weights

      \param bufferWeights -- Buffer the values for the geometrical weights?
    */
    inline void bufferWeights (bool const &bufferWeights=false) {
      init (bufferWeights);
    }

    /*!
      \brief Set the frequencies values for which the phases are computed
      
      \param frequencies  -- The frequency values for which the phases are
                             computed.
      \param bufferPhases -- Buffer the values for the geometrical phases?

      \return status -- Status of the operation; returns <tt>false</tt> if an
                        error was encountered.
    */
    bool setFrequencies (const casa::Vector<double> &frequencies,
			 bool const &bufferPhases=false);
    
    /*!
      \brief Get the geometrical weights

      \return weights -- [frequency,antenna,skyPosition] Array with the values
              of the geometrical weights.
    */
    casa::Cube<DComplex> weights ();
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeometricalWeight.
    */
    std::string className () const {
      return "GeometricalWeight";
    }
    
    /*!
      \brief Provide a summary of the objects status and contents
    */
    inline void summary () {
      summary (std::cout);
    }
    
    /*!
      \brief Provide a summary of the objects status and contents

      \param os -- Output stream to which the summary is written
    */
    void summary (std::ostream &os);
    
    // ------------------------------------------------------------------ Methods
    
  private:

    /*!
      \brief Initialize the internal setting of the object

      \param bufferWeights -- Buffer the values of the geometrical weights?
     */
    void init (bool const &bufferWeights=false);

    /*!
      \brief Set the values of the geometrical weights
    */
    void setWeights();

    /*!
      /\brief Compute the values of the geometrical weights

      \return weights -- [frequency,antenna,skyPosition] Array with the values
              of the geometrical weights.
     */
    casa::Cube<DComplex> calcWeights ();

    /*!
      \brief Unconditional copying
    */
    void copy (GeometricalWeight const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };  // CLASS GEOMETRICALWEIGHT -- END
  
}  // NAMESPACE CR -- END

#endif /* GEOMETRICALWEIGHT_H */
