#include <assert.h>
#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayError.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Slice.h>

/*!
  \class casa_casa

  \brief Test using casacore classes as private data in a custom class

  \author Larrs B&auml;hren
*/
template <class T> class casa_casa {

  casa::Vector<T> vector_p;
  casa::Matrix<T> matrix_p;
  casa::Cube<T> cube_p;
  casa::Array<T> array_p;

 public:

  //! Default constructor
  casa_casa () {};

  //! Argumented constructor
  casa_casa (casa::Vector<T> const &vector) {
    casa_casa<T>::setVector (vector);
  }

  //! Argumented constructor
  casa_casa (casa::Cube<T> const &cube) {
    casa_casa<T>::setCube (cube);
  }

  //! Get the vector from the private data area
  inline casa::Vector<T> getVector () const {
    return vector_p;
  }

  inline void setVector (casa::Vector<T> const &vector) {
    vector_p.resize (vector.shape());
    vector_p = vector;
  }
  
  //! Get the matrix from the private data area
  inline casa::Matrix<T> getMatrix () const {
    return matrix_p;
  }

  inline void setMatrix (casa::Matrix<T> const &matrix) {
    matrix_p.resize (matrix.shape());
    matrix_p = matrix;
  }
  
  //! Get the cube from the private data area
  inline casa::Cube<T> getCube () const {
    return cube_p;
  }

  inline void setCube (casa::Cube<T> const &cube) {
    cube_p.resize (cube.shape());
    cube_p = cube;
  }
  
  inline void setArray (casa::Array<T> const &array) {
    array_p.resize (array.shape());
    array_p = array;
  }

  inline void summary () {
    summary (std::cout);
  }

  void summary (std::ostream &os);    
  
};

