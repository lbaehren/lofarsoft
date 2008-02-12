/*-------------------------------------------------------------------------*
 | $Id:: NewClass.cc 1159 2007-12-21 15:40:14Z baehren                   $ |
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

#include <Data/LOFAR_DipoleDataset.h>
#include <Data/LOFAR_Attributes.h>

namespace LOFAR { // Namespace LOFAR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // -------------------------------------------------------- LOFAR_DipoleDataset

  LOFAR_DipoleDataset::LOFAR_DipoleDataset ()
  {
    datasetID_p = 0;
  }
  
  // -------------------------------------------------------- LOFAR_DipoleDataset
  
  LOFAR_DipoleDataset::LOFAR_DipoleDataset (std::string const &filename,
					    std::string const &dataset)
  {
    hid_t file_id (0);

    // open the file
    file_id = H5Fopen (filename.c_str(),
		       H5F_ACC_RDONLY,
		       H5P_DEFAULT);

    // if opening of file was successfull, try to open dataset
    if (file_id > 0) {
      init (file_id,
	    dataset);
    } else {
      std::cerr << "[LOFAR_DipoleDataset] Error opening HDF5 file "
		<< filename 
		<< " !"
		<< std::endl;
    }

    // release the global file handle
    herr_t h5error = H5Fclose (file_id);
  }

  // -------------------------------------------------------- LOFAR_DipoleDataset

  LOFAR_DipoleDataset::LOFAR_DipoleDataset (hid_t const &location,
					    std::string const &dataset)
  {
    init (location,
	  dataset);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  LOFAR_DipoleDataset::~LOFAR_DipoleDataset ()
  {
    destroy();
  }

  // -------------------------------------------------------------------- destroy
  
  void LOFAR_DipoleDataset::destroy ()
  {
    /*
      If an identifier to the dataset was assigned, we need to release it; if
      no assignment was done, there is nothing else to be done here.
    */
    if (datasetID_p) {
      herr_t h5error = H5Dclose (datasetID_p);
    }
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // -------------------------------------------------------------------- summary
  
  void LOFAR_DipoleDataset::summary (std::ostream &os)
  {
    os << "[LOFAR_DipoleDataset::summary]"   << std::endl;
    os << "-- Dataset ID          = " << datasetID_p  << std::endl;
  
    if (datasetID_p) {
      os << "-- STATION_ID          = " << station_id()          << std::endl;
      os << "-- RSP_ID              = " << rsp_id()              << std::endl;
      os << "-- RCU_ID              = " << rcu_id()              << std::endl;
      os << "-- SAMPLE_FREQUENCY    = " << sample_frequency()    << std::endl;
      os << "-- NYQUIST_ZONE        = " << nyquist_zone()        << std::endl;
      os << "-- TIME                = " << time()                << std::endl;
      os << "-- SAMPLE_NUMBER       = " << sample_number()       << std::endl;
      os << "-- SAMPLES_PER_FRAME   = " << samples_per_frame()   << std::endl;
      os << "-- FEED                = " << feed()                << std::endl;
//       os << "-- ANTENNA_POSITION    = " << antenna_position()    << std::endl;
//       os << "-- ANTENNA_ORIENTATION = " << antenna_orientation() << std::endl;
    }
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init

  void LOFAR_DipoleDataset::init (hid_t const &location,
				  std::string const &dataset)
  {
    bool status (true);
    hid_t dataset_id (0);

    /*
      Try to open the dataset within the HDF5 file; the dataset is expected
      to reside below the object identified by "location".
    */
    try {
      dataset_id = H5Dopen (location,
			    dataset.c_str());
    } catch (std::string message) {
      std::cerr << "[LOFAR_DipoleDataset::init] " << message << std::endl;
      status = false;
    }

    if (dataset_id > 0) {
      datasetID_p = dataset_id;
    } else {
      datasetID_p = 0;
    }
    
  }
  
  // --------------------------------------------------------------- attribute_id
  
  hid_t LOFAR_DipoleDataset::attribute_id (std::string const &name)
  {
    hid_t id (0);
    
    /*
     * First check if the attribute actually exists
     */
    id = H5Aopen_name(datasetID_p,
		      name.c_str());

    
    return id;
  }

  // ----------------------------------------------------------------- station_id

  uint LOFAR_DipoleDataset::station_id ()
  {
    uint id (0);

    if (h5get_attribute(id,
			attribute_name(LOFAR::STATION_ID),
			datasetID_p)) {
      return id;
    } else {
      return 0;
    }
  }
  
  // --------------------------------------------------------------------- rsp_id

  uint LOFAR_DipoleDataset::rsp_id ()
  {
    uint id (0);

    if (h5get_attribute(id,
			attribute_name(LOFAR::RSP_ID),
			datasetID_p)) {
      return id;
    } else {
      return 0;
    }
  }
  
  // --------------------------------------------------------------------- rcu_id

  uint LOFAR_DipoleDataset::rcu_id ()
  {
    uint val (0);

    if (h5get_attribute(val,
			attribute_name(LOFAR::RCU_ID),
			datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // ----------------------------------------------------------------------- time

  uint LOFAR_DipoleDataset::time ()
  {
    uint val (0);

    if (h5get_attribute(val,
			attribute_name(LOFAR::TIME),
			datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }

  // ----------------------------------------------------------- sample_frequency
  
  double LOFAR_DipoleDataset::sample_frequency ()
  {
    double val (0);

    if (h5get_attribute(val,
			attribute_name(LOFAR::SAMPLE_FREQUENCY),
			datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }

  // ----------------------------------------------------------------------- time

  uint LOFAR_DipoleDataset::nyquist_zone ()
  {
    uint val (0);

    if (h5get_attribute(val,
			attribute_name(LOFAR::NYQUIST_ZONE),
			datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // -------------------------------------------------------------- sample_number

  uint LOFAR_DipoleDataset::sample_number ()
  {
    uint val (0);

    if (h5get_attribute(val,
			attribute_name(LOFAR::SAMPLE_NUMBER),
			datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // ---------------------------------------------------------- samples_per_frame

  uint LOFAR_DipoleDataset::samples_per_frame ()
  {
    uint val (0);

    if (h5get_attribute(val,
			attribute_name(LOFAR::SAMPLES_PER_FRAME),
			datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // ---------------------------------------------------------------- data_length

  uint LOFAR_DipoleDataset::data_length ()
  {
    uint val (0);

    if (h5get_attribute(val,
			attribute_name(LOFAR::DATA_LENGTH),
			datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // ----------------------------------------------------------------------- feed

  std::string LOFAR_DipoleDataset::feed ()
  {
    std::string val;

    if (h5get_attribute(val,
			attribute_name(LOFAR::FEED),
			datasetID_p)) {
      return val;
    } else {
      return std::string ("");
    }
  }

  // ----------------------------------------------------------- antenna_position

  casa::Vector<double> LOFAR_DipoleDataset::antenna_position ()
  {
    casa::Vector<double> val;

    if (h5get_attribute(val,
			attribute_name(LOFAR::ANTENNA_POSITION),
			datasetID_p)) {
      return val;
    } else {
      return casa::Vector<double> (1);
    }
  }

  // -------------------------------------------------------- antenna_orientation

  casa::Vector<double> LOFAR_DipoleDataset::antenna_orientation ()
  {
    casa::Vector<double> val;

    if (h5get_attribute(val,
			attribute_name(LOFAR::ANTENNA_ORIENTATION),
			datasetID_p)) {
      return val;
    } else {
      return casa::Vector<double> (1);
    }
  }

  // ---------------------------------------------------------- attributes2record

  casa::Record LOFAR_DipoleDataset::attributes2record ()
  {
    casa::Record rec;

    try {
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::STATION_ID)),
		 station_id());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::RSP_ID)),
		 rsp_id());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::RCU_ID)),
		 rcu_id());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::TIME)),
		 time());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::SAMPLE_FREQUENCY)),
		 sample_frequency());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::NYQUIST_ZONE)),
		 nyquist_zone());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::FEED)),
		 feed());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::ANTENNA_POSITION)),
		 antenna_position());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::ANTENNA_ORIENTATION)),
		 antenna_orientation());
    } catch (std::string message) {
      std::cerr << "[LOFAR_DipoleDataset::attributes2record] "
		<< "Error filling the record with attribute values!\n"
		<< message
		<< std::endl;
    }

    return rec;
  }

} // Namespace LOFAR -- end
