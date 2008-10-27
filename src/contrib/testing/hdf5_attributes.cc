/*-------------------------------------------------------------------------*
 | $Id:: TimeFreqSkymap.cc 1967 2008-09-09 13:16:22Z baehren             $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <iostream>
#include <string>
#include <vector>

#include <hdf5/hdf5.h>

const int32_t MAX_COL_NAME_SIZE = 256;

/*!
  \file hdf5_attributes.cc

  \ingroup contrib

  \brief Test access schemes and implementation strategies to HDF5 attributes

  \author Lars B&auml;hren

  \date 2008/10/24

  A collection of functions to explore how to properly set up the write/read
  access to HDF5 attributes with the DAL.
*/

//_______________________________________________________________________________

/*!
  \param datatype    -- HDF5 datatype of the attribute
  \param location_id -- HDF5 identifier of the attribute within the file
  \param name        -- Name of the attribute
  \param value       -- Value of the attribute
  \param size        -- The size of the attribute
*/
template <typename T>
bool h5set_attribute (hid_t const &datatype,
		      hid_t const &location_id,
		      std::string name,
		      T * value,
		      int size)
{
  bool status          = true;
  hid_t   attribute_id = 0;  /* Attribute identifier */
  hid_t   dataspace_id = 0;  /* Attribute dataspace identifier */
  hsize_t dims[1]      = { size };
  
  dataspace_id  = H5Screate_simple( 1, dims, NULL );
  if ( dataspace_id < 0 ) {
    std::cerr << "ERROR: Could not set attribute '" << name
	      << "' dataspace.\n";
    return false;
  }
  
  attribute_id = H5Acreate( location_id, name.c_str(),
			    datatype, dataspace_id, NULL, NULL );
  if ( attribute_id < 0 ) {
    std::cerr << "ERROR: Could not create attribute '" << name
	      << "'.\n";
    return false;
  }
  
  if ( H5Awrite(attribute_id, datatype, value) < 0 ) {
    std::cerr << "ERROR: Could not write attribute '" << name << "'.\n";
    return false;
  }
  
  if ( H5Aclose( attribute_id ) < 0 ) {
    std::cerr << "ERROR: Could not close attribute '" << name << "'.\n";
    status = false;
  }
  
  return true;
}

template <>
bool h5set_attribute (hid_t const &datatype,
		      hid_t const &location_id,
		      std::string name,
		      std::string * value,
		      int size)
{
  bool status          = true;
  hid_t   attribute_id = 0;  /* Attribute identifier */
  hid_t   dataspace_id = 0;  /* Attribute dataspace identifier */
  hsize_t dims[1]      = { size };
  
  char ** string_attr = (char**)malloc( size * sizeof(char*) );
  for ( int ii = 0; ii < size; ii++ ) {
    string_attr[ii] = (char*)malloc(MAX_COL_NAME_SIZE * sizeof(char));
    strcpy( string_attr[ii], value[ii].c_str() );
  }
  
  hid_t type = H5Tcopy (H5T_C_S1);
  if ( type < 0 ) {
    std::cerr << "ERROR: Could not set attribute '" << name << "' type."
	      << std::endl;
    return false;
  }
  
  if ( H5Tset_size(type, H5T_VARIABLE) < 0 ) {
    std::cerr << "ERROR: Could not set attribute '" << name
	      << "' size." << std::endl;
    return false;
  }
  
  dataspace_id = H5Screate_simple(1, dims, NULL);
  if ( dataspace_id < 0 ) {
    std::cerr << "ERROR: Could not set attribute '" << name
	      << "' dataspace.\n";
    return false;
  }
  
  attribute_id = H5Acreate( location_id,
			    name.c_str(),
			    type,
			    dataspace_id,
			    NULL, NULL );
  if ( attribute_id < 0 ) {
    std::cerr << "ERROR: Could not create attribute '" << name << "'.\n";
    return false;
  }
  
  if ( H5Awrite( attribute_id, type, string_attr ) < 0 ) {
    std::cerr << "ERROR: Could not write attribute '" << name << "'.\n";
    return false;
  }
  
  if (  H5Aclose( attribute_id ) < 0 ) {
    std::cerr << "ERROR: Could not close attribute '" << name << "'.\n";
    return false;
  }
  
  for ( int ii = 0; ii < size; ii++ ) {
    free( string_attr[ii] );
  }
  free( string_attr );
  
  return true;
}

//_______________________________________________________________________________

/*!
  \param location_id -- HDF5 identifier of the attribute within the file
  \param name        -- Name of the attribute
  \param value       -- Value of the attribute
  \param size        -- The size of the attribute
*/
template <typename T>
bool h5set_attribute (hid_t const &location_id,
		      std::string name,
		      T * value,
		      int size);

template <>
bool h5set_attribute (hid_t const &location_id,
		      std::string name,
		      int * value,
		      int size)
{
  hid_t datatype = H5T_NATIVE_INT;
  return h5set_attribute (datatype,
			  location_id,
			  name,
			  value,
			  size);
}

template <>
bool h5set_attribute (hid_t const &location_id,
		      std::string name,
		      uint * value,
		      int size)
{
  hid_t datatype = H5T_NATIVE_UINT;
  return h5set_attribute (datatype,
			  location_id,
			  name,
			  value,
			  size);
}

template <>
bool h5set_attribute (hid_t const &location_id,
		      std::string name,
		      short * value,
		      int size)
{
  hid_t datatype = H5T_NATIVE_SHORT;
  return h5set_attribute (datatype,
			  location_id,
			  name,
			  value,
			  size);
}

template <>
bool h5set_attribute (hid_t const &location_id,
		      std::string name,
		      long * value,
		      int size)
{
  hid_t datatype = H5T_NATIVE_LONG;
  return h5set_attribute (datatype,
			  location_id,
			  name,
			  value,
			  size);
}

template <>
bool h5set_attribute (hid_t const &location_id,
		      std::string name,
		      float * value,
		      int size)
{
  hid_t datatype = H5T_NATIVE_FLOAT;
  return h5set_attribute (datatype,
			  location_id,
			  name,
			  value,
			  size);
}

template <>
bool h5set_attribute (hid_t const &location_id,
		      std::string name,
		      double * value,
		      int size)
{
  hid_t datatype = H5T_NATIVE_DOUBLE;
  return h5set_attribute (datatype,
			  location_id,
			  name,
			  value,
			  size);
}

template <>
bool h5set_attribute (hid_t const &location_id,
		      std::string name,
		      std::string * value,
		      int size)
{
  hid_t datatype = H5T_STRING;
  return h5set_attribute (datatype,
			  location_id,
			  name,
			  value,
			  size);
}

//_______________________________________________________________________________

/*!
  \param location_id -- HDF5 identifier of the attribute within the file
  \param name        -- Name of the attribute
  \param value       -- Value of the attribute
*/
template <typename T>
bool h5set_attribute (hid_t location_id,
		      std::string name,
		      std::vector<T> const &value)
{
  T * data;
  int nelem = value.size();

  data = new T [nelem];
  
  for (int n(0); n<nelem; n++) {
    data[n] = value[n];
  }
  
  return h5set_attribute (location_id,
			  name,
			  data,
			  nelem);

  delete [] data;
}

//_______________________________________________________________________________

/*!
  \param location_id -- HDF5 identifier of the attribute within the file
  \param name        -- Name of the attribute
  \param value       -- Value of the attribute
*/
template <typename T>
bool h5set_attribute (hid_t location_id,
		      std::string name,
		      T const &value)
{
  int nelem (1);
  T * data = new T [nelem];

  data[0] = value;
  
  return h5set_attribute (location_id,
			  name,
			  data,
			  nelem);
}

//_______________________________________________________________________________

/*!
  \param location_id -- HDF5 identifier of the attribute within the file
  \param name        -- Name of the attribute
  \param value       -- Value of the attribute
*/
#ifdef HAVE_CASA
template <typename T>
bool h5set_attribute (hid_t location_id,
		      std::string name,
		      casa::Vector<T> const &value)
{
  T * data;
  int nelem = value.nelements();

  data = new T [nelem];
  
  for (int n(0); n<nelem; n++) {
    data[n] = value(n);
  }
  
  return h5set_attribute (location_id,
			  name,
			  data,
			  nelem);
  
  delete [] data;
}
#endif

// ==============================================================================
//
//  Main function
//
// ==============================================================================

int main ()
{
  
  int nofFailedTests (0);
  bool status (true);
  std::string file_name ("test.h5");
  int nelem (3);
  hid_t file_id;
  
  //__________________________________________________________________
  
  std::cout << "[1] Creating HDF5 file for testing ..." << std::endl;
  try {
    file_id = H5Fcreate (file_name.c_str(),
			 H5F_ACC_TRUNC,
			 H5P_DEFAULT,
			 H5P_DEFAULT);
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  if (nofFailedTests || file_id < 0) {
    std::cerr << "ERROR: Failed to create HDF5 file!" << std::endl;
    return nofFailedTests;
  }
  
  //__________________________________________________________________

  std::cout << "[2] Test writing of single valued attributes ..." << std::endl;
  try {
    int data_int      = 1;
    uint data_uint    = 2;
    uint data_short   = 3;
    uint data_long    = 4;
    uint data_float   = 5;
    uint data_double  = 6;
    std::string data_string = "Hello";
    //
    std::cout << "-- Writing int ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_INT", data_int);
    std::cout << "-- Writing uint ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_UINT", data_uint);
    std::cout << "-- Writing short ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_SHORT", data_short);
    std::cout << "-- Writing long ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_LONG", data_long);
    std::cout << "-- Writing float ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_FLOAT", data_float);
    std::cout << "-- Writing double ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_DOUBLE", data_double);
    std::cout << "-- Writing std::string ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_STRING", data_string);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[3] Test writing of T* attributes ..." << std::endl;
  try {
    int * data_int = new int [nelem];
    data_int[0] = 1;
    data_int[1] = 2;
    data_int[2] = 3;
    std::cout << "-- Writing int * ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_INT_ARRAY", data_int, nelem);
    //
    uint * data_uint = new uint [nelem];
    data_uint[0] = 4;
    data_uint[1] = 5;
    data_uint[2] = 6;
    std::cout << "-- Writing uint * ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_UINT_ARRAY", data_uint, nelem);
    //
    short * data_short = new short [nelem];
    data_short[0] = 7;
    data_short[1] = 8;
    data_short[2] = 9;
    std::cout << "-- Writing short * ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_SHORT_ARRAY", data_short, nelem);
    //
    long * data_long = new long [nelem];
    data_long[0] = 10;
    data_long[1] = 11;
    data_long[2] = 12;
    std::cout << "-- Writing long * ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_LONG_ARRAY", data_long, nelem);
    //
    float * data_float = new float [nelem];
    data_float[0] = 13;
    data_float[1] = 14;
    data_float[2] = 15;
    std::cout << "-- Writing float * ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_FLOAT_ARRAY", data_float, nelem);
    //
    double * data_double = new double [nelem];
    data_double[0] = 16;
    data_double[1] = 17;
    data_double[2] = 18;
    std::cout << "-- Writing double * ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_DOUBLE_ARRAY", data_double, nelem);
    //
    std::string * data_string = new std::string [nelem];
    data_string[0] = "a";
    data_string[1] = "b";
    data_string[2] = "c";
    std::cout << "-- Writing std::string * ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_STRING_ARRAY", data_string, nelem);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[4] Test writing of vector<T> attributes ..." << std::endl;
  try {
    std::vector<int> data_int (nelem,1);
    std::vector<uint> data_uint (nelem,2);
    std::vector<short> data_short (nelem,3);
    std::vector<long> data_long (nelem,4);
    std::vector<float> data_float (nelem,5);
    std::vector<double> data_double (nelem,6);
    //
    std::cout << "-- Writing std::vector<int> ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_INT_VECTOR", data_int);
    std::cout << "-- Writing std::vector<uint> ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_UINT_VECTOR", data_uint);
    std::cout << "-- Writing std::vector<short> ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_SHORT_VECTOR", data_short);
    std::cout << "-- Writing std::vector<long> ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_LONG_VECTOR", data_long);
    std::cout << "-- Writing std::vector<float> ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_FLOAT_VECTOR", data_float);
    std::cout << "-- Writing std::vector<double> ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_DOUBLE_VECTOR", data_double);
    //
    std::vector<std::string> data_string (nelem);
    data_string[0] = "a";
    data_string[1] = "b";
    data_string[2] = "c";
    std::cout << "-- Writing std::vector<std::string> ..." << std::endl;
    status = h5set_attribute (file_id, "ATTR_STRING_VECTOR", data_string);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  return nofFailedTests;
}
