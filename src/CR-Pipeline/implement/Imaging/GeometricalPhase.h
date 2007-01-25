/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

// Custom header files
#include <Imaging/GeometricalDelay.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class GeometricalPhase
    
    \ingroup Imaging
    
    \brief Brief description for class GeometricalPhase
    
    \author Lars Baehren

    \date 2007/01/15

    \test tGeometricalPhase.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class GeometricalPhase : public GeometricalDelay {

  protected:

    //! The frequency values for which the phases are computed
    blitz::Array<double,1> frequencies_p;

    //! Array with the geometrical phases
    blitz::Array<double,3> phase_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    GeometricalPhase ();
    
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
      \brief Get the frequencies values for which the phases are computed
      
      \return frequencies -- The frequency values for which the phases are
                             computed.
    */
    blitz::Array<double,1> frequencies () {
      return frequencies_p;
    }
    
    /*!
      \brief Set the frequencies values for which the phases are computed
      
      \param frequencies -- The frequency values for which the phases are
                            computed.
    */
    void setFrequencies (const blitz::Array<double,1> &frequencies);

    /*!
      \brief Get the geometrical phase(s)

      \return delay -- Array with the geometrical phase(s) with a combintation
                       of baselines, pointing positions and observation 
		       frequencies.
    */
    blitz::Array<double,3> phase () {
      return phase_p;
    }

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeometricalPhase.
    */
    std::string className () const {
      return "GeometricalPhase";
    }
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (GeometricalPhase const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };  // CLASS GEOMETRICALPHASE -- END
  
}  // NAMESPACE CR -- END

#endif /* GEOMETRICALPHASE_H */
  
