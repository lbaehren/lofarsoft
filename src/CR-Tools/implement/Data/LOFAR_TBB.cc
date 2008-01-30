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

  // ------------------------------------------------------------------ LOFAR_TBB
  
  LOFAR_TBB::LOFAR_TBB (std::string const &filename)
    : DataReader (),
      filename_p(filename)
  {
    init ();
  }
  
  // ------------------------------------------------------------------ LOFAR_TBB
  
  LOFAR_TBB::LOFAR_TBB (std::string const &filename,
			uint const &blocksize)
    : DataReader (blocksize),
      filename_p(filename)
  {
    init ();
  }
  
  // ------------------------------------------------------------------ LOFAR_TBB
  
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
			   bool const &listStationGroups,
			   bool const &listChannelIDs)
  {
    os << "[LOFAR_TBB] Summar of object properties" << endl;

    /* Variables describing the dataset itself */
    os << "-- Name of data file ... : " << filename_p                 << endl;
    os << "-- Dataset type ........ : " << dataset_p->getType()       << endl;
    os << "-- Dataset name ........ : " << dataset_p->getName()       << endl;
    os << "-- HDF5 file handle ID . : " << dataset_p->getFileHandle() << endl;
    os << "-- nof. station groups . : " << stationGroups_p.size()     << endl;
    os << "-- nof. data channels .. : " << channelID_p.size()         << endl;

    /* Variables describing the setup of the DataReader */
    os << "-- blocksize  [samples ] : " << blocksize_p                << endl;
    os << "-- FFT length [channels] : " << DataReader::fftLength()    << endl;

    /* The rest of the summary output is conditional, because given the number
       station it might get quite a lot. */

    if (listChannelIDs) {
      os << "-- Data channel IDs .. : [" << channelID_p[0];
      for (uint channel(1); channel<channelID_p.size(); channel++) {
	os << " " << channelID_p[channel];
      }
      os << "]" << endl;
    }
    
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
      [1] If opening the data file was successful, we can continue. First we scan
      for the station groups available in the file, since this is the basic unit
      within which the actual data for the individual dipoles are stored.
      [2] Once we are done getting the list of station groups, we can assemble
      the list of channel IDs, which are required for accessing the raw data of
      each individual dipole.
    */
    
    std::vector<std::string> stationGroups = getStationGroups();
    uint channel (0);

    /* Always check if actually a list of groups has been extracted */
    if (stationGroups.size() > 0) {

      for (uint station(0); station<stationGroups.size(); station++) {
	// assemble internal list of station groups
	LOFAR_StationGroup group ((*dataset_p),
				  stationGroups[station]);
	stationGroups_p.push_back(group);
	//
	std::vector<std::string> channel_ids = group.channelIDs();
	for (channel=0; channel<channel_ids.size(); channel++) {
	  channelID_p.push_back(channel_ids[channel]);
	}
      }
      
    } else {
      std::cerr << "[LOFAR_TBB::init] No station group found in data set!"
		<< std::endl;
      status = false;
    }

    if (status) {
      return setStreams();
    }

    return status;
  }

  // ----------------------------------------------------------------- setStreams

  bool LOFAR_TBB::setStreams ()
  {
    bool status (true);
    
    /*
      Set up the iterators to navigate through the data volume and the selection
      of data input channels.
    */

    uint blocksize (blocksize_p);
    nofStreams_p = channelID_p.size();

    iterator_p = new DataIterator[nofStreams_p];
    selectedAntennas_p.resize(nofStreams_p);

    for (uint antenna(0); antenna<nofStreams_p; antenna++) {
      iterator_p[antenna].setDataStart(0);
      iterator_p[antenna].setStride(0);
      iterator_p[antenna].setShift(0);
      iterator_p[antenna].setStepWidthToSample();
      iterator_p[antenna].setBlocksize(blocksize);
      // keep track of the antennas/data channels selected
      selectedAntennas_p(antenna) = antenna;
    }

    /*
      Set the conversion arrays: adc2voltage & fft2calfft
    */
    try {
      // create arrays with default values
      casa::Vector<double> adc2voltage (nofStreams_p,1.0);
      casa::Matrix<casa::DComplex> fft2calfft (DataReader::fftLength(),nofStreams_p,1.0);
      // set values through the init function of the DataReader class
      DataReader::init (blocksize,
			adc2voltage,
			fft2calfft);
    } catch (std::string message) {
      std::cerr << "[LOFAR_TBB::setStreams]" << message << endl;
      status = false;
    }
    
    return status;
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
    
    return stationGroups;
  }

  // ------------------------------------------------------------ setHeaderRecord

  bool LOFAR_TBB::setHeaderRecord ()
  {
    bool status (true);
    uint nofStationGroups (stationGroups_p.size());

    /*
     * Basic check: are there at least data for a single station within the
     * dataset?
     */

    if (nofStationGroups) {

      try {
	// Name of the observatory
	header_p.define("Observatory",stationGroups_p[0].telescope());
	header_p.define("Observer",stationGroups_p[0].observer());
	header_p.define("Project",stationGroups_p[0].project());
	header_p.define("ObservationID",stationGroups_p[0].observationID());
	header_p.define("ObservationMode",stationGroups_p[0].observationMode());
	header_p.define("TriggerType",stationGroups_p[0].triggerType());
      } catch (std::string message) {
	std::cerr << "[LOFAR_TBB::setHeaderRecord] " << message << std::endl;
	status = false;
      }
      
    } else {
      std::cerr << "[LOFAR_TBB::setHeaderRecord] No station group present!"
		<< std::endl;
      status = false;
    }
    
    return status;
  }
  

} // Namespace CR -- end
