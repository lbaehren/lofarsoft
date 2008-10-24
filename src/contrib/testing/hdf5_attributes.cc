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

/*!
  \file hdf5_attributes.cc

  \ingroup contrib

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
  std::cout << "[h5set_attribute]" << std::endl;
  std::cout << "-- datatype        = " << datatype    << std::endl;
  std::cout << "-- location ID     = " << location_id << std::endl;
  std::cout << "-- attribute name  = " << name << std::endl;
  std::cout << "-- attribute size  = " << size << std::endl;
  std::cout << "-- attribute value = [";
  for (int n(0); n<size; n++) {
    std::cout << " " << value[n];
  }
  std::cout << " ]" << std::endl;
  
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

//_______________________________________________________________________________

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
}

//_______________________________________________________________________________

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
// Main function

int main ()
{

  int nofFailedTests (0);
  bool status (true);
  std::string file_name ("test.h5");
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
    hid_t location_id = 1;
    int data_int      = 1;
    uint data_uint    = 2;
    uint data_short   = 3;
    uint data_long    = 4;
    uint data_float   = 5;
    uint data_double  = 6;
    //
    status = h5set_attribute (location_id, "ATTR_INT", data_int);
    status = h5set_attribute (location_id, "ATTR_UINT", data_uint);
    status = h5set_attribute (location_id, "ATTR_SHORT", data_short);
    status = h5set_attribute (location_id, "ATTR_LONG", data_long);
    status = h5set_attribute (location_id, "ATTR_FLOAT", data_float);
    status = h5set_attribute (location_id, "ATTR_DOUBLE", data_double);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[3] Test writing of vector<T> attributes ..." << std::endl;
  try {
    int nelem (3);
    hid_t location_id = 1;
    std::vector<int> data_int (nelem,1);
    std::vector<uint> data_uint (nelem,2);
    std::vector<short> data_short (nelem,3);
    std::vector<long> data_long (nelem,4);
    std::vector<float> data_float (nelem,5);
    std::vector<double> data_double (nelem,6);
    //
    status = h5set_attribute (location_id, "ATTR_INT_VECTOR", data_int);
    status = h5set_attribute (location_id, "ATTR_UINT_VECTOR", data_uint);
    status = h5set_attribute (location_id, "ATTR_SHORT_VECTOR", data_short);
    status = h5set_attribute (location_id, "ATTR_LONG_VECTOR", data_long);
    status = h5set_attribute (location_id, "ATTR_FLOAT_VECTOR", data_float);
    status = h5set_attribute (location_id, "ATTR_DOUBLE_VECTOR", data_double);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  return nofFailedTests;
}
