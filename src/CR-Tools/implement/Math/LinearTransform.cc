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

#include <Math/LinearTransform.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  LinearTransform::LinearTransform ()
  {
    init (1);
  }
  
  LinearTransform::LinearTransform (uint const &rank)
  {
    init (rank);
  }
  
  LinearTransform::LinearTransform (uint const &rank,
				    double const *matrix,
				    double const *shift)
  {
    rank_p = rank;

    if (!setMatrix (matrix)) {
      init (rank);
    }

    if (!setShift (shift)) {
      init (rank);
    }
  }
  
#ifdef HAVE_BLITZ
  LinearTransform::LinearTransform (blitz::Array<double,2> const &matrix,
				    blitz::Array<double,1> const &shift)
  {
    bool status (true);
    uint nelem (shift.numElements());

    /* Check if the shapes of the input arrays are consistent */
    if (matrix.rows() == nelem && matrix.cols() == nelem) {
      rank_p = nelem;
      status = setMatrix (matrix);
      status = setShift (shift);
    } else {
      std::cerr << "[LinearTransform::LinearTransform] Inconsisten array shapes!"
		<< std::endl;
      std::cerr << " --> Matrix : " << matrix.shape() << std::endl;
      std::cerr << " --> Shift  : " << shift.shape()  << std::endl;
    }
  }
#endif

#ifdef HAVE_CASA
  LinearTransform::LinearTransform (casa::Matrix<double> const &matrix,
				    casa::Vector<double> const &shift)
  {
    bool status (true);
    casa::IPosition shapeMatrix (matrix.shape());
    casa::IPosition shapeShift (shift.shape());

    /* Check if the shapes of the input arrays are consistent */
    if (shapeMatrix(0) == shapeShift(0) && shapeMatrix(1) == shapeShift(0)) {
      rank_p = shapeShift(0);
      status = setMatrix (matrix);
      status = setShift (shift);
    } else {
      std::cerr << "[LinearTransform::LinearTransform] Inconsisten array shapes!"
		<< std::endl;
      std::cerr << " --> Matrix : " << shapeMatrix << std::endl;
      std::cerr << " --> Shift  : " << shapeShift  << std::endl;
    }
  }
#endif
  
  LinearTransform::LinearTransform (LinearTransform const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Initialization
  //
  // ============================================================================
  
  void LinearTransform::init (uint const &rank)
  {
    rank_p = rank;
    
    matrix_p = new double[rank_p];
    setUnitMatrix();
    
    shift_p  = new double[rank_p];
    for (uint n(0); n<rank; n++) {
      shift_p[n] = 0.0;
    }
  }

  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  LinearTransform::~LinearTransform ()
  {
    destroy();
  }
  
  void LinearTransform::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  LinearTransform& LinearTransform::operator= (LinearTransform const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }

  // ----------------------------------------------------------------------- copy
  
  void LinearTransform::copy (LinearTransform const &other)
  {
    rank_p = other.rank_p;
    
    matrix_p = new double [rank_p*rank_p];
    shift_p  = new double [rank_p];

    uint i (0);
    uint j (0);
    uint n (0);

    for (i=0; i<rank_p; i++) {
      shift_p[i] = other.shift_p[i];
      for (j=0; j<rank_p; j++) {
	matrix_p[n] = other.matrix_p[n];
	n++;
      }
    }
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ------------------------------------------------------------------ setMatrix

  bool LinearTransform::setMatrix (double const *matrix)
  {
    bool status (true);
    uint nelem (rank_p*rank_p);

    /* Check if the input array has enough elements by trying to access the
       last expected element. */
    try {
      double elem = matrix[nelem-1];
    } catch (std::string message) {
      std::cerr << "[LinearTransform::setMatrix] To few elements in input array!"
		<< std::endl;
      std::cerr << " --> " << message << std::endl;
      status = false;
    }
    /*
      If the input array has (at least) the number of required elements, we start
      storing this.
     */
    if (status) {
      matrix_p = new double [nelem];
      try {
	for (uint n(0); n<nelem; n++) {
	  matrix_p[n] = matrix[n];
	}
      } catch (std::string message) {
	std::cerr << "[LinearTransform::setMatrix] Error while copying matrix values!"
		  << std::endl;
	std::cerr << " --> " << message << std::endl;
	status = false;
      }
    }

    return status;
  }

  // ------------------------------------------------------------------ setMatrix
  
#ifdef HAVE_BLITZ
  bool LinearTransform::setMatrix (blitz::Array<double,2> const &matrix)
  {
    bool status (true);

    /*
      Check if the provided matrix has the correct shape
    */
    if (matrix.rows() == rank_p && matrix.cols() == rank_p) {
      // local variables
      int row (0);
      int col (0);
      uint n(0);
      // copy the array elements
      for (row=0; row<rank_p; row++) {
	for (col=0; col<rank_p; col++) {
	  matrix_p[n] = matrix(col,row);
	  n++;
	}
      }
    } else {
      std::cerr << "[LinearTransform::setMatrix] Provided matrix has wrong shape!"
		<< std::endl;
      status = false;
    }
    
    return status;
  }
#endif
  
  // ------------------------------------------------------------------ setMatrix
  
#ifdef HAVE_CASA
  bool LinearTransform::setMatrix (casa::Matrix<double> const &matrix)
  {
    bool status (true);
    casa::IPosition shape (matrix.shape());

    if (shape(0) == rank_p && shape(1) == rank_p) {
      // local variables
      uint row (0);
      uint col (0);
      uint n(0);
      // copy the array elements
      for (row=0; row<rank_p; row++) {
	for (col=0; col<rank_p; col++) {
	  matrix_p[n] = matrix(col,row);
	  n++;
	}
      }
    } else {
      cerr::cout << "[LinearTransform::setMatrix] Wrong shape of input array!"
		 << std::endl;
      std::cerr << " -- Rank         : " << rank_p << std::endl;
      std::cerr << " -- Matrix shape : " << shape  << std::endl;
    }
    
    return status;
  }
#endif
  
  // ---------------------------------------------------------------------- shift
  
  void LinearTransform::shift (std::vector<double> &vect)
  {
    vect.resize(rank_p);
    for (int n(0); n<rank_p; n++) {
      vect[n] = shift_p[n];
    }
  }
#ifdef HAVE_BLITZ
  void LinearTransform::shift (blitz::Array<double,1> &shift)
  {
    shift.resize(rank_p);
    for (int n(0); n<rank_p; n++) {
      shift(n) = shift_p[n];
    }
  }
#endif
#ifdef HAVE_CASA
  void LinearTransform::shift (casa::Vector<double> &shift)
  {
    shift.resize(rank_p);
    for (int n(0); n<rank_p; n++) {
      vect(n) = shift_p[n];
    }
  }
#endif

  // ------------------------------------------------------------------- setShift

  bool LinearTransform::setShift (double const *shift)
  {
    bool status (true);

    /* Check if the input array has enough elements by trying to access the
       last expected element. */
    try {
      double elem = shift[rank_p-1];
    } catch (std::string message) {
      std::cerr << "[LinearTransform::setShift] To few elements in input array!"
		<< std::endl;
      std::cerr << " --> " << message << std::endl;
      status = false;
    }
    /*
      If the input array has (at least) the number of required elements, we start
      storing this.
     */
    if (status) {
      shift_p = new double [rank_p];
      try {
	for (uint n(0); n<rank_p; n++) {
	  shift_p[n] = shift[n];
	}
      } catch (std::string message) {
	std::cerr << "[LinearTransform::setShift] Error while copying shift values!"
		  << std::endl;
	std::cerr << " --> " << message << std::endl;
	status = false;
      }
    }

    return status;
  }

  // ------------------------------------------------------------------- setShift
  
#ifdef HAVE_BLITZ
  bool LinearTransform::setShift (blitz::Array<double,1> const &shift)
  {
    bool status (true);

    if (shift.numElements() == rank_p) {
      for (int n(0); n<rank_p; n++) {
	shift_p[n] = shift(n);
      }
    } else {
      status = false;
    }

    return status;
  }
#endif
  
  // ------------------------------------------------------------------- setShift
  
#ifdef HAVE_CASA
  bool LinearTransform::setShift (casa::Vector<double> const &shift)
  {
    bool status (true);

    return status;
  }
#endif
  
  // -------------------------------------------------------------------- summary
  
  void LinearTransform::summary (std::ostream &os)
  {
    os << "[LinearTransform::summary]" << std::endl;
    os << " -- Rank : " << rank_p      << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // -------------------------------------------------------------- setUnitMatrix
  
  void LinearTransform::setUnitMatrix ()
  {
    uint row (0);
    uint col (0);
    uint n (0);
    
    for (row=0; row<rank_p; row++) {
      for (col=0; col<rank_p; col++) {
	matrix_p[n] = 0;
	n++;
      }
      setMatrix (row,row,1.0);
    }
  }
  
  // ------------------------------------------------------------------ setMatrix

  void LinearTransform::setMatrix (uint const &i,
				   uint const &j,
				   double const &x)
  {
    matrix_p[i*rank_p+j] = x;
  }

  // ------------------------------------------------------------------------ det

  double LinearTransform::det ()
  {
    if (rank_p == 2) {
      return (matrix_p[0]*matrix_p[3]-matrix_p[1]*matrix_p[2]);
    } else if (rank_p == 2) {
      return (matrix(0,0)*matrix(1,1)*matrix(2,2)
	      +matrix(0,1)*matrix(1,2)*matrix(2,0)
	      +matrix(0,2)*matrix(1,0)*matrix(2,1)
	      -matrix(2,0)*matrix(1,1)*matrix(0,2)
	      -matrix(2,1)*matrix(1,2)*matrix(0,0)
	      -matrix(2,2)*matrix(1,0)*matrix(0,1));
    } else {
      std::cerr << "[LinearTransform::det] Not yet implemented for rank "
		<< rank_p << std::endl;
    }
  }

  // -------------------------------------------------------------------- forward
  
  bool LinearTransform::forward (double *out,
				 double const *in)
  {
    bool status (true);
    uint row (0);
    uint col (0);
    uint n (0);
    
    try {
      for (row=0; row<rank_p; row++) {
	out[row] = shift_p[row];
	for (col=0; col<rank_p; col++) {
	  out[row] += in[col]*matrix_p[n];
	    n++;
	}
      }
    } catch (std::string message) {
      std::cerr << "[LinearTransform::forward] " << message << std::endl;
      status = false;
    }

    return status;
  }
  
  // -------------------------------------------------------------------- forward
  
  bool LinearTransform::forward (std::vector<double> &out,
				 std::vector<double> const &in)
  {
    bool status (true);

    // check the length of the input vector
    if (in.size() != rank_p) {
      std::cerr << "[LinearTransform::forward] Incorrect length of input vector!"
		<< std::endl;
      return false;
    }

    uint row (0);
    uint col (0);
    uint n (0);
    
    out.resize (rank_p);
    
    try {
      for (row=0; row<rank_p; row++) {
	out[row] = shift_p[row];
	for (col=0; col<rank_p; col++) {
	  out[row] += in[col]*matrix_p[n];
	    n++;
	}
      }
    } catch (std::string message) {
      std::cerr << "[LinearTransform::forward] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // -------------------------------------------------------------------- forward
  
#ifdef HAVE_BLITZ
  bool LinearTransform::forward (blitz::Array<double,1> &out,
				 blitz::Array<double,1> const &in)
  {
    bool status (true);

    // check the length of the input vector
    if (in.numElements() != rank_p) {
      std::cerr << "[LinearTransform::forward] Incorrect length of input vector!"
		<< std::endl;
      return false;
    }
    
    uint row (0);
    uint col (0);
    uint n (0);
    
    out.resize (rank_p);
    
    try {
      for (row=0; row<rank_p; row++) {
	out(row) = shift_p[row];
	for (col=0; col<rank_p; col++) {
	  out(row) += in(col)*matrix_p[n];
	    n++;
	}
      }
    } catch (std::string message) {
      std::cerr << "[LinearTransform::forward] " << message << std::endl;
      status = false;
    }

    return status;
  }
#endif
  
} // Namespace CR -- end
