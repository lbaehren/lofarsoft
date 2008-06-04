/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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
#include <Data/LOFAR_StationGroup.h>

using std::cout;
using std::endl;

namespace DAL { // Namespace DAL -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // --------------------------------------------------------- LOFAR_StationGroup
  
  LOFAR_StationGroup::LOFAR_StationGroup ()
  {
    init ();
  }
  
  // --------------------------------------------------------- LOFAR_StationGroup
  
  LOFAR_StationGroup::LOFAR_StationGroup (std::string const &filename,
					  std::string const &group)
  {
    init (filename,
	  group);
  }
  
  // --------------------------------------------------------- LOFAR_StationGroup
  
  LOFAR_StationGroup::LOFAR_StationGroup (hid_t const &location,
					  std::string const &group)
  {
    // Initialize internal variables
    groupID_p = 0;
    
    init (location,
	  group);
  }
    
  // --------------------------------------------------------- LOFAR_StationGroup
  
  LOFAR_StationGroup::LOFAR_StationGroup (hid_t const &group_id)
  {
    init (group_id);
  }
  
  // --------------------------------------------------------- LOFAR_StationGroup
  
  LOFAR_StationGroup::LOFAR_StationGroup (LOFAR_StationGroup const &other)
  {
    // Initialize internal variables
    groupID_p = 0;
    
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  LOFAR_StationGroup::~LOFAR_StationGroup ()
  {
    destroy();
  }
  
  void LOFAR_StationGroup::destroy ()
  {
    herr_t h5error (0);
    
    if (groupID_p > 0) {
      h5error = H5Gclose (groupID_p);
    }
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================

  // ------------------------------------------------------------------ operator=
  
  LOFAR_StationGroup& LOFAR_StationGroup::operator= (LOFAR_StationGroup const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy

  void LOFAR_StationGroup::copy (LOFAR_StationGroup const &other)
  {
    if (other.groupID_p > 0) {
      init (other.groupID_p);
    } else {
      /*
       * if previous operation failed, at least ensure internal handling remains
       * consistent
       */
      groupID_p = 0;
    }
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ------------------------------------------------------------------ telescope

  std::string LOFAR_StationGroup::telescope ()
  {
    std::string val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::TELESCOPE),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("");
    }
  }
  
  // ------------------------------------------------------------------- observer
  
  std::string LOFAR_StationGroup::observer ()
  {
    std::string val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::OBSERVER),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("");
    }
  }
  
  // -------------------------------------------------------------------- project
  
  std::string LOFAR_StationGroup::project ()
  {
    std::string val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::PROJECT),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("");
    }
  }
  
  // ------------------------------------------------------------- observation_id
  
  std::string LOFAR_StationGroup::observation_id ()
  {
    std::string val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::OBSERVATION_ID),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("");
    }
  }
  
  // ----------------------------------------------------------- observation_mode
  
  std::string LOFAR_StationGroup::observation_mode ()
  {
    std::string val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::OBSERVATION_MODE),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("");
    }
  }
  
  // ----------------------------------------------------- station_position_value

  casa::Vector<double> LOFAR_StationGroup::station_position_value ()
  {
    casa::Vector<double> val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::STATION_POSITION_VALUE),
			     groupID_p)) {
      return val;
    } else {
      return casa::Vector<double> (1);
    }
  }
  
  // ------------------------------------------------------ station_position_unit

  std::string LOFAR_StationGroup::station_position_unit ()
  {
    std::string val;

    if (DAL::h5get_attribute(val,
			     DAL::attribute_name(DAL::STATION_POSITION_UNIT),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("UNDEFINED");
    }
  }

  // ----------------------------------------------------- station_position_frame

  std::string LOFAR_StationGroup::station_position_frame ()
  {
    std::string val;

    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::STATION_POSITION_FRAME),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("UNDEFINED");
    }
  }

  // ----------------------------------------------------------- station_position
  
  casa::MPosition LOFAR_StationGroup::station_position ()
  {
    return DAL::h5get_position (DAL::STATION_POSITION_VALUE,
				DAL::STATION_POSITION_UNIT,
				DAL::STATION_POSITION_FRAME,
				groupID_p);
  }
  
  // ------------------------------------------------------- beam_direction_value

  casa::Vector<double> LOFAR_StationGroup::beam_direction_value ()
  {
    casa::Vector<double> val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::BEAM_DIRECTION_VALUE),
			     groupID_p)) {
      return val;
    } else {
      return casa::Vector<double> (1);
    }
  }
  

  // -------------------------------------------------------- beam_direction_unit

  std::string LOFAR_StationGroup::beam_direction_unit ()
  {
    std::string val;

    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::BEAM_DIRECTION_UNIT),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("UNDEFINED");
    }
  }
  
  // ------------------------------------------------------- beam_direction_frame

  std::string LOFAR_StationGroup::beam_direction_frame ()
  {
    std::string val;

    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::BEAM_DIRECTION_FRAME),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("UNDEFINED");
    }
  }
  
  // ------------------------------------------------------------- beam_direction
  
#ifdef HAVE_CASA
  casa::MDirection LOFAR_StationGroup::beam_direction ()
  {
    return DAL::h5get_direction (DAL::BEAM_DIRECTION_VALUE,
				 DAL::BEAM_DIRECTION_UNIT,
				 DAL::BEAM_DIRECTION_FRAME,
				 groupID_p);
  }
#endif
  
  // --------------------------------------------------------------- trigger_type
  
  std::string LOFAR_StationGroup::trigger_type ()
  {
    std::string val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::TRIGGER_TYPE),
			     groupID_p)) {
      return val;
    } else {
      return std::string ("");
    }
  }
  
  // ------------------------------------------------------------- trigger_offset
  
  double LOFAR_StationGroup::trigger_offset ()
  {
    double val (0);
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::TRIGGER_OFFSET),
			     groupID_p)) {
      return val;
    } else {
      return 0.0;
    }
  }
  
  // --------------------------------------------------------- triggered_antennas
  
#ifdef HAVE_CASA
  casa::Vector<uint> LOFAR_StationGroup::triggered_antennas ()
  {
    casa::Vector<uint> val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::TRIGGERED_ANTENNAS),
			     groupID_p)) {
      return val;
    } else {
      return casa::Vector<uint> (1,0);
    }
  }
#else 
  std::vector<uint> LOFAR_StationGroup::triggered_antennas ()
  {
    std::vector<uint> val;
    
    if (DAL::h5get_attribute(val,
			     attribute_name(DAL::TRIGGERED_ANTENNAS),
			     groupID_p)) {
      return val;
    } else {
      return std::vector<uint> (1,0);
    }
  }
#endif

  // -------------------------------------------------------------------- summary
  
  void LOFAR_StationGroup::summary (std::ostream &os)
  {
    os << "[LOFAR_StationGroup] Summary of object properties"     << endl;
    
    os << "-- Group ID      ...... : " << groupID_p           << endl;
    os << "-- nof. dipole datasets : " << nofDipoleDatasets() << endl;

    if (groupID_p > 0) {
      os << "-- Telesope ............... : " << telescope()              << endl;
      os << "-- Observer ............... : " << observer()               << endl;
      os << "-- Project description .... : " << project()                << endl;
      os << "-- Observation ID ......... : " << observation_id()         << endl;
      os << "-- Observation mode ....... : " << observation_mode()       << endl;
      os << "-- Station position (Unit)  : " << station_position_unit()  << endl;
      os << "-- Station position (Frame) : " << station_position_frame() << endl;
      os << "-- Beam direction (Unit) .. : " << beam_direction_unit()    << endl;
      os << "-- Beam direction (Frame).. : " << beam_direction_frame()   << endl;
      os << "-- Trigger type ........... : " << trigger_type()           << endl;
      os << "-- Trigger offset ......... : " << trigger_offset()         << endl;
    }

#ifdef HAVE_CASA
    if (groupID_p > 0) {
      os << "-- Triggered antennas  : " << triggered_antennas()     << endl;
    }
#endif
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init

  void LOFAR_StationGroup::init ()
  {
    groupID_p = 0;
  }
  
  // ----------------------------------------------------------------------- init

  void LOFAR_StationGroup::init (hid_t const &group_id)
  {
      bool status (true);
      std::string filename;
      std::string group;

      //  group ID -> file name
      status = DAL::h5get_filename (filename,
				    group_id);
      //  group ID -> group name
      if (status) {
	status = DAL::h5get_name (group,
				  group_id);
      }
      /*
       * Forward the reverse engineered information to the init() function to 
       * set up a new object identifier for the group in question.
       */
      if (status) {
	init (filename,
	      group);
      }
  }
  
  // ----------------------------------------------------------------------- init

  void LOFAR_StationGroup::init (std::string const &filename,
				 std::string const &group)
  {
    hid_t file_id (0);
    herr_t h5error (0);
    
    // Initialize internal variables
    groupID_p = 0;
    
    // open the file
    file_id = H5Fopen (filename.c_str(),
		       H5F_ACC_RDONLY,
		       H5P_DEFAULT);
    
    // if opening of file was successfull, try to open group
    if (file_id > 0) {
      init (file_id,
	    group);
    } else {
      std::cerr << "[LOFAR_StationGroup::init] Error opening HDF5 file "
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

  void LOFAR_StationGroup::init (hid_t const &location,
				 std::string const &group)
  {
    bool status (true);
    hid_t group_id (0);

    /*
      Try to open the group within the HDF5 file; the group is expected
      to reside below the object identified by "location".
    */
    try {
      group_id = H5Gopen (location,
			  group.c_str());
    } catch (std::string message) {
      std::cerr << "[LOFAR_StationGroup::init] " << message << std::endl;
      status = false;
    }

    if (group_id > 0) {
      groupID_p = group_id;
    } else {
      groupID_p = 0;
    }

    /* Set up the list of dipole datasets contained within this group */
    status = setDipoleDatasets ();    
  }

  // ---------------------------------------------------------- setDipoleDatasets

  bool LOFAR_StationGroup::setDipoleDatasets ()
  {
    /* Check minimal condition for operations below. */
    if (groupID_p < 1) {
      return false;
    }

    /* Local variables. */

    bool status (true);
    std::string datasetName;
    hsize_t nofObjects (0);
    herr_t h5error (0);

    try {
      // Number of objects in the group specified by its identifier
      h5error = H5Gget_num_objs(groupID_p,
				&nofObjects);
      // go throught the list of objects to identify the datasets
      for (hsize_t idx (0); idx<nofObjects; idx++) {
	// get the type of the object
	if (H5G_DATASET == H5Gget_objtype_by_idx (groupID_p,idx)) {
	  // get the name of the dataset
	  status = DAL::h5get_name (datasetName,
				    groupID_p,
				    idx);
	  // if name retrieval was successful, create new LOFAR_DipoleDataset
	  if (status) {
	    datasets_p.push_back(DAL::LOFAR_DipoleDataset (groupID_p,
							   datasetName));
	  }
	}
      }
    } catch (std::string message) {
      std::cerr << "[LOFAR_StationGroup::setDipoleDatasets] "
		<< message
		<< std::endl;
      return false;
    }
    
    return true;
  }
  
  // ---------------------------------------------------------------- channelsIDs

  std::vector<int> LOFAR_StationGroup::channelIDs ()
  {
    std::vector<int> channels_ids;

    for (uint n(0); datasets_p.size(); n++) {
      channels_ids.push_back(datasets_p[n].channelID());
    }

    return channels_ids;
  }

  // --------------------------------------------------------------- channelNames

  std::vector<std::string> LOFAR_StationGroup::channelNames ()
  {
    std::vector<std::string> names;

    for (uint n(0); datasets_p.size(); n++) {
      names.push_back(datasets_p[n].channelName());
    }

    return names;
  }

  // ----------------------------------------------------------------- datasetIDs

  std::vector<hid_t> LOFAR_StationGroup::datasetIDs ()
  {
    std::vector<hid_t> dataset_ids;

    for (uint n(0); n<datasets_p.size(); n++) {
      dataset_ids.push_back(datasets_p[n].dataset_id());
    }

    return dataset_ids;
  }

  // ---------------------------------------------------------------------- times

#ifdef HAVE_CASA
    casa::Vector<uint> LOFAR_StationGroup::times ()
    {
      uint nofDatasets (datasets_p.size());
      casa::Vector<uint> time (nofDatasets);
      
      for (uint n(0); n<nofDatasets; n++) {
	time(n) = datasets_p[n].time();
      }
      
      return time;
    }
#else
    std::vector<uint> LOFAR_StationGroup::times ()
    {
      std::vector<uint> time;
      
      for (uint n(0); n<datasets_p.size(); n++) {
	time.push_back(datasets_p[n].time());
      }
      
      return time;
    }
#endif

  // --------------------------------------------------------- sample_frequencies

#ifdef HAVE_CASA
    casa::Vector<double> LOFAR_StationGroup::sample_frequencies (std::string const &units)
    {
      uint nofDatasets (datasets_p.size());
      casa::Vector<double> sample_frequency (nofDatasets);
      
      for (uint n(0); n<nofDatasets; n++) {
	sample_frequency(n) = datasets_p[n].sample_frequency_value(units);
      }
      
      return sample_frequency;
    }
#else
    std::vector<double> LOFAR_StationGroup::sample_frequencies (std::string const &units)
    {
      std::vector<double> sample_frequency;
      
      for (uint n(0); n<datasets_p.size(); n++) {
	sample_frequency.push_back(datasets_p[n].sample_frequency(units));
      }
      
      return sample_frequency;
    }
#endif

  // --------------------------------------------------------------- data_lengths

#ifdef HAVE_CASA
    casa::Vector<uint> LOFAR_StationGroup::data_lengths ()
    {
      uint nofDatasets (datasets_p.size());
      casa::Vector<uint> data_length (nofDatasets);
      
      for (uint n(0); n<nofDatasets; n++) {
	data_length(n) = datasets_p[n].data_length();
      }
      
      return data_length;
    }
#else
    std::vector<uint> LOFAR_StationGroup::data_lengths ()
    {
      std::vector<uint> data_length;
      
      for (uint n(0); n<datasets_p.size(); n++) {
	data_length.push_back(datasets_p[n].data_length());
      }
      
      return data_length;
    }
#endif

  // ------------------------------------------------------------------------- fx

  casa::Matrix<double> LOFAR_StationGroup::fx (int const &start,
					       int const &nofSamples)
  {
    /* Check minimal condition for operations below. */
    if (groupID_p < 1) {
      return casa::Matrix<double> (1,1,0);
    }

    uint nofDipoles = nofDipoleDatasets();
    casa::Matrix<double> data (nofSamples,nofDipoles);

    /* Go through the set of dipole datasets and retrieve the data */

    casa::Vector<double> tmp (nofSamples);

    for (uint n(0); n<nofDipoles; n++) {
      // get the channel data ...
      tmp = datasets_p[n].fx(start,nofSamples);
      // ... and add them to the returned array
      data.column(n) = tmp;
    }

    return data;
  }

  // ------------------------------------------------------------------------- fx
  
  casa::Matrix<double> LOFAR_StationGroup::fx (int const &start,
					       int const &nofSamples,
					       std::vector<uint> const &dipoleSelection)
  {
    /* Check minimal condition for operations below. */
    if (groupID_p < 1) {
      return casa::Matrix<double> (1,1,0);
    }

    uint nofDipoles = dipoleSelection.size();
    casa::Matrix<double> data (nofSamples,nofDipoles);

    /* Go through the set of dipole datasets and retrieve the data */
    
    casa::Vector<double> tmp (nofSamples);
    
    for (uint n(0); n<nofDipoles; n++) {
      // get the channel data ...
      tmp = datasets_p[dipoleSelection[n]].fx(start,nofSamples);
      // ... and add them to the returned array
      data.column(n) = tmp;
    }
    
    return data;
  }
  
  // ---------------------------------------------------------- attributes2record

  casa::Record LOFAR_StationGroup::attributes2record ()
  {
    casa::Record rec;

    try {
      rec.define(casa::RecordFieldId(attribute_name(DAL::TELESCOPE)),
		 telescope());
      rec.define(casa::RecordFieldId(attribute_name(DAL::OBSERVER)),
		 observer());
      rec.define(casa::RecordFieldId(attribute_name(DAL::PROJECT)),
		 project());
      rec.define(casa::RecordFieldId(attribute_name(DAL::OBSERVATION_ID)),
		 observation_id());
      rec.define(casa::RecordFieldId(attribute_name(DAL::OBSERVATION_MODE)),
		 observation_mode());
      rec.define(casa::RecordFieldId(attribute_name(DAL::TRIGGER_TYPE)),
		 trigger_type());
      rec.define(casa::RecordFieldId(attribute_name(DAL::TRIGGER_OFFSET)),
		 trigger_offset());
      rec.define(casa::RecordFieldId(attribute_name(DAL::TRIGGERED_ANTENNAS)),
 		 triggered_antennas());
    std::cerr << "ERROR: Castrated code due to changes in the DAL! AH" << std::endl;
      //      rec.define(casa::RecordFieldId(attribute_name(DAL::BEAM_DIRECTION)),
      // 		 beam_direction());
    } catch (std::string message) {
      std::cerr << "[LOFAR_StationGroup::attributes2record] "
		<< "Error filling the record with attribute values!\n"
		<< message
		<< std::endl;
    }

    return rec;
  }

} // Namespace DAL -- end
