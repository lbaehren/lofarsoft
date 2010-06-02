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

#ifndef LOFAR_TBB_H
#define LOFAR_TBB_H

/* CR-Tools header files */
#include <crtools.h>
#include <IO/DataReader.h>
#include <Math/VectorConversion.h>
#include <Utilities/StringTools.h>

/* casacore header files */
#include <casa/aips.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Record.h>
#include <casa/HDF5/HDF5File.h>
#include <casa/HDF5/HDF5Record.h>

#ifndef WCSLIB_GETWCSTAB
 #define WCSLIB_GETWCSTAB
#endif

/* DAL header files */
#include <dal/TBB_Timeseries.h>

namespace CR { // Namespace CR -- begin

  /*!
    \class LOFAR_TBB
    
    \ingroup CR
    \ingroup IO
    
    \brief Interface between Data Access Library (DAL) and DataReader framework
    
    \author Lars B&auml;hren

    \date 2007/12/07

    \test tLOFAR_TBB.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[DAL] dalCommon.h -- A collection of functions to work with HDF5-based
      datafiles.
      <li>[DAL] Enumerations.h -- A collection of enumerations defining 
          attributes and keywords.
      <li>DAL::TBB_Timeseries, DAL::TBB_StationGroup, DAL::TBB_DipoleDataset --
          High-level interface classes to the HDF5 TBB time-series data sets.
      <li>CR::DataReader -- Base class of CR-Tools data I/O framework.
    </ul>
    
    <h3>Synopsis</h3>

    This class encapsulates the required functionality to connect the DataReader
    framework of the CR-Tools to the lower-level functionality of the Data Access
    Library (DAL), which handles the access to the standard LOFAR data products.
    In that sense CR::LOFAR_TBB is intended to bridge the gap between the two
    packages, hiding from an application programmer the differences in internal 
    handling of e.g. performing selections and book-keeping.
    
    <h3>Example(s)</h3>
    
  */  
  class LOFAR_TBB : private DAL::TBB_Timeseries, public CR::DataReader {

    //! List of channel IDs pointing to the dipole datasets holding the TBB data
    std::vector <std::string> dipoleNames_p;

  public:
    
    // === Construction =========================================================

    //! Default constructor
    LOFAR_TBB ();
    //! Argumented constructor
    LOFAR_TBB (std::string const &filename,
	       uint const &blocksize=1);
    //! Argumented constructor
    LOFAR_TBB (DAL::TBB_Timeseries const &timeseries);
    //! Copy constructor
    LOFAR_TBB (LOFAR_TBB const &other);
    
    // === Destruction ==========================================================

    //! Destructor
    ~LOFAR_TBB ();
    
    // === Operators ============================================================
    
    //! Overloading of the copy operator
    LOFAR_TBB& operator= (LOFAR_TBB const &other); 
    
    // === Parameters access ====================================================

    //! Get list of the channel IDs pointing to the dipole datasets
    inline std::vector <std::string> dipoleNames () const {
      return dipoleNames_p;
    }
    //! Selection of the antennas in the dataset
    bool setSelectedAntennas (Vector<uint> const &antennaSelection,
			      bool const &absolute=false);
    //! Selection of the antennas in the dataset
    bool setSelectedAntennas (Vector<Bool> const &antennaSelection);
    //! Selection of the antennas in the dataset
    bool setSelectedAntennas (std::set<std::string> const &antennaSelection);    
    //! Get the name of the class, "LOFAR_TBB".
    std::string className () const {
      return "LOFAR_TBB";
    }
    //! Provide a summary of the internal status
    void summary (std::ostream &os=std::cout,
		  bool const &listStationGroups=false,
		  bool const &listChannelIDs=false);    

    // === Methods ==============================================================
    
    //! Get a block of raw time-series data for the available data channels
    void fx (casa::Matrix<double> &data);
    //! Get a block of raw time-series data for the available data channels
    casa::Matrix<double> fx ();
    //! Set the record with the header information
    bool setHeaderRecord (casa::Record const &rec) {
      return DataReader::setHeaderRecord (rec);
    }
    //! Get the number of samples of the embedded dipole datasets
    casa::Vector<uint> dataLength ();
    //! Get the sample frequency
    casa::Vector<casa::MFrequency> sampleFrequency ();
    //! Retrieve the list of channel IDs. 
    casa::Vector<int> channelID ();
    //! Get the number of samples elapsed since the last full second. 
    casa::Vector<uint> sampleNumber ();
    //! Time offset between the individual antennas in units of samples
    casa::Vector<int> sampleOffset (uint const &refAntenna=0);

  protected:
    
    //! Set up the streams for reading in the data
    bool setStreams ();
    //! Fill the header information from the into a header record.
    bool setHeaderRecord ();
    //! Time in full seconds.
    casa::Vector<uint> time ();
    
  private:

    //! Initialize the internal settings
    bool init ();
    //! Unconditional copying
    void copy (LOFAR_TBB const &other);
    //! Unconditional deletion 
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* LOFAR_TBB_H */
  
