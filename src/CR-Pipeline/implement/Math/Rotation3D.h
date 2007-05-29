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

/* $Id: template-class.h,v 1.18 2007/03/06 14:53:26 bahren Exp $*/

#ifndef ROTATION3D_H
#define ROTATION3D_H

#include <string>

#include <Math/RotationMatrix.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Rotation3D
    
    \ingroup Math
    
    \brief Brief description for class Rotation3D
    
    \author Lars Baehren

    \date 2007/05/29

    \test tRotation3D.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class Rotation3D : public RotationMatrix {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Rotation3D ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Rotation3D object from which to create this new
      one.
    */
    Rotation3D (Rotation3D const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~Rotation3D ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Rotation3D object from which to make a copy.
    */
    Rotation3D& operator= (Rotation3D const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Rotation3D.
    */
    std::string className () const {
      return "Rotation3D";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary ();    

    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (Rotation3D const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* ROTATION3D_H */
  
