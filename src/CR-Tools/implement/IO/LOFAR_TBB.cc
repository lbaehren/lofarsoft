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
    : TBB_Timeseries (),
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
    : TBB_Timeseries (filename),
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
    : TBB_Timeseries (timeseries),
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
    : TBB_Timeseries (other),
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
    TBB_Timeseries::operator= (other);
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
    os << "-- Channel IDs            : " << channelID_p                   << endl;

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
    uint nofDipoles (0);

    /* Check if we are actually connected to a dataset */
    
    if (location_p < 0) {
      std::cerr << "[LOFAR_TBB::init] Not connected to dataset!" << std::endl;
      return false;
    }
    
    /*
     * Set up the vector collecting the IDs for the individual dipoles
     */
    nofDipoles = TBB_Timeseries::nofDipoleDatasets();
    channelID_p.resize (nofDipoles);
    
    std::cout << "-- nof dipole datasets = " << nofDipoles << std::endl;
    
    /*
     * Set the correct data for the time and frequency axis
     * - sample frequency
     * - nyquist zone
     */
#ifdef HAVE_CASA
    // retrieve the values
    casa::Vector<uint> nyquistZone            = TBB_Timeseries::nyquist_zone();
    casa::Vector<casa::MFrequency> sampleFreq = TBB_Timeseries::sample_frequency();
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
    std::vector<uint> nyquistZone = TBB_Timeseries::nyquist_zone();
    std::vector<double> sampleFreq = TBB_Timeseries::sample_frequency_value();
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
  
  bool LOFAR_TBB::setStreams ()
  {
#ifdef DEBUGGING_MESSAGES
    std::cout << " [ LOFAR_TBB::setStreams () ] " << std::endl;
#endif
    bool status (true);
    
    /*
     * Set up the iterators to navigate through the data volume and the selection
     * of data input channels.
     */
    std::cout << "-- Setting up DataIterator objects ..." << std::endl;

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
     * Set up the record with the header information
     */
    std::cout << "-- Setting up header record ..." << std::endl;

    status = setHeaderRecord ();
    
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
  
  //_______________________________________________________________________________
  //                                                                setHeaderRecord
  
  /*!
    \param header -- Record containing the header information
    
    \return status -- Status of the operation; returns <tt>false</tt> in case an
            error was encountered.
  */
  bool LOFAR_TBB::setHeaderRecord () 
  {
    bool status (true);

    try {
      // Mandatory fields ________________________

//       header_p.define("Date",headerpoint_p->JDR);
      header_p.define("AntennaIDs",channelID());
      header_p.define("Observatory","LOFAR");
      header_p.define("SampleFreq",DataReader::sampleFrequency());

      // Optional fields _________________________

      header_p.define("SampleNumber",sampleNumber());
      header_p.define("SampleOffset",sampleOffset());
    } catch (AipsError x) {
      cerr << "[LOFAR_TBB::setHeaderRecord] " << x.getMesg() << endl;
      status = false;
    }; 
    
    return status;
  };
  
  //_______________________________________________________________________________
  //                                                                             fx
  
  /*!
    \return data -- [sample,dipole] 2-dim array with a block of time-series
            values for the selected data channels (dipoles).
  */
  casa::Matrix<double> LOFAR_TBB::fx ()
  {
    casa::Matrix<double> data;
    casa::Vector<int> start (nofStreams_p);

    for (uint n(0); n<nofStreams_p; ++n) {
      start(n) = iterator_p[n].position(); 
    }
    
    TBB_Timeseries::fx (data,
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
    
    TBB_Timeseries::fx (data,
			start,
			blocksize_p);
  }

  //_______________________________________________________________________________
  //                                                                     dataLength
  
  casa::Vector<uint> LOFAR_TBB::dataLength ()
  {
    std::vector<uint> val = TBB_Timeseries::data_length();
    casa::Vector<uint> length (val.size());

    for (uint n(0); n<val.size(); ++n) {
      length(n) = val[n];
    }

    return length;
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
    std::vector<int> val = TBB_Timeseries::channelID();
    casa::Vector<int> sample (val.size());

    for (unsigned int n(0); n<val.size(); ++n) {
      sample(n) = val[n];
    }

    return sample;
  }

  //_______________________________________________________________________________
  //                                                                   sampleNumber
  
  casa::Vector<uint> LOFAR_TBB::sampleNumber ()
  {
    std::vector<uint> val = TBB_Timeseries::sample_number();
    casa::Vector<uint> sample (val.size());

    for (unsigned int n(0); n<val.size(); ++n) {
      sample(n) = val[n];
    }

    return sample;
  }

  //_______________________________________________________________________________
  //                                                                   sampleOffset
  
  casa::Vector<int> LOFAR_TBB::sampleOffset (uint const &refAntenna)
  {
    std::vector<int> val = TBB_Timeseries::sample_offset (refAntenna);
    casa::Vector<int> offset (val.size());

    for (unsigned int n(0); n<val.size(); ++n) {
      offset(n) = val[n];
    }

    return offset;
  }
  
} // Namespace CR -- end
