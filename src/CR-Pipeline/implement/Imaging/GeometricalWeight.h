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

#ifndef GEOMETRICALWEIGHT_H
#define GEOMETRICALWEIGHT_H

// Standard library header files
#include <string>

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
    \f[ A_{\rm geom} = \exp \left( i \Phi \right) = \exp \left( i \frac{2\pi\nu}{c} R L^{T} \right) \f]
    where \f$ \nu \f$ is the observation frequency, \f$ c \f$ the speed of light,
    \f$ \mathbf{R} \f$ is a matrix collecting the positions of the \f$ 2p \f$
    array elements and \f$ L \f$ is a matrix collecting the \f$ q \f$ source
    positions. The geometrical delay \f$ \tau = R L^{T} \f$ is computed in the
    class GeometricalDelay.

    <h3>Example(s)</h3>
    
  */  
  class GeometricalWeight : public GeometricalPhase {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    GeometricalWeight ();
    
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
      \brief Get the name of the class
      
      \return className -- The name of the class, GeometricalWeight.
    */
    std::string className () const {
      return "GeometricalWeight";
    }
    
    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
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
  
