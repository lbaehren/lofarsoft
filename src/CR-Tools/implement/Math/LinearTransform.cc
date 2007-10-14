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

  // ------------------------------------------------------------ LinearTransform
  
  LinearTransform::LinearTransform ()
  {
    init (1);
  }
  
  // ------------------------------------------------------------ LinearTransform
  
  LinearTransform::LinearTransform (int const &rank)
  {
    init (rank);
  }
  
  // ------------------------------------------------------------ LinearTransform
  
  LinearTransform::LinearTransform (std::vector<int> const &shape,
				    double const *matrix,
				    double const *shift)
  {
    shape_p.resize(2);

    if (shape.size() > 1) {
      shape_p[0] = shape[0];
      shape_p[1] = shape[1];
    } else {
      shape_p[0] = shape[0];
      shape_p[1] = shape[0];
    }

    if (!setMatrix (matrix)) {
      init (shape);
    }

    if (!setShift (shift)) {
      init (shape);
    }
  }
  
  // ------------------------------------------------------------ LinearTransform
  
#ifdef HAVE_BLITZ
  LinearTransform::LinearTransform (blitz::Array<double,2> const &matrix,
				    blitz::Array<double,1> const &shift)
  {
    bool status (true);

    /* Check if the shapes of the input arrays are consistent */
    if (matrix.cols() == shift.numElements()) {
      // store the shape information
      shape_p.resize(2);
      shape_p[0] = matrix.rows();
      shape_p[1] = matrix.cols();
      // store the transformation matrix
      matrix_p = new double [shape_p[0]*shape_p[1]];
      status = setMatrix (matrix);
      // store the translation vector
      shift_p = new double [shape_p[1]];
      status = setShift (shift);
    } else {
      std::cerr << "[LinearTransform::LinearTransform] Inconsisten array shapes!"
		<< std::endl;
      std::cerr << " --> Matrix : " << matrix.shape() << std::endl;
      std::cerr << " --> Shift  : " << shift.shape()  << std::endl;
    }
  }
#endif

  // ------------------------------------------------------------ LinearTransform
  
#ifdef HAVE_CASA
  LinearTransform::LinearTransform (casa::Matrix<double> const &matrix,
				    casa::Vector<double> const &shift)
  {
    bool status (true);
    casa::IPosition shapeMatrix (matrix.shape());
    casa::IPosition shapeShift (shift.shape());

    /* Check if the shapes of the input arrays are consistent */
    if (shapeMatrix(1) == shapeShift(0)) {
      // store shape information
      shape_p.resize(2);
      shape_p[0] = shapeMatrix(0);
      shape_p[1] = shapeMatrix(1);
      // store the transformation matrix
      matrix_p = new double [shape_p[0]*shape_p[1]];
      status = setMatrix (matrix);
      // store the translation vector
      shift_p = new double [shape_p[1]];
      status = setShift (shift);
    } else {
      std::cerr << "[LinearTransform::LinearTransform] Inconsisten array shapes!"
		<< std::endl;
      std::cerr << " --> Matrix : " << shapeMatrix << std::endl;
      std::cerr << " --> Shift  : " << shapeShift  << std::endl;
    }
  }
#endif
  
  // ------------------------------------------------------------ LinearTransform
  
  LinearTransform::LinearTransform (LinearTransform const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Initialization
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init
  
  void LinearTransform::init (int const &rank)
  {
    std::vector<int> shape (2);
    // stored shape values
    shape[0] = shape[1] = rank;
    // forwarding of parameters
    init (shape);
  }
  
  // ----------------------------------------------------------------------- init
  
  void LinearTransform::init (std::vector<int> const &shape)
  {
    if (shape.size() == 1) {
      init (shape[0]);
    } else {
      // set the shape of the matrix
      shape_p.resize(2);
      shape_p[0] = shape[0];
      shape_p[1] = shape[1];
      // initialize the transformation matrix
      matrix_p = new double[shape_p[0]*shape_p[1]];
      setUnitMatrix();
      // initialize the shift vector
      shift_p = new double[shape_p[0]];
      for (int n(0); n<shape_p[0]; n++) {
	shift_p[n] = 0.0;
      }
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
    shape_p.resize(2);
    shape_p = other.shape_p;
    
    matrix_p = new double [shape_p[0]*shape_p[0]];
    shift_p  = new double [shape_p[0]];

    int i (0);
    int j (0);
    int n (0);

    for (i=0; i<shape_p[0]; i++) {
      shift_p[i] = other.shift_p[i];
      for (j=0; j<shape_p[1]; j++) {
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
    int nelem (shape_p[0]*shape_p[1]);
    double element (0);

    /* Check if the input array has enough elements by trying to access the
       last expected element. */
    try {
      element = matrix[nelem-1];
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
	for (int n(0); n<nelem; n++) {
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
    if (matrix.rows() == shape_p[0] && matrix.cols() == shape_p[1]) {
      // local variables
      int row (0);
      int col (0);
      uint n(0);
      // copy the array elements
      for (row=0; row<shape_p[0]; row++) {
	for (col=0; col<shape_p[1]; col++) {
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

    if (shape(0) == shape_p[0] && shape(1) == shape_p[1]) {
      // local variables
      int row (0);
      int col (0);
      int n(0);
      // copy the array elements
      for (row=0; row<shape_p[0]; row++) {
	for (col=0; col<shape_p[1]; col++) {
	  matrix_p[n] = matrix(col,row);
	  n++;
	}
      }
    } else {
      std::cerr << "[LinearTransform::setMatrix] Wrong shape of input array!"
		<< std::endl;
      std::cerr << " -- Expected shape : " << showShape() << std::endl;
      std::cerr << " -- Provided shape : " << shape       << std::endl;
    }
    
    return status;
  }
#endif
  
  // ---------------------------------------------------------------------- shift
  
  void LinearTransform::shift (std::vector<double> &vect)
  {
    vect.resize(shape_p[0]);
    for (int n(0); n<shape_p[0]; n++) {
      vect[n] = shift_p[n];
    }
  }
#ifdef HAVE_BLITZ
  void LinearTransform::shift (blitz::Array<double,1> &shift)
  {
    shift.resize(shape_p[0]);
    for (int n(0); n<shape_p[0]; n++) {
      shift(n) = shift_p[n];
    }
  }
#endif
#ifdef HAVE_CASA
  void LinearTransform::shift (casa::Vector<double> &shift)
  {
    shift.resize(shape_p[0]);
    for (int n(0); n<shape_p[0]; n++) {
      shift(n) = shift_p[n];
    }
  }
#endif

  // ------------------------------------------------------------------- setShift

  bool LinearTransform::setShift (double const *shift)
  {
    bool status (true);
    double element (0);

    /* Check if the input array has enough elements by trying to access the
       last expected element. */
    try {
      element = shift[shape_p[0]-1];
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
      shift_p = new double [shape_p[0]];
      try {
	for (int n(0); n<shape_p[0]; n++) {
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
  
  bool LinearTransform::setShift (std::vector<double> const &shift)
  {
    bool status (true);

    if (int(shift.size()) == shape_p[0]) {
      shift_p = new double [shape_p[0]];
      try {
	for (int n(0); n<shape_p[0]; n++) {
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
    int nelem (shape_p[1]);

    try {
      if (shift.numElements() == nelem) {
	for (int n(0); n<nelem; n++) {
	  shift_p[n] = shift(n);
	}
      } else {
	std::cerr << "[LinearTransform::setShift] Incorrect shape of vector!"
		  << std::endl;
	status = false;
      }
    } catch (std::string message) {
      std::cerr << "[LinearTransform::setShift] " << message << std::endl;
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
    int nelem (shift.nelements());

    if (nelem == shape_p[0]) {
      for (int n(0); n<shape_p[0]; n++) {
	shift_p[n] = shift(n);
      }
    } else {
      std::cerr << "[LinearTransform::setShift] Wrong length of vector!"
		<< std::endl;
      std::cerr << " -- Expected vector length = " << shape_p[0] << std::endl;
      std::cerr << " -- Provided vector length = " << nelem      << std::endl;
      status = false;
    }
    
    return status;
  }
#endif
  
  // -------------------------------------------------------------------- summary
  
  void LinearTransform::summary (std::ostream &os)
  {
    os << "[LinearTransform::summary]" << std::endl;
    os << " -- Shape : " << showShape() << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // -------------------------------------------------------------- setUnitMatrix
  
  void LinearTransform::setUnitMatrix ()
  {
    int row (0);
    int col (0);
    int n (0);
    
    for (row=0; row<shape_p[0]; row++) {
      for (col=0; col<shape_p[0]; col++) {
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
    matrix_p[i*shape_p[0]+j] = x;
  }

  // ------------------------------------------------------------------------ det

  double LinearTransform::det ()
  {
    if (shape_p[0] == 2 && shape_p[1] == 2) {
      return (matrix_p[0]*matrix_p[3]-matrix_p[1]*matrix_p[2]);
    } else if (shape_p[0] == 3 && shape_p[0] == 3) {
      return (matrix(0,0)*matrix(1,1)*matrix(2,2)
	      +matrix(0,1)*matrix(1,2)*matrix(2,0)
	      +matrix(0,2)*matrix(1,0)*matrix(2,1)
	      -matrix(2,0)*matrix(1,1)*matrix(0,2)
	      -matrix(2,1)*matrix(1,2)*matrix(0,0)
	      -matrix(2,2)*matrix(1,0)*matrix(0,1));
    } else {
      std::cerr << "[LinearTransform::det] Not yet implemented for rank "
		<< shape_p[0] << std::endl;
      return 0;
    }
  }

  // -------------------------------------------------------------------- forward
  
  bool LinearTransform::forward (double *out,
				 double const *in)
  {
    bool status (true);
    int row (0);
    int col (0);
    int n (0);
    
    try {
      for (row=0; row<shape_p[0]; row++) {
	out[row] = shift_p[row];
	for (col=0; col<shape_p[1]; col++) {
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
    if (int(in.size()) != shape_p[1]) {
      std::cerr << "[LinearTransform::forward] Incorrect length of input vector!"
		<< std::endl;
      return false;
    }

    int row (0);
    int col (0);
    int n (0);
    
    out.resize (shape_p[0]);
    
    try {
      for (row=0; row<shape_p[0]; row++) {
	out[row] = shift_p[row];
	for (col=0; col<shape_p[1]; col++) {
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
    if (in.numElements() != shape_p[1]) {
      std::cerr << "[LinearTransform::forward] Incorrect length of input vector!"
		<< std::endl;
      return false;
    }
    
    int row (0);
    int col (0);
    int n (0);
    
    out.resize (shape_p[0]);
    
    try {
      for (row=0; row<shape_p[0]; row++) {
	out(row) = shift_p[row];
	for (col=0; col<shape_p[1]; col++) {
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
