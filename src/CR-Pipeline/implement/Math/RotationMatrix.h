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

#ifndef ROTATIONMATRIX_H
#define ROTATIONMATRIX_H

#include <iostream>
#include <string>
#include <vector>

#ifdef HAVE_BLITZ
#include <blitz/array.h>
#endif

#ifdef HAVE_CASA
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#endif


using std::vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class RotationMatrix
    
    \ingroup Math
    
    \brief Brief description for class RotationMatrix
    
    \author Lars B&auml;hren

    \date 2007/05/29

    \test tRotationMatrix.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class RotationMatrix {

  protected:

    //! Rank of the space in which the rotation takes places
    unsigned int rank_p;
    //! Number of angles required to parametrize the rotation
    unsigned int nofAngles_p;
    //! Rotation angles
    vector<double> angles_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    RotationMatrix ();
    
    /*!
      \brief Argumented constructor

      \param rank -- Rank of the space in which the rotation takes places
    */
    RotationMatrix (unsigned int const &rank);
    
    /*!
      \brief Argumented constructor

      \param rank           -- 
      \param angles         -- 
      \param anglesInDegree -- 
    */
    RotationMatrix (unsigned int const &rank,
		    vector<double> const &angles,
		    bool const &anglesInDegree=true);
    
#ifdef HAVE_BLITZ

    /*!
      \brief Argumented constructor (using Blitz++ array classes)

      \param rank           -- 
      \param angles         -- 
      \param anglesInDegree -- 
    */
    RotationMatrix (unsigned int const &rank,
		    blitz::Array<double,1> const &angles,
		    bool const &anglesInDegree=true);

#endif

#ifdef HAVE_CASA
    
    /*!
      \brief Argumented constructor (CASA array classes)

      \param rank           -- 
      \param angles         -- 
      \param anglesInDegree -- 
    */
    RotationMatrix (unsigned int const &rank,
		    casa::Vector<double> const &angles,
		    bool const &anglesInDegree=true);

#endif
    
    /*!
      \brief Copy constructor
      
      \param other -- Another RotationMatrix object from which to create this new
      one.
    */
    RotationMatrix (RotationMatrix const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~RotationMatrix ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another RotationMatrix object from which to make a copy.
    */
    RotationMatrix& operator= (RotationMatrix const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief The rank of the space in which the rotation takes places

      \return rank -- Rank of the space in which the rotation takes places
    */
    inline unsigned int rank () {
      return rank_p;
    }

    /*!
      \brief Get the number of angles required to parametrize the rotation

      \return nofAngles -- The number of angles required to parametrize the rotation
     */
    inline unsigned int nofAngles () {
      return nofAngles_p;
    }

    /*!
      \brief Get the angles parametrizing the rotation

      \return angles -- 
    */
    inline vector<double> angles () {
      return angles_p;
    }
    
    /*!
      \brief Get the angles parametrizing the rotation

      \param angles -- 

      \return status -- 
    */
    bool setAngles (vector<double> const &angles,
		    bool const &anglesInDegree=true);

#ifdef HAVE_BLITZ
    
    /*!
      \brief Get the angles parametrizing the rotation

      \param angles -- 

      \return status -- 
    */
    bool setAngles (blitz::Array<double,1> const &angles,
		    bool const &anglesInDegree=true);

#endif
    
#ifdef HAVE_CASA
    
    /*!
      \brief Get the angles parametrizing the rotation

      \param angles -- 

      \return status -- 
    */
    bool setAngles (casa::Vector<double> const &angles,
		    bool const &anglesInDegree=true);

#endif
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, RotationMatrix.
    */
    std::string className () const {
      return "RotationMatrix";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);

    // ------------------------------------------------------------------ Methods

    /*!
      \brief Apply rotation to a vector

      \retval out -- Vector after application of the rotation
      \param  in  -- Vector to which the rotation is applied

      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool rotate (vector<double> &out,
		 vector<double> const &in);

#ifdef HAVE_BLITZ
    
    /*!
      \brief Apply rotation to a vector

      \retval out -- Vector after application of the rotation
      \param  in  -- Vector to which the rotation is applied

      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool rotate (blitz::Array<double,1> &out,
		 blitz::Array<double,1> const &in);

    /*!
      \brief Apply rotation to a matrix

      \retval out -- Matrix after application of the rotation
      \param  in  -- Matrix to which the rotation is applied

      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool rotate (blitz::Array<double,2> &out,
		 blitz::Array<double,2> const &in);

#endif
    
#ifdef HAVE_CASA
    
    /*!
      \brief Apply rotation to a vector

      \retval out -- Vector after application of the rotation
      \param  in  -- Vector to which the rotation is applied

      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool rotate (casa::Vector<double> &out,
		 casa::Vector<double> const &in);

    /*!
      \brief Apply rotation to a Matrix

      \retval out -- Matrix after application of the rotation
      \param  in  -- Matrix to which the rotation is applied

      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool rotate (casa::Matrix<double> &out,
		 casa::Matrix<double> const &in);

#endif
    
  private:

    /*!
      \brief Set the rank of the space in which the rotation takes places
      
      \param rank -- The rank of the space in which the rotation takes places

      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool setRank (unsigned int const &rank);
    
    /*!
      \brief Unconditional copying
    */
    void copy (RotationMatrix const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* ROTATIONMATRIX_H */
  
