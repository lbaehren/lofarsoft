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

#include <Data/LOFAR_StationGroup.h>
#include <Utilities/StringTools.h>

namespace CR { // Namespace CR -- begin
  
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
    
    telescope_p     = other.telescope_p;
    observer_p      = other.observer_p;
    project_p       = other.project_p;
    observationID_p = other.observationID_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void LOFAR_StationGroup::summary (std::ostream &os)
  {
    os << "[LOFAR_StationGroup] Summar of object properties"     << endl;

    os << "-- dalGroup name ..... : " << group_p->getName()      << endl;
    os << "-- dalGroup ID   ..... : " << group_p->getId()        << endl;
    os << "-- HDF5 file handle ID : " << H5fileID_p              << endl;

    os << "-- Telesope .......... : " << telescope_p             << endl;
    os << "-- Observer .......... : " << observer_p              << endl;
    os << "-- Project description : " << project_p               << endl;
    os << "-- Observation ID .... : " << observationID_p         << endl;
    os << "-- Observation mode .. : " << observationMode_p       << endl;
    os << "-- Trigger type ...... : " << triggerType_p           << endl;
    os << "-- Trigger offset .... : " << triggerOffset_p         << endl;
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

    telescope_p       = "UNDEFINED";
    observer_p        = "UNDEFINED";
    project_p         = "UNDEFINED";
    observationID_p   = "UNDEFINED";
    observationMode_p = "UNDEFINED";
    triggerType_p     = "UNDEFINED";
    triggerOffset_p   = 0.0;
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

    if (status) {
      status = extractAttributes ();
    }

    return status;
  }
  
  // ---------------------------------------------------------- extractAttributes

  bool LOFAR_StationGroup::extractAttributes ()
  {
    bool status (true);

    /* Extract name of telescope */
    try {
      char *telescope = (char*)(group_p->getAttribute("TELESCOPE"));
      telescope_p = telescope;
    } catch (std::string message) {
      std::cerr << "-- Error extracting name of telescope" << endl;
      status = false;
    }
    
    /* Extract name of the observer */
    try {
      char *observer = (char*)(group_p->getAttribute("OBSERVER"));
      observer_p = observer;
    } catch (std::string message) {
      std::cerr << "-- Error extracting name of the observer" << endl;
      status = false;
    }

    /* Extract name/description of the project */
    try {
      char *project = (char*)(group_p->getAttribute("PROJECT"));
      project_p = project;
    } catch (std::string message) {
      std::cerr << "-- Error extracting name/description of the project" << endl;
      status = false;
    }
    
    /* Extract observation ID */
    try {
      char *observationID = (char*)(group_p->getAttribute("OBS_ID"));
      observationID_p = observationID;
    } catch (std::string message) {
      std::cerr << "-- Error extracting observation ID" << endl;
      status = false;
    }
    
    /* Extract observation mode */
    try {
      char *observationMode = (char*)(group_p->getAttribute("OBS_MODE"));
      observationMode_p = observationMode;
    } catch (std::string message) {
      std::cerr << "-- Error extracting observation mode" << endl;
      status = false;
    }

    /* Extract trigger type */
    try {
      char *triggerType = (char*)(group_p->getAttribute("TRIG_TYPE"));
      triggerType_p = triggerType;
    } catch (std::string message) {
      std::cerr << "-- Error extracting trigger type" << endl;
      status = false;
    }
    
    /* Extract trigger offset */
    try {
      double *triggerOffset = (double*)(group_p->getAttribute("TRIG_OFST"));
      triggerOffset_p = (*triggerOffset);
    } catch (std::string message) {
      std::cerr << "-- Error extracting trigger offset" << endl;
      triggerOffset_p = 0.0;
      status = false;
    }
    
    return status;
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
    uint max_rsp (10);
    uint max_rcu (10);

    for (station=0; station<max_station; station++) {
      for (rsp=0; rsp<max_rsp; rsp++) {
	for (rcu=0; rcu<max_rcu; rcu++) {
	  // convert the individual ID number of a joint ID string
	  channelName = channelID (station,rsp,rcu);
	  id = groupName + "/" + channelName;
	  // probe the HDF5 file for the presence of a dataset with the given ID
	  if (H5Dopen(H5fileID_p, CR::string2char(id)) > 0) {
	    IDs.push_back(channelName);
	  }
	}
      }
    }

    return IDs;
  }

  // --------------------------------------------------- extractDatasetAttributes

  bool LOFAR_StationGroup::extractDatasetAttributes ()
  {
    bool status (true);

    return status;
  }

} // Namespace CR -- end
