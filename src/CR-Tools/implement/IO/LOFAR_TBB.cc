/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <IO/LOFAR_TBB.h>

using std::cout;
using std::endl;

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                    LOFAR_TBB
  
  LOFAR_TBB::LOFAR_TBB ()
    : DAL::TBB_Timeseries (),
      DataReader ()
  {
    init ();
  }
  
  //_____________________________________________________________________________
  //                                                                    LOFAR_TBB
  
  /*!
    \param filename -- Name of the file from which to read in the data
    \param blocksize -- Size of a block of data, [samples]
  */
  LOFAR_TBB::LOFAR_TBB (std::string const &filename,
			uint const &blocksize)
    : DAL::TBB_Timeseries (filename),
      DataReader (blocksize)
  {
    init ();
  }
  
  //_____________________________________________________________________________
  //                                                                    LOFAR_TBB
  
  /*!
    \param timeseries -- TBB_Timeseries object encapsulating the basic 
           information on how to interact with a LOFAR TBB time-series 
	   dataset.
  */
  LOFAR_TBB::LOFAR_TBB (TBB_Timeseries const &timeseries)
    : DAL::TBB_Timeseries (timeseries),
      DataReader ()
  {
    init();
  }
  
  //_____________________________________________________________________________
  //                                                                    LOFAR_TBB
  
  /*!
    \param other -- Another LOFAR_TBB object from which to create this new
           one.
  */
  LOFAR_TBB::LOFAR_TBB (LOFAR_TBB const &other)
    : DAL::TBB_Timeseries (other),
      DataReader (other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                   ~LOFAR_TBB
  
  LOFAR_TBB::~LOFAR_TBB ()
  {
    destroy();
  }
  
  //_____________________________________________________________________________
  //                                                                      destroy
  
  void LOFAR_TBB::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ operator=
  
  /*!
    \param other -- Another LOFAR_TBB object from which to make a copy.
  */
  LOFAR_TBB& LOFAR_TBB::operator= (LOFAR_TBB const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  //_____________________________________________________________________________
  //                                                                         copy
  
  void LOFAR_TBB::copy (LOFAR_TBB const &other)
  {
    /* Copy operations for the base classes */
    DAL::TBB_Timeseries::operator= (other);
    DataReader::operator= (other);
    
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                      summary
  
  /*!
    \param os -- Output stream to which the summary is being written.
    \param listStationGroups -- Recursive summary of the embedded
           TBB_StationGroup objects?
  */
  void LOFAR_TBB::summary (std::ostream &os,
			   bool const &listStationGroups,
			   bool const &listChannelIDs)
  {
    std::string telescope     ("UNDEFINED");
    std::string observer      ("UNDEFINED");
    std::string projectID     ("UNDEFINED");
    std::string projectTitle  ("UNDEFINED");
    std::string projectPI     ("UNDEFINED");
    std::string observationID ("UNDEFINED");

    // Retrieve LOFAR common metadata ______________________
    
    if (H5Iis_valid (location_p)) {
      DAL::CommonAttributes attr = commonAttributes();
      telescope     = attr.telescope();
      observer      = attr.observer();
      projectID     = attr.projectID();
      projectTitle  = attr.projectTitle();
      projectPI     = attr.projectPI();
      observationID = attr.observationID();
    }
    
    // Display summary _____________________________________

    os << "[LOFAR_TBB] Summary of object properties" << endl;
    os << "-- Name of data file      : " << filename_p                    << endl;
    os << "-- HDF5 file ID           : " << location_p                    << endl;
    os << "-- blocksize   [samples ] : " << blocksize_p                   << endl;
    os << "-- FFT length  [channels] : " << DataReader::fftLength()       << endl;
    os << "-- Sample frequency  [Hz] : " << DataReader::sampleFrequency() << endl;
    os << "-- Nyquist zone           : " << DataReader::nyquistZone()     << endl;
    os << "-- Telescope              : " << telescope                     << endl;
    os << "-- Observer               : " << observer                      << endl;
    os << "-- Project ID             : " << projectID                     << endl;
    os << "-- Project title          : " << projectTitle                  << endl;
    os << "-- Project PI             : " << projectPI                     << endl;
    os << "-- Observation ID         : " << observationID                 << endl;
    os << "-- nof. station groups    : " << stationGroups_p.size()        << endl;
    os << "-- nof. dipole datasets   : " << nofDipoleDatasets()           << endl;
    os << "-- Channel IDs            : " << dipoleNames_p                 << endl;

    // Summary of underlying structures ____________________
    
    if (H5Iis_valid (location_p)) {
      
      if (listChannelIDs) {
	/* [1] Retrieve the channel IDs */
	/* [2] Display the channel IDs  */
      }
      
      if (listStationGroups) {
	std::map<std::string,DAL::TBB_StationGroup>::iterator it;

	for (it=stationGroups_p.begin(); it!=stationGroups_p.end(); ++it) {
	  (*it).second.summary();
	}
      }
    }
  }  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                         init
  
  bool LOFAR_TBB::init ()
  {
    bool status (true);
    unsigned int nofDipoles (0);

    /* Check if we are actually connected to a dataset */
    
    if (location_p < 0) {
      std::cerr << "[LOFAR_TBB::init] Not connected to dataset!" << std::endl;
      return false;
    }

    //________________________________________________________________
    // Set up the vector collecting the IDs for the individual dipoles

    nofDipoles = DAL::TBB_Timeseries::nofDipoleDatasets();
    dipoleNames_p.resize (nofDipoles);
    dipoleNames_p = DAL::TBB_Timeseries::dipoleNames();

    casa::Vector<unsigned int> antennas;
    DAL::convertVector (antennas,DAL::TBB_Timeseries::dipoleNumbers());
    setAntennas (antennas);

    //________________________________________________________________
    // Set the correct data for the time and frequency axis

#ifdef HAVE_CASA
    // retrieve the values
    casa::Vector<uint> nyquistZone            = DAL::TBB_Timeseries::nyquist_zone();
    casa::Vector<casa::MFrequency> sampleFreq = DAL::TBB_Timeseries::sample_frequency();
    // adjust internal settings
    if (sampleFreq.nelements() > 0) {
      nyquistZone_p     = nyquistZone(0);
      sampleFrequency_p = sampleFreq(0).get("Hz").getValue();
    } else {
      std::cerr << "[LOFAR_TBB::init] Error retrieving sample frequency!"
		<< std::endl;
    }
#else
    // retrieve the values
    std::vector<uint> nyquistZone  = DAL::TBB_Timeseries::nyquist_zone();
    std::vector<double> sampleFreq = DAL::TBB_Timeseries::sample_frequency_value();
    // adjust internal settings
    if (sampleFreq.size() > 0) {
      nyquistZone_p     = nyquistZone[0];
      sampleFrequency_p = sampleFreq[0];
    } else {
      std::cerr << "[LOFAR_TBB::init] Error retrieving sample frequency!"
		<< std::endl;
    }
#endif

    /*
     * Connect the streams (or at least the pointers normally connected to a
     * stream)
     */
    
    if (status) {
      return setStreams();
    }
    
    return status;
  }
  
  //_______________________________________________________________________________
  //                                                                     setStreams
  
  /*!
    \return status -- Status of the operation; returns <tt>false</tt> if an
            error was encountered.
  */
  bool LOFAR_TBB::setStreams ()
  {
    bool status (true);
    
    //________________________________________________________________
    // Set up DataIterator object for navigation through data volume

    uint blocksize (blocksize_p);
    nofStreams_p = dipoleNames_p.size();
    
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
    
    //________________________________________________________________
    // Set up header record

    status = setHeaderRecord ();

    return status;
  }
  
  //_______________________________________________________________________________
  //                                                                setHeaderRecord
  
  /*!
    \return status -- Status of the operation; returns <tt>false</tt> in case an
            error was encountered.
  */
  bool LOFAR_TBB::setHeaderRecord () 
  {
    bool status (true);
    int filesize = min(LOFAR_TBB::dataLength());
    uint time    = min(LOFAR_TBB::time());

    try {
      // Mandatory fields ________________________

      header_p.define("Date",time); 
      header_p.define("AntennaIDs",channelID());
      header_p.define("Observatory","LOFAR");
      header_p.define("SampleFreq",DataReader::sampleFrequency());
      header_p.define("Filesize",filesize);

      // Optional fields _________________________

      header_p.define("TIME",LOFAR_TBB::time());
      header_p.define("SAMPLE_NUMBER",sampleNumber());
      header_p.define("SAMPLE_OFFSET",sampleOffset());
    } catch (casa::AipsError x) {
      cerr << "[LOFAR_TBB::setHeaderRecord] " << x.getMesg() << endl;
      status = false;
    }; 
    
    return status;
  };

  //_______________________________________________________________________________
  //                                                                 getDipoleNames
  
  bool LOFAR_TBB::getDipoleNames ()
  {
    bool status (true);

    return status;
  }
  
  //_______________________________________________________________________________
  //                                                            setSelectedAntennas
  
  /*!
    \param antennaSelection -- Selection of the antennas in the dataset
    \param absolute -- Is the number to be considered the absolute number of the
           dipole? For a LOFAR TBB dataset this option is not supported, as dipole
	   selection is performed on the name of the dipole.

    \return status -- Status of the operation; returns \e false in case an error 
            -- such as a mismatch in number of array elements -- is encountered.
  */
  bool LOFAR_TBB::setSelectedAntennas (Vector<uint> const &antennaSelection,
				       bool const &absolute)
  {
    bool status (absolute);
    unsigned int nofDipoles = dipoleNames_p.size();
    unsigned int nelem      = antennaSelection.nelements();
    std::set<std::string> selection;

    for (unsigned int n(0); n<nelem; ++n) {
      if (antennaSelection(n)<nofDipoles) {
	selection.insert(dipoleNames_p[antennaSelection(n)]);
      }
    }

    /* Apply dipole dataset selection */
    status = setSelectedAntennas (selection);
    
    return status;
  }
  
  //_______________________________________________________________________________
  //                                                            setSelectedAntennas
  
  /*!
    \param antennaSelection -- Selection of the antennas in the dataset

    \return status -- Status of the operation; returns \e false in case an error 
            -- such as a mismatch in number of array elements -- is encountered.
  */
  bool LOFAR_TBB::setSelectedAntennas (Vector<Bool> const &antennaSelection)
  {
    bool status (true);
    unsigned int nelem = antennaSelection.nelements();

    if (nelem == dipoleNames_p.size()) {
      std::set<std::string> selection;
      /* Determine which dipole to select */
      for (unsigned int n(0); n<nelem; ++n) {
	if (antennaSelection(n)==true) {
	  selection.insert(dipoleNames_p[n]);
	}
      }
      /* Apply dipole dataset selection */
      status = setSelectedAntennas (selection);
    } else {
      /* Error message*/
      std::cerr << "[LOFAR_TBB::setSelectedAntennas]"
		<< " Mismatching number of array elements!" << std::endl;
      std::cerr << "-- size(antennaSelection) = " << nelem << std::endl;
      std::cerr << "-- size(dipoleNames)      = " << dipoleNames_p.size() << std::endl;
      /* Adjust status */
      status = false;
    }
    
    return status;
  }
  
  //_______________________________________________________________________________
  //                                                            setSelectedAntennas
  
  /*!
    \return selection -- Names of the dipole datasets to be selected.

    \return status -- Status of the operation; returns \e false in case an error
            was encountered.
  */
  bool LOFAR_TBB::setSelectedAntennas (std::set<std::string> const &antennaSelection)
  {
    bool status        = true;
    unsigned int count = 0;
    unsigned int nelem = dipoleNames_p.size();
    std::set<std::string> selectedDipoles;
    unsigned int nofSelectedDipoles;

    /* Forward the method call ... */
    status             = TBB_Timeseries::selectDipoles (antennaSelection);
    /* ... before retrieving the actual selection. */
    selectedDipoles    = TBB_Timeseries::selectedDipoles();
    nofSelectedDipoles = selectedDipoles.size();
 
    selectedAntennas_p.resize (nofSelectedDipoles);
    selectedAntennas_p = 0;

    /* Go through the list of dipoles and check which ones are part of the
     * selection; if a dipole indeed is selected, its position in the list of
     * dipole names is stored in the selectedAntennas array.  */
    for (unsigned int n(0); n<nelem; ++n) {
      if (static_cast<bool>(antennaSelection.count(dipoleNames_p[n]))) {
	if (count < nofSelectedDipoles) {
	  selectedAntennas_p(count) = n;
	  ++count;
	}
      }
    }
    
    return status;
  }
  
  //_______________________________________________________________________________
  //                                                                             fx
  
  /*!
    \return data -- [sample,dipole] 2-dim array with a block of time-series
            values for the selected data channels (dipoles).
  */
  casa::Matrix<double> LOFAR_TBB::fx ()
  {
    unsigned int sizeSelection = selectedAntennas_p.size();
    casa::Vector<int> start (sizeSelection);
    casa::Matrix<double> data;

    for (uint n(0); n<sizeSelection; ++n) {
      start(n) = iterator_p[selectedAntennas_p(n)].position(); 
    }
    
    DAL::TBB_Timeseries::readData (data,
				   start,
				   blocksize_p);
    
    return data;
  }
  
  //_______________________________________________________________________________
  //                                                                             fx
  
  /*!
    \retval data -- [sample,dipole] 2-dim array with a block of time-series
            values for the selected data channels (dipoles).
  */
  void LOFAR_TBB::fx (casa::Matrix<double> &data)
  {
   casa::Vector<int> start (nofStreams_p);

    for (uint n(0); n<nofStreams_p; ++n) {
      start(n) = iterator_p[n].position(); 
    }
    
    DAL::TBB_Timeseries::readData (data,
				   start,
				   blocksize_p);
  }
  
  //_______________________________________________________________________________
  //                                                                     dataLength
  
  casa::Vector<uint> LOFAR_TBB::dataLength ()
  {
    casa::Vector<uint> val;

    convertVector (val,DAL::TBB_Timeseries::data_length ());

    return val;
  }

  //_______________________________________________________________________________
  //                                                                     dataLength
  
  casa::Vector<casa::MFrequency> LOFAR_TBB::sampleFrequency ()
  {
    return TBB_Timeseries::sample_frequency();
  }
  
  //_______________________________________________________________________________
  //                                                                      channelID
  
  casa::Vector<int> LOFAR_TBB::channelID ()
  {
    std::vector<int> val;
    casa::Vector<int> sample (val.size());

    for (unsigned int n(0); n<val.size(); ++n) {
      sample(n) = val[n];
    }

    return sample;
  }

  //_______________________________________________________________________________
  //                                                                           time
  
  casa::Vector<uint> LOFAR_TBB::time ()
  {
    casa::Vector<uint> val;

    convertVector (val,DAL::TBB_Timeseries::time ());

    return val;
  }

  //_______________________________________________________________________________
  //                                                                   sampleNumber
  
  casa::Vector<uint> LOFAR_TBB::sampleNumber ()
  {
    casa::Vector<uint> val;

    convertVector (val,TBB_Timeseries::sample_number ());

    return val;
  }

  //_______________________________________________________________________________
  //                                                                   sampleOffset
  
  casa::Vector<int> LOFAR_TBB::sampleOffset (uint const &refAntenna)
  {
    casa::Vector<int> val;

    convertVector (val,DAL::TBB_Timeseries::sample_offset (refAntenna));

    return val;
  }
  
} // Namespace CR -- end
