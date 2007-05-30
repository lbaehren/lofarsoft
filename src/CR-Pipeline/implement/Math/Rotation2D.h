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

#ifndef ROTATION2D_H
#define ROTATION2D_H

#include <string>

#include <Math/RotationMatrix.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Rotation2D
    
    \ingroup Math
    
    \brief Brief description for class Rotation2D
    
    \author Lars Baehren

    \date 2007/05/29

    \test tRotation2D.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    \f[
      \mathbf{R}(\alpha) = \left[ \begin{array}{cc} \cos\alpha & \sin\alpha \\
      -\sin\alpha & \cos\alpha \end{array} \right]
    \f]
    
    <h3>Example(s)</h3>
    
  */  
  class Rotation2D : public RotationMatrix {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Rotation2D ();
    
    /*!
      \brief Argumented constructor

      \param angles         -- 
      \param anglesInDegree -- 
    */
    Rotation2D (vector<double> const &angles,
		bool const &anglesInDegree=true);
    
#ifdef HAVE_BLITZ

    /*!
      \brief Argumented constructor (using Blitz++ array classes)

      \param angles         -- 
      \param anglesInDegree -- 
    */
    Rotation2D (blitz::Array<double,1> const &angles,
		bool const &anglesInDegree=true);

#endif

#ifdef HAVE_CASA
    
    /*!
      \brief Argumented constructor (CASA array classes)

      \param angles         -- 
      \param anglesInDegree -- 
    */
    Rotation2D (casa::Vector<double> const &angles,
		bool const &anglesInDegree=true);

#endif

    /*!
      \brief Copy constructor
      
      \param other -- Another Rotation2D object from which to create this new
      one.
    */
    Rotation2D (Rotation2D const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~Rotation2D ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Rotation2D object from which to make a copy.
    */
    Rotation2D& operator= (Rotation2D const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Rotation2D.
    */
    std::string className () const {
      return "Rotation2D";
    }

    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (Rotation2D const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* ROTATION2D_H */
  
