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

#ifndef LOFAR_TBB_H
#define LOFAR_TBB_H

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>

#include <dal/dal.h>
#include <dal/dalDataset.h>

#include <Data/LOFAR_StationGroup.h>
#include <IO/DataReader.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class LOFAR_TBB
    
    \ingroup CR_Data
    
    \brief Interface between Data Access Library (DAL) and DataReader framework
    
    \author Lars B&auml;hren

    \date 2007/12/07

    \test tLOFAR_TBB.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[CR] DataReader -- Base class of CR-Tools data I/O framework.
      <li>[CR] LOFAR_StationGroup -- Container for the data in the StationGroup
      of LOFAR times-series data, essentially a wrapper for the special case of
      a dalGroup object.
      <li>[DAL] dalDataset
      <li>[DAL] dalGroup
    </ul>
    
    <h3>Synopsis</h3>

    This class encapsulates the required functionality to connect the DataReader
    framework of the CR-Tools to the lower-level functionality of the Data Access
    Library (DAL), which handles the access to the standard LOFAR data products.

    <ul>
      <li><b>Structure of the data set</b> <br>
      \verbatim
      /                             ... Group
      |-- Station001                ... Group
      |   |-- TELESCOPE             ... Attribute       ... string
      |   |-- OBSERVER              ... Attribute       ... string
      |   |-- PROJECT               ... Attribute       ... string
      |   |-- OBS_ID                ... Attribute       ... string
      |   |-- OBS_MODE              ... Attribute       ... string
      |   |-- TRIG_TYPE             ... Attribute       ... string
      |   |-- TRIG_OFST             ... Attribute       ... string
      |   |-- TRIG_ANTS             ... Attribute       ... array<int,1>
      |   |-- BEAM_DIR              ... Attribute       ... array<double,2>
      |   |-- 001000000             ... Dataset         ... array<uint,1>
      |   |   |-- STATION_ID        ... Attribute       ... uint
      |   |   |-- RSP_ID            ... Attribute       ... uint 
      |   |   |-- RCU_ID            ... Attribute       ... uint
      |   |   |-- SAMPLE_FREQ       ... Attribute       ... double
      |   |   |-- TIME              ... Attribute       ... uint
      |   |   |-- SAMPLE_NR         ... Attribute       ... uint
      |   |   |-- SAMPLES_PER_FRAME ... Attribute       ... uint
      |   |   |-- DATA_LENGTH       ... Attribute       ... uint
      |   |   |-- NYQUIST_ZONE      ... Attribute       ... uint
      |   |   |-- FEED              ... Attribute       ... string
      |   |   |-- ANT_POSITION      ... Attribute       ... array<double,1>
      |   |   `-- ANT_ORIENTATION   ... Attribute       ... array<double,1>
      |   |-- 001000001
      |   |-- 001000002
      |   |-- 001000003
      |   |-- 001000004
      |   |
      |   |-- 001001000
      |   |-- 001001001
      |   |
      |
      \endverbatim
      <li><b>Signal channel ID</b> <br> 
      The signal channel for an individual reception element (dipole) can be
      uniquely identified through a combination of the following three separate
      identifiers:
      <ol>
        <li>STATION_ID  -- Identification number of the LOFAR station within the
	complete array.
	<li>RSP_ID -- Identification number of a Remote Station Processing (RSP)
	board within a given LOFAR station.
	<li>RCU_ID -- Identification number of a Receiver Unit (RCU) attached to
	a given RSP board.
      </ol>
    </ul>

    
    <h3>Example(s)</h3>
    
  */  
  class LOFAR_TBB : public DataReader {

    //! Filename of the dataset
    std::string filename_p;
    //! DAL Dataset object to handle the basic I/O
    dalDataset *dataset_p;

    //! Set of station groups within the data set
    std::vector<CR::LOFAR_StationGroup> stationGroups_p;

    //! Identifiers for the individual channels/dipoles
    std::vector<std::string> channelID_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Argumented constructor

      \param filename -- Name of the file from which to read in the data
    */
    LOFAR_TBB (std::string const &filename);
    
    /*!
      \brief Argumented constructor

      \param filename -- Name of the file from which to read in the data
      \param blocksize -- Size of a block of data, [samples]
    */
    LOFAR_TBB (std::string const &filename,
	       uint const &blocksize);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another LOFAR_TBB object from which to create this new
      one.
    */
    LOFAR_TBB (LOFAR_TBB const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~LOFAR_TBB ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another LOFAR_TBB object from which to make a copy.
    */
    LOFAR_TBB& operator= (LOFAR_TBB const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name of the data file
      
      \return filename -- The name of the data file
    */
    inline std::string filename () const {
      return filename_p;
    }

    /*!
      \brief Get station groups embedded within the dataset

      \return stationGroups -- Vector with a set of LOFAR_StationGroup objects,
              encapsulating the contents of the groups within the dataset.
    */
    inline std::vector<CR::LOFAR_StationGroup> stationGroups () const {
      return stationGroups_p;
    }

    /*!
      \brief Get the object encapsulating the data group for a specific station

      \param id -- Identifier for the station (group).
      \param isStationID -- 

      \return group -- LOFAR_StationGroup object encapsulating the contents of
              a specific StationGroup within the dalDataset.
    */
    LOFAR_StationGroup stationGroup (uint const &id,
				     bool const &isStationID=false);

    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, LOFAR_TBB.
    */
    std::string className () const {
      return "LOFAR_TBB";
    }

    /*!
      \brief Provide a summary of the internal status
      
      \param listStationGroups -- Recursive summary of the embedded
             LOFAR_StationGroup objects?
    */
    inline void summary (bool const &listStationGroups=false,
			 bool const &listChannelIDs=false) {
      summary (std::cout,
	       listStationGroups,
	       listChannelIDs);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os,
		  bool const &listStationGroups=false,
		  bool const &listChannelIDs=false);    

    // ------------------------------------------------------------------ Methods
    
    casa::Matrix<double> fx ();

  protected:

    bool setStreams ();
    
  private:

    /*
      \brief Initialize the internal settings

      \return status -- Status of the operation; returns <tt>false</tt> if an
              error was encountered.
    */
    bool init ();

    /*!
      \brief Get the list of available station groups within the data file
     */
    std::vector<std::string> getStationGroups ();

    /*!
      \brief Unconditional copying
    */
    void copy (LOFAR_TBB const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* LOFAR_TBB_H */
  
