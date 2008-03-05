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

#include <Data/LOFAR_Timeseries.h>

namespace DAL { // Namespace DAL -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ----------------------------------------------------------- LOFAR_Timeseries
  
  LOFAR_Timeseries::LOFAR_Timeseries ()
  {
    filename_p = "UNDEFINED";
    fileID_p   = 0;
  }

  // ----------------------------------------------------------- LOFAR_Timeseries
  
  LOFAR_Timeseries::LOFAR_Timeseries (std::string const &filename)
  {
    init (filename);
  }
  
  // ----------------------------------------------------------- LOFAR_Timeseries
  
  LOFAR_Timeseries::LOFAR_Timeseries (LOFAR_Timeseries const &other)
  {
    // Initialize internal variables
    fileID_p   = 0;
    filename_p = "UNDEFINED";

    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  LOFAR_Timeseries::~LOFAR_Timeseries ()
  {
    destroy();
  }
  
  void LOFAR_Timeseries::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================

  // ------------------------------------------------------------------ operator=
  
  LOFAR_Timeseries& LOFAR_Timeseries::operator= (LOFAR_Timeseries const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy

  void LOFAR_Timeseries::copy (LOFAR_Timeseries const &other)
  {
    if (other.fileID_p == 0) {
      filename_p = "UNDEFINED";
      fileID_p   = 0;
    } else {
      init (other.filename_p);
    }
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void LOFAR_Timeseries::summary (std::ostream &os)
  {
    os << "[LOFAR_Timeseries] Summary of object properties"       << endl;
    os << "-- File name  ........  : " << filename_p              << endl;
    os << "-- File ID      ......  : " << fileID_p                << endl;

    /*
      Further output is conditional - only makes sense if successfully connected
      to a file
    */

    if (fileID_p > 0) {
      os << "-- nof. station groups  : " << groups_p.size()     << endl;
      os << "-- nof. dipole datasets : " << nofDipoleDatasets() << endl;
    }
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init
  
  void LOFAR_Timeseries::init (std::string const &filename)
  {
    herr_t h5error (0);

    // try to open the HDF5 file
    fileID_p = H5Fopen (filename.c_str(),
			H5F_ACC_RDONLY,
			H5P_DEFAULT);
    
    if (fileID_p > 0) {
      // store the filename
      filename_p = filename;
      // locate and register the station groups
      bool status = setStationGroups ();
    } else {
      std::cerr << "[LOFAR_Timeseries::init] Error opening file "
		<< filename
		<< std::endl;
    }
  }

  // ----------------------------------------------------------- setStationGroups
  
  bool LOFAR_Timeseries::setStationGroups ()
  {
    /* Check minimal condition for operations below. */
    if (fileID_p < 1) {
      return false;
    }

    /* Local variables. */

    bool status        = true;
    herr_t h5error     = 0;
    hsize_t nofObjects = 0;
    std::string name;

    try {
      // get the number of objects attached to the root level of the file
      h5error = H5Gget_num_objs(fileID_p,
				&nofObjects);
      if (nofObjects > 0) {
	// go through the list of objects to identify the station groups
	for (hsize_t n(0); n<nofObjects; n++) {
	  // check type of object
	  if (H5G_GROUP == H5Gget_objtype_by_idx (fileID_p,n)) {
	    // get the name of the group
	    status = DAL::h5get_name (name,
				      fileID_p,
				      n);
	    /*
	     * If retrieval of group name was successful, create new
	     * LOFAR_StationGroup object to be added to the internal list.
	     */
	    if (status) {
	      groups_p.push_back(DAL::LOFAR_StationGroup(filename_p,
							 name));
	    } else {
	      std::cerr << "[LOFAR_Timeseries::setStationGroups]"
			<< " Error retrieving group name!"
			<< std::endl;
	    }
	  }
	}
      } else {
	std::cout << "[LOFAR_Timeseries::setStationGroups]"
		  << " No objects found at root level of the file!" << std::endl;
      }
    } catch (std::string message) {
      std::cerr << "[LOFAR_Timeseries::setStationGroups] " << message
		<< std::endl;
      return false;
    }
    
    return true;
  }
  
  // ------------------------------------------------------------------------- fx

  casa::Matrix<short> LOFAR_Timeseries::fx (int const &start,
					    int const &nofSamples)
  {
    /* Check minimal condition for operations below. */
    if (fileID_p < 1) {
      return casa::Matrix<short> (1,1,0);
    }

    uint n (0);
    uint station(0);
    uint dipole(0);
    std::vector<uint> selection (1);
    uint nofStations = groups_p.size();
    uint nofDipoles  = nofDipoleDatasets();
    casa::Matrix<short> data (nofSamples,nofDipoles);
    casa::Vector<short> tmp (nofSamples);
    
    for (station=0; station<nofStations; station++) {
      // get the number dipoles 
      nofDipoles = groups_p[station].nofDipoleDatasets();
      for (dipole=0; dipole<nofDipoles; dipole++) {
	selection[0] = dipole;
	// get the channel data ...
	tmp = groups_p[station].fx(start,
				   nofSamples,
				   selection);
	// ... and add them to the returned array
	data.column(n) = tmp;
	// increment the counter for the number of dipoles
	n++;
      }
    }
    
    return data;
    
  }
  
} // Namespace DAL -- end
