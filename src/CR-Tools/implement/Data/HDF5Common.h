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

#ifndef HDF5COMMON_H
#define HDF5COMMON_H

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

#include <hdf5/H5LT.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \file HDF5Common.h
  
    \ingroup CR_Data
    
    \brief A collection of routines to work with HDF5 datasets
    
    \author Lars B&auml;hren

    \date 2008/01/17

    \test tHDF5Common.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>HDF5 Library
      <ul>
        <li><a href="http://hdfgroup.com/HDF5/doc/RM_H5Front.html">API
	Specification</a>
	<li><a href="http://hdf.ncsa.uiuc.edu/HDF5/doc/UG/index.html">User's
	Guide</a>
      </ul>
      <li>Classes to handle the interaction with LOFAR time-series datasets
      <ul>
        <li>CR::LOFAR_TBB
        <li>CR::LOFAR_StationGroup
        <li>CR::LOFAR_DipoleDataset
      </ul>
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  

  // ============================================================================
  //
  //  Access to attributes
  //
  // ============================================================================

  /*!
    \brief Provide a summary of an attribute's internal structure

    \param os           -- Output stream to which the summary is written
    \param attribute_id -- 
  */
  void h5attribute_summary (std::ostream &os,
			    hid_t const &attribute_id);

  /*!
    \brief Provide a summary of an attribute's internal structure

    \param os          -- Output stream to which the summary is written
    \param name        -- Name of the attribute, as which it is attached to
           another structure within the file
    \param location_id -- Identifier of the structure within the file, to which 
           the attribut is attached to.
  */
  void h5attribute_summary (std::ostream &os,
			    std::string const &name,
			    hid_t const &location_id);

  //@{
  /*!
    \brief Get the value of an attribute attached to a group or dataset

    \retval value       -- Value of the attribute
    \param attribute_id -- Identifier of the attribute within the HDF5 file
    
    \return status -- Status of the operation; returns <tt>false</tt> in case
            an error was encountered
  */
  bool h5get_attribute (uint &value,
			hid_t const &attribute_id);
  bool h5get_attribute (int &value,
			hid_t const &attribute_id);
  bool h5get_attribute (double &value,
			hid_t const &attribute_id);
  bool h5get_attribute (std::vector<uint> &value,
			hid_t const &attribute_id);
  bool h5get_attribute (std::vector<int> &value,
			hid_t const &attribute_id);
  bool h5get_attribute (std::vector<double> &value,
			hid_t const &attribute_id);
  //@}

  /*!
    \brief Get the value of an attribute attached to a group or dataset

    \retval value      -- Value of the attribute
    \param name        -- Name of the attribute
    \param location_id -- Identifier for the HDF5 object - file, group, dataset,
           array - the attribute is attached to.
    
    \return status -- Status of the operation; returns <tt>false</tt> in case
            an error was encountered
  */
  bool h5get_attribute (uint &value,
			std::string const &name,
			hid_t const &location_id);  
  bool h5get_attribute (int &value,
			std::string const &name,
			hid_t const &location_id);  
  bool h5get_attribute (double &value,
			std::string const &name,
			hid_t const &location_id);
  bool h5get_attribute (std::string &value,
			std::string const &name,
			hid_t const &location_id);

  // ============================================================================
  //
  //  Dataspaces and Datatypes
  //
  // ============================================================================

} // Namespace CR -- end

#endif /* HDF5COMMON_H */
  
