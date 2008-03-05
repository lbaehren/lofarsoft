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

#include <casa/Arrays/ArrayIO.h>
#include <dal/Enumerations.h>
#include <dal/HDF5Common.h>
#include <Data/LOFAR_DipoleDataset.h>

namespace DAL { // Namespace DAL -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // -------------------------------------------------------- LOFAR_DipoleDataset

  LOFAR_DipoleDataset::LOFAR_DipoleDataset ()
  {
    init ();
  }
  
  // -------------------------------------------------------- LOFAR_DipoleDataset
  
  LOFAR_DipoleDataset::LOFAR_DipoleDataset (std::string const &filename,
					    std::string const &dataset)
  {
    init (filename,
	  dataset);
  }

  // -------------------------------------------------------- LOFAR_DipoleDataset

  LOFAR_DipoleDataset::LOFAR_DipoleDataset (hid_t const &location,
					    std::string const &dataset)
  {
    // Initialize internal variables
    datasetID_p = 0;
    
    init (location,
	  dataset);
  }

  // -------------------------------------------------------- LOFAR_DipoleDataset

  LOFAR_DipoleDataset::LOFAR_DipoleDataset (hid_t const &dataset_id)
  {
    init (dataset_id);
  }
  
  // -------------------------------------------------------- LOFAR_DipoleDataset

  LOFAR_DipoleDataset::LOFAR_DipoleDataset (LOFAR_DipoleDataset const &other)
  {
    // Initialize internal variables
    datasetID_p = 0;
    
    copy (other);
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
    if (datasetID_p > 0) {
      herr_t h5error (0);

      h5error = H5Dclose (datasetID_p);
      h5error = H5Eclear ();
    }
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  LOFAR_DipoleDataset& LOFAR_DipoleDataset::operator= (LOFAR_DipoleDataset const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy

  void LOFAR_DipoleDataset::copy (LOFAR_DipoleDataset const &other)
  {
    if (other.datasetID_p > 0) {
      init (other.datasetID_p);
    } else {
      datasetID_p = 0;
    }
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // -------------------------------------------------------------- nofAttributes

  int LOFAR_DipoleDataset::nofAttributes ()
  {
    if (datasetID_p > 0) {
      return H5Aget_num_attrs (datasetID_p);
    } else {
      return -1;
    }
  }
  
  // -------------------------------------------------------------------- summary
  
  void LOFAR_DipoleDataset::summary (std::ostream &os)
  {
    os << "[LOFAR_DipoleDataset::summary]"   << std::endl;
    os << "-- Dataset ID          = " << datasetID_p  << std::endl;
  
    if (datasetID_p) {
      /*
       * Additional check in the HDF5 dataset object; if it is valid, we should
       * be able to retrieve the number of attributes attached to it.
       */
      int nofAttributes = H5Aget_num_attrs (datasetID_p);

      os << "-- nof. attributes     = " << nofAttributes         << std::endl;

      if (nofAttributes < 0) {
	os << "--> Illegal number of attached attributes!" << std::endl;
      } else {
	os << "-- STATION_ID          = " << station_id()          << std::endl;
	os << "-- RSP_ID              = " << rsp_id()              << std::endl;
	os << "-- RCU_ID              = " << rcu_id()              << std::endl;
	os << "-- CHANNEL_ID          = " << channel_id()          << std::endl;
	os << "-- SAMPLE_FREQUENCY    = " << sample_frequency()    << std::endl;
	os << "-- NYQUIST_ZONE        = " << nyquist_zone()        << std::endl;
	os << "-- TIME                = " << time()                << std::endl;
	os << "-- SAMPLE_NUMBER       = " << sample_number()       << std::endl;
	os << "-- SAMPLES_PER_FRAME   = " << samples_per_frame()   << std::endl;
	os << "-- FEED                = " << feed()                << std::endl;
	os << "-- ANTENNA_POSITION    = " << antenna_position()    << std::endl;
	os << "-- ANTENNA_ORIENTATION = " << antenna_orientation() << std::endl;
	os << "-- DATA_LENGTH         = " << data_length()         << std::endl;
      }
    }
  }
  
  // ----------------------------------------------------------------- station_id

  uint LOFAR_DipoleDataset::station_id ()
  {    
    if (datasetID_p > 0) {
      uint val (0);
      if (DAL::h5get_attribute(val,
			       attribute_name(DAL::STATION_ID),
			       datasetID_p)) {
	return val;
      } else {
	std::cerr << "[LOFAR_DipoleDataset::station_id]"
		  << " Error retrieving attribute value!" << std::endl;
	return 0;
      }
    } else {
      return 0;
    }
  }
  
  // --------------------------------------------------------------------- rsp_id
  
  uint LOFAR_DipoleDataset::rsp_id ()
  {
    if (datasetID_p > 0) {
      uint val (0);
      if (DAL::h5get_attribute(val,
			       attribute_name(DAL::RSP_ID),
			       datasetID_p)) {
	return val;
      } else {
	std::cerr << "[LOFAR_DipoleDataset::rsp_id]"
		  << " Error retrieving attribute value!" << std::endl;
	return 0;
      }
    } else {
      return 0;
    }
  }
  
  // --------------------------------------------------------------------- rcu_id

  uint LOFAR_DipoleDataset::rcu_id ()
  {
    if (datasetID_p > 0) {
      uint val (0);
      if (DAL::h5get_attribute(val,
			       attribute_name(DAL::RCU_ID),
			       datasetID_p)) {
	return val;
      } else {
	std::cerr << "[LOFAR_DipoleDataset::rcu_id]"
		  << " Error retrieving attribute value!" << std::endl;
	return 0;
      }
    } else {
      return 0;
    }
  }
  
  // ----------------------------------------------------------------------- time
  
  uint LOFAR_DipoleDataset::time ()
  {
    if (datasetID_p > 0) {
      uint val (0);
      if (DAL::h5get_attribute(val,
			       attribute_name(DAL::TIME),
			       datasetID_p)) {
	return val;
      } else {
	std::cerr << "[LOFAR_DipoleDataset::time]"
		  << " Error retrieving attribute value!" << std::endl;
	return 0;
      }
    } else {
      return 0;
    }
  }
  
  // ----------------------------------------------------------- sample_frequency
  
  double LOFAR_DipoleDataset::sample_frequency ()
  {
    if (datasetID_p > 0) {
      double val (0);
      if (DAL::h5get_attribute(val,
			       attribute_name(DAL::SAMPLE_FREQUENCY),
			       datasetID_p)) {
	return val;
      } else {
	return 0;
      }
    } else {
      return 0;
    }
  }

  // --------------------------------------------------------------- nyquist_zone

  uint LOFAR_DipoleDataset::nyquist_zone ()
  {
    if (datasetID_p > 0) {
      uint val (0);
      if (DAL::h5get_attribute(val,
			       attribute_name(DAL::NYQUIST_ZONE),
			       datasetID_p)) {
	return val;
      } else {
	return 0;
      }
    } else {
      return 0;
    }
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init

  void LOFAR_DipoleDataset::init ()
  {
    datasetID_p = 0;
  }

  // ----------------------------------------------------------------------- init

  void LOFAR_DipoleDataset::init (hid_t const &dataset_id)
  {
      bool status (true);
      std::string filename;
      std::string dataset;
      /*
       * In order to avoid using the same object identifier, we first need to
       * reconstruct the filename from the dataset identifier.
       */
      status = DAL::h5get_filename (filename,
				    dataset_id);
      /*
       * If reconstruction of the filename was ok, then we need the name of the
       * dataset next, to know which position to point to within the file
       * structure.
       */
      if (status) {
	status = DAL::h5get_name (dataset,
				  dataset_id);
      }
      /*
       * Forward the reverse engineered information to the init() function to 
       * set up a new object identifier for the dataset in question.
       */
      if (status) {
	init (filename,
	      dataset);
      }
  }

  // ----------------------------------------------------------------------- init

  void LOFAR_DipoleDataset::init (std::string const &filename,
				  std::string const &dataset)
  {
    hid_t file_id (0);
    herr_t h5error (0);

    // Initialize internal variables
    datasetID_p = 0;

    // open the file
    file_id = H5Fopen (filename.c_str(),
		       H5F_ACC_RDONLY,
		       H5P_DEFAULT);

    // if opening of file was successfull, try to open dataset
    if (file_id > 0) {
      init (file_id,
	    dataset);
    } else {
      std::cerr << "[LOFAR_DipoleDataset::init] Error opening HDF5 file "
		<< filename 
		<< " !"
		<< std::endl;
    }
    
    // release the global file handle and clear the error stack
    if (file_id > 0) {
      h5error = H5Fclose (file_id);
      h5error = H5Eclear ();
    }
  }
  
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
  
  // -------------------------------------------------------------- sample_number

  uint LOFAR_DipoleDataset::sample_number ()
  {
    if (datasetID_p > 0) {
      uint val (0);
      if (DAL::h5get_attribute(val,
			       attribute_name(DAL::SAMPLE_NUMBER),
			       datasetID_p)) {
	return val;
      } else {
	return 0;
      }
    } else {
      return 0;
    }
  }
  
  // ---------------------------------------------------------- samples_per_frame
  
  uint LOFAR_DipoleDataset::samples_per_frame ()
  {
    if (datasetID_p > 0) {
      uint val (0);
      if (DAL::h5get_attribute(val,
			       attribute_name(DAL::SAMPLES_PER_FRAME),
			       datasetID_p)) {
	return val;
      } else {
	return 0;
      }
    } else {
      return 0;
    }
  }
  
  // ---------------------------------------------------------------- data_length
  
  uint LOFAR_DipoleDataset::data_length ()
  {
    uint val (0);
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::DATA_LENGTH),
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
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::FEED),
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
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::ANTENNA_POSITION),
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
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::ANTENNA_ORIENTATION),
			     datasetID_p)) {
      return val;
    } else {
      return casa::Vector<double> (1);
    }
  }
  
  // ----------------------------------------------------------------- channel_id
  
  std::string LOFAR_DipoleDataset::channel_id ()
  {
    char uid[10];
    sprintf(uid,
	    "%03d%03d%03d",
	    station_id(),
	    rsp_id(),
	    rcu_id());
    std::string channelID (uid);

    return channelID;
  }

  // ------------------------------------------------------------------------- fx
  
  bool LOFAR_DipoleDataset::fx (int const &start,
				int const &nofSamples,
				short *data)
  {
    if (datasetID_p > 0) {
      herr_t h5error (0);
      hid_t filespaceID (0);
      int rank (0);
      
      /* Retrieve the identifier of the dataspace associated with the dataset */

      filespaceID = H5Dget_space(datasetID_p);
      
      if (filespaceID < 0) {
	std::cerr << "[LOFAR_DipoleDataset::fx]"
		  << " Error retrieving filespace of dataset!" << std::endl;
	return false;
      }

      /* Retrieve the rank of the dataspace asssociated with the dataset */
      
      rank = H5Sget_simple_extent_ndims(filespaceID);
	
      if (rank < 0) {
	std::cerr << "[LOFAR_DipoleDataset::fx]"
		  << " Error retrieving rank of dataspace!" << std::endl;
	return false;
      }

      /* Retrieve the dimension of the dataspace, i.e. the number of samples */
      
      hsize_t shape[1];
      h5error = H5Sget_simple_extent_dims (filespaceID,
					   shape,
					   NULL);
      if (h5error < 0) {
	std::cerr << "[LOFAR_DipoleDataset::fx]"
		  << " Error retrieving dataspace dimension!" << std::endl;
	return false;
      } else {
	shape[0] = nofSamples;
      }
      
      /* Set up memory space to retrieve the data read from the file */

      hid_t memspace = H5Screate_simple (rank,
					 shape,
					 NULL);
      hsize_t offset[1];
      hsize_t offset_out[1];

      if (filespaceID < 0) {
	std::cerr << "[LOFAR_DipoleDataset::fx]"
		  << " Error creating memory space for reading in data!"
		  << std::endl;
	return false;
      } else {
	offset[0] = start;
      }

      /* Select the hyperslab through the data volume */
      
      h5error = H5Sselect_hyperslab (filespaceID,
				     H5S_SELECT_SET,
				     offset,
				     NULL, 
				     shape,
				     NULL);
      
      if (h5error < 0) {
	std::cerr << "[LOFAR_DipoleDataset::fx]"
		  << " Error selecting hyperslab through the data!"
		  << std::endl;
	return false;
      }

      /* Retrieve the actual data from the file */

      h5error = H5Dread (datasetID_p,
			 H5T_NATIVE_SHORT,
			 memspace,
			 filespaceID,
			 H5P_DEFAULT,
			 data);
      
      if (h5error < 0) {
	std::cerr << "[LOFAR_DipoleDataset::fx]"
		  << " Error reading data from file into buffer!"
		  << std::endl;
	return false;
      }
    } else {
      std::cerr << "[LOFAR_DipoleDataset::fx]"
		<< " Unable to read with connection to dataset object!"
		<< std::endl;
      return false;
    }

    return true;
  }
  
  // ------------------------------------------------------------------------- fx
  
  casa::Vector<short> LOFAR_DipoleDataset::fx (int const &start,
					       int const &nofSamples)
  {
    if (datasetID_p > 0) {
      bool status (true);
      short *dataBuffer;

      dataBuffer = new short [nofSamples];

      /* Retrieve the data from the file */
      
      status = fx (start,
		   nofSamples,
		   dataBuffer);
      
      /* Copy the data from the buffer into the vector returned by this function */
      
      if (status) {
	casa::Vector<short> data (nofSamples);
	// copy values to returned vector
	for (int sample(0); sample<nofSamples; sample++) {
	  data(sample) = dataBuffer[sample];
	}
	// clean up memory
	delete [] dataBuffer;
	// return data
	return data;
      } else {
	return casa::Vector<short> (1,0);
      }
      
    } else {
      return casa::Vector<short> (1,0);
    }
  }
  
  // ---------------------------------------------------------- attributes2record
  
  casa::Record LOFAR_DipoleDataset::attributes2record ()
  {
    casa::Record rec;
    
    try {
      rec.define(casa::RecordFieldId(attribute_name(DAL::STATION_ID)),
		 station_id());
      rec.define(casa::RecordFieldId(attribute_name(DAL::RSP_ID)),
		 rsp_id());
      rec.define(casa::RecordFieldId(attribute_name(DAL::RCU_ID)),
		 rcu_id());
      rec.define(casa::RecordFieldId(attribute_name(DAL::TIME)),
		 time());
      rec.define(casa::RecordFieldId(attribute_name(DAL::SAMPLE_FREQUENCY)),
		 sample_frequency());
      rec.define(casa::RecordFieldId(attribute_name(DAL::NYQUIST_ZONE)),
		 nyquist_zone());
      rec.define(casa::RecordFieldId(attribute_name(DAL::FEED)),
		 feed());
      rec.define(casa::RecordFieldId(attribute_name(DAL::ANTENNA_POSITION)),
		 antenna_position());
      rec.define(casa::RecordFieldId(attribute_name(DAL::ANTENNA_ORIENTATION)),
		 antenna_orientation());
      rec.define(casa::RecordFieldId(attribute_name(DAL::DATA_LENGTH)),
		 data_length());
    } catch (std::string message) {
      std::cerr << "[LOFAR_DipoleDataset::attributes2record] "
		<< "Error filling the record with attribute values!\n"
		<< message
		<< std::endl;
    }

    return rec;
  }

} // Namespace DAL -- end
