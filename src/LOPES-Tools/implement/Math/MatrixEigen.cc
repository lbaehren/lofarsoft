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
/* $Id: MatrixEigen.cc,v 1.3 2006/10/31 18:24:08 bahren Exp $*/

#include <Math/MatrixEigen.h>

using std::cout;
using std::cerr;
using std::endl;

namespace LOPES {
  
  // --- Construction ------------------------------------------------------------
  
  MatrixEigen::MatrixEigen (const Matrix<double>& matrix)
    : computationDone_p(false), maxRotations_p (50)
  {
    MatrixEigen::setMatrix (matrix);
  }
  
  MatrixEigen::MatrixEigen (const Matrix<double>& matrix,
			    const int& rotations)
    : computationDone_p(false), maxRotations_p (rotations)
  {
    MatrixEigen::setMatrix (matrix);
  }
  
  
  
  // --- Destruction -------------------------------------------------------------
  
  MatrixEigen::~MatrixEigen ()
  {;}
  
  // --- Parameters --------------------------------------------------------------
  
  void MatrixEigen::setMatrix (const Matrix<double>& matrix)
  {
    bool ok (true);
    IPosition shape (matrix.shape()); 
    int ip (0);
    int iq (0);
    
    // Check 1: Test if matrix is square
    if (shape(0) != shape(1)) {
      cerr << "[MatrixEigen::setMatrix] Input matrix is not square!" << endl;
      ok = false;
    } else {
      // Check 2: Test if matrix is symmetric
      for (iq=0; iq<shape(1); iq++) {
	for (ip=iq+1; ip<shape(0); ip++) {
	  if (matrix(ip,iq) != matrix(iq,ip)) {
	    cerr << "[MatrixEigen::setMatrix] Input matrix is not symmetric!" << endl;
	    ok = false;
	  }
	}
      }
    }
    
    //
    if (ok) {
      cout << "[MatrixEigen::setMatrix] Matrix is square and symmetric." << endl;
      //
      matrix_p.resize (matrix.shape());
      matrix_p = matrix;
      //
      MatrixEigen::setMaxRotations (2*shape(0));
      //
      MatrixEigen::compute ();
    }
  }
  
  int MatrixEigen::maxRotations ()
  {
    return maxRotations_p;
  }
  
  void MatrixEigen::setMaxRotations (const int& rotations)
  {
    maxRotations_p = rotations;
  }
  
  Vector<double> MatrixEigen::eigenvalues () { 
    return eigenvalues_p;
  }
  
  Matrix<double> MatrixEigen::eigenvectors () {
    return eigenvectors_p;
  }
  
  // --- Computation -------------------------------------------------------------
  
  void MatrixEigen::compute ()
  {
    MatrixEigen::jacobi ();
  }
  
  void MatrixEigen::jacobi ()
  {
    IPosition shape(matrix_p.shape());
    int n (shape(0));
    int numRotation (0);
    int j(0);
    int iq(0);
    int ip(0);
    float tresh (0.0);
    float theta(0.0);
    float t (0.0);
    float sumOffDiag(0.0);
    float c;
    Vector<float> b(n,0.0);
    Vector<float> z(n,0.0); 
    
    // Resize arrays
    eigenvalues_p.resize(n);
    eigenvectors_p.resize (shape);
    
    // Initialize to the identity matrix
    eigenvectors_p = 0.0;
    for (ip=0; ip<n; ip++) {
      eigenvectors_p(ip,ip) = 1.0;
    }
    
    // Initialize b and d to the diagonal of a
    for (ip=0;ip<n;ip++) {
      b(ip) = eigenvalues_p(ip) = matrix_p(ip,ip);
    }
    
    // Start of the computation
    nofRotations_p = 0;
    for (numRotation=0; numRotation<maxRotations_p; numRotation++) {
      
      // Sum off-diagonal elements
      sumOffDiag = 0.0;
      for (ip=0; ip<n; ip++) {
	for (iq=0; iq<n; iq++) {
	  sumOffDiag += fabs(matrix_p(ip,iq));
	}
      }
      
      // The normal return, which relies on quadratic convergence to
      // machine underflow
      if (sumOffDiag == 0.0) {
	return;
      }
      
      // on the first three sweeps ...
      if (numRotation<3) {
	tresh = 0.2*sumOffDiag/(n*n);
      }
      // ... thereafter
      else {
	tresh = 0.0;
      }
      
      for (ip=0; ip<n-1; ip++) {
	for (iq=ip+1; iq<n; iq++) {
	  g_p=100.0*fabs(matrix_p(ip,iq)); 
	  // After four sweeps, skip the rotation if the off-diagonal 
	  // element is small.
	  if (numRotation>3
	      && (float)(fabs(eigenvalues_p(ip))+g_p) == (float)fabs(eigenvalues_p(ip))
	      && (float)(fabs(eigenvalues_p(iq))+g_p) == (float)fabs(eigenvalues_p(iq))) {
	    matrix_p(ip,iq) = 0.0;
	  }
	  else if (fabs(matrix_p(ip,iq)) > tresh) {
	    h_p = eigenvalues_p(iq) - eigenvalues_p(ip);
	  }
	  //
	  if ((float)(fabs(h_p)+g_p) == (float)fabs(h_p)) {
	    t=(matrix_p(ip,iq))/h_p; 
	  }
	  else {
	    theta=0.5*h_p/(matrix_p(ip,iq));
	    t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
	    if (theta < 0.0) {
	      t = -t;
	    }
	  }
	  // compute variables
	  c = 1.0/sqrt(1+t*t);
	  s_p = t*c;
	  tau_p = s_p/(1.0+c);
	  h_p = t*matrix_p(ip,iq);
	  // Update the arrays
	  z(ip) -= h_p;
	  z(iq) += h_p;
	  eigenvalues_p(ip) -= h_p;
	  eigenvalues_p(iq) += h_p;
	  matrix_p(ip,iq) = 0.0;
	  // Rotations: This needs some extensive testing, as NR use 1-based array
	  // indexing, instead of 0-based array indexing.
	  for (j=0; j<ip-1; j++) {
	    rotate (matrix_p,j,ip,j,iq);
	  }
	  for (j=ip+1;j<=iq-1;j++) {
	    rotate (matrix_p,ip,j,j,iq);
	  }
	  for (j=iq+1;j<n;j++) {
	    rotate (matrix_p,ip,j,iq,j);
	  }
	  for (j=0; j<n; j++) {
	    rotate (eigenvectors_p,j,ip,j,iq);
	  }
	  // Update counter
	  ++nofRotations_p;
	  
	} // end: iq
	
      } // end: ip
      
    } // end: numRotation
    
    for (ip=0; ip<n; ip++) {
      b(ip) += z(ip);
      eigenvalues_p(ip) = b(ip);
      z(ip) = 0.0;
    }
    
  }
  
  void MatrixEigen::rotate (Matrix<double>& mat,
			    int& i,
			    int& j,
			    int& k,
			    int& l)
  {
    g_p = mat(i,j);
    h_p = mat(k,l);
    mat(i,j) = g_p - s_p*(h_p+g_p*tau_p);
  }
  
}  // Namespace LOPES -- end
