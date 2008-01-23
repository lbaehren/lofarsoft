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

#include <Data/HDF5Common.h>

using std::cout;
using std::endl;

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Access to attributes
  //
  // ============================================================================

  void h5attribute_summary (std::ostream &os,
			    hid_t const &attribute_id)
  {
    /*
     * Dataspace of the attribute
     */
    hid_t dataspace_id       = H5Aget_space (attribute_id);
    bool dataspace_is_simple = H5Sis_simple(dataspace_id);
    
    cout << "\t-- Dataspace ID            = " << dataspace_id << endl;
    cout << "\t-- Dataspace is simple?    = " << dataspace_is_simple << endl;
    
    H5Sclose (dataspace_id);

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

    /*
     * Dataspace of the attribute
     */
    int rank (0);
    hsize_t *dimensions;
    hsize_t *maxDimensions;

    H5Tclose (datatype_id);
  }
  
  // ----------------------------------------------------- h5get_attribute (uint)
  
  bool h5get_attribute (uint &value,
			hid_t const &attribute_id)
  {
    herr_t h5error (0);
    
    h5error = H5Aread(attribute_id,
		      H5T_NATIVE_UINT,
		      &value);

    if (h5error == 0) {
      return true;
    } else {
      return false;
    }
  }
  
  // ----------------------------------------------------- h5get_attribute (uint)
  
  bool h5get_attribute (uint &value,
			std::string const &name,
			hid_t const &locationID)
  {
    hid_t attribute_id (0);
    
    // get the identifier for the attribute
    attribute_id = H5Aopen_name(locationID,
				name.c_str());
    
    if (attribute_id > 0) {
      return h5get_attribute (value,
			      attribute_id);
    } else {
      std::cerr << "[h5get_attribute] No valid ID for attribute "
		<< name 
		<< std::endl;
      return false;
    }
  }  

  // ------------------------------------------------------ h5get_attribute (int)
  
  bool h5get_attribute (int &value,
			hid_t const &attribute_id)
  {
    herr_t h5error (0);
    
    h5error = H5Aread(attribute_id,
		      H5T_NATIVE_INT,
		      &value);

    if (h5error == 0) {
      return true;
    } else {
      return false;
    }
  }
  
  // ------------------------------------------------------ h5get_attribute (int)
  
  bool h5get_attribute (int &value,
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

  // --------------------------------------------------- h5get_attribute (double)
  
  bool h5get_attribute (double &value,
			hid_t const &attribute_id)
  {
    herr_t h5error (0);
    
    h5error = H5Aread(attribute_id,
		      H5T_NATIVE_DOUBLE,
		      &value);

    if (h5error == 0) {
      return true;
    } else {
      return false;
    }
  }
  
  // --------------------------------------------------- h5get_attribute (double)
  
  bool h5get_attribute (double &value,
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
  
  // --------------------------------------------------- h5get_attribute (string)
  
  bool h5get_attribute (std::string &value,
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
      H5Aclose(attribute_id);
    } else {
      std::cerr << "[h5get_attribute] No valid ID for attribute "
		<< name 
		<< std::endl;
      status = false;
    }
    
    return status;
  }  

  // ============================================================================
  //
  //  Dataspaces and Datatypes
  //
  // ============================================================================

  

} // Namespace CR -- end
