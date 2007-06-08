/***************************************************************************
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

/* $Id: template-class.h,v 1.17 2006/11/08 22:07:16 bahren Exp $*/

#ifndef GEOMETRICALPHASE_H
#define GEOMETRICALPHASE_H

// Standard library header files
#include <string>

#ifdef HAVE_CASA
#include <casa/Arrays/Cube.h>
#endif

// Custom header files
#include <Imaging/GeometricalDelay.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class GeometricalPhase
    
    \ingroup Imaging
    
    \brief Brief description for class GeometricalPhase
    
    \author Lars B&auml;hren

    \date 2007/01/15

    \test tGeometricalPhase.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>GeometricalDelay
      <li>GeometricalWeight
    </ul>
    
    <h3>Synopsis</h3>
    
    <b>Geometry.</b>
    The basic equation, for which the delays are computed, is documented in
    the GeometricalWeight class. The procedure to arrive at the geometrical
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

#ifdef HAVE_CASA
    //! The frequency values for which the phases are computed
    casa::Vector<double> frequencies_p;
    //! Array with the geometrical phases
    casa::Cube<double> phases_p;
#else 
#ifdef HAVE_BLITZ
    //! The frequency values for which the phases are computed
    blitz::Array<double,1> frequencies_p;
    //! Array with the geometrical phases
    blitz::Array<double,3> phases_p;
#endif
#endif

    // Buffer the values for the geometrical phases?
    bool bufferPhases_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    GeometricalPhase ();
    
    /*!
      \brief Argumented constructor
      
      \param frequencies  -- Frequencies for which the geometrical delays are
                             converted into phases
      \param bufferPhases -- Buffer the values of the phases?
    */
#ifdef HAVE_CASA
    GeometricalPhase (casa::Vector<double> const &frequencies,
		      bool const &bufferPhases=false);
#else
#ifdef HAVE_BLITZ
    GeometricalPhase (blitz::Array<double,1> const &frequencies,
		      bool const &bufferPhases=false);
#endif
#endif
    
    /*!
      \brief Argumented constructor

      \param delay -- GeometricalDelay object encapsulating the functionality
                      on top of which this class builds.
    */
    GeometricalPhase (GeometricalDelay const &delay);
    
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
    ~GeometricalPhase ();
    
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
    inline bool bufferPhases () {
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
    inline int nofFrequencies () {
      return frequencies_p.nelements();
    }
    
    /*!
      \brief Get the frequencies values for which the phases are computed
      
      \return frequencies -- The frequency values for which the phases are
                             computed.
    */
#ifdef HAVE_CASA
    inline casa::Vector<double> frequencies () {
      return frequencies_p;
    }
#else
#ifdef HAVE_BLITZ
    inline blitz::Array<double,1> frequencies () {
      return frequencies_p;
    }
#endif
#endif
    
    /*!
      \brief Set the frequencies values for which the phases are computed
      
      \param frequencies -- The frequency values for which the phases are
                            computed.
    */
#ifdef HAVE_CASA
    bool setFrequencies (const casa::Vector<double> &frequencies,
			 bool const &bufferPhases=false);
#else
#ifdef HAVE_BLITZ
    bool setFrequencies (const blitz::Array<double,1> &frequencies,
			 bool const &bufferPhases=false);
#endif
#endif

    /*!
      \brief Get the geometrical phase(s)

      \return delay -- Array with the geometrical phase(s) with a combintation
                       of baselines, pointing positions and observation 
		       frequencies.
    */
#ifdef HAVE_CASA
    casa::Cube<double> phases ();
#else
#ifdef HAVE_BLITZ
    blitz::Array<double,3> phases ();
#endif
#endif

    // ------------------------------------------------------------------ Methods
    
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
    
#ifdef HAVE_CASA
    casa::Cube<double> calcPhases ();
#else
#ifdef HAVE_BLITZ
    blitz::Array<double,3> calcPhases ();
#endif
#endif

  };  // CLASS GEOMETRICALPHASE -- END
  
}  // NAMESPACE CR -- END

#endif /* GEOMETRICALPHASE_H */
  
