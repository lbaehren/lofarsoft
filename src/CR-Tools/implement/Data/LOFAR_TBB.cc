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

using std::cout;
using std::endl;

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ LOFAR_TBB
  
  LOFAR_TBB::LOFAR_TBB ()
    : TBB_Timeseries (),
      DataReader ()
  { }
  
  // ------------------------------------------------------------------ LOFAR_TBB
  
  LOFAR_TBB::LOFAR_TBB (std::string const &filename,
			uint const &blocksize)
    : TBB_Timeseries (filename),
      DataReader (blocksize)
  {
    init ();
  }
  
  // ------------------------------------------------------------------ LOFAR_TBB
  
  LOFAR_TBB::LOFAR_TBB (TBB_Timeseries const &timeseries)
    : TBB_Timeseries (timeseries),
      DataReader ()
  {
    init();
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
  
  // -------------------------------------------------------------------- destroy
  
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
  
  // ----------------------------------------------------------------------- copy
  
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
  
  void LOFAR_TBB::summary (std::ostream &os,
			   bool const &listStationGroups,
			   bool const &listChannelIDs)
  {
    os << "[LOFAR_TBB] Summary of object properties" << endl;

      os << "-- Name of data file ... : " << filename_p            << endl;
      os << "-- HDF5 file ID ........ : " << fileID_p              << endl;
    
    if (fileID_p > 0) {
      /* Variables describing the dataset itself */
      os << "-- Telescope ........... : " << telescope()           << endl;
      os << "-- Observer ............ : " << observer()            << endl;
      os << "-- Project ............. : " << project()             << endl;
      os << "-- nof. station groups . : " << groups_p.size()       << endl;
      os << "-- nof. data channels .. : " << nofDipoleDatasets()   << endl;
      
      /* Variables describing the setup of the DataReader */
      os << "-- blocksize  [samples ] : " << blocksize_p                   << endl;
      os << "-- FFT length [channels] : " << DataReader::fftLength()       << endl;
      os << "-- Sample frequency [Hz] : " << DataReader::sampleFrequency() << endl;
      os << "-- Nyquist zone ........ : " << DataReader::nyquistZone()     << endl;
      
      /* The rest of the summary output is conditional, because given the number
	 station it might get quite a lot. */
      
      if (listChannelIDs) {
	/* [1] Retrieve the channel IDs */
	/* [2] Display the channel IDs  */
      }
      
      if (listStationGroups) {
	for (uint station(0); station<groups_p.size(); station++) {
	  groups_p[station].summary();
	}
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
#ifdef DEBUGGING_MESSAGES
    std::cout << "[LOFAR_TBB::init()]" << std::endl;
#endif
    
    bool status (true);
    
    /* Check if we are actually connected to a dataset */
    
    if (fileID_p < 0) {
      std::cerr << "[LOFAR_TBB::init] Not connected to dataset!" << std::endl;
      return false;
    }
    
    /*
     * Set up the vector collecting the IDs for the individual dipoles
     */
    uint nofDipoles = TBB_Timeseries::nofDipoleDatasets();
    channelID_p.resize (nofDipoles);
    
    std::cout << "-- nof dipole datasets = " << nofDipoles << std::endl;
    
    /*
     * Set the correct data for the time and frequency axis
     */
#ifdef HAVE_CASA
    // retrieve the values
    casa::Vector<double> sampleFreq = TBB_Timeseries::sample_frequency_value();
    // Feedback
    std::cout << "-- sample frequencies = " << sampleFreq << std::endl;
    // adjust internal settings
    if (sampleFreq.nelements() > 0) {
      sampleFrequency_p = sampleFreq(0);
    } else {
      std::cerr << "[LOFAR_TBB::init] Error retrieving sample frequency!"
		<< std::endl;
    }
#else
    // retrieve the values
    std::vector<double> sampleFreq = TBB_Timeseries::sample_frequency_value();
    // adjust internal settings
    if (sampleFreq.size() > 0) {
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
  
  // ----------------------------------------------------------------- setStreams
  
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
  
  // ------------------------------------------------------------ setHeaderRecord
  
  bool LOFAR_TBB::setHeaderRecord () 
  {
    /* Get basic record entries from the attributes attached to the data set */
    header_p = DAL::TBB_Timeseries::attributes2headerRecord();
    
    try {
      header_p.define("SampleFreq",DataReader::sampleFrequency());
      //      header_p.define("StartSample",headerpoint_p->SampleNr);
      //      header_p.define("dDate",(Double)headerpoint_p->Date + 
      //		      (Double)headerpoint_p->SampleNr/(Double)headerpoint_p->sampleFreq/1e6);
    } catch (AipsError x) {
      cerr << "[LOFAR_TBB::setHeaderRecord] " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  };
  
  // ------------------------------------------------------------------------- fx
  
  casa::Matrix<double> LOFAR_TBB::fx ()
  {
    int start = iterator_p[0].position();
    
    return TBB_Timeseries::fx (start,
			       blocksize_p);
  }
  
} // Namespace CR -- end
