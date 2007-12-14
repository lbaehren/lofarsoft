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

    channelID_p.resize (other.channelID_p.size());
    channelID_p = other.channelID_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void LOFAR_TBB::summary (std::ostream &os,
			   bool const &listStationGroups)
  {
    os << "[LOFAR_TBB] Summar of object properties" << endl;
    os << "-- Name of data file . : " << filename_p                 << endl;
    os << "-- Dataset type ...... : " << dataset_p->getType()       << endl;
    os << "-- Dataset name ...... : " << dataset_p->getName()       << endl;
    os << "-- HDF5 file handle ID : " << dataset_p->getFileHandle() << endl;
    os << "-- nof. station groups : " << stationGroups_p.size()     << endl;

    /* The rest of the summary output is conditional, because given the number
       station it might get quite a lot. */
    
    if (listStationGroups) {
      for (uint station(0); station<stationGroups_p.size(); station++) {
	stationGroups_p[station].summary();
      }
    }
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

    /* Connection to dataset via DAL layer */

    dataset_p = new dalDataset();
    
    if (dataset_p->open(string2char(filename_p))) {
      std::cerr << "[LOFAR_TBB::init] Error opening file into dalDataset!"
		<< std::endl;
      return false;
    }
    
    /*
      If opening the data file was successful, we can continue. First we scan for
      the station groups available in the file, since this is the basic unit
      within which the actual data for the individual dipoles are stored.
    */
    
    std::vector<std::string> stationGroups = getStationGroups();
    
    for (uint station(0); station<stationGroups.size(); station++) {
      std::cout << "-- storing information for group "
		<< stationGroups[station]
		<< endl;
      //
      LOFAR_StationGroup group ((*dataset_p),
				stationGroups[station]);
      stationGroups_p.push_back(group);
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
  
  // ------------------------------------------------------------------------- fx

  casa::Matrix<double> LOFAR_TBB::fx ()
  {
    uint sample (0);
    uint nofSelectedAntennas (DataReader::nofSelectedAntennas());
    short buffer[blocksize_p];
    casa::Matrix<double> data (blocksize_p,nofSelectedAntennas);

    for (uint antenna (0); antenna<nofSelectedAntennas; antenna++) {
      /* Read the TBB data for the specified channel from the file. */
      dataset_p->read_tbb(channelID_p[antenna],
			  iterator_p[selectedAntennas_p(antenna)].position(),
			  blocksize_p,
			  buffer);
      /* Copy the retrieved data values to the array returned by this function;
	 there might be a more efficient way to do this (e.g. via slicing) but
	 for now this is a working solution.  */
      for (sample=0; sample<blocksize_p; sample++) {
	data(sample,antenna) = buffer[sample];
      }      
    }
    
    return data;
  }

} // Namespace CR -- end
