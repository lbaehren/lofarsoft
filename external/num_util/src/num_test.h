#ifndef NUM_TEST_H
#define NUM_TEST_H

#include <boost/python.hpp>

namespace num_test {

  /**
   *A test routine that instantiates a 1-dimensional numeric::array object by
   *extracting arguments from another numeric array.
   *@param arr a Boost.Python numeric array object.
   *@return a numeric array having the same data elements and size as 'arr'.
   */
  boost::python::numeric::array 
    test_1d_float_construct( boost::python::numeric::array arr );

  /**
   *A test routine that instantiates a n-dimensional numeric::array object by
   *extracting arguments from another numeric array.
   *@param arr a Boost.Python numeric array object.
   *@return a numeric array having the same data and dimensions as 'arr'.
   */
  boost::python::numeric::array 
    test_nd_double_construct( boost::python::numeric::array arr );

  /**
   *A test routine that creates a numeric::array object from another array, 
   *referencing the data.
   *@param arr a Boost.Python numeric array.
   *@return a numeric::array object referencing the input array.
   */
  boost::python::numeric::array 
    test_copy_construct( boost::python::numeric::array arr );


  /**
   *Creates a 1-dimensional numeric::array object with type defined by e_type. 
   *The elements of the array are initialized to zero.
   *@param size an integer indicates the size of the output array.
   *@param e_type a character representation of a Numeric type.
   *@return a 1-dimensional numeric::array object.
   */
  boost::python::numeric::array 
    test_int_type_construct( int size, char e_type );

  /**
   *Creates a n-dimensional numeric::array object with type defined by e_type
   *and shape defined by e_dims. The elements of the array are initialized to 
   *zero.
   *@param e_dims a Python tuple of integers representing the shape. 
   *@param e_type a character representation of a Numeric type.
   *@return a n-dimensional numeric::array object.
   */
  boost::python::numeric::array 
    test_dims_type_construct( boost::python::tuple e_dims, char e_type );

  /**
   *A test routine invokes the clone method from num_util.
   *@param ar a Boost.Python numeric array.
   *@return a replicate of the Boost/Python numeric array.
   */
  boost::python::numeric::array 
    test_clone( boost::python::numeric::array arr );

  /**
   *A test routine invokes the astype method from num_util.
   *@param arr a Boost.Python numeric array.
   *@param e_type a character representation of a Numeric type.
   *@return a clone of 'theArray' with the new Numeric type.
   */
  boost::python::numeric::array 
    test_astype( boost::python::numeric::array arr, char e_type );

  /**
   *A test routine that performs various checks on a numeric::array object. 
   *It invokes the check_dims, check_rank, check_size, check_type methods from 
   *num_util.
   *@param arr a Boost.Python numeric array.
   *@param e_dims the expected dimensions of 'arr'.
   *@param e_rank the expected rank of 'arr'.
   *@param e_size the expected size of 'arr'.
   *@param e_type the expected type of 'arr'.
   *@return -----
   */
  void test_checks( boost::python::numeric::array arr, 
		    boost::python::tuple e_dims, 
		    int e_rank, int e_size, char e_type );

  /**
   *A test routine that extracts the data from a numeric::array object and 
   *stores it in a Python list.
   *@param arr a Boost.Python numeric array.
   *@return a Python list of data from 'arr'.
   */
  boost::python::list test_data_float( boost::python::numeric::array arr );

  /**
   *A test routine that extracts the characteristics from a numeric::array 
   *object and stores them in a Python list.
   *@param arr a Boost.Python numeric array.
   *@return a Python list containing an array's characteristics.
   */
  boost::python::list test_stats( boost::python::numeric::array arr );

} // namespace num_test

#endif
