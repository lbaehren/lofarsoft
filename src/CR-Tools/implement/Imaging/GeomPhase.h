/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#ifndef GEOMPHASE_H
#define GEOMPHASE_H

// Standard library header files
#include <iostream>
#include <string>
// casacore header files
#include <casa/Quanta/MVFrequency.h>
// CR-Tools header files
#include <Imaging/GeomDelay.h>

using casa::MVFrequency;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class GeomPhase
    
    \ingroup CR_Imaging
    
    \brief Brief description for class GeomPhase
    
    \author Lars B&auml;hren

    \date 2008/12/17

    \test tGeomPhase.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::GeomDelay
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class GeomPhase : public GeomDelay {
    
  protected:

    //! Frequency values
    Vector<double> frequencies_p;
    //! The geometrical phases
    Matrix<double> phases_p;

    //! Buffer the values of the phases?
    bool bufferPhases_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    GeomPhase ();
    
    /*!
      \brief Argumented constructor

      \param frequencies  -- The frequencies, [Hz], for which the phases are
             being computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeomPhase (Vector<double> const &frequencies,
	       bool const &bufferPhases=false);
    
    /*!
      \brief Argumented constructor

      \param frequencies  -- The frequencies for which the phases are being
             computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeomPhase (Vector<MVFrequency> const &frequencies,
	       bool const &bufferPhases=false);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another GeomPhase object from which to create this new
             one.
    */
    GeomPhase (GeomPhase const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~GeomPhase ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeomPhase object from which to make a copy.
    */
    GeomPhase& operator= (GeomPhase const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Are the values of the phases getting buffered?

      \return bufferPhases -- Returns \e true in case the values of the
              geometrical phases are getting buffered and need not be recomputed
	      at each request
    */
    inline bool bufferPhases () const {
      return bufferPhases_p;
    }

    /*!
      \brief Get the frequency values

      \return frequencies -- The frequency values for which the geometrical
              phases are computed
    */
    inline Vector<double> frequencies () const {
      return frequencies_p;
    }

    /*!
      \brief Set the frequency values

      \param frequencies -- The frequency values for which the geometrical
             phases are computed

      \return status -- Status of the operation; returns \e false in case an
              error was encountered.
    */
    bool setFrequencies (Vector<double> const &frequencies);
    
    /*!
      \brief Set the frequency values

      \param frequencies -- The frequency values for which the geometrical
             phases are computed

      \return status -- Status of the operation; returns \e false in case an
              error was encountered.
    */
    bool setFrequencies (Vector<MVFrequency> const &frequencies);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeomPhase.
    */
    inline std::string className () const {
      return "GeomPhase";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    //! Unconditional copying
    void copy (GeomPhase const &other);
    
    //! Unconditional deletion 
    void destroy(void);

    //! Initialize internal parameters
    void init ();
    
    /*!
      \brief Initialize internal parameters

      \param frequencies  -- The frequencies, [Hz], for which the phases are
             being computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    void init (Vector<double> const &frequencies,
	       bool const &bufferPhases=false);
  };
  
} // Namespace CR -- end

#endif /* GEOMPHASE_H */
  
