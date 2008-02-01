/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include "HDF5Common.h"

using std::cout;
using std::endl;

namespace LOFAR { // Namespace LOFAR -- begin
  
  // ============================================================================
  //
  //  Inspection and Feedback
  //
  // ============================================================================

  template<class T>
  void show_vector (std::ostream& os,
		    std::vector<T> &vec)
  {
    os << "[";
    for (uint n(0); n<vec.size(); n++) {
      os << " " << vec[n];
    }
    os << " ]";
  }

  // -------------------------------------------------------- h5attribute_summary

  void h5attribute_summary (std::ostream &os,
			    hid_t const &attribute_id)
  {
    bool status (true);
    
    /*
     * Datatype of the attribute
     */
    hid_t datatype_id        = H5Aget_type (attribute_id);
    hsize_t datatype_size    = H5Tget_size (datatype_id);
    bool datatype_is_integer = H5Tdetect_class (datatype_id,H5T_INTEGER);
    bool datatype_is_float   = H5Tdetect_class (datatype_id,H5T_FLOAT);
    bool datatype_is_string  = H5Tdetect_class (datatype_id,H5T_STRING);
    
    cout << "\t-- Datatype ID             = " << datatype_id  << endl;
    cout << "\t-- Datatype size [Bytes]   = " << datatype_size << endl;
    cout << "\t-- Datatype is H5T_INTEGER = " << datatype_is_integer << endl;
    cout << "\t-- Datatype is H5T_FLOAT   = " << datatype_is_float << endl;
    cout << "\t-- Datatype is H5T_STRING  = " << datatype_is_string << endl;

    H5Tclose (datatype_id);

    /*
     * Dataspace of the attribute
     */
    int rank (0);
    hid_t dataspace_id       = H5Aget_space (attribute_id);
    bool dataspace_is_simple = H5Sis_simple(dataspace_id);

    try {
      rank = H5Sget_simple_extent_ndims (dataspace_id);
    } catch (std::string message) {
      std::cerr << "[h5attribute_summary] " << message << endl;
      status = false;
    }

    cout << "\t-- Dataspace ID            = " << dataspace_id << endl;
    cout << "\t-- Dataspace is simple?    = " << dataspace_is_simple << endl;
    cout << "\t-- Rank of the data array  = " << rank << endl;

    H5Sclose (dataspace_id);

  }

  // ------------------------------------------------------ h5get_dataspace_shape

  bool h5get_dataspace_shape (std::vector<uint> &shape,
			      hid_t const &attribute_id)
  {
    bool status (true);

    hid_t dataspace_id       = H5Aget_space (attribute_id);
    bool dataspace_is_simple = H5Sis_simple(dataspace_id);
    int rank                 = H5Sget_simple_extent_ndims (dataspace_id);
    hsize_t *dimensions;

    if (rank > 0) {
      herr_t ret;
      dimensions = new hsize_t[rank];
      ret = H5Sget_simple_extent_dims(dataspace_id,
				      dimensions,
				      NULL);
      shape.resize(rank);
      for (int n(0); n<rank; n++) {
	shape[n] = dimensions[n];
      }

    } else {
      std::cerr << "[h5get_dataspace_shape] Error, rank of dataspace < 0"
		<< std::endl;
    }

    return status;
  }
  
  // ------------------------------------------------------ h5get_dataspace_shape

#ifdef HAVE_CASA
  bool h5get_dataspace_shape (casa::IPosition &shape,
			      hid_t const &attribute_id)
  {
    bool status (true);

    hid_t dataspace_id       = H5Aget_space (attribute_id);
    bool dataspace_is_simple = H5Sis_simple(dataspace_id);
    int rank                 = H5Sget_simple_extent_ndims (dataspace_id);
    hsize_t *dimensions;

    if (rank > 0) {
      herr_t ret;
      dimensions = new hsize_t[rank];
      ret = H5Sget_simple_extent_dims(dataspace_id,
				      dimensions,
				      NULL);
      shape.resize(rank);
      for (int n(0); n<rank; n++) {
	shape(n) = dimensions[n];
      }

    } else {
      std::cerr << "[h5get_dataspace_shape] Error, rank of dataspace < 0"
		<< std::endl;
    }

    return status;
  }
#endif
  
  // ============================================================================
  //
  //  Access attribute values
  //
  // ============================================================================

  // ------------------------------------------------------------ h5get_attribute

  template <class T>
  bool h5get_attribute (T &value,
			std::string const &name,
			hid_t const &locationID)
  {
    bool status (true);
    hid_t attribute_id (0);
    
    // get the identifier for the attribute
    attribute_id = H5Aopen_name(locationID,
				name.c_str());
    
    if (attribute_id > 0) {
      status = h5get_attribute (value,
				attribute_id);
      H5Aclose (attribute_id);
    } else {
      std::cerr << "[h5get_attribute] No valid ID for attribute "
		<< name 
		<< std::endl;
      status = false;
    }

    return status;
  }  

  // ------------------------------------------------------------ h5get_attribute

  template <class T>
  bool h5get_attribute (std::vector<T> &value,
			std::string const &name,
			hid_t const &locationID)
  {
    bool status (true);
    hid_t attribute_id (0);
    
    // get the identifier for the attribute
    attribute_id = H5Aopen_name(locationID,
				name.c_str());
    
    if (attribute_id > 0) {
      status = h5get_attribute (value,
				attribute_id);
      H5Aclose (attribute_id);
    } else {
      std::cerr << "[h5get_attribute] No valid ID for attribute "
		<< name 
		<< std::endl;
      status = false;
    }

    return status;
  }  

  // ------------------------------------------------------------ h5get_attribute

#ifdef HAVE_CASA
  template <class T>
  bool h5get_attribute (casa::Vector<T> &value,
			std::string const &name,
			hid_t const &locationID)
  {
    bool status (true);
    hid_t attribute_id (0);
    
    // get the identifier for the attribute
    attribute_id = H5Aopen_name(locationID,
				name.c_str());
    
    if (attribute_id > 0) {
      status = h5get_attribute (value,
				attribute_id);
      H5Aclose (attribute_id);
    } else {
      std::cerr << "[h5get_attribute] No valid ID for attribute "
		<< name 
		<< std::endl;
      status = false;
    }

    return status;
  }  
#endif

  // -------------------------------------------------------- h5get_attribute (T)
  
  template <class T>
  bool h5get_attribute (T &value,
			hid_t const &attribute_id)
  {
    // Get the datatype of the attribute's value
    hid_t datatype_id (H5Aget_type (attribute_id));
    
    // read attribute value into buffer
    herr_t h5error = H5Aread (attribute_id,
			      datatype_id,
			      &value);
    
    if (h5error == 0) {
      return true;
    } else {
      return false;
    }
  }
  
  // --------------------------------------------------- h5get_attribute (string)
  
  bool h5get_attribute (std::string &value,
			hid_t const &attribute_id)
  {
    bool status (true);
    herr_t h5error (0);
    hid_t dataspace_id = H5Aget_space (attribute_id);
    hid_t datatype_id  = H5Aget_type (attribute_id);
    
    if (datatype_id > 0) {
      if (H5Tget_class (datatype_id) == H5T_STRING) {
	char* data;
	hsize_t datatype_size = H5Tget_size (datatype_id);
	data = new char[datatype_size];
	h5error = H5Aread(attribute_id,
			  datatype_id,
			  data);
	if (h5error == 0) {
	  value = data;
	} else {
	  value  = "";      
	  status = false;
	}
      }
    } else {
      value  = "";      
      status = false;
    }
    
    // Release identifiers
    H5Tclose (datatype_id);
    H5Sclose (dataspace_id);

    return status;
  }
  
  // ------------------------------------------------ h5get_attribute (vector<T>)

  template <class T>
  bool h5get_attribute (std::vector<T> &value,
			hid_t const &attribute_id)
  {
    bool status (true);
    std::vector<uint> shape;

    // get the shape of the dataspace
    status = h5get_dataspace_shape (shape,attribute_id);

    if (shape.size() == 1) {
      // adjust size of vector returning the result
      value.resize(shape[0]);
      // additional local variables
      herr_t h5error (0);
      hid_t dataspace_id = H5Aget_space (attribute_id);
      hid_t datatype_id  = H5Aget_type (attribute_id);
      T *buffer;
      // allocate buffer memory
      buffer = new T [shape[0]];
      // read attribute value into buffer
      h5error = H5Aread (attribute_id,
			 datatype_id,
			 buffer);
      // copy retrieved data to returned vector
      if (h5error == 0) {
	for (uint n(0); n<shape[0]; n++) {
	  value[n] = buffer[n];
	}
      } else {
	std::cerr << "[h5get_attribute] Error reading value of attribute."
		  << std::endl;
	status = false;
      }
    } else {
      std::cerr << "[h5get_attribute] Wrong shape of attribute dataspace!"
		<< std::endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------ h5get_attribute (casa::Vector<T>)

#ifdef HAVE_CASA
  template <class T>
  bool h5get_attribute (casa::Vector<T> &value,
			hid_t const &attribute_id)
  {
    bool status (true);
    casa::IPosition shape;

    // get the shape of the dataspace
    status = h5get_dataspace_shape (shape,
				    attribute_id);

    if (shape.nelements() == 1) {
      // adjust size of vector returning the result
      value.resize(shape);
      // additional local variables
      herr_t h5error (0);
      hid_t dataspace_id = H5Aget_space (attribute_id);
      hid_t datatype_id  = H5Aget_type (attribute_id);
      T *buffer;
      // allocate buffer memory
      buffer = new T [shape(0)];
      // read attribute value into buffer
      h5error = H5Aread (attribute_id,
			 datatype_id,
			 buffer);
      // copy retrieved data to returned vector
      if (h5error == 0) {
	for (uint n(0); n<shape(0); n++) {
	  value(n) = buffer[n];
	}
      } else {
	std::cerr << "[h5get_attribute] Error reading value of attribute."
		  << std::endl;
	status = false;
      }
    } else {
      std::cerr << "[h5get_attribute] Wrong shape of attribute dataspace!"
		<< std::endl;
      status = false;
    }

    return status;
  }
#endif

  // ============================================================================
  //
  //  Dataspaces and Datatypes
  //
  // ============================================================================

  
  // ============================================================================
  //
  //  Template instantiation
  //
  // ============================================================================
  
  template void show_vector (std::ostream& os,
			     std::vector<uint> &vec);
  template void show_vector (std::ostream& os,
			     std::vector<int> &vec);
  template void show_vector (std::ostream& os,
			     std::vector<long> &vec);
  template void show_vector (std::ostream& os,
			     std::vector<float> &vec);
  template void show_vector (std::ostream& os,
			     std::vector<double> &vec);
  template void show_vector (std::ostream& os,
			     std::vector<std::string> &vec);
  
  template bool h5get_attribute (uint &value,
				 std::string const &name,
				 hid_t const &locationID);
  template bool h5get_attribute (int &value,
				 std::string const &name,
				 hid_t const &locationID);
  template bool h5get_attribute (double &value,
				 std::string const &name,
				 hid_t const &locationID);
  template bool h5get_attribute (std::string &value,
				 std::string const &name,
				 hid_t const &locationID);

  template bool h5get_attribute (std::vector<uint> &value,
				 std::string const &name,
				 hid_t const &location_id);
  template bool h5get_attribute (std::vector<int> &value,
				 std::string const &name,
				 hid_t const &location_id);
  template bool h5get_attribute (std::vector<double> &value,
				 std::string const &name,
				 hid_t const &location_id);

#ifdef HAVE_CASA
  template bool h5get_attribute (casa::Vector<uint> &value,
				 std::string const &name,
				 hid_t const &location_id);
  template bool h5get_attribute (casa::Vector<int> &value,
				 std::string const &name,
				 hid_t const &location_id);
  template bool h5get_attribute (casa::Vector<double> &value,
				 std::string const &name,
				 hid_t const &location_id);
#endif

} // Namespace LOFAR -- end
