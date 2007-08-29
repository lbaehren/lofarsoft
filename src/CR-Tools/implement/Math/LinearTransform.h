/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#ifndef LINEARTRANSFORM_H
#define LINEARTRANSFORM_H

// Standard library header files
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

namespace CR { // Namespace CR -- begin
  
  /*!
    \class LinearTransform
    
    \ingroup Math
    
    \brief Brief description for class LinearTransform
    
    \author Lars B&auml;hren

    \date 2007/08/20

    \test tLinearTransform.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Some basic knowledge of linear algebra.
    </ul>
    
    <h3>Synopsis</h3>

    A general linear transform in N-dimensional coordinates is given by 
    \f[ \vec y = \mathbf{A} \vec x + \vec s \f]
    where \f$ \mathbf{A} \f$ is a N times N matrix and \f$ \vec s \f$ a vector
    of length N. Written out for individual components we have 
    \f[ y_{i} = \sum_{k=1}^{N} a_{ik} x_{k} + s_{i} \f]
    where the \f$ a_{ik} \f$ are elements of the matrix \f$ \mathbf{A} \f$.
    The corresponding inverse (backwards) transform is given by
    \f[ \vec x = \mathbf{A}^{-1} (\vec s - \vec y) \f]
    
    <h3>Example(s)</h3>
   
    The transformation matrix \f$ \mathbf{A} \f$ should be populated as follows:
    \code
    uint row (0);
    uint col (0);
    uint n (0);
   
    for (row=0; row<rank; row++) {
      for (col=0; col<rank; col++) {
        matrix[n] = A[row][col];
        n++;
      }
    }
    \endcode
    For a simple case thus:
    \f[ \left[ \begin{array}{ccc} 0 & 1 & 2 \\ 3 & 4 & 5 \\ 6 & 7 & 8 
    \end{array} \right] \qquad \rightarrow \qquad [0,1,2,3,4,5,6,7,8] \f]
    
  */  
  class LinearTransform {
    
    //! Rank of vector space within which we operate
    uint rank_p;
    //! Transformation matrix
    double *matrix_p;
    //! Position shift
    double *shift_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    LinearTransform ();
    
    /*! 
      \brief Argumented constructor 
      
      \param rank -- Rank of vector space within which we operate
    */ 
    LinearTransform (uint const &rank); 
    
    /*! 
      \brief Argumented constructor 
      
      \param rank   -- Rank of the matrix and the vector
      \param matrix -- Transformation matrix, \f$ \mathbf A \f$.
      \param shift  -- Position shift, \f$ \vec s \f$.
    */
    LinearTransform (uint const &rank,
		     double const *matrix,
		     double const *shift);
    
#ifdef HAVE_BLITZ
    /*! 
      \brief Argumented constructor using Blitz++ arrays
      
      \param matrix -- Transformation matrix, \f$ \mathbf A \f$.
      \param shift  -- Position shift, \f$ \vec s \f$.
    */ 
    LinearTransform (blitz::Array<double,2> const &matrix,
		     blitz::Array<double,1> const &shift);
#endif
#ifdef HAVE_CASA
    /*! 
      \brief Argumented constructor using CASA arrays
      
      \param matrix -- Transformation matrix, \f$ \mathbf A \f$.
      \param shift  -- Position shift, \f$ \vec s \f$.
    */ 
    LinearTransform (casa::Matrix<double> const &matrix,
		     casa::Vector<double> const &shift);
#endif
    
    /*!
      \brief Copy constructor
      
      \param other -- Another LinearTransform object from which to create this
                      new one.
    */
    LinearTransform (LinearTransform const &other);
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~LinearTransform ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another LinearTransform object from which to make a copy.
    */
    LinearTransform& operator= (LinearTransform const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    bool setMatrix (double const *matrix);
#ifdef HAVE_BLITZ
    bool setMatrix (blitz::Array<double,2> const &matrix);
#endif
#ifdef HAVE_CASA
    bool setMatrix (casa::Matrix<double> const &matrix);
#endif
    
    void shift (double *shift);
    void shift (std::vector<double> &vect);
#ifdef HAVE_BLITZ
    void shift (blitz::Array<double,1> &shift);
#endif
#ifdef HAVE_CASA
    void shift (casa::Vector<double> &shift);
#endif

    /*!
      \brief Assign a new value to the shift
    */
    bool setShift (double const *shift);
    bool setShift (std::vector<double> const &shift);
#ifdef HAVE_BLITZ
    bool setShift (blitz::Array<double,1> const &shift);
#endif
#ifdef HAVE_CASA
    bool setShift (casa::Vector<double> const &shift);
#endif
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, LinearTransform.
    */
    std::string className () const {
      return "LinearTransform";
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
      \brief Apply forward linear transform

      \retval out -- 
      \param in   -- 
      
      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool forward (double *out,
		  double const *in);
    /*!
      \brief Apply forward linear transform

      \retval out -- 
      \param in   -- 
      
      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool forward (std::vector<double> &out,
		  std::vector<double> const &in);
#ifdef HAVE_BLITZ
    /*!
      \brief Apply forward linear transform

      \retval out -- 
      \param in   -- 
      
      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool forward (blitz::Array<double,1> &out,
		  blitz::Array<double,1> const &in);
#endif
#ifdef HAVE_CASA
    /*!
      \brief Apply forward linear transform

      \retval out -- 
      \param in   -- 
      
      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool forward (casa::Vector<double> &out,
		  casa::Vector<double> const &in);
#endif
    
    /*!
      \brief Apply backward (inverse) linear transform

      \retval out -- 
      \param in   -- 
      
      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool backward (std::vector<double> &out, 
		   std::vector<double> const &in);

    /*!
      \brief Chain up two linear transforms

      \retval result      -- 
      \param  other       -- 
      \param  otherIsLast -- 
      
      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool chainTransforms (LinearTransform &result,
			  LinearTransform const &other,
			  bool const &otherIsLast=true);
    
    /*!
      \brief Chain up two linear transforms

      For two linear transforms \f$ y = \mathbf{A}_1 x + s_1 \f$ and
      \f$ z = \mathbf{A}_2 y + s_2 \f$ we have
      \f[ z = \mathbf{A}_2 \left( \mathbf{A}_1 x + s_1 \right) + s_2 =
      \mathbf{A}_2 \mathbf{A}_2 x + \mathbf{A}_2 s_1 + s_2 =
      \mathbf{A}' x + s' \f]

      \retval result -- 
      \param  xform1 -- 
      \param  xform2 -- 
      
      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool chainTransforms (LinearTransform &result,
			  LinearTransform const &xform1,
			  LinearTransform const &xform2);
    
  private:

    void init (uint const &rank);

    /*!
      \brief Set the transformation matrix to a unit matrix
    */
    void setUnitMatrix ();
    
    /*!
      \brief Retrieve an individual element of the transformation matrix

      \param i -- Row index 
      \param j -- Column index

      \return a -- Value of matrix element \f$ a_{ij} \f$
    */
    inline double matrix (uint const &i,
			  uint const &j) {
      return matrix_p[i*rank_p+j];
    }

    /*!
      \brief Set an individual element of the transformation matrix

      \param i -- Row index 
      \param j -- Column index
      \param x -- Value to assign to the matrix element \f$ a_{ij} \f$
    */
    void setMatrix (uint const &i,
		    uint const &j,
		    double const &x);

    /*!
      \brief Get the determinant of the transformation matrix

      \return det -- The determinant of the transformation matrix
    */
    double det ();
    
    /*!
      \brief Unconditional copying
    */
    void copy (LinearTransform const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* LINEARTRANSFORM_H */
  
