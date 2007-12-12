/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#include <Data/LOFAR_TBB.h>
#include <Utilities/StringTools.h>

using std::endl;

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  LOFAR_TBB::LOFAR_TBB (std::string const &filename)
    : DataReader (),
      filename_p(filename)
  {
    init ();
  }
  
  LOFAR_TBB::LOFAR_TBB (LOFAR_TBB const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  LOFAR_TBB::~LOFAR_TBB ()
  {
    destroy();
  }
  
  void LOFAR_TBB::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  LOFAR_TBB& LOFAR_TBB::operator= (LOFAR_TBB const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void LOFAR_TBB::copy (LOFAR_TBB const &other)
  {
    /* Copy operations for the base class */
    DataReader::operator= (other);

    /* Copy operations for this class*/
    filename_p  = other.filename_p;
    telescope_p = other.telescope_p;
    observer_p  = other.observer_p;
    project_p   = other.project_p;
    channelID_p = other.channelID_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void LOFAR_TBB::summary (std::ostream &os)
  {
    os << "[LOFAR_TBB] Summar of object properties" << endl;
    os << "-- Name of data file . : " << filename_p              << endl;
    os << "-- Dataset type ...... : " << dataset_p->getType()    << endl;
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
  
  bool LOFAR_TBB::init ()
  {
    bool status (true);

    /* Initialize keywords/metadata */

    telescope_p     = "UNDEFINED";
    observer_p      = "UNDEFINED";
    project_p       = "UNDEFINED";
    observationID_p = "UNDEFINED";

    /* Connection to dataset via DAL layer */

    dataset_p = new dalDataset();

    if (dataset_p->open(string2char(filename_p))) {
      std::cerr << "[LOFAR_TBB::init] Error opening datafile." << endl;
      return false;
    }

    /* If opening the data file was successful, we can continued. First we
       scan for the station groups available in the file, since this is the
       basic unit within which the actual data for the individual dipoles are
       stored.
    */

    std::vector<std::string> stationGroups = getStationGroups();

    if (stationGroups.size() == 1) {
      status = getStationGroupKeywords(stationGroups[0]);
    } else {
      std::cerr << "[LOFAR_TBB] "
		<< "Handling of multiple station groups not yet implemented!"
		<< endl;
    }
    
    return status;
  }

  // ----------------------------------------------------------- getStationGroups

  std::vector<std::string> LOFAR_TBB::getStationGroups ()
  {
    unsigned int nofStations (100);
    char stationstr[10];
    std::vector<std::string> stationGroups;

    for (unsigned int station(0); station<nofStations; station++) {
      /* create ID string for the group */
      sprintf( stationstr, "Station%03d", station );
      /* Check if group of given name exists in the file; according to the
	 documentation of the DAL, the openGroup() function returns a NULL
	 pointer in case no group of the given name exists. */
      if (dataset_p->openGroup(stationstr) != NULL) {
	stationGroups.push_back(stationstr);
      }
    }
    
    std::cout << stationGroups.size() << " station group(s) found." << endl;

    return stationGroups;
  }
  
  // ---------------------------------------------------- getStationGroupKeywords

  bool LOFAR_TBB::getStationGroupKeywords (std::string const &groupName)
  {
    bool status (true);
    dalGroup * group = new dalGroup();

    std::cout << "-> Extracting group of name " << groupName << std::endl;
    try {
      group = dataset_p->openGroup(groupName);
    } catch (std::string message) {
      std::cerr << "[LOFAR_TBB::getStationGroupKeywords] " 
		<< message << endl;
      return false;
    }

    /* Extract name of telescope */
    char *telescope = (char*)(group->getAttribute("TELESCOPE"));
    telescope_p = telescope;
    /* Extract name of the observer */
    char *observer = (char*)(group->getAttribute("OBSERVER"));
    observer_p = observer;
    /* Extract name/description of the project */
    char *project = (char*)(group->getAttribute("PROJECT"));
    project_p = project;
    /* Extract observation ID */
    char *observationID = (char*)(group->getAttribute("OBS_ID"));
    observationID_p = observationID;      
    /* Extract observation mode */
    char *observationMode = (char*)(group->getAttribute("OBS_MODE"));
    observationMode_p = observationMode;
    /* Extract trigger type */
    char *triggerType = (char*)(group->getAttribute("TRIG_TYPE"));
    triggerType_p = triggerType;
    /* Extract trigger offset */
    double *triggerOffset = (double*)(group->getAttribute("TRIG_OFST"));
    triggerOffset_p = (*triggerOffset);
    
    return status;
  }
  
  // ------------------------------------------------------------------------- fx

  casa::Matrix<double> LOFAR_TBB::fx ()
  {
    casa::Matrix<double> data;

    return data;
  }

} // Namespace CR -- end
