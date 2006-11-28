/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: MatrixEigen.h,v 1.3 2006/10/31 18:24:08 bahren Exp $*/

#ifndef MATRIXEIGEN_H
#define MATRIXEIGEN_H

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/IPosition.h>

using casa::IPosition;
using casa::Matrix;
using casa::Vector;

namespace LOPES {

/*!
  \class MatrixEigen.h

  \brief Eigenvalue decomposition of a real symmetric matrix

  \author Lars B&auml;hren

  \date 2005/06/25

  <h3>Prerequisites</h3>
  <ul type="square">
    <li><a href="http://mathworld.wolfram.com/Eigenvalue.html">Eigenvalue</a>,
    <a href="http://mathworld.wolfram.com/Eigenvector.html">Eigenvector</a>,
    <a href="http://mathworld.wolfram.com/Eigenspace.html">Eigenspace</a>,
    <a href="http://mathworld.wolfram.com/EigenDecomposition.html">Eigen
    Decomposition</a>
    (MathWorld, Wolfram Research)
    <li>W. H. Press, S. A. Teukolsky, W. T. Vetterling & B. P. Flannery.
    "Eigensystems." Chapter 11 in <a href="http://www.nr.com">Numerical Recipes
    in C</a>, Cambridge University Press, 1992.

    <li>[<a href="http://aips2.nrao.edu/docs/programmer/classlib.html">AIPS++</a>]
    Array -- A templated N-D Array class with zero origin. Vector, Matrix, and
    Cube are one, two, and three dimensional specializations of Array.
    <li>[<a href="http://aips2.nrao.edu/docs/programmer/classlib.html">AIPS++</a>]
    ArrayMath -- Mathematical operations for Arrays.
    <li>[<a href="http://aips2.nrao.edu/docs/programmer/classlib.html">AIPS++</a>]
    IPosition -- A Vector of integers, for indexing into Array objects.
  </ul>
*/

class MatrixEigen {

  //! The matrix which is decomposed
  Matrix<double> matrix_p;

  //! Vector with the eigenvalues
  Vector<double> eigenvalues_p;

  //! Matrix whose columns contain the normalized eigenvectors
  Matrix<double> eigenvectors_p;

  //! Keep track of wether the computation has been performed already or not
  bool computationDone_p;

  //! Maximum number of rotations to be performed during computation.
  int maxRotations_p;

  //! Number of rotations required for convergence of the algorithm.
  int nofRotations_p;

  //! Auxilliary variables used in the computation
  double g_p;
  double h_p;
  double s_p;
  double tau_p;
  
 public:

  // --- Construction ----------------------------------------------------------

  /*!
    \brief Constructor

    \param matrix -- The matrix, for which the decomposition is performed.
  */
  MatrixEigen (const Matrix<double>& matrix);

  /*!
    \brief Constructor

    \param matrix    -- The matrix, for which the decomposition is performed.
    \param rotations -- Maximum number of allowed Jacobi rotations in the 
                        computation process.
  */
  MatrixEigen (const Matrix<double>& matrix,
	       const int& rotations);

  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Destructor
  */
  ~MatrixEigen ();

  // --- Parameters ------------------------------------------------------------

  /*!
    \brief Get the maximum number of allowed Jacobi rotations.

    \return rotations -- Maximum number of allowed Jacobi rotations in the 
                         computation process.
   */
  int maxRotations ();

  /*!
    \brief Set the maximum number of allowed Jacobi rotations.

    \param rotations -- Maximum number of allowed Jacobi rotations in the 
           computation process.
   */
  void setMaxRotations (const int& rotations);

  /*!
    \brief Return the eigenvalues of the matrix.
  */
  Vector<double> eigenvalues ();

  /*!
    \brief Return the eigenvector of the matrix.
  */
  Matrix<double> eigenvectors ();

 private:

  /*!
    \brief Copy the input matrix to the internal storage area.

    When provided a matrix, it is checked for being (a) square and (b)
    symmetric. If the checks are passed, the matrix is accepted and the maximum
    number of Jacobi rotations is set to a default value (2 times the
    dimensionality).
   */
  void setMatrix (const Matrix<double>& matrix);

  /*!
    \brief Start computation of the eigenvalues and eigenvectors

    At present this only will make a call to MatrixEigen::jacobi, but for a
    future version we may put in a switch here, to selected between different 
    algorithms.
  */
  void compute ();
  
  /*!
    \brief Eigenvalue decomposition
    
    This routine is adopted from <tt>jacobi</tt> of Numerical recipes:
    \verbatim
    void jacobi(float **a, int n, float d[], float **v, int *nrot)
    \endverbatim
    Computes all eigenvalues and eigenvectors of a real symmetric matrix
    <b>a[1..n][1..n]</b>. On output, elements of <b>a</b> above the diagonal
    are destroyed. <b>d[1..n]</b> returns the eigenvalues of <b>a</b>.
    <b>v[1..n][1..n]</b> is a matrix whose columns contain, on output, the
    normalized eigenvectors of <b>a</b>. <b>nrot</b> returns the number of 
    Jacobi rotations that were required. 
  */
  void jacobi ();

  /*!
    \brief Jacobi rotation.

    The Jacobi rotation is just a plane rotation designed to annihilate one
    of the off-diagonal matrix elements.

    \param mat -- Array, for which the rotation is performed.
    \param i   -- Array index.
    \param j   -- Array index.
    \param k   -- Array index.
    \param l   -- Array index.
  */
  void rotate (Matrix<double>& mat,
	       int& i,
	       int& j,
	       int& k,
	       int& l);
  
};

}

#endif
