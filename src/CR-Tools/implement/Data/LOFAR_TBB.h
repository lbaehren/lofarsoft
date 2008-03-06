/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (lbaehren@gmal.com)                                      *
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
#include <dal/Enumerations.h>

#include <Data/LOFAR_Timeseries.h>
#include <Data/LOFAR_StationGroup.h>
#include <IO/DataReader.h>

using DAL::LOFAR_Timeseries;
using DAL::LOFAR_StationGroup;

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
      <li>[DAL] HDF5Common.h -- A collection of functions to work with HDF5-based
      datafiles.
      <li>[DAL] Enumerations.h -- A collection of enumerations defining 
          attributes and keywords.
      <li>[DAL] DAL::LOFAR_Timeseries -- Class encapsulating the basic
          information of how to interact with a LOFAR TBB time-series dataset.
      <li>[CR] DataReader -- Base class of CR-Tools data I/O framework.
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
    </ol>

    
    <h3>Example(s)</h3>
    
  */  
  class LOFAR_TBB : public LOFAR_Timeseries, public DataReader {

    //! List of channel IDs pointing to the dipole datasets holding the TBB data
    std::vector <std::string> channelID_p;

  public:
    
    // ------------------------------------------------------------- Construction

    /*!
      \brief Default constructor
    */
    LOFAR_TBB ();
    
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
      \brief Argumented constructor
      
      \param timeseries -- LOFAR_Timeseries object encapsulating the basic 
             information on how to interact with a LOFAR TBB time-series 
	     dataset.
    */
    LOFAR_TBB (LOFAR_Timeseries const &timeseries);
    
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
    
    /*!
      \brief Get a block of raw time-series data for the available data channels

      \todo We still need to implement the support for the selection of dipoles
      within this framework; the arrays originally set up by the DataReader class
      are not yet being used here.
      
      \return fx -- [sample,dipole] 2-dim array with a block of time-series
              values for the selected data channels (dipoles).
    */
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
  
