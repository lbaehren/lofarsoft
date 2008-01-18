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

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Access to attributes
  //
  // ============================================================================

  void h5attribute_summary (std::ostream &os,
			    hid_t const &attribute_id)
  {
    hid_t dataspace_id = H5Aget_space(attribute_id);
    hid_t datatype_id  = H5Aget_type(attribute_id);
    hssize_t nofSelectedElements (0);
    int dataspaceDimensions (0);
    hsize_t *dimensions;
    hsize_t *maxDimensions;
    
    /* Probe the attribute's dataspace */
    if (dataspace_id > 0) {
      nofSelectedElements = H5Sget_select_npoints (dataspace_id);
      dataspaceDimensions = H5Sget_simple_extent_dims (dataspace_id,
						       dimensions,
						       maxDimensions);
    }
    
    os << "\tAttribute ID              = " << attribute_id << endl;
    os << "\tDataspace ID              = " << dataspace_id << endl;
    os << "\t-- nof. selected elements = " << nofSelectedElements << endl;
    os << "\t-- Dataspace dimensions   = " << dataspaceDimensions << endl;
    if (dataspaceDimensions > 0) {
      os << "[ ";
      for (int n(0);n<dataspaceDimensions; n++) {
	os << dimensions[n] << " ";
      }
    }
    cout << "\tDatatype ID               = " << datatype_id  << endl;    
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

  // --------------------------------------------------- h5get_attribute (string)
  
  bool h5get_attribute (std::string &value,
			hid_t const &attribute_id)
  {
    herr_t h5error (0);
    char *buffer;
    
    h5error = H5Aread(attribute_id,
		      H5T_NATIVE_CHAR,
		      buffer);

    if (h5error == 0) {
//       value = buffer;
      return true;
    } else {
      value = "";
      return false;
    }
  }
  
  // --------------------------------------------------- h5get_attribute (string)
  
  bool h5get_attribute (std::string &value,
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


} // Namespace CR -- end
