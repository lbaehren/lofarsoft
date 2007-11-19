
/* $Id: tArrays.cc,v 1.1 2006/05/04 11:46:29 bahren Exp $ */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>

using std::cout;
using std::endl;

using casa::Array;
using casa::Cube;
using casa::IPosition;
using casa::Matrix;
using casa::Slice;
using casa::Vector;

/*!
 \file tArrays.cc

 \ingroup CR_Data
 
 \brief A set of test for working with AIPS++ Arrays
 
 \author Lars B&auml;hren
 
 \date 2005/03/10
 
 <h3>Prerequisites</h3>
 
 <ul>
   <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Array.html">Array</a>
       A templated N-D Array class with zero origin.
   <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/IPosition.html">IPosition</a> 
   A Vector of integers, for indexing into Array objects
   <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/ArrayMath.html">ArrayMath</a> 
   Mathematical operations for Arrays.
 </ul>
*/

// =============================================================================

/*!
 \brief Test access to the array values via IPosition indexing.
 
 Directly access a set of array elements via IPosition. While using IPosition 
 objects provides an elegant and efficient way to handle the shape of arrays,
 care must be taken about not exceeding the size of the addressed array.
 
 \param cube -- Cube of doubles on which the tests are performed.

 \return ok -- Status of the performed operation; will return False if an 
         error was detected.
*/
bool test_accessIPosition (const IPosition& shape)
{
  cout << "\n[test_accessIPosition]\n" << endl;

  bool ok (true);
  Cube<double> cube (shape,1.0);
  Vector<double> vec (shape(2));
  IPosition shapeVec (1,shape(2));
  IPosition blc (shape);
  IPosition trc (shape);

  blc(2) = 0;
  trc(2) = shape(2)-1;

  for (int i=0; i<shape(0); i++) {
    blc(0) = trc(0) = i;
    for (int j=0; j<shape(1); j++) {
        blc(1) = trc(1) = j;
	//
	vec = cube(blc,trc).reform(shapeVec);
    }
  }

  return ok;
}

// =============================================================================

/*!
 \brief Test access to the array values via nested loops.
 
 Access the contents of the array on a per-element-basis.
 
 \param cube -- Cube of doubles on which the tests are performed.
*/
int test_accessNestedLoops1 (Cube<double>& cube)
{
  int nofFailedTests (0);
  IPosition shape (cube.shape());
  Vector<double> vec (shape(2));

  for (int i=0; i<shape(0); i++) {
    for (int j=0; j<shape(1); j++) {
      for (int k=0; k<shape(2); k++) {
	vec(k) = cube(i,j,k);
      }
    }
  }
  
  return nofFailedTests;
}

// =============================================================================

/*!
 \brief Test access to the array values via nested loops.
 
 Access the contents of the array on a per-element-basis.
 
 \param cube -- Cube of doubles on which the tests are performed.
*/
int test_accessNestedLoops2 (Cube<double>& cube)
{
  int nofFailedTests (0);
  IPosition shape (cube.shape());
  Vector<double> vec (shape(2));

  for (int k=0; k<shape(2); k++) {
    for (int j=0; j<shape(1); j++) {
      for (int i=0; i<shape(0); i++) {
	vec(k) = cube(i,j,k);
      }
    }
  }
  
  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test row-wise extraction of values from a matrix
*/
int test_extractVectorFromMatrix ()
{
  cout << "\n[test_extractVectorFromMatrix]\n" << endl;

  int nofFailedTests (0);
  IPosition shape (20,20);
  Matrix<double> mat (shape,1.0);

  {
    Vector<double> sumRows (shape(1));
    sumRows = 0.0;
    //
    for (int i(0); i<shape(1); i++) {
      sumRows += mat.row(i);
    }
  }

  return nofFailedTests;
}

// =============================================================================

/*!
  \brief Test working with the Slice class for N-dimensional arrays
  
   To fully specify a subarray, you must supply three pieces of information for
   each axis of the subarray:
   <ol>
     <li>where to start
     <li>how many elements to extract
     <li>what stride (or "increment" or "interval") to use: a stride of "n"
     means pick extract only every "nth" element along an axis 
   </ol>

   The program output should look like this:
   \verbatim
   [1] Manipulation of sub-matrices ...
   Axis Lengths: [10, 10]  (NB: Matrix in Row/Column order)
   [1, 1, 0, 0, 0, 0, 0, 0, 0, 0
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0
    0, 0, 3, 3, 0, 0, 0, 0, 0, 0
    0, 0, 3, 3, 0, 0, 0, 0, 0, 0
    0, 0, 0, 0, 5, 5, 0, 0, 0, 0
    0, 0, 0, 0, 5, 5, 0, 0, 0, 0
    0, 0, 0, 0, 0, 0, 7, 7, 0, 0
    0, 0, 0, 0, 0, 0, 7, 7, 0, 0
    0, 0, 0, 0, 0, 0, 0, 0, 9, 9
    0, 0, 0, 0, 0, 0, 0, 0, 9, 9]
   
   [2] Manipulation of row elements ...
   Axis Lengths: [10, 10]  (NB: Matrix in Row/Column order)
   [10, 0, 0, 0, 0, 0, 0, 0, 0, 0
    11, 11, 0, 0, 0, 0, 0, 0, 0, 0
    12, 12, 12, 0, 0, 0, 0, 0, 0, 0
    13, 13, 13, 13, 0, 0, 0, 0, 0, 0
    14, 14, 14, 14, 14, 0, 0, 0, 0, 0
    15, 15, 15, 15, 15, 15, 0, 0, 0, 0
    16, 16, 16, 16, 16, 16, 16, 0, 0, 0
    17, 17, 17, 17, 17, 17, 17, 17, 0, 0
    18, 18, 18, 18, 18, 18, 18, 18, 18, 0
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19]
   \endverbatim
*/
int test_ArraySlicing ()
{
  cout << "\n[test_ArraySlicing]\n" << endl;

  int nofFailedTests (0);
  int nelem (10);
  IPosition shape2d (2,nelem,nelem);
  IPosition shape3d (3,nelem,nelem,nelem);

  cout << "[1] Manipulation of sub-matrices ..." << endl;
  {
    Matrix<int> mat (shape2d);
    mat = 0;

    for (int n(0); n<shape2d(0); n+=2) {
      mat (Slice(n,2,1),Slice(n,2,1)) = n+1;
    }

    cout << mat << endl;
  }

  cout << "[2] Manipulation of row elements ..." << endl;
  {
    Matrix<int> mat (shape2d);
    mat = 0;
    
    for (int n(0); n<shape2d(0); n++) {
      mat (Slice(n,1,1),Slice(0,n+1,1)) = n+10;
    }

    cout << mat << endl;
  }

  cout << "[3] Manipulation of row elements ..." << endl;
  {
    Cube<int> mat (shape3d);
    mat = 0;
    
    for (int n(0); n<nelem; n++) {
      mat (Slice(0,n+1,1),Slice(0,n+1,1),Slice(n,1,1)) = n+1;
    }
    
    cout << mat << endl;
  }

  return nofFailedTests;
}

// --------------------------------------------------------- test_crossAssignment

/*!
  \brief Test cooperation and compatibility between the different Array classes

  As Cube, Matrix and Vector are specialization of the Array class, it should be
  possible to e.g. assign a Cube to an Array (and vice versa, if the shape
  matches); this function contains a number of simple tests to see if this is
  indeed the case.

  This test in fact is relevant for the concept behind the DataReader::readData
  method, which - depending on the data file format - will return a different 
  result.
*/
int test_crossAssignment () 
{
  cout << "\n[test_crossAssignment]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Assignment cube -> array" << endl;
  {
    IPosition shape (3,10,10,10);

    Cube<float> cube (shape);
    cube = 1.0;
    cout << " - shape of the input cube : " << cube.shape() << endl;
    cout << " - sum of the elements     : " << sum(cube) << endl;

    Array<float> array (cube);
    cout << " - shape of the derived array : " << array.shape() << endl;
    cout << " - sum of the elements        : " << sum(array) << endl;
  }
  
  cout << "[2] Assignment array -> cube" << endl;
  {
    IPosition shape (3,10,10,10);

    Array<float> array (shape);
    array = 1.0;
    cout << " - shape of the input array   : " << array.shape() << endl;
    cout << " - sum of the elements        : " << sum(array) << endl;

    Cube<float> cube (array);
    cout << " - shape of the derived cube  : " << cube.shape() << endl;
    cout << " - sum of the elements        : " << sum(cube) << endl;
  }
  
  cout << "[3] Assignment matrix -> array" << endl;
  {
    IPosition shape (2,10,10);

    Matrix<float> matrix (shape);
    matrix = 1.0;
    cout << " - shape of the input matrix : " << matrix.shape() << endl;
    cout << " - sum of the elements     : " << sum(matrix) << endl;

    Array<float> array (matrix);
    cout << " - shape of the derived array : " << array.shape() << endl;
    cout << " - sum of the elements        : " << sum(array) << endl;
  }
  
  cout << "[4] Assignment array -> matrix" << endl;
  {
    IPosition shape (2,10,10);

    Array<float> array (shape);
    array = 1.0;
    cout << " - shape of the derived array : " << array.shape() << endl;
    cout << " - sum of the elements        : " << sum(array) << endl;

    Matrix<float> matrix (array);
    cout << " - shape of the input matrix : " << matrix.shape() << endl;
    cout << " - sum of the elements     : " << sum(matrix) << endl;
  }
  
  return nofFailedTests;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main ()
{
  int nofFailedTests (0);

  IPosition shape (3,60,60,4096);

  {
    nofFailedTests += test_accessIPosition (shape);
  }
  
  {
    nofFailedTests += test_ArraySlicing ();
  }  
  
  {
    nofFailedTests += test_crossAssignment ();
  }  
  
  return nofFailedTests;
}
