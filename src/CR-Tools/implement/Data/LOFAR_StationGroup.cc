/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#include <Data/LOFAR_Attributes.h>
#include <Data/LOFAR_StationGroup.h>

namespace LOFAR { // Namespace LOFAR -- begin
  
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
  
  LOFAR_StationGroup::LOFAR_StationGroup (dalDataset &dataset,
					  std::string const &name)
  {
    bool status (true);
    
    init ();
    
    status = setStationGroup (dataset,
			      name);
  }
  
  // --------------------------------------------------------- LOFAR_StationGroup
  
  LOFAR_StationGroup::LOFAR_StationGroup (LOFAR_StationGroup const &other)
  {
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
  {;}
  
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
    H5fileID_p      = other.H5fileID_p;
    group_p         = other.group_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ------------------------------------------------------------------ telescope

  std::string LOFAR_StationGroup::telescope ()
  {
    std::string attribute_telescope ("");
    
    if (group_p->getName() != "UNDEFINED") {
      try {
	char *telescope = (char*)(group_p->getAttribute("TELESCOPE"));
	attribute_telescope = telescope;
      } catch (std::string message) {
	std::cerr << "-- Error extracting attribute telescope" << endl;
	attribute_telescope = "";
      }
    }
    
    return attribute_telescope;
  }

  // ------------------------------------------------------------------- observer

  std::string LOFAR_StationGroup::observer ()
  {
    std::string attribute_observer ("");
    
    if (group_p->getName() != "UNDEFINED") {
      try {
	char *observer = (char*)(group_p->getAttribute("OBSERVER"));
	attribute_observer = observer;
      } catch (std::string message) {
	std::cerr << "-- Error extracting attribute OBSERVER" << endl;
	attribute_observer = "";
      }
    }
    
    return attribute_observer;
  }

  // -------------------------------------------------------------------- project

  std::string LOFAR_StationGroup::project ()
  {
    std::string tmp ("");
    
    if (group_p->getName() != "UNDEFINED") {
      try {
	char *buffer = (char*)(group_p->getAttribute("PROJECT"));
	tmp = buffer;
      } catch (std::string message) {
	std::cerr << "-- Error extracting attribute PROJECT" << endl;
	tmp = "";
      }
    }
    
    return tmp;
  }

  // -------------------------------------------------------------- observationID

  std::string LOFAR_StationGroup::observationID ()
  {
    std::string tmp ("");
    
    if (group_p->getName() != "UNDEFINED") {
      try {
	char *buffer = (char*)(group_p->getAttribute("OBS_ID"));
	tmp = buffer;
      } catch (std::string message) {
	std::cerr << "-- Error extracting attribute OBS_ID" << endl;
	tmp = "";
      }
    }
    
    return tmp;
  }

  // ------------------------------------------------------------ observationMode

  std::string LOFAR_StationGroup::observationMode ()
  {
    std::string tmp ("");
    
    if (group_p->getName() != "UNDEFINED") {
      try {
	char *buffer = (char*)(group_p->getAttribute("OBS_MODE"));
	tmp = buffer;
      } catch (std::string message) {
	std::cerr << "-- Error extracting attribute OBS_MODE" << endl;
	tmp = "";
      }
    }
    
    return tmp;
  }

  // ---------------------------------------------------------------- triggerType

  std::string LOFAR_StationGroup::triggerType ()
  {
    std::string tmp ("");
    
    if (group_p->getName() != "UNDEFINED") {
      try {
	char *buffer = (char*)(group_p->getAttribute("TRIG_TYPE"));
	tmp = buffer;
      } catch (std::string message) {
	std::cerr << "-- Error extracting attribute TRIG_TYPE" << endl;
	tmp = "";
      }
    }
    
    return tmp;
  }

  // -------------------------------------------------------------- triggerOffset

  double LOFAR_StationGroup::triggerOffset ()
  {
    double tmp (0.0);
    
    if (group_p->getName() != "UNDEFINED") {
      try {
	double *buffer = (double*)(group_p->getAttribute("TRIG_OFST"));
	tmp = (*buffer);
      } catch (std::string message) {
	std::cerr << "-- Error extracting attribute TRIG_TYPE" << endl;
	tmp = 0.0;
      }
    }
    
    return tmp;
  }

  // ----------------------------------------------------------------------- time

  std::vector<uint> LOFAR_StationGroup::time ()
  {
    hid_t attribute_id;
    hid_t dataset_id;
    uint tmp (0);
    std::string id;
    std::vector<uint> attrTime;
    std::vector<std::string> channel_ids (channelIDs ());

    /*
     * Go through the list of available data channels to extract the time-stamps
     */
    for (uint channel (0); channel<channel_ids.size(); channel++) {
      id = groupName() + "/" + channel_ids[channel];
      // get the identifier for the dataset
      dataset_id = H5Dopen(H5fileID_p, id.c_str());
      // get the identifier for the attribute
      attribute_id = H5Aopen_name (dataset_id,
				   attribute_name(LOFAR::TIME).c_str());
      // If the attribute could be located, we can retrieve its value
      if (attribute_id > 0) {
	if (H5Aread(attribute_id,
		    H5T_NATIVE_UINT,
		    &tmp) ==  0) {
	  attrTime.push_back(tmp);
	}
      }
      // Feedback
//       std::cout << channel << "\t"
// 		<< id << "\t"
// 		<< dataset_id << "\t"
// 		<< attribute_id << "\t"
// 		<< tmp
// 		<< std::endl;
    }

    return attrTime;
  }
  
  // -------------------------------------------------------------------- summary
  
  void LOFAR_StationGroup::summary (std::ostream &os)
  {
    os << "[LOFAR_StationGroup] Summary of object properties"     << endl;

    os << "-- dalGroup name ..... : " << groupName()             << endl;
    os << "-- dalGroup ID   ..... : " << group_p->getId()        << endl;
    os << "-- HDF5 file handle ID : " << H5fileID_p              << endl;

    os << "-- Telesope .......... : " << telescope()             << endl;
    os << "-- Observer .......... : " << observer()              << endl;
    os << "-- Project description : " << project()               << endl;
    os << "-- Observation ID .... : " << observationID()         << endl;
    os << "-- Observation mode .. : " << observationMode()       << endl;
    os << "-- Trigger type ...... : " << triggerType()           << endl;
    os << "-- Trigger offset .... : " << triggerOffset()         << endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ----------------------------------------------------------------------- init

  void LOFAR_StationGroup::init ()
  {
    H5fileID_p        = -1;
    group_p           = new dalGroup();
    group_p->setName ("UNDEFINED");
  }

  // ------------------------------------------------------------ setStationGroup

  bool LOFAR_StationGroup::setStationGroup (dalDataset &dataset,
					    std::string const &name)
  {
    bool status (true);

    try {
      H5fileID_p = dataset.getFileHandle();
      group_p    = dataset.openGroup(name);
    } catch (std::string message) {
      std::cerr << "[LOFAR_StationGroup::setStationGroup] " << message << endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------------------------------ attribute
  
  void LOFAR_StationGroup::getAttribute (std::string &value,
					 std::string const &keyword)
  {
    try {
      char *buffer = (char*)(group_p->getAttribute(keyword));
      value = buffer;
    } catch (std::string message) {
      std::cerr << "-- Error extracting attribute " << keyword << endl;
      value = "";
    }
  }
  
  // ---------------------------------------------------------------- channelsIDs

  std::vector<std::string> LOFAR_StationGroup::channelIDs ()
  {
    std::string id;
    std::string channelName;
    std::vector<std::string> IDs;
    uint station;
    uint rsp;
    uint rcu;

    std::string groupName = group_p->getName();
    uint max_station (100);
    uint max_rsp (100);
    uint max_rcu (100);

    for (station=0; station<max_station; station++) {
      for (rsp=0; rsp<max_rsp; rsp++) {
	for (rcu=0; rcu<max_rcu; rcu++) {
	  // convert the individual ID number of a joint ID string
	  channelName = channelID (station,rsp,rcu);
	  id = groupName + "/" + channelName;
	  // probe the HDF5 file for the presence of a dataset with the given ID
	  if (H5Dopen(H5fileID_p, id.c_str()) > 0) {
	    IDs.push_back(channelName);
	  }
	}
      }
    }

    return IDs;
  }

  // ----------------------------------------------------------------- datasetIDs

  std::vector<hid_t> LOFAR_StationGroup::datasetIDs ()
  {
    std::vector<hid_t> dataset_ids;
    std::string groupName = group_p->getName();
    std::vector<std::string> dataset_names = channelIDs ();

    // get the group ID
    hid_t group_id = H5Gopen (H5fileID_p,groupName.c_str());

    if (group_id > 0) {
      for (uint channel (0); channel<dataset_names.size(); channel++) {
	dataset_ids[channel] = H5Dopen (group_id,
					dataset_names[channel].c_str());
      }
    } else {
      std::cerr << "[LOFAR_StationGroup::datasetIDs] Unable to access group "
		<< groupName
		<< " in current data file."
		<< std::endl;
    }

    return dataset_ids;
  }

  // ---------------------------------------------------------- attributes2record

  casa::Record LOFAR_StationGroup::attributes2record ()
  {
    casa::Record rec;

    try {
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::TELESCOPE)),
		 telescope());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::OBSERVER)),
		 observer());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::PROJECT)),
		 project());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::OBSERVATION_ID)),
		 observationID());
      rec.define(casa::RecordFieldId(attribute_name(LOFAR::OBSERVATION_MODE)),
		 observationMode());
//       rec.define(casa::RecordFieldId(attribute_name(LOFAR::TIME)),
// 		 time());
    } catch (std::string message) {
      std::cerr << "[LOFAR_StationGroup::attributes2record] "
		<< "Error filling the record with attribute values!\n"
		<< message
		<< std::endl;
    }

    return rec;
  }
} // Namespace LOFAR -- end
