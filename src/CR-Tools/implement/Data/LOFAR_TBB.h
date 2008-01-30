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
      <li>[CR] HDF5Common.h -- A collection of functions to work with HDF5-based
      datafiles.
      <li>[CR] DataReader -- Base class of CR-Tools data I/O framework.
      <li>[CR] LOFAR_StationGroup -- Container for the data in the StationGroup
      of LOFAR times-series data, essentially a wrapper for the special case of
      a dalGroup object.
      <li>[CR] LOFAR_DipoleDataset
      <li>[DAL] dalDataset
      <li>[DAL] dalGroup
    </ul>
    
    <h3>Synopsis</h3>

    This class encapsulates the required functionality to connect the DataReader
    framework of the CR-Tools to the lower-level functionality of the Data Access
    Library (DAL), which handles the access to the standard LOFAR data products.

    <ol>
      <li><b>Structure of the data set</b> <br>
      \verbatim
      /                             ... Group
      |-- TELESCOPE                 ... Attribute       ... string
      |-- OBSERVER                  ... Attribute       ... string
      |-- PROJECT                   ... Attribute       ... string
      |-- Station001                ... Group
      |   |-- TELESCOPE             ... Attribute       ... string
      |   |-- OBSERVER              ... Attribute       ... string
      |   |-- PROJECT               ... Attribute       ... string
      |   |-- OBSERVATION_ID        ... Attribute       ... string
      |   |-- OBSERVATION_MODE      ... Attribute       ... string
      |   |-- TRIGGER_TYPE          ... Attribute       ... string
      |   |-- TRIGGER_OFFSET        ... Attribute       ... string
      |   |-- TRIG_ANTS             ... Attribute       ... array<int,1>
      |   |-- BEAM_DIRECTION        ... Attribute       ... array<double,2>
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
      As can be read from the above listing, the internal structure of a LOFAR
      time-series dataset is made up of a number of sub-components, each of them
      representing a hierarchical level in the setup of the telecope.
      <ol>
        <li>Root group
	  <table>
	    <tr>
	    <td class="indexkey">Field Name
	    <td class="indexkey">Type
	    <td class="indexkey">Status
	    <td class="indexkey">Default
	    <td class="indexkey">Desctription
	    </tr>
	    <tr>
	      <td>TELESCOPE
	      <td>string
	      <td>mandatory
	      <td>LOFAR
	      <td>Name of the telescope with which the observation has been 
	      performed.
	    </tr>
	    <tr>
	      <td>OBSERVER
	      <td>string
	      <td>mandatory
	      <td>LOFAR/CR
	      <td>Name of the person either responsible for carrying out the 
	      observation or having requested the observation; combination of 
	      both possible.
	    </tr>
	    <tr>
	      <td>PROJECT
	      <td>string
	      <td>mandatory
	      <td>LOFAR/CR
	      <td>Name of the project for which this observation has been 
	      carried out.
	    </tr>
	    <tr>
	      <td>OBSERVATION_ID
	      <td>string
	      <td>mandatory
	      <td>
	      <td>Unique identifier for this observation.
	    </tr>
	  </table>
	  <br>
        <li>Station group
	  <table>
	    <tr>
	    <td class="indexkey">Field Name
	    <td class="indexkey">Type
	    <td class="indexkey">Status
	    <td class="indexkey">Default
	    <td class="indexkey">Desctription
	    </tr>
	    <tr>
	      <td>STATION_ID
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>Identifier of the LOFAR station at which this dipole is
	      located.
	    </tr>
	    <tr>
	      <td>STATION_POSITION
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>Position of the LOFAR station in the coordinate reference frame
	      specified by <tt>REFERENCE_FRAME</tt>
	    </tr>
	    <tr>
	      <td>REFERENCE_FRAME
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>Reference frame for the position cordinates of this station.
	    </tr>
	    <tr>
	      <td>TELESCOPE
	      <td>string
	      <td>optional
	      <td>LOFAR
	      <td>Name of the telescope with which the observation has been 
	      performed.
	    </tr>
	    <tr>
	      <td>OBSERVER
	      <td>string
	      <td>optional
	      <td>LOFAR/CR
	      <td>Name of the person either responsible for carrying out the 
	      observation or having requested the observation; combination of 
	      both possible.
	    </tr>
	    <tr>
	      <td>PROJECT
	      <td>string
	      <td>optional
	      <td>LOFAR/CR
	      <td>Name of the project for which this observation has been 
	      carried out.
	    </tr>
	    <tr>
	      <td>OBSERVATION_ID
	      <td>string
	      <td>optional
	      <td>
	      <td>Unique identifier for this observation.
	    </tr>
	    <tr>
	      <td>OBSERVATION_MODE
	      <td>string
	      <td>mandatory
	      <td>
	      <td>Description of the telescope mode in which this observation
	      was carried out.
	    </tr>
	    <tr>
	      <td>TRIGGER_TYPE
	      <td>string
	      <td>mandatory
	      <td>
	      <td>
	    </tr>
	    <tr>
	      <td>TRIGGER_OFFSET
	      <td>string
	      <td>mandatory
	      <td>
	      <td>
	    </tr>
	    <tr>
	      <td>TRIGGERED_ANTENNAS
	      <td>array<int,1>
	      <td>optional
	      <td>
	      <td>
	    </tr>
	  </table>
	  <br>
	<li>Dipole dataset
	  <table>
	    <tr>
	    <td class="indexkey">Field Name
	    <td class="indexkey">Type
	    <td class="indexkey">Status
	    <td class="indexkey">Default
	    <td class="indexkey">Desctription
	    </tr>
	    <tr>
	      <td class="indexkey">STATION_ID
	      <td>uint
	      <td>optional
	      <td>
	      <td>Identifier of the LOFAR station at which this dipole is
	      located.
	    </tr>
	    <tr>
	      <td class="indexkey">RSP_ID
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>Identifier of the RSP board, via which this dipole is connected.
	    </tr>
	    <tr>
	      <td class="indexkey">RCU_ID
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>Identifier of the receiver unit (RCU), to which this dipole
	      is connected.
	    </tr>
	    <tr>
	      <td class="indexkey">TIME
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>
	    </tr>
	    <tr>
	      <td class="indexkey">SAMPLE_NUMBER
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>
	    </tr>
	    <tr>
	      <td class="indexkey">SAMPLE_FREQUENCY
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>
	    </tr>
	    <tr>
	      <td class="indexkey">NYQUIST_ZONE
	      <td>uint
	      <td>mandatory
	      <td>
	      <td>
	    </tr>
	  </table>
	  <br>
      </ol>
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
    </ol>

    
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

      \return stationGroups -- Vector with the HDF5-internal IDs of the station
              groups.
    */
    std::vector<std::string> getStationGroups ();

    /*!
      \brief Fill the relevant metadata into the header record

      \return status -- Status of the operation; returns <tt>false</tt> if an
              error was encountered.
    */
    bool setHeaderRecord ();

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
  
