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
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    The basic equation, by which the geometrical weights for the beamforming
    are computed, is given by
    \f[ A_{\rm geom} = \exp \left( i \Phi \right) = \exp \left( i \frac{2\pi\nu}{c}
    R L^{T} \right) \f]
    where \f$ \nu \f$ is the observation frequency, \f$ c \f$ the speed of light,
    \f$ \mathbf{R} \f$ is a matrix collecting the positions of the \f$ 2p \f$
    array elements and \f$ L \f$ is a matrix collecting the \f$ q \f$ source
    positions. The geometrical delay \f$ \tau = R L^{T} \f$ is computed in the
    class GeometricalDelay.
    
    \f[
    \begin{array}{rcl}
    \tau_j & = & \frac{1}{c} \left( |\vec \rho - \vec x_j| - |\vec \rho| \right) \\
    & \downarrow &  \\
    \phi (\vec x_j, \vec \rho, \nu) & = & 2 \pi \nu \tau_{\rm geom} \\
    & \downarrow &  \\
    w (\vec x_j, \vec \rho, \nu) & = & \exp \Bigl( i\, \phi (\vec x_j, \vec \rho,
    \nu) \Bigr)
    \end{array}
    \f]

    <h3>Example(s)</h3>
    
  */  
  class GeometricalWeight : public GeometricalPhase {

  protected:

    // Buffer the values of the geometrical weights?
    bool bufferWeights_p;

#ifdef HAVE_CASA
    casa::Cube<DComplex> weights_p;
#else
#ifdef HAVE_BLITZ
    blitz::Array<complex<double> > weights_p;
#endif
#endif
    
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
#ifdef HAVE_CASA
    GeometricalWeight (casa::Matrix<double> const &antPositions,
		       casa::Matrix<double> const &skyPositions,
		       casa::Vector<double> const &frequencies,
		       bool const &bufferDelays=false,
		       bool const &bufferPhases=false,
		       bool const &bufferWeights=false);
#else
#ifdef HAVE_BLITZ
    GeometricalWeight (const blitz::Array<double,2> &antPositions,
		       const blitz::Array<double,2> &skyPositions,
		       blitz::Array<double,1> const &frequencies,
		       bool const &bufferDelays=false,
		       bool const &bufferPhases=false,
		       bool const &bufferWeights=false);
#endif
#endif

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
      bufferWeights_p = bufferWeights;
      if (bufferWeights) {
	setWeights();
      }
    }

    /*!
      \brief Get the geometrical weights

      \return weights -- [nofAntennas,nofPositions,nofFrequencies] Array with the
                         values of the geometrical weights.
    */
#ifdef HAVE_CASA
    casa::Cube<DComplex> weights ();
#else
#ifdef HAVE_BLITZ
    blitz::Array<complex<double>,3> weights ();
#endif
#endif
    
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
      \brief Set the values of the geometrical weights
    */
    void setWeights();

    /*!
      /\brief Compute the values of the geometrical weights

      \return weights -- [nofAntennas,nofPositions,nofFrequencies] Array with the
                         values of the geometrical weights.
     */
#ifdef HAVE_CASA
    casa::Cube<DComplex> calcWeights ();
#else
#ifdef HAVE_BLITZ
    blitz::Array<complex<double>,3> calcWeights ();
#endif
#endif

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
  
