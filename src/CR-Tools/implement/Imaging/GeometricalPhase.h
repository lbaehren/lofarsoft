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

#ifndef GEOMETRICALPHASE_H
#define GEOMETRICALPHASE_H

#include <crtools.h>

#include <casa/Arrays/Cube.h>

// Custom header files
#include <Imaging/GeometricalDelay.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class GeometricalPhase
    
    \ingroup CR_Imaging
    
    \brief Brief description for class GeometricalPhase
    
    \author Lars B&auml;hren

    \date 2007/01/15

    \test tGeometricalPhase.cc
    
    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>
    
    <table border="0">
      <tr>
        <td class="indexkey">Quantity</td>
        <td class="indexkey">implemented in</td>
        <td class="indexkey">Relation</td>
      </tr>
      <tr>
        <td>gemoetrical delay</td>
        <td>GeometricalDelay</td>
        <td>\f$ \tau_j = \frac{1}{c} \left( |\vec \rho - \vec x_j| - |\vec \rho|
	\right) \f$</td>
      </tr>
      <tr>
        <td>geometrical phase</td>
        <td>GeometricalPhase</td>
        <td>\f$ \phi (\vec x_j, \vec \rho, \nu) = 2 \pi \nu \tau_{\rm geom} \f$</td>
      </tr>
      <tr>
        <td>geometrical weight</td>
        <td>GeometricalWeight</td>
        <td>\f$ w (\vec x_j, \vec \rho, \nu) = \exp \Bigl( i\, \phi (\vec x_j,
	\vec \rho, \nu) \Bigr) \f$</td>
      </tr>
    </table>
    
    The <i>basic equation</i>, for which the delays are computed, is documented
    in the GeometricalWeight class. The procedure to arrive at the geometrical
    weights passed to the Beamform is divided into three steps, of which this
    class implements the second one.
    
    The geometrical phase \f$ \phi = \phi (\vec x, \vec \rho, \nu) \f$ is given
    by
    \f[ \phi (\vec x, \vec \rho, \nu) = 2 \pi \nu \tau_{\rm geom} \f]
    where \f$ \tau_{\rm geom} \f$ is the geometrical delay (as computed in 
    GeometricalDelay) and \f$ \nu \f$ the frequency.

    <h3>Example(s)</h3>
    
  */  
  class GeometricalPhase : public GeometricalDelay {

  protected:

    //! The frequency values for which the phases are computed
    casa::Vector<double> frequencies_p;
    //! [frequency,antenna,skyPosition] Array with the geometrical phase(s)
    casa::Cube<double> phases_p;
    //! Buffer the values for the geometrical phases?
    bool bufferPhases_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    GeometricalPhase ();
    
    /*!
      \brief Argumented constructor

      This uses the default constructor for GeometricalDelay.
      
      \param frequencies  -- Frequencies for which the geometrical delays are
                             converted into phases
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeometricalPhase (casa::Vector<double> const &frequencies,
		      bool const &bufferPhases=false);
    
    /*!
      \brief Argumented constructor
      
      \param antPositions -- [nofAntennas,3] Antenna positions for which the
             delay is computed, given in Cartesian coordinates \f$ (x,y,z) \f$
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
             which to point, given in the same reference frame as the antenna
	     positions, \f$ (x,y,z) \f$
      \param frequencies  -- Frequencies for which the geometrical delays are
             converted into phases
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna and
	     sky positions and afterwards kept in memory; if set <i>no</i> only
	     the input parameters are stored an no further action is taken.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeometricalPhase (casa::Matrix<double> const &antPositions,
		      casa::Matrix<double> const &skyPositions,
		      casa::Vector<double> const &frequencies,
		      bool const &bufferDelays=false,
		      bool const &bufferPhases=false);
    
    /*!
      \brief Argumented constructor
      
      \param antPositions -- [nofAntennas,3] Antenna positions for which the
             delay is computed, given in Cartesian coordinates \f$ (x,y,z) \f$
      \param antCoordType -- CR::CoordinateTypes::Type of the antenna position
             coordinates; if the coordinates are non-cartesian and thereby
	     include anglular components, the values must be provided in radians.
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
             which to point, given in the same reference frame as the antenna
	     positions, \f$ (x,y,z) \f$
      \param skyCoordType -- CR::CoordinateTypes::Type of the sky position coordinates;
             if the coordinates are non-cartesian and thereby include anglular
	     components, the values must be provided in radians.
      \param frequencies  -- Frequencies for which the geometrical delays are
             converted into phases
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna and
	     sky positions and afterwards kept in memory; if set <i>no</i> only
	     the input parameters are stored an no further action is taken.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeometricalPhase (casa::Matrix<double> const &antPositions,
		      CR::CoordinateTypes::Type const &antCoordType,
		      casa::Matrix<double> const &skyPositions,
		      CR::CoordinateTypes::Type const &skyCoordType,
		      casa::Vector<double> const &frequencies,
		      bool const &bufferDelays=false,
		      bool const &bufferPhases=false);
    
    /*!
      \brief Argumented constructor

      \param delay -- GeometricalDelay object encapsulating the functionality
             on top of which this class builds.
      \param frequencies  -- Frequencies for which the geometrical delays are
             converted into phases, [Hz]
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeometricalPhase (GeometricalDelay const &delay,
		      casa::Vector<double> const &frequencies,
		      bool const &bufferPhases=false);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another GeometricalPhase object from which to create this new
      one.
    */
    GeometricalPhase (GeometricalPhase const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~GeometricalPhase ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeometricalPhase object from which to make a copy.
    */
    GeometricalPhase& operator= (GeometricalPhase const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Are the values for the geometrical phases buffered?

      \return bufferPhases -- Returns <i>true</i> if the values for the phases
                              are buffered.
    */
    inline bool bufferPhases () const {
      return bufferPhases_p;
    }

    /*!
      \brief Enable/disable buffering of the values for the geometrical phases

      \param bufferPhases -- Buffer the values for the geometrical phases?
    */
    inline void bufferPhases (bool const &bufferPhases=false) {
      bufferPhases_p = bufferPhases;
      if (bufferPhases) {
	setPhases();
      }
    }
    
    /*!
      \brief Get the number of frequency channels, for which the phases are computed

      \return nofFrequencies -- The number of frequency channels, for which the
              phases are computed
    */
    inline int nofFrequencies () const {
      return frequencies_p.nelements();
    }
    
    /*!
      \brief Get the frequencies values for which the phases are computed
      
      \return frequencies -- The frequency values for which the phases are
              computed.
    */
    inline casa::Vector<double> frequencies () const {
      return frequencies_p;
    }
    
    /*!
      \brief Set the frequencies values for which the phases are computed
      
      \param frequencies  -- The frequency values for which the phases are
                             computed.
      \param bufferPhases -- Buffer the values for the geometrical phases?

      \return status -- Status of the operation; returns <tt>false</tt> if an
              error was encountered.
    */
    virtual bool setFrequencies (const casa::Vector<double> &frequencies,
				 bool const &bufferPhases=false);

    /*!
      \brief Get the geometrical phase(s)

      \return delay -- [frequency,antenna,skyPosition] Array with the
              geometrical phase(s) for a combination of antenna positions,
	      pointing positions and observation frequencies.
    */
    casa::Cube<double> phases ();

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get a geometrical phase

      \param nFreq    -- Index for selection from the set of frequencies.
      \param nAntenna -- Index for selection from the set of antenna positions.
      \param nSky     -- Index for selection from the set of sky positions.

      \return phase -- The geometrical phase for a given combination of antenna,
              sky position and frequency.
     */
    double phase (int const &nFreq,
		  int const &nAntenna,
		  int const &nSky);
    
    /*!
      \brief Get the geometrical phase

      \param frequency   -- Frequency value, [Hz]
      \param antPosition -- Antenna position.
      \param skyPosition -- Position on the sky.

      \return phase -- The geometrical phase for a given combination of antenna,
              sky position and frequency.
    */
    double phase (double const &frequency,
		  Vector<double> const &antPosition,
		  Vector<double> const &skyPosition);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeometricalPhase.
    */
    std::string className () const {
      return "GeometricalPhase";
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
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (GeometricalPhase const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    void setPhases ();
  
    /*!
      \brief Calculate the geometrical phases

      \return phases -- [frequency,antenna,direction] Array with the
              geometrical phases
    */
    casa::Cube<double> calcPhases ();
    
  };  // CLASS GEOMETRICALPHASE -- END
  
}  // NAMESPACE CR -- END

#endif /* GEOMETRICALPHASE_H */
  
