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

#include <cmath>
#include <iostream>
#include <string>

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/BasicSL/Complex.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

/*!
  \file tMatrixOperations.cc
  
  \ingroup CR_Math

  \brief Collection of tests on operations with/on matrices

  \author Lars B&auml;hren

  \date 2007/01/22
 */

// -----------------------------------------------------------------------------

int invert_complex_matrix ()
{
  cout << "\n[invert_complex_matrix]\n" << endl;

  int nofFailedTests (0);
  int dim (3);
  
  try {
    int row (0);
    int col (0);
    casa::Matrix<casa::DComplex> mat (dim,dim);
    casa::Matrix<casa::DComplex> matInv (dim,dim);

    for (row=0; row<dim; row++) {
      for (col=0; col<dim; col++) {
	mat(row,col) = casa::DComplex(row,col);
      }
    }

    matInv = conj(mat)/casa::DComplex(dim,0);
    
    // matrix multiplication

    casa::Matrix<casa::DComplex> result (dim,dim);
    int j (0);

    result = casa::DComplex(0,0);

    for (row=0; row<dim; row++) {
      for (col=0; col<dim; col++) {
	for (j=0; j<dim; j++) {
	  result(row,col) += mat(row,j)*matInv(j,col);
	}
      }
    }

    // display the matrices

    cout << mat    << endl;
    cout << matInv << endl;
    cout << result << endl;

  } catch (string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Main function 

  \return nofFailedTests -- The number of failed tests
*/
int main ()
{
  int nofFailedTests (0);

  nofFailedTests += invert_complex_matrix ();

  return nofFailedTests;  
}
