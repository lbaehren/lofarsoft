/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: GeomPhase.h,v 1.1 2006/10/31 21:07:59 bahren Exp $*/

#ifndef GEOMPHASE_H
#define GEOMPHASE_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

// LOPES-Tools header files
#include <LopesLegacy/GeomDelay.h>

#include <casa/namespace.h>

namespace LOPES { // Namespace LOPES -- begin
  
  /*!
    \class GeomPhase
    
    \ingroup Beamforming
    
    \brief Brief description for class GeomPhase
    
    \author Lars B&auml;hren
    
    \date 2006/09/18
    
    \test tGeomPhase.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  class GeomPhase : public GeomDelay {
    
  protected:
    
    Complex phase_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    GeomPhase ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another GeomPhase object from which to create this new one.
    */
    GeomPhase (GeomPhase const &other);
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~GeomPhase ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeomPhase object from which to make a copy.
    */
    GeomPhase& operator= (GeomPhase const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (GeomPhase const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
    /*!
      \brief Set the geometrical phase
    */
    void setPhase ();
    
  };
  
} // Namespace LOPES -- end

#endif /* GEOMPHASE_H */
